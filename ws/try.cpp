#include "server.h"
#include <iostream>
using namespace std;
using S = ws::server::Server;

void o(const ix::WebSocketMessagePtr &ms, ix::WebSocket &socket, ix::ConnectionState &){
  cout << "opened" << endl;
  socket.send("Echo opened");
}

void c(const ix::WebSocketMessagePtr &ms, ix::WebSocket &socket){
  cout << "closed" << endl;
  socket.send("Echo msg");
}

void m(const ix::WebSocketMessagePtr &ms, ix::WebSocket &socket){
  cout << "Received: " << ms->str << endl;
}

int main(){
  S ser;
  ser.on_open = o;
  ser.on_close = c;
  ser.on_msg = m;

  cout << "started app! \n";
  ser.start(7485, "0.0.0.0");

  cin.get();  // حتى لا يغلق البرنامج مباشرة
}