// SimpleMail.h
#ifndef SIMPLEMAIL_H
#define SIMPLEMAIL_H

#include <string>
#include <iostream>
#include <vector>
#include <curl/curl.h>

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

    bool send(const Message& msg) {
        CURL *curl = curl_easy_init();
        if (!curl) return false;

        std::vector<std::string> payload = {
            "To: <" + msg.recipient + ">\r\n",
            "From: <" + msg.sender + ">\r\n",
            "Subject: " + msg.subject + "\r\n",
            "\r\n" + msg.body + "\r\n"
        };

        struct UploadStatus {
            int lines_read = 0;
            std::vector<std::string>* payload;
        } upload_ctx = {0, &payload};

        auto payload_source = [](void *ptr, size_t size, size_t nmemb, void *userp) -> size_t {
            UploadStatus *upload = (UploadStatus*)userp;
            if (upload->lines_read >= upload->payload->size()) return 0;
            const std::string& line = upload->payload->at(upload->lines_read++);
            memcpy(ptr, line.c_str(), line.length());
            return line.length();
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
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }
};

} // namespace SimpleMail

#endif // SIMPLEMAIL_H