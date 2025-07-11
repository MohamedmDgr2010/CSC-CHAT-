#pragma once
#include <mariadb/mysql.h>        // الاتصال، تنفيذ الاستعلامات
#include <mariadb/mysqld_error.h> // رسائل الخطأ
#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

namespace db {
    
    class DB {
        public:
            int port = 3306;
            string host = "127.0.0.1";  // إزالة tcp:// لأن MySQL مش بيحتاجها
            string user = "root";
            string passwd = "";
            string name;
            
            DB(string n) : name(n) {}
    };
    
    string sql_q(db::DB* d, string q, vector<string> params) {
        // استخدام unique_ptr للـ connection management
        unique_ptr<MYSQL, decltype(&mysql_close)> conn(mysql_init(NULL), &mysql_close);
        if (!conn) return "mysql_init() failed";

        if (!mysql_real_connect(conn.get(), d->host.c_str(), d->user.c_str(), 
                                d->passwd.c_str(), d->name.c_str(), d->port, NULL, 0)) {
            string err = "mysql_real_connect() failed: ";
            err += mysql_error(conn.get());
            return err;
        }

        unique_ptr<MYSQL_STMT, decltype(&mysql_stmt_close)> stmt(
            mysql_stmt_init(conn.get()), &mysql_stmt_close);
        if (!stmt) {
            return "mysql_stmt_init() failed";
        }

        if (mysql_stmt_prepare(stmt.get(), q.c_str(), q.size())) {
            string err = "mysql_stmt_prepare() failed: ";
            err += mysql_stmt_error(stmt.get());
            return err;
        }

        // تحسين parameter binding
        if (!params.empty()) {
            vector<MYSQL_BIND> bind(params.size());
            vector<unsigned long> lengths(params.size());
            
            // تصفير الـ memory بشكل آمن
            memset(bind.data(), 0, sizeof(MYSQL_BIND) * bind.size());

            for (size_t i = 0; i < params.size(); ++i) {
                lengths[i] = params[i].length();
                bind[i].buffer_type = MYSQL_TYPE_STRING;
                bind[i].buffer = const_cast<char*>(params[i].c_str());
                bind[i].buffer_length = lengths[i];
                bind[i].length = &lengths[i];
            }

            if (mysql_stmt_bind_param(stmt.get(), bind.data())) {
                string err = "mysql_stmt_bind_param() failed: ";
                err += mysql_stmt_error(stmt.get());
                return err;
            }
        }

        if (mysql_stmt_execute(stmt.get())) {
            string err = "mysql_stmt_execute() failed: ";
            err += mysql_stmt_error(stmt.get());
            return err;
        }

        stringstream result;

        // فحص نوع الاستعلام بشكل أكثر أماناً
        string q_lower = q;
        transform(q_lower.begin(), q_lower.end(), q_lower.begin(), ::tolower);
        
        if (q_lower.find("select") == 0) {
            unique_ptr<MYSQL_RES, decltype(&mysql_free_result)> meta(
                mysql_stmt_result_metadata(stmt.get()), &mysql_free_result);
            
            if (!meta) {
                result << "No result.";
            } else {
                int num_fields = mysql_num_fields(meta.get());
                
                // استخدام dynamic buffer size بدلاً من fixed size
                const size_t BUFFER_SIZE = 4096;  // زيادة الحجم
                vector<MYSQL_BIND> result_bind(num_fields);
                vector<vector<char>> buffers(num_fields);  // buffer منفصل لكل field
                vector<unsigned long> lengths(num_fields);
                vector<my_bool> is_null(num_fields);
                
                // تخصيص memory لكل field بشكل منفصل
                for (int i = 0; i < num_fields; ++i) {
                    buffers[i].resize(BUFFER_SIZE);
                }

                memset(result_bind.data(), 0, sizeof(MYSQL_BIND) * num_fields);

                for (int i = 0; i < num_fields; ++i) {
                    result_bind[i].buffer_type = MYSQL_TYPE_STRING;
                    result_bind[i].buffer = buffers[i].data();
                    result_bind[i].buffer_length = BUFFER_SIZE;
                    result_bind[i].length = &lengths[i];
                    result_bind[i].is_null = &is_null[i];
                }

                if (mysql_stmt_bind_result(stmt.get(), result_bind.data())) {
                    return "mysql_stmt_bind_result() failed";
                }

                // قراءة النتائج بشكل آمن
                int fetch_result;
                while ((fetch_result = mysql_stmt_fetch(stmt.get())) == 0) {
                    for (int i = 0; i < num_fields; ++i) {
                        if (!is_null[i]) {
                            // التأكد من أن الطول لا يتجاوز حجم الـ buffer
                            size_t safe_length = min(lengths[i], BUFFER_SIZE - 1);
                            result << string(buffers[i].data(), safe_length) << " ";
                        } else {
                            result << "NULL ";
                        }
                    }
                    result << "\n";
                }
                
                // فحص إذا كان هناك خطأ في الـ fetch
                if (fetch_result != MYSQL_NO_DATA) {
                    result << "Error fetching data: " << mysql_stmt_error(stmt.get());
                }
            }
        } else {
            result << "Query OK.";
        }

        return result.str();
    }
    
} // namespace db