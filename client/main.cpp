#include "ws.h"

void App(){
  if(!check_jwt(jwt)){
    cout<<"\n Log in \n";
    cout<<"Are you have an account [y,n] ";
    char c;
    
    for(int i=0;i<=5;i++){
      cin>>c;
    if(c!='y'&&c!='n'&&c!='Y'&&c!='N')cerr<<"Y: for Yes,N : for No";
    else{
      break;
    }
    if(i>=5)exit();
    }
    string name,email,password;
    int s;
    switch(c){
      case 'y':
      case 'Y':{
        
        cout<<"\n Enter your email : ";
        cin>>email;
        cout<<"\n Enter your password : ";
        cin>>passwd;
        s=login(email,passwd);
        if(s==200)cout<<"\n Loged in \n";
        if(s==404)cout<<"\n Not found account \n ";
        if(s==401)cout<<"\n Wrong email or password \n ";
        if(s=>500)cout<<"Serser error \n";
        
      }
      case 'n':
      case 'N':{
        cout<<"\n Enter your name : ";
        cin>>name;
        cout<<"\n Enter your email : ";
        cin>>email;
        
        string p1,p2;
        for(int p=0;p=<3;p++){
          
        cout<<"\n Create password : ";
        cin>>p1;
        cout<<"\n Retype password : ";
        cin>>p2;
        if(p1==p2){
          passwd=p1;
          break;
        }
        }
        if(p1!=p2)break;
        json a;
        a["email"]=email;
        a["name"]=name;
        a["passwd"]=passwd;
        s=logup(a);
        
        
      }
      break;
      default:
        cout<<"Invalid input \n";
      
    }
  }
}
int main() {
    ws::conn();  // يبدأ الاتصال

    // ب77قاء البرنامج شغال حتى نغلقه يدويًا أو حتى يحصل خطأ
    ws::input();
    return 0;
}
