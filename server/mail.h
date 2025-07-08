#pragma once
#include <string>
#include "SimpleMail.h"
using namespace std;

#include <iostream>
#include <random>


std::string genotp(int length = 6) {
    const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string otp = "";
    for (int i = 0; i < length; ++i) {
        otp += chars[dist(gen)];
    }
    return otp;
}


namespace mail{
  
  class Mail{
    public:
      string email="alzm0826@gmail.com",passwd="ylyf fvgi srsl ckva",server="smtp.gmail.com";
      int port;
      Mail(string e="alzm0826@gmail.com",string p="ylyf fvgi srsl ckva",string s="smtp.gmail.com",int pt=587) : email(e),passwd(p),server(s),port(pt){}
  };
  
  
  
  int send(Mail* m,string t,string s,string b){
    SimpleMail::Message msg;
    msg.sender=m->email;
    msg.recipient=t;
    msg.subject=s;
    msg.body=b;
    
    SimpleMail::SMTPClient c(m->server,m->port);
    
    c.username=m->email;
    c.password=m->passwd;
    
    if(c.send(msg))return 201;
    else return 500;
    
  }
  
  

}

