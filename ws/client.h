#include <ixwebsocket/IXWebSocket.h>
#include<nlohmann/json.hpp>
#include <iostream>

using json=nlohmann::json;
ix::WebSocket socket;

void on_msg(ix::WebSocketMessagePtr&msg_p){
  json msg_j=json::parse(msg_p->str);
  string from=msg_j["from"].get<string>(),
  payload=msg_j["payload"].get<string>();
  
  
  std::cout<<"["<<from<<"]-{"<<payload<<"}-[you]";
  
}
void conn(){

socket.setUrl("ws://localhost:1024");
socket.setOnMessageCallback([](const ix::WebSocketMessagePtr&msg){
  on_msg(msg);
});

}