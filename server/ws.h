#include "../ws/server.h"
#include <mayn/encode.h>
#include <nlohmann/json.hpp>
#include "jwt.h"
#include <map>

using json=nlohmann::json;
map<string,ix::WebSocket*> Sockets;
map<string,string> users_keys,tockens;
map<string,ix::ConnectionState*> Connections;
ws::server::Server ser;


string msg_pri_key=crypto::load_key("keys/ser/msg_pri_key.pem"),
msg_pub_key=crypto::load_key("keys/ser/msg_pub_key.pem");
string jwt_pub_key=crypto::load_key("keys/ser/jwt_pub_key.pem");





std::map<std::string, std::string> parse_query(const std::string& url_path) {
    map<std::string, std::string> query_map;

    size_t qmark_pos = url_path.find('?');
    if (qmark_pos == std::string::npos) return query_map;

    std::string query = url_path.substr(qmark_pos + 1);  // بعد علامة ?
    size_t pos = 0;
    while (pos < query.size()) {
        size_t amp = query.find('&', pos);
        std::string pair = query.substr(pos, amp - pos);
        size_t eq = pair.find('=');
        if (eq != std::string::npos) {
            std::string key = pair.substr(0, eq);
            std::string value = pair.substr(eq + 1);
            query_map[key] = value;
        }
        if (amp == std::string::npos) break;
        pos = amp + 1;
    }

    return query_map;
}


template<typename K, typename V>
std::map<V, K> rm(const std::map<K, V>& fm) {
    std::map<V, K> sm;
    for (const auto& [k, v] : fm)
        sm[v] = k;
    return sm;
}
void on_msg(const ix::WebSocketMessagePtr&msg, ix::WebSocket&socket){
  
  json rm_j=json::parse(msg->str);
  string who;
  
  if(rm_j.contains("to")&&rm_j.contains("payload")){
    who=rm(Sockets)[&socket];
    string rmp=rm_j["payload"].get<string>();
    string smp=crypto::encode(crypto::decode(rmp,msg_pri_key),users_keys[rm_j["to"]]);
    
    json sm_j={{"from",who},{"payload",smp}};
    string sm=sm_j.dump();
    
    Sockets[rm_j["to"]]->send(sm);
    
    
  }
  
  
}


void on_conn(shared_ptr<ix::WebSocket> socket,shared_ptr<ix::ConnectionState> conn){
  cout<<"connected";
  return;
}

void on_open(const ix::WebSocketMessagePtr&msg, ix::WebSocket&socket,ix::ConnectionState&conn){
    cout<<"connected";
    socket.send("x");
    string url=msg->openInfo.uri;
    json headers=msg->openInfo.headers;
    if(!headers.contains("jwt")){
      socket.close();
      return;
    }
    if(!check_jwt(headers["jwt"],jwt_pub_key)){
      socket.close();
      return;
    }
    json data =jwt_to_json(headers["jwt"]);
    if(!(data.contains("id")&&data.contains("email"))){
      socket.close();
      return;
    }
    
    tockens[data["id"]]=headers["jwt"];
    Sockets[data["id"]]=&socket;
    Connections[data["id"]]=&conn;
    users_keys[data["id"]]=load_key("keys/cli/"+data["id"].get<string>()+"_pub_key.pem");
    
    
    
  }
  
  void on_close(const ix::WebSocketMessagePtr&msg, ix::WebSocket&socket){
    
    return;
  }
  
  
void WS(){
    ser.on_close=on_close;
    ser.on_msg=on_msg;
    ser.on_open=on_open;
    ser.on_conn=on_conn;
    
    ser.start(2024,"127.0.0.1");
    cin.get();
  }