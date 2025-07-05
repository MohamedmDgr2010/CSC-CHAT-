#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <random>
#include <cstring>
#include "httplib.h"

#include <nlohmann/json.hpp>


using namespace std;
using namespace httplib;
using json=nlohmann::json;


string gen_uuid() {
    string uuid;
    random_device rd;
    mt19937 gen(rd());  // مولد أرقام عشوائية ممتاز
    const char* chars = "1234567890-=qwertyuiop[]asdfghjkl;zcvbnm,./!@#$%^&*()_+QWERTYUIOP[]|ASDFGHJKL;ZXCVBBNM?><";
    size_t chars_len = strlen(chars);
    uniform_int_distribution<> dis(0, chars_len - 1);  // توزيع متساوي

    for (int i = 0; i < 32; ++i) {
        uuid += chars[dis(gen)];
    }

    return uuid;
}



int login(SSLClient &app){
    string email,otp,uuid;
    json data;

    
    cout<<"Enter your email: ";
    cin>>email;
    ifstream uuid_f("/usr/local/etc/csc-chat/uuid");
    if(!uuid_f){
        cerr<<"Erorr: could not open UUID file \n";
        return 404;
    }
    uuid_f>>uuid;
    uuid_f.close();

    data["email"]=email;data["uuid"]=uuid;
    auto res=app.Post("/login",data.dump(),"application/json");
    if(res){
        if(res->status==404){cerr<<"\n User not found :"<<res->status;return res->status;}
        if(res->status==201){
        cout<<"\n Enter OTP code : ";
        cin>>otp;
        data["otp"]=otp;


        res=app.Post("/login",data.dump(),"application/json");
        if(res){
            if(res->status==400){cerr<<" \n OTP is not :"<<otp<<" \n ";return res->status;}

            if(res->status==200){cout<<"Loged in \n";return res->status;}
        
        }
    }

    }
    cerr<<"Error could not connect to server";
    return -1;

    
}


int logup(SSLClient&app){
    json data;
    string name,email,id,uuid;
    cout<<"\n Enter your name:";
    cin>>name;
    cout<<"\n Enter your email: ";
    cin>>email;
    cout<<"\n Create new ID";
    cin>>id;
    
   
    ifstream uuid_f("/usr/local/etc/csc-chat/uuid");
    if(!uuid_f){
        cerr<<"Erorr: could not open UUID file \n";
        return 404;
    }
    uuid_f>>uuid;
    uuid_f.close();

    data["name"]=name;data["email"]=email;data["id"]=id;data["uuid"]=uuid;


    
    auto res=app.Post("/logup",data.dump(),"application/json");
    if(res!=nullptr&&res->status==201){
        string otp;cin>>otp;
        data["otp"]=otp;
        res=app.Post("/logup",data.dump(),"application/json");


    }
    if(res!=nullptr&&res->status==200){
        ofstream jwt("/usr/local/etc/csc-chat/jwt");
        jwt<<res->get_header_value("jwt");
        jwt.close();
        
        cout<<"\n loged in \n";
        return res->status;

    }

    return -1;
}










int main(){
    bool slinux;
    cout<<"\n is your system linux? ['y','n']";
    char ans;
    cin>>ans;
        if(ans=='y')slinux=true;
        if(ans=='n')slinux=false;

    
    string path;

    if(slinux)path="/usr/local";
    std::string mk = "mkdir -p " + path + "/etc/csc-chat/";
    system(mk.c_str());
    string etc=path+"/etc/csc-chat/";
    /*init server url*/
        ofstream ser(etc+"server");
        string seru;
        cout<<"\nEnter server url ";
        cin>>seru;
        ser<<seru;
        ser.close();
    /*UUID init */

        ofstream uuid(etc+"uuid");
        string uuids=gen_uuid();
        uuid<<uuids;
        uuid.close();
    /* download app*/
            string xx;
            cout<< " معمارية المعالج x86 , arm ";
            cin>>xx;
            if(xx!="x86"&&xx!="arm")return -1;
            
            httplib::SSLClient app(seru, 443);
            app.enable_server_certificate_verification(false); // لتعطيل التحقق من الشهادة

            string dow="/download?aarch="+xx+"&sys=linux";
            auto res=app.Get(dow);
            if(res&&res->status==200){
                ofstream bin("/usr/local/bin/csc-chat",ios::binary);
                bin<<res->body;
                bin.close();
                cout<<"Downloaded app \n ";
            }else{
                cerr<<"Failed to download app! \n";
            }

            cout<<"are you have account ['y','n;]";
            char acc;
            cin>>acc;
            if(acc!='y'&&acc!='n')exit(1);
            if(acc=='y')login(app);
            else logup(app);   
            






}