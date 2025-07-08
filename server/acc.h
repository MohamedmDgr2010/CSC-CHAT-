#pragma once
#include "db.h"
#include "mail.h"
#include "../shared/encode.h"
namespace acc{
  db::DB* users=new db::DB("users");
  mail::Mail *otps=new mail::Mail;
  
  void send_otp(string email,string code=genotp()){
    mail::send(otps,email,"OTP CODE ",code);
    db::sql_q(users,"INSERT INTO otp(email,code) VALUES(?,?)",{email,code});
    
  }
  
  bool is_there_user(string email) {
    string result = db::sql_q(users, "SELECT email FROM accounts WHERE email = ?", {email});
    
    // إزالة نهاية السطر '\n' إن وجدت
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());

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
    db::sql_q(users,"INSERT INTO accounts(email,name,passwd) VALUES(?,?,?);",{data["email"].get<string>(),data["name"].get<string>(),data["passwd"].get<string>()});
    return true;
    
  }
  return false;
}
  int login(json data) {
    if (
        !(data.contains("email")&&
        data.contains("passwd"))
    )return 400;
    string q;
        q = sql_q(users,"SELECT passwd FROM accounts WHERE email = ?", {data["email"]});


    if (q == crypto::sha256(data["passwd"])) {
        return 200;  // تم تسجيل الدخول بنجاح
    }

    return 400;  // كلمة مرور خاطئة أو لا يوجد مستخدم
}
  
  int logup(json data){
    if(data.contains("email")&&data.contains("name")&&data.contains("passwd")){
        if(is_there_user(data["email"]))return 401;
        if(acc::create_acc(data))return 200;
        return 400;
        
        
    }return 400;
      }
  
  
  bool is_activation_acc(string email){
    return db::sql_q(users,"SELECT act from accounts WHERE  email = ? ;",{email})=="TRUE";
  }
  int act_acc(string email,string code=""){
    if(code==""){
      send_otp(email);
      return 201;
    }
    
    if(check_otp(email,code)){
      db::sql_q(users,"UPDATE status SET act=true WHERE email =?;",{email});
      return 200;
    }
    return 409;
  }
  
}