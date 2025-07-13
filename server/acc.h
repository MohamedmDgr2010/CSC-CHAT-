#pragma once
#include "db.h"
#include "mail.h"
#include "jwt.h"

#include "../shared/encode.h"
namespace acc{
  db::DB* users=new db::DB("users");
  mail::Mail *otps=new mail::Mail;
  
  string jwt_pri_key=load_key("keys/ser/jwt_pri_key.pem");
  string jwt_pub_key=load_key("keys/ser/jwt_pub_key.pem");
  
  void send_otp(string email,string code);
  bool is_there_user(const string& email) ;
  bool check_otp(string code,string &email);
  bool create_acc(json data);
  int login(json data);
  int logup(json data);
  bool is_activation_acc(string email);
  int act_acc(string email,string code);
  string gen_jwt(string email);
  
  
  void send_otp(string email,string code){
    mail::send(otps,email,"OTP CODE ",code);
    db::sql_q(users,"INSERT INTO otp(email,code) VALUES(?,?)",{email,code});
    
  }
  
  bool is_there_user(const string& email) {
    string result = db::sql_q(users, "SELECT email FROM accounts WHERE email = ?", {email});
    
    // إزالة الفراغات والمسافات غير المرئية
    result.erase(remove_if(result.begin(), result.end(), ::isspace), result.end());

    return result == email;
}

  bool check_otp(string code,string &email){
    if(sql_q(users,"SELECT code FROM otp WHERE email = ?",{email})==code){
        return true;
      
    }
    else return false;
  }
  bool create_acc(json data){
  if(data.contains("email")&&data.contains("passwd")&&data.contains("name")){
    
    if(is_there_user(data["email"]))return false;
    db::sql_q(users,"INSERT INTO accounts(email,name,passwd) VALUES(?,?,?);",{data["email"].get<string>(),data["name"].get<string>(),crypto::sha256(data["passwd"].get<string>())});
    return true;
    
  }
  return false;
}
  int login(json data) {
    if (
        !(data.contains("email")&&
        data.contains("passwd"))
    )return 400;
    if(!is_activation_acc(data["email"].get<string>()))return 403;
    string q;
        q = db::sql_q(users,"SELECT passwd FROM accounts WHERE email = ?", {data["email"]});

    // إزالة الفراغات والسطر الجديد من بداية ونهاية النص q
q.erase(remove(q.begin(), q.end(), '\n'), q.end());
q.erase(remove(q.begin(), q.end(), ' '), q.end());


    if (q == crypto::sha256(data["passwd"])) {
        return 200;  // تم تسجيل الدخول بنجاح
        
    }

    return 401;  // كلمة مرور خاطئة أو لا يوجد مستخدم
}
  
  int logup(json data){
    if(data.contains("email")&&data.contains("name")&&data.contains("passwd")){
        if(is_there_user(data["email"]))return 401;
        if(acc::create_acc(data))return 200;
        return 400;
        
        
    }return 400;
      }
  
  
  bool is_activation_acc(string email) {
    string result = db::sql_q(users, "SELECT act FROM status WHERE email = ?;", {email});
    
    // إزالة الفراغات الزائدة أو newlines
    result.erase(remove_if(result.begin(), result.end(), ::isspace), result.end());

    return stoi(result) == 1;
}
  int act_acc(string email, string code = "") {
    if (code == "") {
        send_otp(email,genotp());
        return 201;
    }

    if (check_otp(email, code)) {
        string result = db::sql_q(users, "UPDATE status SET act=true WHERE email =?;", {email});
        
        // فحص هل الاستعلام نجح فعلاً
        if (result.find("Query OK") != string::npos) {
            return 200;
        } else {
            return 500; // خطأ في تحديث الحساب
        }
    }

    return 409; // رمز خاطئ
}
  string gen_jwt(string email){
    if(!is_there_user(email)|| !is_activation_acc(email)){
      cerr<<"Not found or not activation";
      return "errore";
      
    }
    string q;
    q=db::sql_q(users,"SELECT id FROM accounts WHERE email = ? ;",{email});
    q.erase(remove(q.begin(), q.end(), '\n'), q.end());
  q.erase(remove(q.begin(), q.end(), ' '), q.end());
  string id=q;
  return create_jwt_from_json(json({{"email",email},{"id",id}}),jwt_pri_key,3600);

  }
}