#pragma once
#include <iostream>
#include <chrono>
#include <mysql/mysql.h>

class mysql_conn{
public:
    // 初始化数据库连接
    mysql_conn();
    // 释放数据库连接
    ~mysql_conn();
    // 连接数据库
    bool connect(std::string user, std::string passwd, std::string db_name, std::string ip, unsigned short port = 3306);
    // 更新数据库 insert update delete
    bool update(std::string sql);
    // 查询数据库
    bool query(std::string sql);
    // 遍历查询的结果集
    bool next();
    // 得到结果集中的字段值
    std::string value(int index);
    // 操作事物
    bool transaction();
    // 提交事物
    bool commit();
    // 回滚事物
    bool rollback();
    // 刷新起始的空闲时间
    void refresh_alive_time();
    // 计算连接存活时常
    long long get_alive_time();

private:
    // 释放MYSQL_RES MYSQL_ROW
    void free_result();

    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;
    MYSQL_ROW m_row = nullptr;
    std::chrono::steady_clock::time_point m_alive_time;
};