#include <mariadb/mysql.h>        // الاتصال، تنفيذ الاستعلامات
#include <mariadb/mysqld_error.h> // رسائل الخطأ
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "mail.h"
using namespace std;

mail::Mail *otps;
namespace db{
  
  class DB{
    public:
      int port=3306;
      string host="tcp://127.0.0.1",user="root",passwd="",name;
      DB(string n) : name(n){}
      
  };
  DB* users=new DB("users");
  
  
 string sql_q(db::DB* d, string q, vector<string> params) {
    MYSQL* conn = mysql_init(NULL);
    if (!conn) return "mysql_init() failed";

    if (!mysql_real_connect(conn, d->host.c_str(), d->user.c_str(), d->passwd.c_str(),
                            d->name.c_str(), d->port, NULL, 0)) {
        string err = "mysql_real_connect() failed: ";
        err += mysql_error(conn);
        mysql_close(conn);
        return err;
    }

    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        mysql_close(conn);
        return "mysql_stmt_init() failed";
    }

    if (mysql_stmt_prepare(stmt, q.c_str(), q.size())) {
        string err = "mysql_stmt_prepare() failed: ";
        err += mysql_stmt_error(stmt);
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return err;
    }

    vector<MYSQL_BIND> bind(params.size());
    vector<unsigned long> lengths(params.size());

    memset(bind.data(), 0, sizeof(MYSQL_BIND) * bind.size());

    for (size_t i = 0; i < params.size(); ++i) {
        lengths[i] = params[i].length();
        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = (void*)params[i].c_str();
        bind[i].buffer_length = lengths[i];
        bind[i].length = &lengths[i];
    }

    if (!params.empty() && mysql_stmt_bind_param(stmt, bind.data())) {
        string err = "mysql_stmt_bind_param() failed: ";
        err += mysql_stmt_error(stmt);
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return err;
    }

    if (mysql_stmt_execute(stmt)) {
        string err = "mysql_stmt_execute() failed: ";
        err += mysql_stmt_error(stmt);
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return err;
    }

    stringstream result;

    if (q.find("SELECT") == 0 || q.find("select") == 0) {
        MYSQL_RES* meta = mysql_stmt_result_metadata(stmt);
        if (!meta) {
            result << "No result.";
        } else {
            int num_fields = mysql_num_fields(meta);
            vector<MYSQL_BIND> result_bind(num_fields);
            vector<char> buffers(1024 * num_fields);
            vector<unsigned long> lengths(num_fields);
            vector<my_bool> is_null(num_fields);

            memset(result_bind.data(), 0, sizeof(MYSQL_BIND) * num_fields);

            for (int i = 0; i < num_fields; ++i) {
                result_bind[i].buffer_type = MYSQL_TYPE_STRING;
                result_bind[i].buffer = &buffers[1024 * i];
                result_bind[i].buffer_length = 1024;
                result_bind[i].length = &lengths[i];
                result_bind[i].is_null = &is_null[i];
            }

            mysql_stmt_bind_result(stmt, result_bind.data());

            while (mysql_stmt_fetch(stmt) == 0) {
                for (int i = 0; i < num_fields; ++i) {
                    if (!is_null[i]) {
                        result << string((char*)&buffers[1024 * i], lengths[i]) << " ";
                    } else {
                        result << "NULL ";
                    }
                }
                result << "\n";
            }

            mysql_free_result(meta);
        }
    } else {
        result << "Query OK.";
    }

    mysql_stmt_close(stmt);
    mysql_close(conn);
    return result.str();
}

  bool is_there_user(string email) {
    string result = sql_q(users, "SELECT email FROM accounts WHERE email = ?", {email});
    
    // إزالة نهاية السطر '\n' إن وجدت
    result.erase(remove(result.begin(), result.end(), '\n'), result.end());

    return result == email;
}

  bool check_otp(string code,string &email){
    if(sql_q(users,"SELECT code FROM otp WHERE email = ?",{email})==code){
        return true;
      
    }
    else return false;
  }
  int login(json data) {
    if (
        !(data.contains("email")&&
        data.contains("passwd"))
    )return 400;
    string q;
        q = sql_q(users,"SELECT passwd FROM users WHERE email = ?", {data["email"]});


    if (q == data["passwd"]) {
        return 200;  // تم تسجيل الدخول بنجاح
    }

    return 400;  // كلمة مرور خاطئة أو لا يوجد مستخدم
}
  int logup(json data){
    if(data.contains("email")&&data.contains("name")&&data.contains("passwd")){
      if(is_there_user(data["email"]))return 401;
      string otp=genotp();
      mail::send(otps,data["email"],"OTP code",otp);
      return 201;
      
    }else return 400;
  }

}

