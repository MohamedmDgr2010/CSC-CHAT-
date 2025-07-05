#ifndef ENCODE_H
#define ENCODE_H

#include <string>

namespace crypto {

std::string load_key(const std::string& path);
std::string encode(const std::string& plaintext, const std::string& pubkey_pem);
std::string decode(const std::string& ciphertext, const std::string& privkey_pem);

}

#endif // ENCODE_H