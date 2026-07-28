/**
 * 单例模式 - 饿汉式 (Eager Initialization Singleton)
 * 
 * 确保一个类只有一个实例，并提供一个全局访问点来获取该实例。
 * 饿汉式在类加载时就立即创建单例实例，利用静态成员变量的初始化顺序
 * 保证线程安全，无需额外的同步机制。
 * 
 * 本例以应用全局配置管理器为场景，演示饿汉式单例的核心结构：
 *   - Singleton（单例类）：AppConfig，管理应用程序的全局配置
 *     · 在程序启动时（静态成员初始化阶段）即创建唯一实例
 *     · 提供静态方法 getInstance() 返回该唯一实例
 *     · 将构造函数、拷贝构造、赋值运算符设为 delete，防止外部创建
 *   - 使用静态成员变量在程序启动时初始化，天然线程安全
 * 
 * 饿汉式特点：
 *   - 优点：实现简单，线程安全（静态变量初始化由编译器保证）
 *   - 缺点：无论是否使用，程序启动时就创建实例，可能浪费资源
 *   - 适用场景：单例对象较小、创建开销低、程序运行期间一定会使用
 */

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iomanip>
#include <mutex>
using namespace std;

// ======================== 饿汉式单例：应用配置管理器 ========================
class AppConfig {
public:
    // ===== 获取唯一实例（饿汉式：实例在程序启动时已创建） =====
    static AppConfig& getInstance() {
        // 静态局部变量在首次调用前已由编译器初始化
        // C++11 保证局部静态变量的初始化是线程安全的
        static AppConfig instance;
        return instance;
    }

    // ===== 禁止拷贝和赋值 =====
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;

    // ===== 配置读写操作 =====
    void setConfig(const string& key, const string& value) {
        lock_guard<mutex> lock(mtx_);
        configMap_[key] = value;
        cout << "    [配置] 设置: " << key << " = " << value << endl;
    }

    string getConfig(const string& key, const string& defaultValue = "") const {
        lock_guard<mutex> lock(mtx_);
        auto it = configMap_.find(key);
        if (it != configMap_.end()) {
            return it->second;
        }
        return defaultValue;
    }

    bool hasConfig(const string& key) const {
        lock_guard<mutex> lock(mtx_);
        return configMap_.find(key) != configMap_.end();
    }

    void removeConfig(const string& key) {
        lock_guard<mutex> lock(mtx_);
        auto it = configMap_.find(key);
        if (it != configMap_.end()) {
            cout << "    [配置] 删除: " << key << " (原值: " << it->second << ")" << endl;
            configMap_.erase(it);
        }
    }

    // ===== 配置分组管理 =====
    void setConfigGroup(const string& group, const map<string, string>& configs) {
        lock_guard<mutex> lock(mtx_);
        for (const auto& pair : configs) {
            string fullKey = group + "." + pair.first;
            configMap_[fullKey] = pair.second;
            cout << "    [配置组] " << group << "." << pair.first << " = " << pair.second << endl;
        }
    }

    vector<string> getKeysByPrefix(const string& prefix) const {
        lock_guard<mutex> lock(mtx_);
        vector<string> keys;
        for (const auto& pair : configMap_) {
            if (pair.first.find(prefix) == 0) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }

    // ===== 工具方法 =====
    void printAll() const {
        lock_guard<mutex> lock(mtx_);
        cout << "    ┌─────────────────────────────────────────┐" << endl;
        cout << "    │         全部配置 (" << configMap_.size() << " 项)              │" << endl;
        cout << "    ├─────────────────────────────────────────┤" << endl;
        for (const auto& pair : configMap_) {
            cout << "    │ " << setw(28) << left << pair.first << " = " << pair.second << endl;
        }
        cout << "    └─────────────────────────────────────────┘" << endl;
    }

    size_t size() const {
        lock_guard<mutex> lock(mtx_);
        return configMap_.size();
    }

    void clear() {
        lock_guard<mutex> lock(mtx_);
        configMap_.clear();
        cout << "    [配置] 已清空所有配置" << endl;
    }

    // ===== 获取实例的内存地址（用于验证单例） =====
    void printInstanceInfo() const {
        cout << "    [单例信息] 实例地址: " << this
             << "，已加载 " << configMap_.size() << " 项配置" << endl;
    }

private:
    // ===== 私有构造函数：程序启动时由静态初始化调用 =====
    AppConfig() {
        cout << "    [初始化] AppConfig 单例实例已创建 (地址: " << this << ")" << endl;
        // 预加载默认配置
        configMap_["app.name"] = "GPH_CPP_SERVER";
        configMap_["app.version"] = "1.0.0";
        configMap_["app.env"] = "production";
        configMap_["log.level"] = "INFO";
        configMap_["log.path"] = "/var/log/gph_server/";
        configMap_["server.port"] = "8080";
        configMap_["server.max_connections"] = "1024";
        configMap_["db.host"] = "127.0.0.1";
        configMap_["db.port"] = "3306";
        configMap_["db.name"] = "gph_db";
        cout << "    [初始化] 预加载 " << configMap_.size() << " 项默认配置" << endl;
    }

    ~AppConfig() {
        cout << "    [销毁] AppConfig 单例实例被销毁 (地址: " << this << ")" << endl;
    }

    map<string, string> configMap_;
    mutable mutex mtx_;  // mutable 允许在 const 方法中加锁
};

// ======================== 模拟使用配置的业务模块 ========================
class DatabaseManager {
public:
    void connect() {
        auto& config = AppConfig::getInstance();
        string host = config.getConfig("db.host", "localhost");
        string port = config.getConfig("db.port", "3306");
        string dbName = config.getConfig("db.name", "test");
        cout << "    [DB] 连接数据库 " << host << ":" << port << "/" << dbName << endl;
    }

    void printConfigSource() {
        cout << "    [DB] 配置来源地址: ";
        AppConfig::getInstance().printInstanceInfo();
    }
};

class LogManager {
public:
    void init() {
        auto& config = AppConfig::getInstance();
        string level = config.getConfig("log.level", "DEBUG");
        string path = config.getConfig("log.path", "/tmp/");
        cout << "    [LOG] 初始化日志: 级别=" << level << ", 路径=" << path << endl;
    }

    void printConfigSource() {
        cout << "    [LOG] 配置来源地址: ";
        AppConfig::getInstance().printInstanceInfo();
    }
};

class ServerManager {
public:
    void start() {
        auto& config = AppConfig::getInstance();
        string port = config.getConfig("server.port", "8080");
        string maxConn = config.getConfig("server.max_connections", "512");
        cout << "    [SERVER] 启动服务器: 端口=" << port << ", 最大连接=" << maxConn << endl;
    }

    void printConfigSource() {
        cout << "    [SERVER] 配置来源地址: ";
        AppConfig::getInstance().printInstanceInfo();
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
    cout << "  单例模式(饿汉式)演示 - 配置管理器      " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 验证单例唯一性 ----------
    cout << endl;
    cout << ">>> 场景一：验证单例唯一性 <<<" << endl;

    printSection("获取单例实例");
    AppConfig& config1 = AppConfig::getInstance();
    AppConfig& config2 = AppConfig::getInstance();
    AppConfig& config3 = AppConfig::getInstance();
    printSectionEnd();

    printSection("地址比较");
    cout << "    config1 地址: " << &config1 << endl;
    cout << "    config2 地址: " << &config2 << endl;
    cout << "    config3 地址: " << &config3 << endl;
    cout << "    三个引用是否指向同一实例: "
         << ((&config1 == &config2 && &config2 == &config3) ? "✓ 是" : "✗ 否") << endl;
    printSectionEnd();

    // ---------- 2. 读取默认配置 ----------
    cout << endl;
    cout << ">>> 场景二：读取预加载的默认配置 <<<" << endl;

    printSection("查看默认配置");
    config1.printAll();
    printSectionEnd();

    // ---------- 3. 修改和读取配置 ----------
    cout << endl;
    cout << ">>> 场景三：运行时修改配置 <<<" << endl;

    printSection("修改配置");
    config1.setConfig("server.port", "9090");
    config1.setConfig("app.env", "development");
    config1.setConfig("cache.enabled", "true");
    config1.setConfig("cache.ttl", "3600");
    printSectionEnd();

    printSection("通过其他引用验证修改");
    cout << "    通过 config2 读取 server.port: " << config2.getConfig("server.port") << endl;
    cout << "    通过 config3 读取 app.env: " << config3.getConfig("app.env") << endl;
    cout << "    通过 config2 读取 cache.enabled: " << config2.getConfig("cache.enabled") << endl;
    printSectionEnd();

    // ---------- 4. 分组配置管理 ----------
    cout << endl;
    cout << ">>> 场景四：分组配置管理 <<<" << endl;

    printSection("设置邮件配置组");
    config1.setConfigGroup("email", {
        {"smtp_host", "smtp.example.com"},
        {"smtp_port", "465"},
        {"from_addr", "noreply@example.com"},
        {"use_ssl", "true"}
    });
    printSectionEnd();

    printSection("设置Redis配置组");
    config1.setConfigGroup("redis", {
        {"host", "192.168.1.200"},
        {"port", "6379"},
        {"password", "****"},
        {"db_index", "0"}
    });
    printSectionEnd();

    printSection("按前缀查询配置");
    vector<string> emailKeys = config1.getKeysByPrefix("email.");
    cout << "    email. 前缀的配置项 (" << emailKeys.size() << " 个):" << endl;
    for (const auto& key : emailKeys) {
        cout << "      " << key << " = " << config1.getConfig(key) << endl;
    }
    printSectionEnd();

    // ---------- 5. 多模块共享单例 ----------
    cout << endl;
    cout << ">>> 场景五：多模块共享同一配置单例 <<<" << endl;

    printSection("各模块初始化");
    DatabaseManager dbMgr;
    LogManager logMgr;
    ServerManager srvMgr;

    dbMgr.connect();
    logMgr.init();
    srvMgr.start();
    printSectionEnd();

    printSection("验证配置来源一致性");
    dbMgr.printConfigSource();
    logMgr.printConfigSource();
    srvMgr.printConfigSource();
    cout << "    结论: 所有模块使用同一配置实例 ✓" << endl;
    printSectionEnd();

    // ---------- 6. 删除和清空配置 ----------
    cout << endl;
    cout << ">>> 场景六：配置删除操作 <<<" << endl;

    printSection("删除单个配置");
    config1.removeConfig("cache.ttl");
    cout << "    cache.ttl 是否存在: " << (config1.hasConfig("cache.ttl") ? "是" : "否") << endl;
    printSectionEnd();

    // ---------- 7. 展示饿汉式特点 ----------
    cout << endl;
    cout << ">>> 最终配置状态 <<<" << endl;
    printSection("配置总览");
    config1.printAll();
    printSectionEnd();

    cout << endl;
    cout << ">>> 饿汉式单例优势总结 <<<" << endl;
    cout << "  ├─ 线程安全：静态变量初始化由编译器保证，无需加锁" << endl;
    cout << "  ├─ 实现简单：利用 C++ 静态局部变量特性，代码简洁" << endl;
    cout << "  ├─ 天然唯一：程序生命周期内只有唯一实例" << endl;
    cout << "  ├─ 全局访问：任何模块通过 getInstance() 获取同一实例" << endl;
    cout << "  ├─ 适用场景：配置管理器、日志器、连接池等启动即需要的组件" << endl;
    cout << "  └─ 注意事项：实例在 main() 之前创建，需注意静态初始化顺序问题" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}