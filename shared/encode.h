#ifndef ENCODE_H
#define ENCODE_H

#include <string>

namespace crypto {
std::string sha256(const std::string& data);
std::string load_key(const std::string& path);
std::string encode(const std::string& plaintext, const std::string& pubkey_pem);
std::string decode(const std::string& ciphertext, const std::string& privkey_pem);
std::string base64_encode(const std::string &in);
std::string base64_decode(const std::string& input);
}

#endif // ENCODE_H