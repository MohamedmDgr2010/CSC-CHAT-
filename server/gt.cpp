#include "jwt.h"

int main(){
  json j={{"id","1"},{"email","almzm0826@gmail.com"}};
  std::string t=create_jwt_from_json(j,load_key("keys/ser/jwt_pri_key.pem"));
  std::ofstream tf("../client/token.jwt");
  tf<<t;
  tf.close();
}