#include "mysql_conn.h"

// 初始化数据库连接
mysql_conn::mysql_conn(){
    // 返回一个MYSQL指针，非空代表创建成功
    m_conn = mysql_init(nullptr);
    // 设置中文编码结构
    mysql_set_character_set(m_conn, "utf8");
}

// 释放数据库连接
mysql_conn::~mysql_conn(){
    if(m_conn != nullptr){
        // 释放掉m_conn的内存
        mysql_close(m_conn);
    }
    free_result();
}

// 连接数据库
    bool mysql_conn::connect(std::string user, std::string passwd, std::string db_name,
                 std::string ip, unsigned short port){
                    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), port, nullptr, 0);
                    
                    return ptr != nullptr;
                 }

// 更新数据库 insert update delete
    bool mysql_conn::update(std::string sql){
        // 执行sql语句的命令 mysql_query
        if(mysql_query(m_conn, sql.c_str())){
            return false;
        }
        return true;
    }

// 查询数据库
    bool mysql_conn::query(std::string sql){
        // 每次查询需要清空上一次的内存
        free_result();
        if(mysql_query(m_conn, sql.c_str())){
            return false;
        }
        // 执行查询的时候会返回一个结果数据集 MYSQL_RES类型的，我们把它存在m_reslute中
        // 也就是说在MYSQL服务器查询到结果后会把这个结果集返回个客户端
        m_result = mysql_store_result(m_conn);
        return true;
    }

// 遍历查询的结果集
    bool mysql_conn::next(){
        if(m_result != nullptr){
            m_row = mysql_fetch_row(m_result);
            if(m_row != nullptr){
                return true;
            }
        }
        return false;
    }

// 得到结果集中的字段值
    std::string mysql_conn::value(int index){
        int row_count = mysql_num_fields(m_result);
        if(index >= row_count || index < 0){
            return std::string();
        }
        char* val = m_row[index];
        // 获取字段值长度
        unsigned long length = mysql_fetch_lengths(m_result)[index];
        return std::string(val, length);
    }

// 操作事物
    bool mysql_conn::transaction(){
        return mysql_autocommit(m_conn, false);
    }

// 提交事物
    bool mysql_conn::commit(){
        return mysql_commit(m_conn);
    }

// 回滚事物
    bool mysql_conn::rollback(){
        return mysql_rollback(m_conn);
    }

// 释放MYSQL_RES MYSQL_ROW
    void mysql_conn::free_result(){
        if(m_result){
            mysql_free_result(m_result);
            m_result = nullptr;
        }
    }

// 刷新起始的空闲时间
    void mysql_conn::refresh_alive_time(){
        // 获得系统当前的时间
        m_alive_time = std::chrono::steady_clock::now();
    }
// 计算连接存活时常
    long long mysql_conn::get_alive_time(){
        // 装换成纳秒单位 --- 存活时间
        std::chrono::nanoseconds res = std::chrono::steady_clock::now() - m_alive_time;
        // 转换为毫秒
        std::chrono::milliseconds millsec = std::chrono::duration_cast<std::chrono::milliseconds>(res);
        // 返回多少个毫秒
        return millsec.count();
    }