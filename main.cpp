#include <iostream>
#include <memory>
#include <chrono>
#include "sly/mysql_conn.h"
#include "sly/connection_pool.h"

using namespace std;

int test01(){
    mysql_conn conn;
    bool falg = conn.connect("root", "Xiaobei123456", "test_db", "127.0.0.1",3306);
    if(falg){
        cout << "connect success" << endl;
    }

    string sql = "insert into people values(4, 18, 'man', 'starry')";
    bool flag = conn.update(sql);
    cout << "flag value" << flag << endl;

    sql = "select * from people";
    // 查询数据
    sql = "select * from people";
    cout << "***************" << endl;
    conn.query(sql);
    // 遍历数据
    while(conn.next()){
        cout << conn.value(0) << ", "
             << conn.value(1) << ", "
             << conn.value(2) << ", "
             << conn.value(3) << endl;
    }
    return 0;
}

/*
1.单线程：使用/不使用连接池
    
2.多线程：使用/不使用连接池
    
*/ 
void op1(int begin, int end) {
    for (int i = begin; i < end; i++) {
        mysql_conn conn;
        conn.connect("root", "Xiaobei123456", "test_db", "127.0.0.1",3306);
        char sql[1024] = {0};
        sprintf(sql,"insert into people values(%d, 18, 'man', 'starry')", i);
        conn.update(sql);
    }
}

void op2(connection_pool* pool, int begin, int end) {
    for (int i = begin; i < end; i++) {
        shared_ptr<mysql_conn> conn = pool->get_connection();
        char sql[1024] = {0};
        sprintf(sql,"insert into people values(%d, 18, 'man', 'starry')", i);
        conn->update(sql);

    }
}

// 单线程情况
void test02() {
#if 0
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    op1(0, 5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "非连接池，单线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#else
    connection_pool* pool = connection_pool::get_connect_pool();
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    op2(pool, 0, 5000);
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "连接池，单线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#endif
}

// 多线程情况
void test03() {
#if 0
 chrono::steady_clock::time_point begin = chrono::steady_clock::now();
 // 1k次插入操作
    thread t1(op1, 0, 1000);
    thread t2(op1, 1000, 2000);
    thread t3(op1, 2000, 3000);
    thread t4(op1, 3000, 4000);
    thread t5(op1, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "非连接池，多线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#else
    chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    connection_pool* pool = connection_pool::get_connect_pool();
    thread t1(op2, pool, 0, 1000);
    thread t2(op2, pool, 1000, 2000);
    thread t3(op2, pool, 2000, 3000);
    thread t4(op2, pool, 3000, 4000);
    thread t5(op2, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto length = end - begin;
    cout << "连接池，多线程，用时：" << length.count() << "纳秒，" << length.count() / 1000000 << "毫秒" << endl;
#endif 
}


int main(){



    //test01();

    //test02();

    test03();

    return 0;
}