#pragma once
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "db.h"
#include "acc.h"
using namespace httplib;

SSLServer *Ser=new SSLServer("cert.pem","key.pem");

namespace http::post{
  void login(SSLServer *ser =Ser){
    ser->Post("/login",[](const Request&req,Response&res){
      json data=json::parse(req.body);
      if(!(data.contains("email")&&data.contains("passwd"))){
        res.status=400;
        return;
      }
      res.status=acc::login(data);
      if(res.status==200){
        res.set_header("Set-Cookie", "session=" + acc::gen_jwt(data["email"]) + "; Path=/; HttpOnly; Secure; SameSite=Strict");
      }
      
    });
    
  }
  void logup(SSLServer* ser=Ser){
    ser->Post("/logup",[](const Request&req,Response&res){
      
        json data =json::parse(req.body);
        if(acc::is_there_user(data["email"])){
          res.status=401;
          return;
        }
        res.status=acc::logup(data);
    });
  }
  void act(SSLServer* ser = Ser) {
    ser->Post("/act", [](const Request& req, Response& res) {
        try {
            json data = json::parse(req.body);

            if (!data.contains("email")) {
                res.status = 400;
                res.set_content("Missing email", "text/plain");
                return;
            }

            int s = acc::act_acc(data);
            std::string token = acc::gen_jwt(data["email"].get<std::string>());

            if (s == 200 && token != "error") {
                res.status = 200;
                if(res.status==200){
        res.set_header("Set-Cookie", "session=" + acc::gen_jwt(data["email"]) + "; Path=/; HttpOnly; Secure; SameSite=Strict");
      }
            } else if (token == "error") {
                res.status = 500;
                res.set_content("JWT generation failed", "text/plain");
            } else {
                res.status = s; // مثلاً لو كانت act_acc ترجع 403
                res.set_content("Account activation failed", "text/plain");
            }

        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(std::string("Exception: ") + e.what(), "text/plain");
        }
    });
}
  }
  
