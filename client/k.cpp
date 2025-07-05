#include <iostream>
#include <fstream>
#include <string>
#include <vector>

bool checkKeyFile(const std::string& filename, const std::string& expectedBegin) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "❌ File not found: " << filename << "\n";
        return false;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "❌ File empty: " << filename << "\n";
        return false;
    }

    if (line.find(expectedBegin) == std::string::npos) {
        std::cerr << "❌ File " << filename << " does not start with " << expectedBegin << "\n";
        return false;
    }

    std::cout << "✅ File " << filename << " looks valid.\n";
    return true;
}

int main() {
    struct KeyInfo {
        std::string filename;
        std::string expectedBegin;
    };

    std::vector<KeyInfo> keys = {
        {"pri_key.pem", "-----BEGIN PRIVATE KEY-----"},
        {"ser_pub_key.pem", "-----BEGIN PUBLIC KEY-----"},
        {"pub_key.pem", "-----BEGIN PUBLIC KEY-----"}
    };

    for (const auto& key : keys) {
        checkKeyFile(key.filename, key.expectedBegin);
    }

    return 0;
}
