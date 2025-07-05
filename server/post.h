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
      res.status=db::login(data);
      
    });
    
  }
  map<string,json> sendedotp;
  
  void logup(SSLServer* ser=Ser){
    ser->Post(R"(/logup(\d+))",[](const Request&req,Response&res){
      if(req.matches[1]=="ckeck-otp"){
        json data =json::parse(req.body);
        if(!(data.contains("email")&&data.contains("otp"))){
          res.status=400;
          return;
        }
        
        if(db::is_there_user(data["email"])){
          res.status=400;
          return;
        }
        if(db::check_otp(data["otp"].get<string>(),data["email"].get<string>())){
          acc::create_acc(sendedotp[data["email"]]);
          res.status=201;
          return;
        }
        else {
          res.status=409;
          return;
        }
        
        
      }
      else{
        
      }
    });
  }
}