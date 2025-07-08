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
  return;
}