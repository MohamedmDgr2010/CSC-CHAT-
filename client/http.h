#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

SSLClient* app=new SSLClient("localhost",1024);


int login(string email,string passwd){
  string body=json({{"email",email},{"passwd",passwd}}).dump();
  return app->Post("/login",body)->status;
}
int logup(json data){
  if(data.contains("email")&&data["email"].is_string()&&
  data.contains("name")&&data["name"].is_string()&&
  data.contains("passwd")&&data["passwd"].is_string()){
    return app->Post("/logup",json({{"email",data["email"]},{"name",data["name"]},{"passwd",data["passwd"]}}).dump())->status;
  }
  return 400;
}
int act(string email,string code=""){
  json body;
  body["email"]=email;
  if(!code==""){
    body["code"]=code;
  }
  return app->Post("act",body.dump())->status;
}

