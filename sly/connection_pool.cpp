#include "connection_pool.h"

// 静态变量创建构造函数 单例模式
connection_pool* connection_pool::get_connect_pool(){
    static connection_pool pool;
    return &pool; 
}

// 用户获取链接
std::shared_ptr<mysql_conn> connection_pool::get_connection(){
    std::unique_lock<std::mutex> locker(m_mutexQ);
    while(m_connectionQ.empty()){
        // 阻塞函数。1. 当到时间的时候解除阻塞 2. 通过唤醒函数
        if(std::cv_status::timeout == m_cond.wait_for(locker, std::chrono::milliseconds(m_timeout))){
            // 双重检查
            if(m_connectionQ.empty()){
                continue;
            }
        }
    }
    // 不为空 返回一个mysql_conn变量
    // 使用智能指针的好处是，在池子中使用使用完的变量智能指针会自动回收，再回首的时候在添加到队列中
    // 从而减少频繁的创建与销毁变量
    std::shared_ptr<mysql_conn> connptr(m_connectionQ.front(), [this](mysql_conn* conn){
        // 锁 避免m_connectionQ同时被多个线程使用
        std::lock_guard<std::mutex> locker(m_mutexQ);
        conn->refresh_alive_time();
        m_connectionQ.push(conn);
    });
    m_connectionQ.pop();
    // 通过条件件变量唤醒生产者
    m_cond.notify_all();

    return connptr;
}

// 配置文件 json解析数据
bool connection_pool::parse_json_file(){
    // md 要用绝对路径 找了一下午
    std::ifstream ifs("/home/ubuntu/code/connection_pool/sly/conf.json");
    Json::Reader rd;
    Json::Value root;
    // 判断能否打开
    if (!ifs.is_open()) {
    std::cerr << "无法打开 JSON 文件。" << std::endl;
    return false;
    }
    // 判断是否解析失败
    if (!rd.parse(ifs, root)) {
    std::cerr << "Failed to parse JSON: " << rd.getFormattedErrorMessages() << std::endl;
    return false;
    }
    if(root.isObject()){
        m_ip = root["ip"].asString();
        m_port = root["port"].asInt();
        m_user = root["user_name"].asString();
        m_passwd = root["password"].asString();
        m_db_name = root["dp_name"].asString();
        m_min_size = root["min_size"].asInt();
        m_max_size = root["max_size"].asInt(); 
        m_max_idle_time = root["max_idel_time"].asInt();
        m_timeout = root["time_out"].asInt();
       
        return true;
    }
    return false;
}

// 添加连接
void connection_pool::add_connection(){
    mysql_conn* conn = new mysql_conn;
    conn->connect(m_user, m_passwd, m_db_name, m_ip, m_port);
    // 记录时间戳
    conn->refresh_alive_time();
    m_connectionQ.push(conn);
}
// 生产任务函数
void connection_pool::produce_connection(){  

    while(true){
        std::unique_lock<std::mutex> locker(m_mutexQ);
        // 连接池内部数量大于最小连接数 不创建新连接
        while(m_connectionQ.size() >= m_min_size){
            m_cond.wait(locker);
        }
        // 创建新连接 
        add_connection();
        // 条件变量调用唤醒函数 最好用两个条件变量，生产者一个，消费者一个
        m_cond.notify_all();
    }
}
// 销毁任务函数
void connection_pool::recycle_connection(){
    while(true){
        // 休眠
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::lock_guard<std::mutex> locker(m_mutexQ);
        while(m_connectionQ.size() > m_min_size){
            // 如果要销毁任务的话一定是队列头部的最长时间没人使用，所以提取出头部
            mysql_conn* conn = m_connectionQ.front();
            // 
            if(conn->get_alive_time() >= m_max_idle_time){
                m_connectionQ.pop();
                delete conn;
            }else{
                break;
            }
        }
    }
}

// 构造函数 生成连接池
connection_pool::connection_pool(){
    // 加载配置文件
    if(!parse_json_file()){
        std::cout << "加载配置失败！" << std::endl;
        return;
    }

        
    // 初始化最小数据库连接
    for(int i=0; i< m_min_size; ++i){
        add_connection();
    }
    // 线程1:生产连接池里面的连接 正常第二个参数指针为NULL 但是我们这个使用单例模式的
    // 静态成员变量创建的，所以加上this指针，表示当前的类
    std::thread producer(&connection_pool::produce_connection, this);
    // 线程2:销毁连接池里面的连接
    std::thread recycler(&connection_pool::recycle_connection, this); 
    // detach（）称为分离线程函数，使用detach()函数会让线程在后台运行，
    // 即说明主线程不会等待子线程运行结束才结束 join()是阻塞的
    producer.detach();
    recycler.detach();
}

// 析构
connection_pool::~connection_pool(){
    while(!m_connectionQ.empty()){
        mysql_conn* conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
}

 