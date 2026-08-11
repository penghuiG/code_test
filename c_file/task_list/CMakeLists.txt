cmake_minimum_required(VERSION 3.10)

# 定义项目名称与使用的语言（这里是 C 语言）
project(MyTaskManager C)

# 设置 C 标准（可选，推荐）
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 查找 pthread 库（多线程支持）
find_package(Threads REQUIRED)

aux_source_directory(src SRC_FILES)
add_executable(my_task_manager ${SRC_FILES})

# 链接 pthread 库
# 注意：find_package(Threads) 会定义 Threads::Threads 目标
target_link_libraries(my_task_manager PRIVATE Threads::Threads)