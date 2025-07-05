#include "encode.h"
#include <iostream>

using namespace std;

int main(){
	string emd="m";
	cout<<crypto::encode(emd,crypto::load_key("pub.pem"));


}
