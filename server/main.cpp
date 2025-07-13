#include "post.h"
#include "ws.h"
#include <thread>
void https(){
  http::post::login();
  http::post::logup();
  http::post::act();
  Ser->listen("localhost",1024);
}
void Ws(){
  WS();
}
int main(){
  std::cout<<"started app ";
  acc::send_otp("almzm0826@gmail.com","test");
  std::thread h(https);
  std::thread w(Ws);
  
  w.join();
  h.join();
  
  return 0;
}