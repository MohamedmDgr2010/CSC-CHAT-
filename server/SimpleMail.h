#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include <cstring> // لـ memcpy

namespace SimpleMail {

class Message {
public:
    std::string sender;
    std::string recipient;
    std::string subject;
    std::string body;
};

class SMTPClient {
public:
    std::string server;
    int port;
    std::string username;
    std::string password;

    SMTPClient(const std::string& s, int p) : server(s), port(p) {}

    bool send(const Message& msg);  // إعلان الدالة
};

// تعريف الدالة خارج الكلاس لكن داخل نفس الملف
inline bool SMTPClient::send(const Message& msg) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to init curl" << std::endl;
        return false;
    }

    std::vector<std::string> payload = {
        "To: <" + msg.recipient + ">\r\n",
        "From: <" + msg.sender + ">\r\n",
        "Subject: " + msg.subject + "\r\n",
        "\r\n" + msg.body + "\r\n"
    };

    struct UploadStatus {
        size_t lines_read = 0;
        std::vector<std::string>* payload;
    } upload_ctx{0, &payload};

    auto payload_source = [](char *ptr, size_t size, size_t nmemb, void *userp) -> size_t {
        UploadStatus *upload = static_cast<UploadStatus*>(userp);
        if (upload->lines_read >= upload->payload->size())
            return 0;

        const std::string& line = upload->payload->at(upload->lines_read++);
        memcpy(ptr, line.c_str(), line.size());
        return line.size();
    };

    curl_easy_setopt(curl, CURLOPT_URL, ("smtp://" + server + ":" + std::to_string(port)).c_str());
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, ("<" + msg.sender + ">").c_str());

    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, ("<" + msg.recipient + ">").c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "❌ Failed to send email: " << curl_easy_strerror(res) << std::endl;
        std::cerr << "SMTP Server: " << server << ":" << port << std::endl;
        std::cerr << "Sender: " << msg.sender << std::endl;
        std::cerr << "Recipient: " << msg.recipient << std::endl;
        std::cerr << "Subject: " << msg.subject << std::endl;
        std::cerr << "Body: " << msg.body << std::endl;
    } else {
        std::cout << "✅ Email sent successfully to " << msg.recipient << std::endl;
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    return res == CURLE_OK;
}

} // namespace SimpleMail