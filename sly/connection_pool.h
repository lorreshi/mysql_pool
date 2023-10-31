#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <json/json.h>
#include <fstream>
#include <thread>
#include <memory>
#include "mysql_conn.h"

class connection_pool{
public:
    static connection_pool* get_connect_pool();

    connection_pool(const connection_pool& object) = delete;

    connection_pool& operator=(const connection_pool& object) = delete;
    // 用户获取连接
    std::shared_ptr<mysql_conn> get_connection();
    // 析构
    ~connection_pool();

private:
    // 私有构造函数，防止创造多个实例对象
    connection_pool();
    // 配置文件
    bool parse_json_file();
    // 添加连接
    void add_connection();
    // 任务函数1
    void produce_connection();
    // 任务函数2
    void recycle_connection();
    // ip地址 用户名 用户密码 数据库库名 端口号 最大连接数 最小连接数超时时长 最大等待市场
    std::string m_ip;
    std::string m_user;
    std::string m_passwd;
    std::string m_db_name;
    unsigned short m_port;
    int m_min_size;
    int m_max_size;
    int m_timeout;
    int m_max_idle_time;
    // 存储连接使用队列
    std::queue<mysql_conn*> m_connectionQ;
    std::mutex m_mutexQ;
    std::condition_variable m_cond;

};