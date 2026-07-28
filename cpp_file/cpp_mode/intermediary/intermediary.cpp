/**
 * 中介者模式 (Mediator Pattern)
 * 
 * 用一个中介对象来封装一系列对象之间的交互。中介者使各对象不需要显式地
 * 相互引用，从而使其耦合松散，而且可以独立地改变它们之间的交互。
 * 
 * 本例以智能家居系统为场景，演示中介者模式的核心结构：
 *   - Mediator（抽象中介者）：IHomeMediator，定义设备间通信的接口
 *   - ConcreteMediator（具体中介者）：SmartHomeHub，协调各设备之间的交互
 *   - Colleague（抽象同事类）：Device，所有设备的基类，持有中介者引用
 *   - ConcreteColleague（具体同事类）：
 *     · Light（灯光）：可开关、调节亮度
 *     · AirConditioner（空调）：可开关、调节温度
 *     · Curtain（窗帘）：可开合
 *     · TV（电视）：可开关、切换频道
 *     · Alarm（报警器）：可触发/解除报警
 * 
 * 场景说明：
 *   智能家居系统中有多个设备，设备之间存在联动关系。例如：
 *   - 说"晚安模式"时：关灯 → 关窗帘 → 空调调到睡眠温度 → 关电视 → 开启安防
 *   - 烟雾报警器触发时：关闭空调 → 打开所有灯 → 打开窗帘 → 触发警报
 *   设备之间不直接互相引用，而是通过智能中枢（中介者）来协调交互。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
using namespace std;

// 前置声明
class IHomeMediator;

// ======================== 抽象同事类：设备基类 ========================
class Device {
public:
    Device(const string& name, shared_ptr<IHomeMediator> mediator)
        : name_(name), mediator_(mediator), isOn_(false) {}

    virtual ~Device() {}

    // 获取设备名称
    string getName() const { return name_; }

    // 获取设备状态
    bool isOn() const { return isOn_; }

    // 设备开启
    virtual void turnOn() {
        isOn_ = true;
        cout << "    [" << name_ << "] 已开启" << endl;
    }

    // 设备关闭
    virtual void turnOff() {
        isOn_ = false;
        cout << "    [" << name_ << "] 已关闭" << endl;
    }

    // 接收来自中介者的通知（子类可重写）
    virtual void receive(const string& from, const string& event) = 0;

    // 向中介者发送事件通知
    void send(const string& event);

protected:
    string name_;
    shared_ptr<IHomeMediator> mediator_;
    bool isOn_;
};

// ======================== 抽象中介者：智能家居中枢接口 ========================
class IHomeMediator {
public:
    virtual ~IHomeMediator() {}
    // 注册设备到中介者
    virtual void registerDevice(shared_ptr<Device> device) = 0;
    // 设备发送事件通知，中介者协调其他设备响应
    virtual void notify(shared_ptr<Device> sender, const string& event) = 0;
};

// Device::send 的实现（依赖 IHomeMediator，所以放在接口定义之后）
// 注意：基类 Device 无法使用 shared_from_this，子类通过各自的 notifyMediator 方法发送事件
void Device::send(const string& event) {
    cout << "    [" << name_ << "] 发送事件: \"" << event << "\"" << endl;
    mediator_->notify(nullptr, event);
}

// ======================== 具体设备：灯光 ========================
class Light : public Device, public enable_shared_from_this<Light> {
public:
    Light(const string& name, shared_ptr<IHomeMediator> mediator)
        : Device(name, mediator), brightness_(100) {}

    void turnOn() override {
        Device::turnOn();
        brightness_ = 100;
        cout << "    [" << name_ << "] 亮度设置为: " << brightness_ << "%" << endl;
    }

    void setBrightness(int level) {
        brightness_ = level;
        cout << "    [" << name_ << "] 亮度调节为: " << brightness_ << "%" << endl;
    }

    void receive(const string& from, const string& event) override {
        if (event == "GOOD_NIGHT") {
            // 晚安模式：缓慢关闭灯光
            cout << "    [" << name_ << "] 收到晚安指令（来自: " << from
                 << "），灯光缓慢关闭中..." << endl;
            isOn_ = false;
        } else if (event == "SMOKE_ALARM") {
            // 烟雾报警：打开所有灯
            cout << "    [" << name_ << "] 收到烟雾报警（来自: " << from
                 << "），灯光全开！" << endl;
            isOn_ = true;
            brightness_ = 100;
        } else if (event == "WAKE_UP") {
            // 起床模式：柔和灯光
            cout << "    [" << name_ << "] 收到起床指令（来自: " << from
                 << "），灯光柔和开启" << endl;
            isOn_ = true;
            brightness_ = 30;
        }
    }

    // 发送事件给中介者
    void notifyMediator(const string& event) {
        mediator_->notify(make_shared<Light>(*this), event);
    }

private:
    int brightness_;
};

// ======================== 具体设备：空调 ========================
class AirConditioner : public Device, public enable_shared_from_this<AirConditioner> {
public:
    AirConditioner(const string& name, shared_ptr<IHomeMediator> mediator)
        : Device(name, mediator), temperature_(24), mode_("制冷") {}

    void turnOn() override {
        Device::turnOn();
        cout << "    [" << name_ << "] 温度: " << temperature_
             << "°C, 模式: " << mode_ << endl;
    }

    void setTemperature(int temp) {
        temperature_ = temp;
        cout << "    [" << name_ << "] 温度调节为: " << temperature_ << "°C" << endl;
    }

    void setMode(const string& mode) {
        mode_ = mode;
        cout << "    [" << name_ << "] 模式切换为: " << mode_ << endl;
    }

    void receive(const string& from, const string& event) override {
        if (event == "GOOD_NIGHT") {
            // 晚安模式：空调调到睡眠模式
            cout << "    [" << name_ << "] 收到晚安指令（来自: " << from
                 << "），切换睡眠模式，温度调至26°C" << endl;
            isOn_ = true;
            temperature_ = 26;
            mode_ = "睡眠";
        } else if (event == "SMOKE_ALARM") {
            // 烟雾报警：关闭空调（防止助燃）
            cout << "    [" << name_ << "] 收到烟雾报警（来自: " << from
                 << "），紧急关闭！" << endl;
            isOn_ = false;
        } else if (event == "WAKE_UP") {
            // 起床模式：空调恢复正常
            cout << "    [" << name_ << "] 收到起床指令（来自: " << from
                 << "），切换到正常模式，温度24°C" << endl;
            isOn_ = true;
            temperature_ = 24;
            mode_ = "制冷";
        }
    }

    void notifyMediator(const string& event) {
        mediator_->notify(make_shared<AirConditioner>(*this), event);
    }

private:
    int temperature_;
    string mode_;
};

// ======================== 具体设备：窗帘 ========================
class Curtain : public Device, public enable_shared_from_this<Curtain> {
public:
    Curtain(const string& name, shared_ptr<IHomeMediator> mediator)
        : Device(name, mediator), openPercent_(0) {}

    void turnOn() override {
        // 窗帘的"开"即打开窗帘
        Device::turnOn();
        openPercent_ = 100;
        cout << "    [" << name_ << "] 窗帘已完全打开" << endl;
    }

    void turnOff() override {
        // 窗帘的"关"即关闭窗帘
        Device::turnOff();
        openPercent_ = 0;
        cout << "    [" << name_ << "] 窗帘已完全关闭" << endl;
    }

    void setOpenPercent(int percent) {
        openPercent_ = percent;
        cout << "    [" << name_ << "] 窗帘开合度: " << openPercent_ << "%" << endl;
    }

    void receive(const string& from, const string& event) override {
        if (event == "GOOD_NIGHT") {
            // 晚安模式：关闭窗帘
            cout << "    [" << name_ << "] 收到晚安指令（来自: " << from
                 << "），窗帘关闭" << endl;
            isOn_ = false;
            openPercent_ = 0;
        } else if (event == "SMOKE_ALARM") {
            // 烟雾报警：打开窗帘便于排烟
            cout << "    [" << name_ << "] 收到烟雾报警（来自: " << from
                 << "），窗帘全开排烟！" << endl;
            isOn_ = true;
            openPercent_ = 100;
        } else if (event == "WAKE_UP") {
            // 起床模式：打开窗帘
            cout << "    [" << name_ << "] 收到起床指令（来自: " << from
                 << "），窗帘缓慢打开" << endl;
            isOn_ = true;
            openPercent_ = 70;
        }
    }

    void notifyMediator(const string& event) {
        mediator_->notify(make_shared<Curtain>(*this), event);
    }

private:
    int openPercent_;
};

// ======================== 具体设备：电视 ========================
class TV : public Device, public enable_shared_from_this<TV> {
public:
    TV(const string& name, shared_ptr<IHomeMediator> mediator)
        : Device(name, mediator), channel_("CCTV-1") {}

    void turnOn() override {
        Device::turnOn();
        cout << "    [" << name_ << "] 正在播放: " << channel_ << endl;
    }

    void setChannel(const string& ch) {
        channel_ = ch;
        cout << "    [" << name_ << "] 切换频道: " << channel_ << endl;
    }

    void receive(const string& from, const string& event) override {
        if (event == "GOOD_NIGHT") {
            // 晚安模式：关闭电视
            cout << "    [" << name_ << "] 收到晚安指令（来自: " << from
                 << "），电视关闭" << endl;
            isOn_ = false;
        } else if (event == "SMOKE_ALARM") {
            // 烟雾报警：电视切换到报警画面
            cout << "    [" << name_ << "] 收到烟雾报警（来自: " << from
                 << "），显示报警提示画面！" << endl;
            channel_ = "⚠️ 紧急报警频道";
        } else if (event == "WAKE_UP") {
            // 起床模式：打开电视播放早间新闻
            cout << "    [" << name_ << "] 收到起床指令（来自: " << from
                 << "），播放早间新闻" << endl;
            isOn_ = true;
            channel_ = "早间新闻";
        }
    }

    void notifyMediator(const string& event) {
        mediator_->notify(make_shared<TV>(*this), event);
    }

private:
    string channel_;
};

// ======================== 具体设备：烟雾报警器 ========================
class SmokeDetector : public Device, public enable_shared_from_this<SmokeDetector> {
public:
    SmokeDetector(const string& name, shared_ptr<IHomeMediator> mediator)
        : Device(name, mediator) {}

    void turnOn() override {
        Device::turnOn();
        cout << "    [" << name_ << "] 烟雾检测已启动，监控中..." << endl;
    }

    // 检测到烟雾，触发报警
    void detectSmoke() {
        cout << "    [" << name_ << "] ⚠️ 检测到烟雾！触发紧急报警！" << endl;
        // 通过中介者通知所有设备
        mediator_->notify(make_shared<SmokeDetector>(*this), "SMOKE_ALARM");
    }

    void receive(const string& from, const string& event) override {
        // 烟雾报警器通常只发送不接收，但晚安模式时开启安防监控
        if (event == "GOOD_NIGHT") {
            cout << "    [" << name_ << "] 收到晚安指令（来自: " << from
                 << "），进入安防监控模式" << endl;
            isOn_ = true;
        }
    }

    void notifyMediator(const string& event) {
        mediator_->notify(make_shared<SmokeDetector>(*this), event);
    }

private:
    bool alarmTriggered_;
};

// ======================== 具体中介者：智能家庭中枢 ========================
class SmartHomeHub : public IHomeMediator, public enable_shared_from_this<SmartHomeHub> {
public:
    void registerDevice(shared_ptr<Device> device) override {
        devices_[device->getName()] = device;
        cout << "    [SmartHomeHub] 设备注册: " << device->getName() << endl;
    }

    void notify(shared_ptr<Device> sender, const string& event) override {
        string senderName = sender ? sender->getName() : "系统";

        if (event == "GOOD_NIGHT") {
            cout << endl;
            cout << "    ═══ [SmartHomeHub] 收到晚安模式指令，协调所有设备 ═══" << endl;
            for (auto& pair : devices_) {
                if (!sender || pair.first != sender->getName()) {
                    pair.second->receive(senderName, event);
                }
            }
            cout << "    ═══ [SmartHomeHub] 晚安模式部署完成 ═══" << endl;

        } else if (event == "SMOKE_ALARM") {
            cout << endl;
            cout << "    ═══ [SmartHomeHub] ⚠️ 紧急警报！协调所有设备应急响应 ═══" << endl;
            for (auto& pair : devices_) {
                if (!sender || pair.first != sender->getName()) {
                    pair.second->receive(senderName, event);
                }
            }
            cout << "    ═══ [SmartHomeHub] 应急响应部署完成，请注意安全！ ═══" << endl;

        } else if (event == "WAKE_UP") {
            cout << endl;
            cout << "    ═══ [SmartHomeHub] 收到起床模式指令，协调所有设备 ═══" << endl;
            for (auto& pair : devices_) {
                if (!sender || pair.first != sender->getName()) {
                    pair.second->receive(senderName, event);
                }
            }
            cout << "    ═══ [SmartHomeHub] 起床模式部署完成，美好的一天开始了！ ═══" << endl;

        } else {
            cout << "    [SmartHomeHub] 收到未知事件: " << event << "，忽略" << endl;
        }
    }

    // 打印所有设备状态
    void printStatus() const {
        cout << "    ┌── 设备状态一览 ──────────────────" << endl;
        for (const auto& pair : devices_) {
            cout << "    │ " << pair.first << ": "
                 << (pair.second->isOn() ? "🟢 开启" : "🔴 关闭") << endl;
        }
        cout << "    └──────────────────────────────────" << endl;
    }

private:
    unordered_map<string, shared_ptr<Device>> devices_;
};

// ======================== 辅助函数：分隔线 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "      中介者模式演示 - 智能家居系统       " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建中介者（智能中枢） ----------
    cout << endl;
    cout << ">>> 步骤一：创建智能家居中枢 <<<" << endl;

    auto hub = make_shared<SmartHomeHub>();

    // ---------- 2. 创建设备（同事对象）并注册到中介者 ----------
    cout << endl;
    cout << ">>> 步骤二：创建设备并注册 <<<" << endl;

    auto livingRoomLight = make_shared<Light>("客厅灯光", hub);
    auto bedroomLight = make_shared<Light>("卧室灯光", hub);
    auto ac = make_shared<AirConditioner>("空调", hub);
    auto curtain = make_shared<Curtain>("窗帘", hub);
    auto tv = make_shared<TV>("电视", hub);
    auto smokeDetector = make_shared<SmokeDetector>("烟雾报警器", hub);

    hub->registerDevice(livingRoomLight);
    hub->registerDevice(bedroomLight);
    hub->registerDevice(ac);
    hub->registerDevice(curtain);
    hub->registerDevice(tv);
    hub->registerDevice(smokeDetector);

    // ---------- 3. 手动控制各设备 ----------
    cout << endl;
    cout << ">>> 步骤三：手动开启各设备 <<<" << endl;

    printSection("开启客厅灯光和电视");
    livingRoomLight->turnOn();
    tv->turnOn();
    printSectionEnd();

    // 打印当前状态
    cout << endl;
    hub->printStatus();

    // ---------- 4. 晚安模式（通过中介者协调） ----------
    cout << endl;
    cout << ">>> 步骤四：晚安模式 - 通过中介者协调设备联动 <<<" << endl;

    printSection("用户说：\"晚安\"");
    cout << "    用户: \"小管家，帮我执行晚安模式\"" << endl;
    // 任意设备触发，中介者通知所有其他设备
    hub->notify(livingRoomLight, "GOOD_NIGHT");
    printSectionEnd();

    // 打印当前状态
    cout << endl;
    hub->printStatus();

    // ---------- 5. 起床模式 ----------
    cout << endl;
    cout << ">>> 步骤五：起床模式 - 中介者协调设备恢复 <<<" << endl;

    printSection("用户说：\"早安\"");
    cout << "    用户: \"小管家，我要起床了\"" << endl;
    hub->notify(tv, "WAKE_UP");
    printSectionEnd();

    // 打印当前状态
    cout << endl;
    hub->printStatus();

    // ---------- 6. 紧急情况：烟雾报警 ----------
    cout << endl;
    cout << ">>> 步骤六：紧急模式 - 烟雾报警器触发 <<<" << endl;

    printSection("烟雾报警器检测到烟雾");
    smokeDetector->detectSmoke();
    printSectionEnd();

    // 打印当前状态
    cout << endl;
    hub->printStatus();

    // ---------- 7. 展示中介者模式的价值 ----------
    cout << endl;
    cout << ">>> 中介者模式优势总结 <<<" << endl;
    cout << "  ├─ 松耦合：设备之间互不直接引用，只依赖中介者" << endl;
    cout << "  ├─ 集中控制：所有设备交互逻辑集中在中介者中管理" << endl;
    cout << "  ├─ 易扩展：新增设备只需注册到中介者，无需修改其他设备" << endl;
    cout << "  ├─ 联动灵活：通过事件机制，轻松实现场景化联动" << endl;
    cout << "  ├─ 单一职责：每个设备只关注自身功能，协调交给中介者" << endl;
    cout << "  └─ 场景驱动：晚安/起床/报警等场景可灵活组合设备响应" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}