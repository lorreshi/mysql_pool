# 一个MySQL的连接池在服务器中的性能测试demo 在master分支中
## 1. 测试环境
<img width="477" alt="截屏2023-11-02 13 44 49" src="https://github.com/lorreshi/mysql_pool/assets/131593525/97d265b9-4815-4f75-8455-bc3baa000ccc">

## 2. 测试所需要的第三方软件包
1. jsoncpp
   使用教程如下：<https://blog.csdn.net/qq_38104450/article/details/134152721>
2. mysql
   ```bash
   sudo apt-get install libmysql++-dev

  头文件目录：/usr/include/mysql
  
  依赖库：/usr/lib/x86_64-linux-gnu/libmysqlclient.a 以及 /usr/lib/x86_64-linux-gnu/libmysqlclient.so

  ## 3. demo测试结果
  <img width="468" alt="截屏2023-11-02 13 30 02" src="https://github.com/lorreshi/mysql_pool/assets/131593525/a14cb794-f2e3-4408-90de-0ca8e11d3921">
  <img width="472" alt="截屏2023-11-02 13 31 21" src="https://github.com/lorreshi/mysql_pool/assets/131593525/51632594-e60a-4371-8fe6-214c840e5d78">
  <img width="538" alt="截屏2023-11-02 13 34 15" src="https://github.com/lorreshi/mysql_pool/assets/131593525/c9aeba82-660b-4d56-864e-f4fd9d4dc3e5">
  <img width="505" alt="截屏2023-11-02 13 35 23" src="https://github.com/lorreshi/mysql_pool/assets/131593525/2e6392ce-cbe0-44f7-a2eb-f8c842866bda">

## 4. Mysql中的表格创建
1. 进入mysql
   mysql -u(你创建是的mysql用户名字) -p（你的密码）
2. 创建一个test_db的表名，并且运行
   CREATE DATABASE test_db;
   USE test_db;
3. 创建表格
```
-- 创建表格
CREATE TABLE people (
    id INT AUTO_INCREMENT PRIMARY KEY,
    age INT,
    sex VARCHAR(10),
    name VARCHAR(50)
);

-- 插入数据
INSERT INTO people (age, sex, name) VALUES (20, 'woman', 'nami');
INSERT INTO people (age, sex, name) VALUES (21, 'man', 'luffy');
INSERT INTO people (age, sex, name) VALUES (22, 'man', 'sabo');
```
## 5. 程序运行
test01是一个测试能否链接mysql的demo，查看封装的是否好用
test02是单线程情况下的使用与不使用线程池的demo
test03是多线程情况下的使用与不使用线程池的demo
运行步骤入下：
```
先把程序中的build删除了，然后执行下面步骤：
mkdir build
cd build
cmake ..
make
./mysql_pool
```
