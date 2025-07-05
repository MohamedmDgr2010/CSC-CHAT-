#include "encode.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <iostream>

namespace crypto {

// تحميل المفتاح كنص كامل
std::string load_key(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open key file: " + path);
    std::string key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return key;
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

} // namespace crypto
