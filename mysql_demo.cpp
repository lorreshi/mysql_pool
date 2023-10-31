#include <stdio.h>
#include <iostream>
#include <mysql/mysql.h>
using namespace std;
int main(int argc,char *argv[])
{
    MYSQL conn;
    int res;
    mysql_init(&conn);
    //"root":数据库管理员 "123":root密码 "test":数据库的名字
    if(mysql_real_connect(&conn, "127.0.0.1","root","Xiaobei123456","test_db",0,NULL,CLIENT_FOUND_ROWS))
    {
        cout << "connect success" << endl;
    }else
    {
        cout << "connect failed" << endl;
        cout<<mysql_error(&conn);
    }
    return 0;
}
