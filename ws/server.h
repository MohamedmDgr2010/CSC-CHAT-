#include <ixwebsocket/IXWebSocketServer.h>
#include <string>
#include <iostream>

using namespace std;

namespace ws{
  
  
  namespace server{
    
    class Server{
      public:
        void(*on_conn)(
        shared_ptr<ix::WebSocket>,
        shared_ptr<ix::ConnectionState>);
        
        void(*on_msg)(const ix::WebSocketMessagePtr&, ix::WebSocket&);
        void(*on_close)(const ix::WebSocketMessagePtr&, ix::WebSocket&);
        void(*on_open)(const ix::WebSocketMessagePtr&, ix::WebSocket&,ix::ConnectionState&);
        
        ix::WebSocketServer* s;
        void start(int port,string host){
        
          s=new ix::WebSocketServer(port,host);
          s->setOnClientMessageCallback(
    [this](std::shared_ptr<ix::ConnectionState> conn,
           ix::WebSocket& sock,
           const std::unique_ptr<ix::WebSocketMessage>& msg)
    {
        if (msg->type == ix::WebSocketMessageType::Open)
            this->on_open(msg, sock,*conn);

        else if (msg->type == ix::WebSocketMessageType::Close)
            this->on_close(msg, sock);

        else if (msg->type == ix::WebSocketMessageType::Message)
            this->on_msg(msg, sock);
    }
);
          s->setOnConnectionCallback([this](std::weak_ptr<ix::WebSocket> weak_socket,
                                  std::shared_ptr<ix::ConnectionState> conn) {
    if (auto socket = weak_socket.lock()) {
        this->on_conn(socket, conn);
    }
});
       
        auto res = s->listen();  // ✅ أولاً الاستماع
if (!res.first) {
    printf("Failed to listen on port: %s\n", res.second.c_str());
    return;
}

s->start();  // ✅ بعد ما تم فتح البورت بنجاح
      }
        void send(ix::WebSocket sock,string msg){
          sock.send(msg);
        }
      
    };
  }
}


