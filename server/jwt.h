#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// تحميل مفتاح خاص من ملف
std::string load_key(const std::string& path) {
    std::ifstream in(path);
    return std::string((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
}

// دالة تنشئ JWT من json object
std::string create_jwt_from_json(const json& data, const std::string& private_key) {
    auto builder = jwt::create()
        .set_type("JWT")
        .set_issuer("auth_server");

    for (auto& el : data.items()) {
        builder.set_payload_claim(el.key(), jwt::claim(el.value().dump()));
    }

    return builder.sign(jwt::algorithm::rs256("", private_key, "", ""));
}
 

json jwt_to_json(const std::string& token) {
    auto decoded = jwt::decode(token);

    json result;

    // جلب JSON payload بالكامل
    auto claims = decoded.get_payload_json();

    // نحاول تحويله إلى nlohmann::json
    for (auto it = claims.begin(); it != claims.end(); ++it) {
        try {
            result[it->first] = json::parse(it->second.serialize());
        } catch (...) {
            result[it->first] = it->second.to_str();
        }
    }

    return result;
}

bool check_jwt(const std::string& token, const std::string& public_key) {
    try {
        auto decoded = jwt::decode(token);

        jwt::verify()
            .allow_algorithm(jwt::algorithm::rs256(public_key, "", "", ""))
            .verify(decoded);

        return true;  // التوكن صحيح
    }
    catch (const std::exception& e) {
        std::cerr << "JWT verification failed: " << e.what() << std::endl;
        return false; // التوكن غير صحيح
    }
}
