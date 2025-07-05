#include <jwt/jwt.hpp>
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
    // فك التوكن بدون التحقق من التوقيع (يمكنك إضافة تحقق لاحقًا)
    auto decoded = jwt::decode(token);

    json result;

    // كل عنصر في payload
    for (const auto& [key, claim] : decoded.get_payload_claims()) {
        try {
            // نحاول تحويل القيمة من string JSON إلى كائن JSON
            auto val = json::parse(claim.to_json().dump());
            result[key] = val;
        }
        catch (...) {
            // لو لم تكن JSON، نخزنها كنص عادي
            result[key] = claim.to_string();
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
