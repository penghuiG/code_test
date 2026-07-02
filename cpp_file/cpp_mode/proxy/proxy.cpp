/**
 * 代理模式 (Proxy Pattern)
 * 
 * 为其他对象提供一种代理以控制对这个对象的访问。代理对象和目标对象
 * 实现相同的接口，代理在调用目标对象前后可以添加额外的控制逻辑。
 * 
 * 本例以文件服务器访问为场景，演示代理模式的核心结构及多种代理类型：
 *   - Subject（抽象主题）：IFileService，定义文件操作的公共接口
 *   - RealSubject（真实主题）：RealFileService，真正执行文件读写操作的服务
 *   - Proxy（代理）：
 *     · ProtectionProxy（保护代理）：基于用户权限控制对文件服务的访问
 *     · CachingProxy（缓存代理）：缓存文件内容，避免重复读取
 *     · LoggingProxy（日志代理）：记录每次文件操作的日志信息
 * 
 * 场景说明：
 *   一个文件服务器提供文件读写服务。客户端不直接访问真实服务，而是通过
 *   多层代理来访问。代理可以在不改变真实服务的情况下，增加权限控制、
 *   缓存机制、日志记录等功能。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
using namespace std;

// ======================== 用户信息 ========================
struct User {
    string name;
    string role;       // "admin", "editor", "viewer"

    User(const string& name, const string& role) : name(name), role(role) {}
};

// ======================== 抽象主题：文件服务接口 ========================
class IFileService {
public:
    virtual ~IFileService() {}
    virtual string readFile(const string& filename) = 0;
    virtual bool writeFile(const string& filename, const string& content) = 0;
    virtual bool deleteFile(const string& filename) = 0;
    virtual string getServiceInfo() const = 0;
};

// ======================== 真实主题：真实文件服务 ========================
class RealFileService : public IFileService {
public:
    string readFile(const string& filename) override {
        // 模拟从磁盘读取文件
        cout << "    [RealFileService] 从磁盘读取文件: " << filename << endl;
        if (filename == "config.ini") {
            return "server_port=8080\nmax_connections=1000\ndb_host=localhost";
        } else if (filename == "data.txt") {
            return "这是文件数据内容，包含重要的业务数据...";
        } else if (filename == "readme.md") {
            return "# 项目说明\n这是一个代理模式的示例程序";
        }
        return "文件 [" + filename + "] 的默认内容";
    }

    bool writeFile(const string& filename, const string& content) override {
        // 模拟写入文件到磁盘
        cout << "    [RealFileService] 写入文件: " << filename << endl;
        cout << "    [RealFileService] 写入内容长度: " << content.size() << " 字节" << endl;
        return true;
    }

    bool deleteFile(const string& filename) override {
        // 模拟删除文件
        cout << "    [RealFileService] 删除文件: " << filename << endl;
        return true;
    }

    string getServiceInfo() const override {
        return "RealFileService (真实文件服务)";
    }
};

// ======================== 代理一：保护代理（权限控制） ========================
class ProtectionProxy : public IFileService {
public:
    ProtectionProxy(shared_ptr<IFileService> service, const User& user)
        : service_(service), user_(user) {}

    string readFile(const string& filename) override {
        // viewer/editor/admin 都可以读取
        if (user_.role == "viewer" || user_.role == "editor" || user_.role == "admin") {
            cout << "    [ProtectionProxy] 用户 [" << user_.name
                 << "] (角色: " << user_.role << ") → 读取权限 ✓" << endl;
            return service_->readFile(filename);
        }
        cout << "    [ProtectionProxy] 用户 [" << user_.name
             << "] (角色: " << user_.role << ") → 读取权限 ✗ 拒绝访问！" << endl;
        return "[权限不足] 无法读取文件";
    }

    bool writeFile(const string& filename, const string& content) override {
        // 只有 editor 和 admin 可以写入
        if (user_.role == "editor" || user_.role == "admin") {
            cout << "    [ProtectionProxy] 用户 [" << user_.name
                 << "] (角色: " << user_.role << ") → 写入权限 ✓" << endl;
            return service_->writeFile(filename, content);
        }
        cout << "    [ProtectionProxy] 用户 [" << user_.name
             << "] (角色: " << user_.role << ") → 写入权限 ✗ 拒绝访问！" << endl;
        return false;
    }

    bool deleteFile(const string& filename) override {
        // 只有 admin 可以删除
        if (user_.role == "admin") {
            cout << "    [ProtectionProxy] 用户 [" << user_.name
                 << "] (角色: " << user_.role << ") → 删除权限 ✓" << endl;
            return service_->deleteFile(filename);
        }
        cout << "    [ProtectionProxy] 用户 [" << user_.name
             << "] (角色: " << user_.role << ") → 删除权限 ✗ 拒绝访问！" << endl;
        return false;
    }

    string getServiceInfo() const override {
        return "ProtectionProxy → " + service_->getServiceInfo();
    }

private:
    shared_ptr<IFileService> service_;
    User user_;
};

// ======================== 代理二：缓存代理 ========================
class CachingProxy : public IFileService {
public:
    CachingProxy(shared_ptr<IFileService> service) : service_(service), cacheHits_(0), cacheMisses_(0) {}

    string readFile(const string& filename) override {
        auto it = cache_.find(filename);
        if (it != cache_.end()) {
            cacheHits_++;
            cout << "    [CachingProxy] 缓存命中 ✓ 文件: " << filename
                 << " (命中: " << cacheHits_ << ", 未命中: " << cacheMisses_ << ")" << endl;
            return it->second;
        }

        cacheMisses_++;
        cout << "    [CachingProxy] 缓存未命中，从服务读取... 文件: " << filename << endl;
        string content = service_->readFile(filename);
        cache_[filename] = content;
        return content;
    }

    bool writeFile(const string& filename, const string& content) override {
        bool result = service_->writeFile(filename, content);
        if (result) {
            // 写入成功后更新缓存
            cache_[filename] = content;
            cout << "    [CachingProxy] 缓存已更新: " << filename << endl;
        }
        return result;
    }

    bool deleteFile(const string& filename) override {
        bool result = service_->deleteFile(filename);
        if (result) {
            // 删除成功后清除缓存
            cache_.erase(filename);
            cout << "    [CachingProxy] 缓存已清除: " << filename << endl;
        }
        return result;
    }

    string getServiceInfo() const override {
        return "CachingProxy → " + service_->getServiceInfo();
    }

    void printCacheStats() const {
        cout << "    缓存统计 → 命中: " << cacheHits_
             << ", 未命中: " << cacheMisses_
             << ", 缓存文件数: " << cache_.size() << endl;
    }

private:
    shared_ptr<IFileService> service_;
    unordered_map<string, string> cache_;
    int cacheHits_;
    int cacheMisses_;
};

// ======================== 代理三：日志代理 ========================
class LoggingProxy : public IFileService {
public:
    LoggingProxy(shared_ptr<IFileService> service) : service_(service) {}

    string readFile(const string& filename) override {
        string timestamp = getTimeStamp();
        cout << "    [LoggingProxy] 📋 [" << timestamp << "] READ  → " << filename << endl;
        string result = service_->readFile(filename);
        cout << "    [LoggingProxy] 📋 [" << timestamp << "] READ  ← " << filename
             << " (长度: " << result.size() << " 字节)" << endl;
        logs_.push_back("[" + timestamp + "] READ " + filename + " (" + to_string(result.size()) + " bytes)");
        return result;
    }

    bool writeFile(const string& filename, const string& content) override {
        string timestamp = getTimeStamp();
        cout << "    [LoggingProxy] 📋 [" << timestamp << "] WRITE → " << filename
             << " (" << content.size() << " 字节)" << endl;
        bool result = service_->writeFile(filename, content);
        logs_.push_back("[" + timestamp + "] WRITE " + filename + " (" + to_string(content.size()) + " bytes) "
                        + (result ? "成功" : "失败"));
        return result;
    }

    bool deleteFile(const string& filename) override {
        string timestamp = getTimeStamp();
        cout << "    [LoggingProxy] 📋 [" << timestamp << "] DELETE → " << filename << endl;
        bool result = service_->deleteFile(filename);
        logs_.push_back("[" + timestamp + "] DELETE " + filename + " " + (result ? "成功" : "失败"));
        return result;
    }

    string getServiceInfo() const override {
        return "LoggingProxy → " + service_->getServiceInfo();
    }

    void printLogs() const {
        cout << "    ── 操作日志 ──────────────────────────" << endl;
        for (const auto& log : logs_) {
            cout << "    │ " << log << endl;
        }
        cout << "    ───────────────────────────────────────" << endl;
    }

private:
    shared_ptr<IFileService> service_;
    vector<string> logs_;

    string getTimeStamp() const {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);
        char buf[20];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
        return string(buf);
    }
};

// ======================== 辅助函数：分隔线 ========================
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
    cout << "       代理模式演示 - 文件服务系统        " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建真实服务对象 ----------
    cout << endl;
    cout << ">>> 步骤一：创建真实文件服务 <<<" << endl;

    auto realService = make_shared<RealFileService>();
    cout << "  服务类型: " << realService->getServiceInfo() << endl;

    // ---------- 2. 保护代理演示 ----------
    cout << endl;
    cout << ">>> 步骤二：保护代理 - 权限控制 <<<" << endl;

    // 管理员用户
    User admin("张三", "admin");
    auto adminService = make_shared<ProtectionProxy>(realService, admin);

    // 编辑者用户
    User editor("李四", "editor");
    auto editorService = make_shared<ProtectionProxy>(realService, editor);

    // 只读用户
    User viewer("王五", "viewer");
    auto viewerService = make_shared<ProtectionProxy>(realService, viewer);

    // 管理员：拥有全部权限
    printSection("管理员 [张三] 操作");
    adminService->readFile("config.ini");
    adminService->writeFile("config.ini", "new_config=true");
    adminService->deleteFile("temp.log");
    printSectionEnd();

    // 编辑者：可以读写，不能删除
    printSection("编辑者 [李四] 操作");
    editorService->readFile("data.txt");
    editorService->writeFile("data.txt", "更新后的数据");
    editorService->deleteFile("data.txt");    // 被拒绝
    printSectionEnd();

    // 只读用户：只能读取
    printSection("只读用户 [王五] 操作");
    viewerService->readFile("readme.md");
    viewerService->writeFile("readme.md", "修改内容");  // 被拒绝
    viewerService->deleteFile("readme.md");              // 被拒绝
    printSectionEnd();

    // ---------- 3. 缓存代理演示 ----------
    cout << endl;
    cout << ">>> 步骤三：缓存代理 - 自动缓存 <<<" << endl;

    // 在真实服务外包一层缓存代理
    auto cachingService = make_shared<CachingProxy>(realService);

    printSection("首次读取（缓存未命中）");
    string content1 = cachingService->readFile("config.ini");
    cout << "    内容: " << content1.substr(0, 30) << "..." << endl;
    printSectionEnd();

    printSection("再次读取同一文件（缓存命中）");
    string content2 = cachingService->readFile("config.ini");
    cout << "    内容: " << content2.substr(0, 30) << "..." << endl;
    printSectionEnd();

    printSection("读取另一个文件");
    cachingService->readFile("data.txt");
    printSectionEnd();

    printSection("再次读取第一个文件（仍然命中）");
    cachingService->readFile("config.ini");
    cachingService->printCacheStats();
    printSectionEnd();

    // ---------- 4. 日志代理演示 ----------
    cout << endl;
    cout << ">>> 步骤四：日志代理 - 操作记录 <<<" << endl;

    auto loggingService = make_shared<LoggingProxy>(realService);

    loggingService->readFile("config.ini");
    loggingService->writeFile("log.txt", "这是一条日志记录");
    loggingService->readFile("data.txt");
    loggingService->deleteFile("temp.tmp");

    cout << endl;
    loggingService->printLogs();

    // ---------- 5. 代理链（多层代理组合） ----------
    cout << endl;
    cout << ">>> 步骤五：代理链 - 多层代理组合 <<<" << endl;

    cout << "  代理链结构：" << endl;
    cout << "  客户端 → LoggingProxy → CachingProxy → ProtectionProxy → RealFileService" << endl;

    // 构建代理链：日志 → 缓存 → 权限检查 → 真实服务
    auto chainRealService = make_shared<RealFileService>();

    // 最内层：保护代理（admin 用户）
    auto chainProtection = make_shared<ProtectionProxy>(chainRealService, admin);

    // 中间层：缓存代理
    auto chainCaching = make_shared<CachingProxy>(chainProtection);

    // 最外层：日志代理
    auto chainLogging = make_shared<LoggingProxy>(chainCaching);

    printSection("通过代理链首次读取");
    string chainContent = chainLogging->readFile("config.ini");
    cout << "    最终内容: " << chainContent.substr(0, 30) << "..." << endl;
    printSectionEnd();

    printSection("通过代理链再次读取（缓存生效）");
    chainLogging->readFile("config.ini");
    printSectionEnd();

    printSection("通过代理链写入新文件");
    chainLogging->writeFile("new_file.txt", "通过代理链写入的新文件内容");
    printSectionEnd();

    cout << endl;
    cout << "  代理链服务信息: " << chainLogging->getServiceInfo() << endl;

    // ---------- 6. 展示代理模式的价值 ----------
    cout << endl;
    cout << ">>> 代理模式优势总结 <<<" << endl;
    cout << "  ├─ 保护代理：基于角色的访问控制，客户端无需感知权限逻辑" << endl;
    cout << "  ├─ 缓存代理：透明地缓存数据，提升访问性能" << endl;
    cout << "  ├─ 日志代理：透明地记录操作日志，便于审计和调试" << endl;
    cout << "  ├─ 代理链：多种代理可以灵活组合，各自职责单一" << endl;
    cout << "  ├─ 开闭原则：新增代理类型无需修改已有代码" << endl;
    cout << "  └─ 透明性：客户端通过统一接口访问，不感知代理的存在" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}