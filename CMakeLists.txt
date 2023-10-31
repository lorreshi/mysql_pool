cmake_minimum_required(VERSION 3.0.0)
project(server_uring VERSION 0.1.0)

set (CMAKE_C_COMPILER "/usr/bin/gcc")
set (CMAKE_CXX_COMPILER "/usr/bin/g++")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Ofast -std=c++11")

include_directories(/home/ubuntu/jsoncpp-1.9.5/build/lib/)
link_directories(/home/ubuntu/jsoncpp-1.9.5/build/lib/)

find_library(JSON_CPP libjsoncpp.a)


include_directories(.)
set(LIB_SRC sly/connection_pool.cpp sly/mysql_conn.cpp)

add_library(sly SHARED ${LIB_SRC})

set(LIB_LIB
            sly
            ${JSON_CPP}
            )

add_executable(mysql_pool main.cpp)

add_dependencies(mysql_pool sly)


target_link_libraries(mysql_pool ${LIB_LIB} -lmysqlclient)
