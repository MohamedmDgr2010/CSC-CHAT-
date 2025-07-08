#include "encode.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <iostream>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>


namespace crypto {

// تحميل المفتاح كنص كامل
std::string load_key(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open key file: " + path);
    std::string key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return key;
}

std::string sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];

    return ss.str();
}
// طباعة خطأ OpenSSL التفصيلي
void print_openssl_error() {
    unsigned long err = ERR_get_error();
    char err_buf[256];
    ERR_error_string_n(err, err_buf, sizeof(err_buf));
    std::cerr << "OpenSSL error: " << err_buf << std::endl;
}

// تشفير نص باستخدام المفتاح العام بصيغة PEM
std::string encode(const std::string& plaintext, const std::string& pubkey_pem) {
    if(plaintext.empty())return "";
    BIO* bio = BIO_new_mem_buf(const_cast<char*>(pubkey_pem.c_str()), pubkey_pem.size());
    if (!bio) throw std::runtime_error("BIO_new_mem_buf failed");

    RSA* rsa = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    if (!rsa) {
        // حاول قراءة المفتاح بصيغة PKCS#1
        BIO_reset(bio);
        rsa = PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
        if (!rsa) {
            print_openssl_error();
            BIO_free(bio);
            throw std::runtime_error("Invalid public key");
        }
    }
    BIO_free(bio);

    std::vector<unsigned char> encrypted(RSA_size(rsa));
    int len = RSA_public_encrypt(
        (int)plaintext.size(),
        reinterpret_cast<const unsigned char*>(plaintext.data()),
        encrypted.data(),
        rsa,
        RSA_PKCS1_OAEP_PADDING
    );
    RSA_free(rsa);

    if (len == -1) {
        print_openssl_error();
        throw std::runtime_error("RSA_public_encrypt failed");
    }

    return std::string(encrypted.begin(), encrypted.begin() + len);
}

// فك التشفير باستخدام المفتاح الخاص بصيغة PEM
std::string decode(const std::string& ciphertext, const std::string& privkey_pem) {
    if(ciphertext.empty())return "";
    BIO* bio = BIO_new_mem_buf(const_cast<char*>(privkey_pem.c_str()), privkey_pem.size());
    if (!bio) throw std::runtime_error("BIO_new_mem_buf failed");

    RSA* rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
    if (!rsa) {
        print_openssl_error();
        BIO_free(bio);
        throw std::runtime_error("Invalid private key");
    }
    BIO_free(bio);

    std::vector<unsigned char> decrypted(RSA_size(rsa));
    int len = RSA_private_decrypt(
        (int)ciphertext.size(),
        reinterpret_cast<const unsigned char*>(ciphertext.data()),
        decrypted.data(),
        rsa,
        RSA_PKCS1_OAEP_PADDING
    );
    RSA_free(rsa);

    if (len == -1) {
        print_openssl_error();
        throw std::runtime_error("RSA_private_decrypt failed");
    }

    return std::string(decrypted.begin(), decrypted.begin() + len);
}



std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

std::string base64_encode(const std::string &in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4)
        out.push_back('=');
    return out;
}

std::string base64_decode(const std::string& input) {
    int decodedLen = (input.size() / 4) * 3;
    std::string decoded(decodedLen, '\0');

    int actualLen = EVP_DecodeBlock(reinterpret_cast<unsigned char*>(&decoded[0]),
                                    reinterpret_cast<const unsigned char*>(input.data()),
                                    input.size());

    // إزالة البايتات الزائدة إذا وُجد padding
    if (actualLen > 0) {
        while (decoded.size() > 0 && decoded.back() == '\0') {
            decoded.pop_back();
        }
    }

    return decoded;
}

} // namespace crypto
