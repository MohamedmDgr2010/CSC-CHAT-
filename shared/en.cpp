#include "encode.h"
#include <iostream>

using namespace std;

int main() {
    string emd = "m";

    // 1. تحميل المفتاح العام
    RSA* pub_key = crypto::load_key("pub.pem");

    // 2. تشفير النص الأصلي (emd) → بيانات ثنائية
    std::string encrypted_raw = crypto::encode(emd, pub_key);

    // 3. ترميز البيانات الثنائية إلى Base64
    std::string encoded_base64 = crypto::base64_encode(encrypted_raw);

    // 4. طباعة النص المشفر Base64 (آمن للعرض)
    cout << encoded_base64 << endl;

    return 0;
}
