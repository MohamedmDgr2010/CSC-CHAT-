#include <ixwebsocket/IXWebSocket.h>
#include<nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <mayn/encode.h>
#include <fstream>

using namespace std;
using json=nlohmann::json;
  
namespace ws{
  shared_ptr<ix::WebSocket> sock =make_shared<ix::WebSocket>();

string pri_key,ser_pub_key;

void send(string payload,string to){
  ser_pub_key=crypto::load_key("ser_pub_key.pem");
  json msg=
    {{"payload",crypto::base64_encode(crypto::encode(payload,ser_pub_key))},{"to",to}};
  sock->send(msg.dump());
}
void input(){
  while(true){
    string msg,to;
    cout<<"\nWtrite message : ";
    getline(cin,msg);
    cout<<"\nTo : ";
    cin>>to;
    send(msg,to);
  }
}

string get_jwt(){
  ifstream file("token.jwt");
  string token;
  file>>token;
  file.close();
  return token;
}
string jwt=get_jwt();

string decode(string msg){
  pri_key=crypto::load_key("pri_key.pem");
  return crypto::decode(msg,pri_key);
}
void on_msg(const ix::WebSocketMessagePtr&msg_p){
  if(msg_p->type!=ix::WebSocketMessageType::Message){
    cout<<"not msg";
    return;
  }
  if(msg_p->str.empty())return;
  try {
    json msg_j = json::parse(msg_p->str);
    std::string from = msg_j["from"].get<std::string>();
    cout<<"not decoded \n";
    std::string payload = decode(msg_j["payload"].get<std::string>());
    cout<<"decoded\n";
    // التعامل مع الرسالة بعد فك تشفيرها...
} catch (const json::parse_error& e) {
    std::cerr << "❌ JSON parse error: " << e.what() << "\n";
} catch (const json::type_error& e) {
    std::cerr << "❌ JSON type error: " << e.what() << "\n";
} catch (const std::exception& e) {
    std::cerr << "❌ Other exception: " << e.what() << "\n";
}}
void conn(){

sock->setUrl("ws://localhost:2024");
sock->setOnMessageCallback([](const ix::WebSocketMessagePtr&msg){
  on_msg(msg);
});


sock->setExtraHeaders({{"jwt",jwt}});

sock->start();

}}