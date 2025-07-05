namespace acc{
  bool create_acc(json data){
  if(data.contains("email")&&data.contains("passwd")&&data.contains("name")){
    
    if(db::is_there_user(data["email"]))return false;
    db::sql_q(db::users,"INSERT INTO accounts(email,name,passwd) VALUES(?,?,?);",{data["email"].get<string>(),data["name"].get<string>(),data["passwd"].get<string>()});
    return true;
    
  }
  return false;
}
}