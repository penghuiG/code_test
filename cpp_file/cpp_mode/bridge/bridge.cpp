/**
 * 桥接模式 (Bridge Pattern)
 * 
 * 将抽象部分与实现部分分离，使它们都可以独立变化。
 * 桥接模式通过组合而非继承的方式，将抽象（Abstraction）和实现（Implementation）
 * 放在两个不同的类层次结构中，使两者可以独立扩展。
 * 
 * 本例以消息发送系统为场景，演示桥接模式的核心结构：
 *   - Abstraction（抽象层）：消息的类型/级别（普通消息、紧急消息、加密消息）
 *   - Implementor（实现层接口）：消息的发送通道（邮件、短信、微信）
 *   - Refined Abstraction（扩展抽象）：具体的消息类型
 *   - Concrete Implementor（具体实现）：具体的发送通道
 * 
 * 场景说明：
 *   某系统需要支持多种消息类型（普通、紧急、加密）和多种发送通道（邮件、短信、微信）。
 *   如果使用继承，需要为每种组合创建子类（3×3=9 个类），造成类爆炸。
 *   通过桥接模式，将消息类型和发送通道解耦，只需 3+3=6 个类即可实现所有组合，
 *   且新增消息类型或发送通道时互不影响。
 */

#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ======================== 实现层接口：消息发送通道 ========================
class MessageSender {
public:
    virtual ~MessageSender() {}
    virtual void sendMessage(const string& title, const string& content) = 0;
    virtual string getChannel() const = 0;
};

// ======================== 具体实现：邮件发送 ========================
class EmailSender : public MessageSender {
public:
    void sendMessage(const string& title, const string& content) override {
        cout << "  📧 [邮件通道] 发送邮件" << endl;
        cout << "     主题: " << title << endl;
        cout << "     正文: " << content << endl;
        cout << "     ✓ 邮件已发送至收件箱" << endl;
    }

    string getChannel() const override { return "邮件"; }
};

// ======================== 具体实现：短信发送 ========================
class SmsSender : public MessageSender {
public:
    void sendMessage(const string& title, const string& content) override {
        cout << "  📱 [短信通道] 发送短信" << endl;
        cout << "     标题: " << title << endl;
        cout << "     内容: " << content << endl;
        cout << "     ✓ 短信已发送至手机" << endl;
    }

    string getChannel() const override { return "短信"; }
};

// ======================== 具体实现：微信发送 ========================
class WeChatSender : public MessageSender {
public:
    void sendMessage(const string& title, const string& content) override {
        cout << "  💬 [微信通道] 发送微信消息" << endl;
        cout << "     标题: " << title << endl;
        cout << "     内容: " << content << endl;
        cout << "     ✓ 微信消息已发送" << endl;
    }

    string getChannel() const override { return "微信"; }
};

// ======================== 抽象层：消息 ========================
class Message {
public:
    Message(shared_ptr<MessageSender> sender) : sender_(sender) {}
    virtual ~Message() {}

    // 定义消息发送的模板流程
    virtual void send(const string& title, const string& content) {
        cout << endl;
        cout << "  ┌─ [" << getType() << "] 通过 [" 
             << sender_->getChannel() << "] 发送 ─┐" << endl;
        beforeSend();
        sender_->sendMessage(title, formatContent(content));
        afterSend();
        cout << "  └──────────────────────────────────┘" << endl;
    }

    virtual string getType() const = 0;

protected:
    virtual void beforeSend() {}
    virtual void afterSend() {}
    virtual string formatContent(const string& content) = 0;

    shared_ptr<MessageSender> sender_;
};

// ======================== 扩展抽象：普通消息 ========================
class NormalMessage : public Message {
public:
    NormalMessage(shared_ptr<MessageSender> sender) : Message(sender) {}

    string getType() const override { return "普通消息"; }

protected:
    string formatContent(const string& content) override {
        return content;
    }
};

// ======================== 扩展抽象：紧急消息 ========================
class UrgentMessage : public Message {
public:
    UrgentMessage(shared_ptr<MessageSender> sender) : Message(sender) {}

    string getType() const override { return "紧急消息"; }

protected:
    void beforeSend() override {
        cout << "  ⚡ 注意：这是一条紧急消息！" << endl;
    }

    void afterSend() override {
        cout << "  ⚡ 已标记为高优先级，需要回执确认" << endl;
        cout << "  ⚡ 已启动重发保障机制（最多重试 3 次）" << endl;
    }

    string formatContent(const string& content) override {
        return "【紧急】" + content + " —— 请立即处理！";
    }
};

// ======================== 扩展抽象：加密消息 ========================
class EncryptedMessage : public Message {
public:
    EncryptedMessage(shared_ptr<MessageSender> sender) : Message(sender) {}

    string getType() const override { return "加密消息"; }

protected:
    void beforeSend() override {
        cout << "  🔐 正在进行 AES-256 加密..." << endl;
    }

    void afterSend() override {
        cout << "  🔐 消息已端到端加密，接收方需密钥解密" << endl;
    }

    string formatContent(const string& content) override {
        // 模拟加密：实际场景中会使用真正的加密算法
        return "ENCRYPTED[" + content + "]";
    }
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     桥接模式演示 - 消息发送系统         " << endl;
    cout << "========================================" << endl;

    // 创建具体实现：三种发送通道
    auto emailSender  = make_shared<EmailSender>();
    auto smsSender    = make_shared<SmsSender>();
    auto wechatSender = make_shared<WeChatSender>();

    // ---------- 1. 普通消息 + 不同通道 ----------
    cout << endl;
    cout << ">>> 步骤一：普通消息通过不同通道发送 <<<" << endl;

    NormalMessage normalEmail(emailSender);
    normalEmail.send("会议通知", "明天上午 10:00 开周会，请准时参加。");

    NormalMessage normalSms(smsSender);
    normalSms.send("验证码", "您的验证码是 123456，5 分钟内有效。");

    NormalMessage normalWechat(wechatSender);
    normalWechat.send("工作提醒", "项目报告请在周五前提交。");

    // ---------- 2. 紧急消息 + 不同通道 ----------
    cout << endl;
    cout << ">>> 步骤二：紧急消息通过不同通道发送 <<<" << endl;

    UrgentMessage urgentEmail(emailSender);
    urgentEmail.send("服务器告警", "生产环境 CPU 使用率超过 95%");

    UrgentMessage urgentSms(smsSender);
    urgentSms.send("系统宕机", "数据库主节点失去连接");

    UrgentMessage urgentWechat(wechatSender);
    urgentWechat.send("安全告警", "检测到异常登录尝试");

    // ---------- 3. 加密消息 + 不同通道 ----------
    cout << endl;
    cout << ">>> 步骤三：加密消息通过不同通道发送 <<<" << endl;

    EncryptedMessage encryptedEmail(emailSender);
    encryptedEmail.send("机密文件", "本季度财务报表已生成");

    EncryptedMessage encryptedSms(smsSender);
    encryptedSms.send("密码重置", "您的新临时密码已生成");

    EncryptedMessage encryptedWechat(wechatSender);
    encryptedWechat.send("合同签署", "电子合同已准备就绪，请查收签署");

    // ---------- 4. 动态切换实现：运行时更换发送通道 ----------
    cout << endl;
    cout << ">>> 步骤四：动态切换发送通道 <<<" << endl;

    // 同一种消息类型，可以灵活切换不同的发送通道
    shared_ptr<MessageSender> currentSender = emailSender;
    
    NormalMessage dynamicMsg(currentSender);
    cout << endl;
    cout << "  [动态切换] 当前通道: 邮件" << endl;
    dynamicMsg.send("动态消息", "这是一条通过邮件发送的消息");

    // 运行时切换为微信通道
    currentSender = wechatSender;
    NormalMessage dynamicMsg2(currentSender);
    cout << endl;
    cout << "  [动态切换] 切换通道: 微信" << endl;
    dynamicMsg2.send("动态消息", "这是一条通过微信发送的消息");

    // ---------- 5. 展示桥接模式的价值 ----------
    cout << endl;
    cout << ">>> 桥接模式优势总结 <<<" << endl;
    cout << "  ├─ 抽象层（消息类型）: 3 种（普通、紧急、加密）" << endl;
    cout << "  ├─ 实现层（发送通道）: 3 种（邮件、短信、微信）" << endl;
    cout << "  ├─ 可组合总数: 3 × 3 = 9 种组合" << endl;
    cout << "  ├─ 实际创建的类: 3 + 3 = 6 个（无需 9 个）" << endl;
    cout << "  └─ 新增消息类型或通道时，只需各加一个类即可" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}