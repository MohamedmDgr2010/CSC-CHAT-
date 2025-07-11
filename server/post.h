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
        res.set_content(acc::gen_jwt(data["email"]), "text/plain");
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
  void act(SSLServer* ser=Ser){
    ser->Post("/act",[](const Request&req,Response&res){
      json data=json::parse(req.body);
      if(!data.contains("email")){
        res.status=400;
        return;
      }
      res.status=acc::act_acc(data);
      if(res.status==200){
        res.set_content(acc::gen_jwt(data["email"]), "text/plain");
      }
      
    });
  }
  
}