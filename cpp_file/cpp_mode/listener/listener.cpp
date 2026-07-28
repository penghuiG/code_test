/**
 * 观察者模式 (Observer Pattern)
 * 
 * 定义对象间的一种一对多的依赖关系，当一个对象的状态发生改变时，
 * 所有依赖于它的对象都得到通知并自动更新。
 * 
 * 本例以消息发布/订阅系统为场景，演示观察者模式的核心结构及多种订阅方式：
 *   - Subject（主题/被观察者）：MessageCenter，维护观察者列表，负责通知
 *   - Observer（抽象观察者）：ISubscriber，定义更新接口
 *   - ConcreteSubject（具体主题）：NewsCenter / StockCenter / WeatherCenter
 *   - ConcreteObserver（具体观察者）：MobileApp / EmailService / WebDashboard
 * 
 * 场景说明：
 *   一个消息中心管理多个主题频道（新闻、股票、天气）。不同的订阅者
 *  （手机App、邮件服务、Web仪表盘）订阅感兴趣的频道。当频道发布新消息时，
 *   所有订阅该频道的订阅者会自动收到通知并进行各自的处理。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>
using namespace std;

// ======================== 抽象观察者：订阅者接口 ========================
class ISubscriber {
public:
    virtual ~ISubscriber() {}
    virtual void update(const string& topic, const string& message) = 0;
    virtual string getName() const = 0;
};

// ======================== 抽象主题：频道接口 ========================
class ITopic {
public:
    virtual ~ITopic() {}
    virtual void subscribe(shared_ptr<ISubscriber> subscriber) = 0;
    virtual void unsubscribe(shared_ptr<ISubscriber> subscriber) = 0;
    virtual void notify(const string& message) = 0;
    virtual string getTopicName() const = 0;
};

// ======================== 具体主题：消息频道 ========================
class Topic : public ITopic {
public:
    Topic(const string& name) : name_(name) {}

    void subscribe(shared_ptr<ISubscriber> subscriber) override {
        // 检查是否已订阅
        for (const auto& s : subscribers_) {
            if (s == subscriber) {
                cout << "    [主题: " << name_ << "] 订阅者 [" << subscriber->getName()
                     << "] 已经订阅过了" << endl;
                return;
            }
        }
        subscribers_.push_back(subscriber);
        cout << "    [主题: " << name_ << "] 订阅者 [" << subscriber->getName()
             << "] 已订阅 (当前订阅者数: " << subscribers_.size() << ")" << endl;
    }

    void unsubscribe(shared_ptr<ISubscriber> subscriber) override {
        auto it = find(subscribers_.begin(), subscribers_.end(), subscriber);
        if (it != subscribers_.end()) {
            subscribers_.erase(it);
            cout << "    [主题: " << name_ << "] 订阅者 [" << subscriber->getName()
                 << "] 已取消订阅 (当前订阅者数: " << subscribers_.size() << ")" << endl;
        } else {
            cout << "    [主题: " << name_ << "] 订阅者 [" << subscriber->getName()
                 << "] 未找到，无法取消" << endl;
        }
    }

    void notify(const string& message) override {
        if (subscribers_.empty()) {
            cout << "    [主题: " << name_ << "] 无订阅者，消息未发送" << endl;
            return;
        }
        cout << "    [主题: " << name_ << "] 通知 " << subscribers_.size()
             << " 个订阅者..." << endl;
        for (const auto& subscriber : subscribers_) {
            subscriber->update(name_, message);
        }
    }

    string getTopicName() const override { return name_; }
    int getSubscriberCount() const { return subscribers_.size(); }

private:
    string name_;
    vector<shared_ptr<ISubscriber>> subscribers_;
};

// ======================== 具体观察者一：手机App ========================
class MobileApp : public ISubscriber {
public:
    MobileApp(const string& appName) : appName_(appName) {}

    void update(const string& topic, const string& message) override {
        cout << "    [📱 " << appName_ << "] 收到 [" << topic
             << "] 推送通知: " << message << endl;
        notifications_.push_back("[" + topic + "] " + message);
    }

    string getName() const override { return appName_; }

    void showNotifications() const {
        cout << "    ── " << appName_ << " 通知列表 ──────────────────" << endl;
        for (const auto& n : notifications_) {
            cout << "    │ " << n << endl;
        }
        cout << "    ──────────────────────────────────────────" << endl;
    }

private:
    string appName_;
    vector<string> notifications_;
};

// ======================== 具体观察者二：邮件服务 ========================
class EmailService : public ISubscriber {
public:
    EmailService(const string& emailAddress) : emailAddress_(emailAddress) {}

    void update(const string& topic, const string& message) override {
        cout << "    [📧 " << emailAddress_ << "] 收到 [" << topic
             << "] 邮件通知: " << message << endl;
        emails_.push_back("[" + topic + "] " + message);
    }

    string getName() const override { return emailAddress_; }

    void showEmails() const {
        cout << "    ── " << emailAddress_ << " 邮件列表 ──────────────────" << endl;
        for (const auto& e : emails_) {
            cout << "    │ " << e << endl;
        }
        cout << "    ──────────────────────────────────────────" << endl;
    }

private:
    string emailAddress_;
    vector<string> emails_;
};

// ======================== 具体观察者三：Web仪表盘 ========================
class WebDashboard : public ISubscriber {
public:
    WebDashboard(const string& dashboardName) : dashboardName_(dashboardName), updateCount_(0) {}

    void update(const string& topic, const string& message) override {
        updateCount_++;
        cout << "    [🖥️  " << dashboardName_ << "] 收到 [" << topic
             << "] 实时更新 (#" << updateCount_ << "): " << message << endl;
    }

    string getName() const override { return dashboardName_; }
    int getUpdateCount() const { return updateCount_; }

private:
    string dashboardName_;
    int updateCount_;
};

// ======================== 消息中心：管理所有频道 ========================
class MessageCenter {
public:
    // 创建频道
    shared_ptr<Topic> createTopic(const string& name) {
        auto topic = make_shared<Topic>(name);
        topics_.push_back(topic);
        cout << "    [消息中心] 创建频道: " << name << endl;
        return topic;
    }

    // 发布消息到指定频道
    void publish(const string& topicName, const string& message) {
        for (auto& topic : topics_) {
            if (topic->getTopicName() == topicName) {
                cout << endl;
                cout << "    📢 发布消息到 [" << topicName << "]: " << message << endl;
                topic->notify(message);
                return;
            }
        }
        cout << "    [消息中心] 频道 [" << topicName << "] 不存在" << endl;
    }

    // 显示所有频道状态
    void showStatus() const {
        cout << "    ── 消息中心状态 ──────────────────────────" << endl;
        for (const auto& topic : topics_) {
            cout << "    │ 频道: " << topic->getTopicName()
                 << " | 订阅者数: " << topic->getSubscriberCount() << endl;
        }
        cout << "    ──────────────────────────────────────────" << endl;
    }

private:
    vector<shared_ptr<Topic>> topics_;
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
    cout << "       观察者模式演示 - 消息发布系统       " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建消息中心和频道 ----------
    cout << endl;
    cout << ">>> 步骤一：创建消息中心和频道 <<<" << endl;

    printSection("初始化");
    MessageCenter center;
    auto newsTopic = center.createTopic("新闻频道");
    auto stockTopic = center.createTopic("股票频道");
    auto weatherTopic = center.createTopic("天气频道");
    center.showStatus();
    printSectionEnd();

    // ---------- 2. 创建订阅者 ----------
    cout << endl;
    cout << ">>> 步骤二：创建订阅者 <<<" << endl;

    printSection("创建观察者");
    auto mobileApp = make_shared<MobileApp>("今日头条App");
    auto emailService = make_shared<EmailService>("user@example.com");
    auto webDashboard = make_shared<WebDashboard>("运营监控大屏");
    cout << "    已创建 3 个订阅者: " << mobileApp->getName()
         << ", " << emailService->getName()
         << ", " << webDashboard->getName() << endl;
    printSectionEnd();

    // ---------- 3. 订阅频道 ----------
    cout << endl;
    cout << ">>> 步骤三：订阅频道 <<<" << endl;

    printSection("订阅操作");
    // 手机App订阅所有频道
    cout << "  [手机App 订阅所有频道]" << endl;
    newsTopic->subscribe(mobileApp);
    stockTopic->subscribe(mobileApp);
    weatherTopic->subscribe(mobileApp);

    // 邮件服务只订阅新闻和股票
    cout << "  [邮件服务 订阅新闻和股票]" << endl;
    newsTopic->subscribe(emailService);
    stockTopic->subscribe(emailService);

    // Web仪表盘订阅所有频道
    cout << "  [Web仪表盘 订阅所有频道]" << endl;
    newsTopic->subscribe(webDashboard);
    stockTopic->subscribe(webDashboard);
    weatherTopic->subscribe(webDashboard);
    printSectionEnd();

    printSection("订阅后状态");
    center.showStatus();
    printSectionEnd();

    // ---------- 4. 发布消息 ----------
    cout << endl;
    cout << ">>> 步骤四：发布消息（自动通知订阅者） <<<" << endl;

    printSection("发布新闻");
    center.publish("新闻频道", "【快讯】中国成功发射新一代通信卫星");
    printSectionEnd();

    printSection("发布股票信息");
    center.publish("股票频道", "【行情】上证指数收盘上涨1.5%，报3280点");
    printSectionEnd();

    printSection("发布天气预报");
    center.publish("天气频道", "【预警】明天将有大范围暴雨，请注意防范");
    printSectionEnd();

    // ---------- 5. 取消订阅 ----------
    cout << endl;
    cout << ">>> 步骤五：取消订阅 <<<" << endl;

    printSection("邮件服务取消新闻订阅");
    newsTopic->unsubscribe(emailService);
    printSectionEnd();

    printSection("再次发布新闻（邮件服务不会收到）");
    center.publish("新闻频道", "【快讯】全国多地出台稳经济新政策");
    printSectionEnd();

    // ---------- 6. 动态订阅 ----------
    cout << endl;
    cout << ">>> 步骤六：动态订阅（运行时添加新订阅者） <<<" << endl;

    printSection("邮件服务重新订阅天气频道");
    weatherTopic->subscribe(emailService);
    printSectionEnd();

    printSection("发布天气信息");
    center.publish("天气频道", "【高温】多地气温将超38℃，请注意防暑");
    printSectionEnd();

    // ---------- 7. 查看各订阅者收到的消息 ----------
    cout << endl;
    cout << ">>> 步骤七：查看各订阅者收到的消息 <<<" << endl;

    printSection("手机App 收到的全部通知");
    mobileApp->showNotifications();
    printSectionEnd();

    printSection("邮件服务 收到的全部邮件");
    emailService->showEmails();
    printSectionEnd();

    printSection("Web仪表盘 更新统计");
    cout << "    Web仪表盘 [" << webDashboard->getName()
         << "] 共收到 " << webDashboard->getUpdateCount() << " 次实时更新" << endl;
    printSectionEnd();

    // ---------- 8. 边界情况：重复订阅和空频道通知 ----------
    cout << endl;
    cout << ">>> 步骤八：边界情况测试 <<<" << endl;

    printSection("重复订阅");
    newsTopic->subscribe(mobileApp);
    printSectionEnd();

    printSection("空频道通知");
    center.publish("天气频道", "");  // 先取消所有天气订阅
    printSectionEnd();

    // ---------- 9. 最终状态 ----------
    cout << endl;
    cout << ">>> 步骤九：最终消息中心状态 <<<" << endl;

    printSection("最终状态");
    center.showStatus();
    printSectionEnd();

    // ---------- 10. 展示观察者模式的价值 ----------
    cout << endl;
    cout << ">>> 观察者模式优势总结 <<<" << endl;
    cout << "  ├─ 松耦合：主题和观察者之间松耦合，各自独立变化" << endl;
    cout << "  ├─ 动态关系：运行时可以动态添加/删除观察者" << endl;
    cout << "  ├─ 广播通信：主题无需知道观察者的具体类型，统一通知" << endl;
    cout << "  ├─ 开闭原则：新增观察者类型无需修改主题代码" << endl;
    cout << "  ├─ 一对多依赖：一个主题可以同时通知多个观察者" << endl;
    cout << "  └─ 事件驱动：基于通知机制，实现事件驱动的架构" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}