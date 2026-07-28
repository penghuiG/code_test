/**
 * 单例模式 - 懒汉式 (Lazy Initialization Singleton)
 * 
 * 确保一个类只有一个实例，并提供一个全局访问点来获取该实例。
 * 懒汉式在第一次使用时才创建单例实例，延迟初始化可以节省资源。
 * 
 * 本例以数据库连接池管理器为场景，演示懒汉式单例的核心结构：
 *   - Singleton（单例类）：ConnectionPool，管理数据库连接池
 *     · 在第一次调用 getInstance() 时才创建唯一实例
 *     · 使用 std::call_once / std::mutex 保证线程安全
 *     · 将构造函数、拷贝构造、赋值运算符设为 delete，防止外部创建
 *   - 延迟初始化：只有真正需要时才创建，节省启动时间和内存
 * 
 * 懒汉式特点：
 *   - 优点：延迟加载，节省资源；只在需要时创建实例
 *   - 缺点：需要额外处理线程安全问题（双重检查锁定或 call_once）
 *   - 适用场景：单例对象创建开销大、程序可能不会使用该单例
 * 
 * 本文件演示两种线程安全的懒汉式实现：
 *   方案1：Meyers' Singleton（C++11 静态局部变量，推荐）
 *   方案2：双重检查锁定（Double-Checked Locking，经典方案）
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <sstream>
#include <iomanip>
using namespace std;

// ======================== 模拟数据库连接 ========================
struct DbConnection {
    int id;
    string host;
    int port;
    bool inUse;
    chrono::steady_clock::time_point createTime;
    chrono::steady_clock::time_point lastUsedTime;

    void execute(const string& sql) {
        lastUsedTime = chrono::steady_clock::now();
        cout << "    [连接#" << id << "] 执行: " << sql << endl;
    }

    void printInfo() const {
        auto now = chrono::steady_clock::now();
        auto age = chrono::duration_cast<chrono::seconds>(now - createTime).count();
        cout << "    [连接#" << id << "] " << host << ":" << port
             << " | " << (inUse ? "使用中" : "空闲")
             << " | 存活: " << age << "s" << endl;
    }
};

// ======================== 方案1：Meyers' Singleton（推荐） ========================
// 利用 C++11 静态局部变量的线程安全初始化特性
class ConnectionPool {
public:
    // ===== 获取唯一实例（懒汉式：首次调用时创建） =====
    static ConnectionPool& getInstance() {
        // C++11 标准保证：局部静态变量的初始化是线程安全的
        // 首次调用时构造，后续调用直接返回引用
        static ConnectionPool instance;
        return instance;
    }

    // ===== 禁止拷贝和赋值 =====
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    // ===== 初始化连接池 =====
    void init(const string& host, int port, int poolSize) {
        lock_guard<mutex> lock(mtx_);
        if (initialized_) {
            cout << "    [连接池] 已初始化，跳过重复初始化" << endl;
            return;
        }

        host_ = host;
        port_ = port;
        cout << "    [连接池] 初始化: " << host << ":" << port
             << ", 池大小: " << poolSize << endl;

        for (int i = 0; i < poolSize; i++) {
            auto conn = make_shared<DbConnection>();
            conn->id = i + 1;
            conn->host = host;
            conn->port = port;
            conn->inUse = false;
            conn->createTime = chrono::steady_clock::now();
            conn->lastUsedTime = conn->createTime;
            freeQueue_.push(conn);
            allConns_.push_back(conn);
        }

        initialized_ = true;
        cout << "    [连接池] 创建 " << poolSize << " 个连接完成" << endl;
    }

    // ===== 获取连接 =====
    shared_ptr<DbConnection> acquire() {
        lock_guard<mutex> lock(mtx_);
        if (freeQueue_.empty()) {
            cout << "    [连接池] ⚠ 无可用连接，等待..." << endl;
            return nullptr;
        }

        auto conn = freeQueue_.front();
        freeQueue_.pop();
        conn->inUse = true;
        activeCount_++;
        cout << "    [连接池] 获取连接#" << conn->id
             << " (活跃: " << activeCount_ << ", 空闲: " << freeQueue_.size() << ")" << endl;
        return conn;
    }

    // ===== 归还连接 =====
    void release(shared_ptr<DbConnection> conn) {
        lock_guard<mutex> lock(mtx_);
        if (!conn) return;
        conn->inUse = false;
        activeCount_--;
        freeQueue_.push(conn);
        cout << "    [连接池] 归还连接#" << conn->id
             << " (活跃: " << activeCount_ << ", 空闲: " << freeQueue_.size() << ")" << endl;
    }

    // ===== 状态查询 =====
    void printStatus() const {
        lock_guard<mutex> lock(mtx_);
        cout << "    ┌─────────────────────────────────────────┐" << endl;
        cout << "    │         连接池状态                       │" << endl;
        cout << "    ├─────────────────────────────────────────┤" << endl;
        cout << "    │ 总连接数:   " << allConns_.size() << endl;
        cout << "    │ 活跃连接:   " << activeCount_ << endl;
        cout << "    │ 空闲连接:   " << freeQueue_.size() << endl;
        cout << "    │ 连接地址:   " << host_ << ":" << port_ << endl;
        cout << "    ├─────────────────────────────────────────┤" << endl;
        for (const auto& conn : allConns_) {
            conn->printInfo();
        }
        cout << "    └─────────────────────────────────────────┘" << endl;
    }

    void printInstanceInfo() const {
        cout << "    [单例信息] 实例地址: " << this
             << "，总连接: " << allConns_.size() << endl;
    }

private:
    // ===== 私有构造函数：首次 getInstance() 时调用 =====
    ConnectionPool() : initialized_(false), activeCount_(0) {
        cout << "    [初始化] ConnectionPool 单例实例已创建 (地址: " << this << ")" << endl;
    }

    ~ConnectionPool() {
        cout << "    [销毁] ConnectionPool 单例实例被销毁 (地址: " << this << ")" << endl;
    }

    string host_;
    int port_ = 0;
    bool initialized_;
    int activeCount_;
    queue<shared_ptr<DbConnection>> freeQueue_;
    vector<shared_ptr<DbConnection>> allConns_;
    mutable mutex mtx_;
};

// ======================== 方案2：双重检查锁定（Double-Checked Locking） ========================
// 经典的线程安全懒汉式实现，适用于不能使用局部静态变量的场景
class Logger {
public:
    // ===== 获取唯一实例（双重检查锁定） =====
    static Logger* getInstance() {
        if (instance_ == nullptr) {                  // 第一次检查（无锁，快速路径）
            lock_guard<mutex> lock(mtx_);
            if (instance_ == nullptr) {              // 第二次检查（有锁，确保唯一）
                instance_ = new Logger();
                cout << "    [初始化] Logger 单例实例已创建 (地址: " << instance_ << ")" << endl;
            }
        }
        return instance_;
    }

    // ===== 清理资源（程序退出时调用） =====
    static void destroy() {
        lock_guard<mutex> lock(mtx_);
        if (instance_ != nullptr) {
            cout << "    [销毁] Logger 单例实例被销毁 (地址: " << instance_ << ")" << endl;
            delete instance_;
            instance_ = nullptr;
        }
    }

    // ===== 禁止拷贝和赋值 =====
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ===== 日志操作 =====
    void log(const string& level, const string& module, const string& message) {
        lock_guard<mutex> lock(logMtx_);
        string entry = "[" + level + "] [" + module + "] " + message;
        logs_.push_back(entry);
        cout << "    [LOG] " << entry << endl;
    }

    void info(const string& module, const string& message) {
        log("INFO", module, message);
    }

    void warn(const string& module, const string& message) {
        log("WARN", module, message);
    }

    void error(const string& module, const string& message) {
        log("ERROR", module, message);
    }

    // ===== 查询 =====
    size_t getLogCount() const {
        lock_guard<mutex> lock(logMtx_);
        return logs_.size();
    }

    void printRecentLogs(int count = 5) const {
        lock_guard<mutex> lock(logMtx_);
        cout << "    ┌─────────────────────────────────────────┐" << endl;
        cout << "    │ 最近日志 (共 " << logs_.size() << " 条，显示最近 " << count << " 条) │" << endl;
        cout << "    ├─────────────────────────────────────────┤" << endl;
        int start = max(0, static_cast<int>(logs_.size()) - count);
        for (size_t i = start; i < logs_.size(); i++) {
            cout << "    │ " << logs_[i] << endl;
        }
        cout << "    └─────────────────────────────────────────┘" << endl;
    }

    void printInstanceInfo() const {
        cout << "    [单例信息] 实例地址: " << this
             << "，日志条数: " << logs_.size() << endl;
    }

private:
    // ===== 私有构造函数 =====
    Logger() {
        logs_.reserve(1000);
    }

    ~Logger() {
        cout << "    [销毁] Logger 析构，共 " << logs_.size() << " 条日志" << endl;
    }

    static Logger* instance_;
    static mutex mtx_;

    vector<string> logs_;
    mutable mutex logMtx_;
};

// 静态成员初始化（设为 nullptr，首次 getInstance() 时创建）
Logger* Logger::instance_ = nullptr;
mutex Logger::mtx_;

// ======================== 模拟业务模块 ========================
class UserService {
public:
    void createUser(const string& name) {
        Logger::getInstance()->info("UserService", "创建用户: " + name);
        auto& pool = ConnectionPool::getInstance();
        auto conn = pool.acquire();
        if (conn) {
            conn->execute("INSERT INTO users(name) VALUES('" + name + "')");
            pool.release(conn);
        }
    }
};

class OrderService {
public:
    void createOrder(const string& user, const string& product, double amount) {
        ostringstream oss;
        oss << "创建订单: 用户=" << user << ", 商品=" << product << ", 金额=¥" << fixed << setprecision(2) << amount;
        Logger::getInstance()->info("OrderService", oss.str());
        auto& pool = ConnectionPool::getInstance();
        auto conn = pool.acquire();
        if (conn) {
            ostringstream sql;
            sql << "INSERT INTO orders(user,product,amount) VALUES('" << user << "','" << product << "'," << amount << ")";
            conn->execute(sql.str());
            pool.release(conn);
        }
    }
};

// ======================== 辅助函数 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "  单例模式(懒汉式)演示 - 连接池 & 日志器   " << endl;
    cout << "========================================" << endl;

    // ==========================================
    //  方案1演示：Meyers' Singleton（ConnectionPool）
    // ==========================================
    cout << endl;
    cout << "  ══════════════════════════════════════" << endl;
    cout << "  方案1: Meyers' Singleton (ConnectionPool)" << endl;
    cout << "  ══════════════════════════════════════" << endl;

    // ---------- 1. 懒加载验证 ----------
    cout << endl;
    cout << ">>> 场景一：懒加载 - 首次使用时才创建 <<<" << endl;

    printSection("首次获取连接池实例（此时才创建）");
    auto& pool = ConnectionPool::getInstance();
    pool.printInstanceInfo();
    printSectionEnd();

    printSection("初始化连接池");
    pool.init("192.168.1.100", 3306, 5);
    printSectionEnd();

    // ---------- 2. 唯一性验证 ----------
    cout << endl;
    cout << ">>> 场景二：验证单例唯一性 <<<" << endl;

    printSection("多次获取实例");
    auto& pool2 = ConnectionPool::getInstance();
    auto& pool3 = ConnectionPool::getInstance();
    cout << "    pool1 地址: " << &pool << endl;
    cout << "    pool2 地址: " << &pool2 << endl;
    cout << "    pool3 地址: " << &pool3 << endl;
    cout << "    是否同一实例: "
         << ((&pool == &pool2 && &pool2 == &pool3) ? "✓ 是" : "✗ 否") << endl;
    printSectionEnd();

    // ---------- 3. 连接池使用 ----------
    cout << endl;
    cout << ">>> 场景三：使用连接池执行业务 <<<" << endl;

    printSection("业务操作");
    UserService userSvc;
    OrderService orderSvc;

    userSvc.createUser("张三");
    userSvc.createUser("李四");
    orderSvc.createOrder("张三", "MacBook Pro", 14999.00);
    orderSvc.createOrder("李四", "iPhone 16", 7999.00);
    printSectionEnd();

    printSection("连接池状态");
    pool.printStatus();
    printSectionEnd();

    // ==========================================
    //  方案2演示：双重检查锁定（Logger）
    // ==========================================
    cout << endl;
    cout << "  ══════════════════════════════════════" << endl;
    cout << "  方案2: 双重检查锁定 (Logger)" << endl;
    cout << "  ══════════════════════════════════════" << endl;

    // ---------- 4. 懒加载验证 ----------
    cout << endl;
    cout << ">>> 场景四：Logger 懒加载 <<<" << endl;

    printSection("首次获取 Logger 实例");
    Logger* logger1 = Logger::getInstance();
    logger1->printInstanceInfo();
    printSectionEnd();

    // ---------- 5. 唯一性验证 ----------
    cout << endl;
    cout << ">>> 场景五：Logger 单例唯一性 <<<" << endl;

    printSection("多次获取实例");
    Logger* logger2 = Logger::getInstance();
    Logger* logger3 = Logger::getInstance();
    cout << "    logger1 地址: " << logger1 << endl;
    cout << "    logger2 地址: " << logger2 << endl;
    cout << "    logger3 地址: " << logger3 << endl;
    cout << "    是否同一实例: "
         << ((logger1 == logger2 && logger2 == logger3) ? "✓ 是" : "✗ 否") << endl;
    printSectionEnd();

    // ---------- 6. 日志记录 ----------
    cout << endl;
    cout << ">>> 场景六：多模块共享 Logger <<<" << endl;

    printSection("各模块记录日志");
    logger1->info("System", "系统启动完成");
    logger1->info("Server", "HTTP服务器监听端口 8080");
    logger1->warn("Cache", "Redis连接延迟较高: 150ms");
    logger1->info("DB", "数据库连接池初始化完成，5个连接");
    logger1->info("UserService", "用户 张三 注册成功");
    logger1->info("OrderService", "订单 ORD-001 创建成功");
    logger1->error("Payment", "支付网关超时，订单 ORD-002 支付失败");
    logger1->info("System", "健康检查通过");
    printSectionEnd();

    printSection("日志统计");
    cout << "    总日志条数: " << logger1->getLogCount() << endl;
    printSectionEnd();

    printSection("最近日志");
    logger1->printRecentLogs(6);
    printSectionEnd();

    // ---------- 7. 线程安全演示 ----------
    cout << endl;
    cout << ">>> 场景七：多线程并发获取单例 <<<" << endl;

    printSection("启动多线程获取连接");
    vector<thread> threads;
    atomic<int> successCount(0);

    for (int i = 0; i < 3; i++) {
        threads.emplace_back([&successCount, i]() {
            auto& p = ConnectionPool::getInstance();
            auto conn = p.acquire();
            if (conn) {
                ostringstream oss;
                oss << "线程" << i << " 使用连接#" << conn->id;
                Logger::getInstance()->info("Thread", oss.str());
                this_thread::sleep_for(chrono::milliseconds(50));
                p.release(conn);
                successCount++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    cout << "    多线程执行完成: " << successCount.load() << "/3 成功" << endl;
    printSectionEnd();

    printSection("最终连接池状态");
    pool.printStatus();
    printSectionEnd();

    printSection("最终日志统计");
    logger1->printRecentLogs(5);
    printSectionEnd();

    // ---------- 8. 释放 Logger ----------
    cout << endl;
    cout << ">>> 场景八：手动释放 Logger 资源 <<<" << endl;
    printSection("销毁 Logger");
    Logger::destroy();
    printSectionEnd();

    // ---------- 展示懒汉式特点 ----------
    cout << endl;
    cout << ">>> 懒汉式单例优势总结 <<<" << endl;
    cout << "  ├─ 延迟加载：首次使用时才创建实例，节省启动时间和内存" << endl;
    cout << "  ├─ 线程安全方案1：Meyers' Singleton（推荐，C++11局部静态变量保证）" << endl;
    cout << "  ├─ 线程安全方案2：双重检查锁定（经典，适用于非局部静态场景）" << endl;
    cout << "  ├─ 按需创建：程序可能不需要该单例时不会浪费资源" << endl;
    cout << "  ├─ 全局访问：任何模块通过 getInstance() 获取同一实例" << endl;
    cout << "  ├─ 适用场景：连接池、日志器、缓存管理器等创建开销大的组件" << endl;
    cout << "  └─ 注意事项：双重检查锁定需使用 atomic/mutex 防止指令重排" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}