# C++ 学习路线图

> 从零基础到高级开发，循序渐进掌握 C++

---

## 第一阶段：C 语言基础 (2-4 周)

> C++ 是 C 的超集，扎实的 C 基础是学好 C++ 的前提

### 1.1 基本语法
- [ ] 数据类型: `int`, `char`, `float`, `double`, `long`, `short`, `unsigned`
- [ ] 变量与常量: `const`, `#define`, `enum`
- [ ] 运算符: 算术、关系、逻辑、位运算、三目运算符
- [ ] 流程控制: `if/else`, `switch`, `for`, `while`, `do-while`, `break`, `continue`
- [ ] 函数: 定义、声明、参数传递（值传递 vs 指针传递）、递归

### 1.2 指针与内存
- [ ] 指针基础: 指针定义、解引用、指针运算
- [ ] 数组与指针的关系
- [ ] 字符串: `char[]`, `char*`, 字符串函数 (`strlen`, `strcpy`, `strcmp` 等)
- [ ] 动态内存: `malloc`, `calloc`, `realloc`, `free`
- [ ] 指针进阶: 函数指针、回调函数、多级指针

### 1.3 结构体与文件
- [ ] 结构体: 定义、嵌套、结构体指针
- [ ] 联合体与枚举
- [ ] 文件操作: `fopen`, `fread`, `fwrite`, `fclose`
- [ ] 预处理器: `#include`, `#define`, `#ifdef`, 宏函数

### 📚 推荐资源
- 《C Primer Plus》
- 《C程序设计语言》(K&R)

---

## 第二阶段：C++ 基础入门 (3-4 周)

> 从 C 过渡到 C++，掌握面向对象核心概念

### 2.1 C++ 新特性入门
- [ ] `namespace` 命名空间
- [ ] `bool` 类型、`string` 类
- [ ] 引用 `&` (左值引用)
- [ ] 函数重载与默认参数
- [ ] `cin` / `cout` 输入输出流
- [ ] `new` / `delete` 动态内存管理

### 2.2 面向对象基础
- [ ] 类与对象: 成员变量、成员函数、访问权限 (`public`, `private`, `protected`)
- [ ] 构造函数与析构函数
- [ ] `this` 指针
- [ ] 拷贝构造函数与赋值运算符
- [ ] 初始化列表
- [ ] `static` 成员（静态成员变量与静态成员函数）
- [ ] `const` 成员函数、`mutable` 关键字
- [ ] 友元函数与友元类

### 2.3 继承与多态
- [ ] 继承: 公有继承、保护继承、私有继承
- [ ] 多重继承与菱形继承、虚继承
- [ ] 虚函数与多态 (`virtual`)
- [ ] 纯虚函数与抽象类
- [ ] 虚析构函数（为什么基类需要虚析构）
- [ ] 运行时类型识别: `dynamic_cast`, `typeid`

### 📚 推荐资源
- 《C++ Primer》(第5版) — 最经典的 C++ 入门书
- 《C++ Primer Plus》

---

## 第三阶段：C++ 核心进阶 (4-6 周)

> 深入理解 C++ 内存模型与高级特性

### 3.1 运算符重载
- [ ] 重载算术运算符: `+`, `-`, `*`, `/`
- [ ] 重载比较运算符: `==`, `!=`, `<`, `>`
- [ ] 重载输入输出: `<<`, `>>`
- [ ] 重载下标运算符: `[]`
- [ ] 重载函数调用运算符: `operator()`

### 3.2 模板
- [ ] 函数模板
- [ ] 类模板
- [ ] 模板特化（全特化、偏特化）
- [ ] 非类型模板参数
- [ ] 可变参数模板 (variadic templates)
- [ ] SFINAE 基础概念

### 3.3 异常处理
- [ ] `try`, `catch`, `throw`
- [ ] 标准异常类: `std::exception`, `std::runtime_error`
- [ ] 自定义异常类
- [ ] `noexcept` 说明符
- [ ] RAII 与异常安全

### 3.4 文件与流
- [ ] 文件流: `ifstream`, `ofstream`, `fstream`
- [ ] 字符串流: `stringstream`, `ostringstream`
- [ ] 流的格式化控制

### 📚 推荐资源
- 《Effective C++》— 55 条 C++ 编程建议
- 《More Effective C++》

---

## 第四阶段：标准模板库 STL (3-4 周)

> STL 是 C++ 最强大的工具库，必须熟练掌握

### 4.1 序列容器
- [ ] `std::vector` — 动态数组（最常用）
- [ ] `std::list` — 双向链表
- [ ] `std::deque` — 双端队列
- [ ] `std::array` — 固定大小数组
- [ ] `std::forward_list` — 单向链表

### 4.2 关联容器
- [ ] `std::set` / `std::multiset` — 有序集合
- [ ] `std::map` / `std::multimap` — 有序映射
- [ ] `std::unordered_set` / `std::unordered_map` — 哈希容器
- [ ] 容器选择策略

### 4.3 迭代器与算法
- [ ] 迭代器类型: 输入、输出、前向、双向、随机访问
- [ ] 迭代器适配器: `reverse_iterator`, `back_insert_iterator`
- [ ] 常用算法: `sort`, `find`, `count`, `for_each`, `transform`, `accumulate`
- [ ] `lambda` 表达式
- [ ] 函数对象 (functor)

### 4.4 其他组件
- [ ] `std::pair` 与 `std::tuple`
- [ ] `std::optional` (C++17)
- [ ] `std::variant` (C++17)
- [ ] `std::any` (C++17)

### 📚 推荐资源
- 《STL源码剖析》(侯捷)
- cppreference.com — 在线权威参考

---

## 第五阶段：现代 C++ (C++11/14/17/20) (4-6 周)

> 现代 C++ 改变了编程范式，必须跟上

### 5.1 C++11 核心特性 ⭐
- [ ] `auto` 类型推导
- [ ] `decltype` 类型推导
- [ ] 右值引用 `&&` 与移动语义
- [ ] 完美转发 `std::forward`
- [ ] `std::move`
- [ ] 智能指针: `unique_ptr`, `shared_ptr`, `weak_ptr`
- [ ] `nullptr` 替代 `NULL`
- [ ] 范围 `for` 循环
- [ ] 初始化列表 `{}` 与 `std::initializer_list`
- [ ] `lambda` 表达式详解
- [ ] `enum class` 强类型枚举
- [ ] `constexpr` 编译期常量
- [ ] `std::thread` 多线程基础

### 5.2 C++14 改进
- [ ] 泛型 lambda
- [ ] `auto` 返回类型推导
- [ ] `std::make_unique`

### 5.3 C++17 重要特性
- [ ] 结构化绑定 `auto [a, b] = ...`
- [ ] `if constexpr` 编译期分支
- [ ] `std::filesystem` 文件系统库
- [ ] `std::optional`, `std::variant`, `std::any`
- [ ] `string_view` 字符串视图
- [ ] 并行算法 (parallel algorithms)

### 5.4 C++20 新特性 (了解)
- [ ] `concept` 概念
- [ ] `ranges` 库
- [ ] 协程 `co_await` / `co_yield`
- [ ] `consteval`, `constinit`
- [ ] `std::format` 格式化库
- [ ] 三路比较运算符 `<=>`

### 📚 推荐资源
- 《Effective Modern C++》— Scott Meyers
- 《C++ Primer》第5版 + 在线补充
- CppCon 演讲视频 (YouTube)

---

## 第六阶段：并发与多线程编程 (3-4 周)

> 系统级编程必备技能

### 6.1 线程基础
- [ ] `std::thread` 创建与管理
- [ ] 线程生命周期与 `join()` / `detach()`
- [ ] 线程局部存储 `thread_local`

### 6.2 同步机制
- [ ] `std::mutex` 互斥锁
- [ ] `std::lock_guard` / `std::unique_guard` RAII 锁
- [ ] `std::shared_mutex` 读写锁 (C++17)
- [ ] `std::condition_variable` 条件变量
- [ ] `std::atomic` 原子操作
- [ ] 死锁分析与避免

### 6.3 高级并发
- [ ] `std::future` / `std::promise` / `std::async`
- [ ] `std::packaged_task`
- [ ] 线程池实现
- [ ] 生产者-消费者模型
- [ ] 无锁编程基础 (lock-free)

### 📚 推荐资源
- 《C++ Concurrency in Action》— Anthony Williams
- 《Linux多线程服务端编程》(陈硕)

---

## 第七阶段：设计模式与软件工程 (3-4 周)

> 写出可维护、可扩展的代码

### 7.1 创建型模式
- [ ] 单例模式 (Singleton)
- [ ] 工厂模式 (Factory / Abstract Factory)
- [ ] 建造者模式 (Builder)
- [ ] 原型模式 (Prototype)

### 7.2 结构型模式
- [ ] 适配器模式 (Adapter)
- [ ] 装饰器模式 (Decorator)
- [ ] 外观模式 (Facade)
- [ ] 代理模式 (Proxy)
- [ ] 组合模式 (Composite)

### 7.3 行为型模式
- [ ] 观察者模式 (Observer)
- [ ] 策略模式 (Strategy)
- [ ] 命令模式 (Command)
- [ ] 状态模式 (State)
- [ ] 模板方法模式 (Template Method)

### 7.4 现代 C++ 设计
- [ ] RAII 模式
- [ ] CRTP (奇异递归模板模式)
- [ ] Type Erasure (类型擦除)
- [ ] Policy-based Design

### 📚 推荐资源
- 《设计模式》(GoF)
- 《大话设计模式》
- 《Modern C++ Design》(Andrei Alexandrescu)

---

## 第八阶段：系统编程与网络编程 (4-6 周)

> Linux 环境下 C++ 系统级开发

### 8.1 Linux 系统编程
- [ ] 进程管理: `fork`, `exec`, `wait`, 信号处理
- [ ] 文件 I/O: `open`, `read`, `write`, `close`, 文件描述符
- [ ] 内存映射: `mmap`
- [ ] 进程间通信: 管道、共享内存、消息队列、信号量

### 8.2 网络编程
- [ ] Socket 编程: TCP/UDP
- [ ] 多路复用: `select`, `poll`, `epoll`
- [ ] Reactor 模式
- [ ] Proactor 模式
- [ ] 常见网络协议实现 (HTTP, 自定义协议)

### 8.3 常用框架与库
- [ ] Boost 库 (智能指针、Asio 网络库、日志等)
- [ ] libevent / libev 事件驱动库
- [ ] gRPC / Protobuf 序列化与RPC
- [ ] spdlog 高性能日志库
- [ ] Google Test 单元测试框架

### 📚 推荐资源
- 《Unix环境高级编程》(APUE)
- 《Unix网络编程》(UNP)
- 《Linux高性能服务器编程》(游双)

---

## 第九阶段：性能优化与底层深入 (持续学习)

> 成为 C++ 高手的必经之路

### 9.1 性能优化
- [ ] 编译器优化选项: `-O2`, `-O3`, LTO
- [ ] 内存对齐与缓存友好 (cache-friendly)
- [ ] 避免不必要的拷贝 (移动语义)
- [ ] `constexpr` 编译期计算
- [ ] Profile 工具: `gprof`, `perf`, `valgrind`, `AddressSanitizer`
- [ ] Benchmark 测试方法

### 9.2 内存管理深入
- [ ] C++ 内存模型: 栈、堆、全局/静态区、常量区、代码区
- [ ] 内存泄漏检测: `valgrind`, `AddressSanitizer`
- [ ] 自定义内存分配器 (allocator)
- [ ] 内存池实现
- [ ] 智能指针实现原理

### 9.3 编译与构建
- [ ] GCC / Clang 编译选项详解
- [ ] CMake 构建系统 (必须掌握)
- [ ] 交叉编译基础
- [ ] 动态库与静态库: `.so`, `.a`, `.dll`, `.lib`
- [ ] ABI 兼容性

### 9.4 底层原理
- [ ] C++ 对象模型: 虚函数表 (vtable) 原理
- [ ] 模板实例化机制
- [ ] 编译链接过程: 预处理 → 编译 → 汇编 → 链接
- [ ] Name Mangling 与 `extern "C"`
- [ ] 汇编基础 (x86-64)

### 📚 推荐资源
- 《深度探索C++对象模型》(Stanley Lippman)
- 《C++ Templates: The Complete Guide》
- 《程序员的自我修养》— 俞甲子

---

## 学习建议

### 🎯 学习原则
1. **多写代码** — 每个知识点都要动手实践，光看不练等于没学
2. **造轮子** — 自己实现 `string`, `vector`, `shared_ptr`, 线程池等
3. **读源码** — STL 源码、Boost 源码、优秀开源项目
4. **做项目** — 每个阶段至少完成一个小项目

### 📝 推荐练手项目

| 阶段 | 项目 | 技能点 |
|------|------|--------|
| 入门期 | 学生管理系统 | 类、继承、文件IO |
| 基础期 | 简易计算器 | 运算符重载、模板 |
| 进阶期 | JSON 解析器 | 递归、模板、异常处理 |
| STL期 | 文本搜索引擎 | STL容器、算法、文件操作 |
| 并发期 | 多线程下载器 | 线程、锁、网络编程 |
| 系统期 | HTTP 服务器 | Socket、epoll、Reactor |
| 高级期 | 内存池/线程池 | 内存管理、模板、设计模式 |

### 🔗 在线资源
- **cppreference.com** — C++ 标准参考手册 (必收藏)
- **Compiler Explorer (godbolt.org)** — 在线查看汇编代码
- **C++ Core Guidelines** — C++ 官方编码指南
- **CppCon (YouTube)** — C++ 年度大会演讲
- **LeetCode** — 算法练习 (建议用 C++ 刷题)

### ⏰ 时间规划 (约 6-12 个月)

```
第 1-2 月:  C 语言基础 + C++ 基础入门
第 3-4 月:  C++ 核心进阶 + STL
第 4-5 月:  现代 C++ (C++11/14/17)
第 5-6 月:  并发编程 + 设计模式
第 6-8 月:  系统编程 + 网络编程
第 8-12 月: 性能优化 + 底层深入 + 项目实战
         (持续学习，没有终点)
```

---

> **记住**: C++ 是一门需要长期积累的语言。不要急于求成，扎实基础比什么都重要。
> 编程能力 = 基础知识 × 刻意练习 × 项目经验