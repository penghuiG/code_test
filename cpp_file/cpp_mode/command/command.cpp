/**
 * 命令模式 (Command Pattern)
 * 
 * 将一个请求封装为一个对象，从而使你可以用不同的请求对客户进行参数化，
 * 对请求排队或记录日志，以及支持可撤销的操作。
 * 
 * 本例以智能家居遥控器为场景，演示命令模式的核心结构：
 *   - Command（抽象命令）：Command，定义执行和撤销的接口
 *   - ConcreteCommand（具体命令）：
 *     · LightOnCommand：开灯命令
 *     · LightOffCommand：关灯命令
 *     · AirConditionerOnCommand：开空调命令
 *     · AirConditionerOffCommand：关空调命令
 *     · TVOnCommand：开电视命令
 *     · TVOffCommand：关电视命令
 *   - Receiver（接收者）：Light、AirConditioner、TV，真正执行业务逻辑的对象
 *   - Invoker（调用者）：RemoteControl，持有命令对象并触发执行
 *   - 命令历史记录：支持撤销操作（Undo）
 * 
 * 场景说明：
 *   用户通过智能家居遥控器控制家中的各种设备。
 *   遥控器上的每个按钮对应一个命令对象，用户按下按钮时，
 *   命令对象会调用对应设备的方法来执行操作。
 *   同时支持撤销上一步操作，实现"后悔"功能。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <stack>
using namespace std;

// ======================== 接收者：电灯 ========================
class Light {
public:
    Light(const string& location) : location_(location), isOn_(false) {}

    void on() {
        isOn_ = true;
        cout << "    💡 " << location_ << " 的灯已打开" << endl;
    }

    void off() {
        isOn_ = false;
        cout << "    💡 " << location_ << " 的灯已关闭" << endl;
    }

    bool isOn() const { return isOn_; }
    string getLocation() const { return location_; }

private:
    string location_;
    bool isOn_;
};

// ======================== 接收者：空调 ========================
class AirConditioner {
public:
    AirConditioner(const string& location) : location_(location), isOn_(false), temperature_(26) {}

    void on() {
        isOn_ = true;
        cout << "    ❄️  " << location_ << " 的空调已打开，温度: " << temperature_ << "°C" << endl;
    }

    void off() {
        isOn_ = false;
        cout << "    ❄️  " << location_ << " 的空调已关闭" << endl;
    }

    void setTemperature(int temp) {
        temperature_ = temp;
        if (isOn_) {
            cout << "    ❄️  " << location_ << " 的空调温度已设置为: " << temperature_ << "°C" << endl;
        }
    }

    int getTemperature() const { return temperature_; }
    bool isOn() const { return isOn_; }
    string getLocation() const { return location_; }

private:
    string location_;
    bool isOn_;
    int temperature_;
};

// ======================== 接收者：电视 ========================
class TV {
public:
    TV(const string& location) : location_(location), isOn_(false), channel_(1), volume_(30) {}

    void on() {
        isOn_ = true;
        cout << "    📺 " << location_ << " 的电视已打开，频道: " << channel_
             << "，音量: " << volume_ << endl;
    }

    void off() {
        isOn_ = false;
        cout << "    📺 " << location_ << " 的电视已关闭" << endl;
    }

    void setChannel(int ch) {
        channel_ = ch;
        if (isOn_) {
            cout << "    📺 " << location_ << " 的电视频道已切换到: " << channel_ << endl;
        }
    }

    void setVolume(int vol) {
        volume_ = vol;
        if (isOn_) {
            cout << "    📺 " << location_ << " 的电视音量已设置为: " << volume_ << endl;
        }
    }

    bool isOn() const { return isOn_; }
    int getChannel() const { return channel_; }
    int getVolume() const { return volume_; }
    string getLocation() const { return location_; }

private:
    string location_;
    bool isOn_;
    int channel_;
    int volume_;
};

// ======================== 抽象命令：Command ========================
class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual string getDescription() const = 0;
};

// ======================== 具体命令：开灯命令 ========================
class LightOnCommand : public Command {
public:
    LightOnCommand(shared_ptr<Light> light) : light_(light) {}

    void execute() override {
        light_->on();
    }

    void undo() override {
        light_->off();
    }

    string getDescription() const override {
        return "开灯 (" + light_->getLocation() + ")";
    }

private:
    shared_ptr<Light> light_;
};

// ======================== 具体命令：关灯命令 ========================
class LightOffCommand : public Command {
public:
    LightOffCommand(shared_ptr<Light> light) : light_(light) {}

    void execute() override {
        light_->off();
    }

    void undo() override {
        light_->on();
    }

    string getDescription() const override {
        return "关灯 (" + light_->getLocation() + ")";
    }

private:
    shared_ptr<Light> light_;
};

// ======================== 具体命令：开空调命令 ========================
class AirConditionerOnCommand : public Command {
public:
    AirConditionerOnCommand(shared_ptr<AirConditioner> ac) : ac_(ac) {}

    void execute() override {
        ac_->on();
    }

    void undo() override {
        ac_->off();
    }

    string getDescription() const override {
        return "开空调 (" + ac_->getLocation() + ")";
    }

private:
    shared_ptr<AirConditioner> ac_;
};

// ======================== 具体命令：关空调命令 ========================
class AirConditionerOffCommand : public Command {
public:
    AirConditionerOffCommand(shared_ptr<AirConditioner> ac) : ac_(ac) {}

    void execute() override {
        ac_->off();
    }

    void undo() override {
        ac_->on();
    }

    string getDescription() const override {
        return "关空调 (" + ac_->getLocation() + ")";
    }

private:
    shared_ptr<AirConditioner> ac_;
};

// ======================== 具体命令：设置空调温度命令 ========================
class AirConditionerTempCommand : public Command {
public:
    AirConditionerTempCommand(shared_ptr<AirConditioner> ac, int newTemp)
        : ac_(ac), newTemp_(newTemp), prevTemp_(ac->getTemperature()) {}

    void execute() override {
        prevTemp_ = ac_->getTemperature();
        ac_->setTemperature(newTemp_);
    }

    void undo() override {
        ac_->setTemperature(prevTemp_);
    }

    string getDescription() const override {
        return "设置空调温度 (" + ac_->getLocation() + ") -> " + to_string(newTemp_) + "°C";
    }

private:
    shared_ptr<AirConditioner> ac_;
    int newTemp_;
    int prevTemp_;
};

// ======================== 具体命令：开电视命令 ========================
class TVOnCommand : public Command {
public:
    TVOnCommand(shared_ptr<TV> tv) : tv_(tv) {}

    void execute() override {
        tv_->on();
    }

    void undo() override {
        tv_->off();
    }

    string getDescription() const override {
        return "开电视 (" + tv_->getLocation() + ")";
    }

private:
    shared_ptr<TV> tv_;
};

// ======================== 具体命令：关电视命令 ========================
class TVOffCommand : public Command {
public:
    TVOffCommand(shared_ptr<TV> tv) : tv_(tv) {}

    void execute() override {
        tv_->off();
    }

    void undo() override {
        tv_->on();
    }

    string getDescription() const override {
        return "关电视 (" + tv_->getLocation() + ")";
    }

private:
    shared_ptr<TV> tv_;
};

// ======================== 具体命令：切换电视频道命令 ========================
class TVChannelCommand : public Command {
public:
    TVChannelCommand(shared_ptr<TV> tv, int channel)
        : tv_(tv), newChannel_(channel), prevChannel_(tv->getChannel()) {}

    void execute() override {
        prevChannel_ = tv_->getChannel();
        tv_->setChannel(newChannel_);
    }

    void undo() override {
        tv_->setChannel(prevChannel_);
    }

    string getDescription() const override {
        return "切换电视频道 (" + tv_->getLocation() + ") -> 频道" + to_string(newChannel_);
    }

private:
    shared_ptr<TV> tv_;
    int newChannel_;
    int prevChannel_;
};

// ======================== 宏命令：一键执行多个命令 ========================
class MacroCommand : public Command {
public:
    MacroCommand(const string& desc, vector<shared_ptr<Command>> commands)
        : desc_(desc), commands_(commands) {}

    void execute() override {
        for (auto& cmd : commands_) {
            cmd->execute();
        }
    }

    void undo() override {
        // 撤销时按相反顺序执行
        for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
            (*it)->undo();
        }
    }

    string getDescription() const override {
        return desc_;
    }

private:
    string desc_;
    vector<shared_ptr<Command>> commands_;
};

// ======================== 调用者：智能家居遥控器 ========================
class RemoteControl {
public:
    RemoteControl() : commandCount_(0) {
        cout << "  🎮 智能家居遥控器已初始化" << endl;
    }

    // 设置命令
    void setCommand(shared_ptr<Command> cmd) {
        currentCommand_ = cmd;
        commandCount_++;
    }

    // 执行当前命令
    void pressButton() {
        if (currentCommand_) {
            cout << "  ▶️  执行命令: " << currentCommand_->getDescription() << endl;
            currentCommand_->execute();
            history_.push(currentCommand_);
        } else {
            cout << "  ⚠️  没有设置命令" << endl;
        }
    }

    // 撤销上一个命令
    void pressUndo() {
        if (!history_.empty()) {
            auto lastCmd = history_.top();
            history_.pop();
            cout << "  ⏪ 撤销命令: " << lastCmd->getDescription() << endl;
            lastCmd->undo();
        } else {
            cout << "  ⚠️  没有可撤销的命令" << endl;
        }
    }

    // 获取历史记录
    void printHistory() const {
        if (history_.empty()) {
            cout << "    暂无历史记录" << endl;
            return;
        }
        // 临时栈用于遍历
        stack<shared_ptr<Command>> temp = history_;
        vector<shared_ptr<Command>> cmds;
        while (!temp.empty()) {
            cmds.push_back(temp.top());
            temp.pop();
        }
        // 从最早到最近打印
        for (int i = cmds.size() - 1; i >= 0; --i) {
            cout << "    " << (cmds.size() - i) << ". " << cmds[i]->getDescription() << endl;
        }
    }

    int getHistorySize() const { return history_.size(); }

private:
    shared_ptr<Command> currentCommand_;
    stack<shared_ptr<Command>> history_;
    int commandCount_;
};

// ======================== 辅助函数：打印分隔线 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "============================================" << endl;
    cout << "   命令模式演示 - 智能家居遥控器系统        " << endl;
    cout << "============================================" << endl;

    // ---------- 1. 创建接收者（智能设备）----------
    cout << endl;
    cout << ">>> 步骤一：创建智能家居设备 <<<" << endl;

    auto livingRoomLight = make_shared<Light>("客厅");
    auto bedroomLight = make_shared<Light>("卧室");
    auto livingRoomAC = make_shared<AirConditioner>("客厅");
    auto livingRoomTV = make_shared<TV>("客厅");

    cout << "  已创建设备: 客厅灯、卧室灯、客厅空调、客厅电视" << endl;

    // ---------- 2. 创建命令对象 ----------
    cout << endl;
    cout << ">>> 步骤二：创建命令对象 <<<" << endl;

    auto livingRoomLightOn = make_shared<LightOnCommand>(livingRoomLight);
    auto livingRoomLightOff = make_shared<LightOffCommand>(livingRoomLight);
    auto bedroomLightOn = make_shared<LightOnCommand>(bedroomLight);
    auto bedroomLightOff = make_shared<LightOffCommand>(bedroomLight);
    auto acOn = make_shared<AirConditionerOnCommand>(livingRoomAC);
    auto acOff = make_shared<AirConditionerOffCommand>(livingRoomAC);
    auto acTempCmd = make_shared<AirConditionerTempCommand>(livingRoomAC, 22);
    auto tvOn = make_shared<TVOnCommand>(livingRoomTV);
    auto tvOff = make_shared<TVOffCommand>(livingRoomTV);
    auto tvChannelCmd = make_shared<TVChannelCommand>(livingRoomTV, 8);

    cout << "  已创建10个命令对象（开/关灯、开/关空调、开/关电视、调温、换台等）" << endl;

    // ---------- 3. 创建遥控器并执行命令 ----------
    cout << endl;
    cout << ">>> 步骤三：通过遥控器执行命令 <<<" << endl;

    RemoteControl remote;

    // 开灯
    printSection("按下按钮 - 打开客厅灯");
    remote.setCommand(livingRoomLightOn);
    remote.pressButton();
    printSectionEnd();

    // 开空调
    printSection("按下按钮 - 打开客厅空调");
    remote.setCommand(acOn);
    remote.pressButton();
    printSectionEnd();

    // 调节空调温度
    printSection("按下按钮 - 设置空调温度为22°C");
    remote.setCommand(acTempCmd);
    remote.pressButton();
    printSectionEnd();

    // 开电视
    printSection("按下按钮 - 打开客厅电视");
    remote.setCommand(tvOn);
    remote.pressButton();
    printSectionEnd();

    // 切换电视频道
    printSection("按下按钮 - 切换到频道8");
    remote.setCommand(tvChannelCmd);
    remote.pressButton();
    printSectionEnd();

    // ---------- 4. 演示撤销功能 ----------
    cout << endl;
    cout << ">>> 步骤四：演示撤销（Undo）功能 <<<" << endl;
    cout << "  依次撤销最近的操作：" << endl;

    // 撤销换台
    printSection("按下撤销键 #1");
    remote.pressUndo();
    printSectionEnd();

    // 撤销开电视
    printSection("按下撤销键 #2");
    remote.pressUndo();
    printSectionEnd();

    // 撤销调温
    printSection("按下撤销键 #3");
    remote.pressUndo();
    printSectionEnd();

    // ---------- 5. 宏命令：一键回家模式 ----------
    cout << endl;
    cout << ">>> 步骤五：宏命令 - 一键回家模式 <<<" << endl;
    cout << "  创建宏命令，同时执行多个操作：" << endl;

    vector<shared_ptr<Command>> homeModeCommands = {
        livingRoomLightOn,
        bedroomLightOn,
        acOn,
        tvOn
    };
    auto homeMode = make_shared<MacroCommand>("🏠 一键回家模式", homeModeCommands);

    // 先把设备都关掉，模拟外出状态
    cout << endl;
    cout << "  [模拟] 先关闭所有设备..." << endl;
    livingRoomLight->off();
    bedroomLight->off();
    livingRoomAC->off();
    livingRoomTV->off();

    printSection("按下宏命令按钮 - 一键回家模式");
    remote.setCommand(homeMode);
    remote.pressButton();
    printSectionEnd();

    // 一键撤销回家模式
    printSection("按下撤销键 - 撤销回家模式");
    remote.pressUndo();
    printSectionEnd();

    // ---------- 6. 宏命令：一键离家模式 ----------
    cout << endl;
    cout << ">>> 步骤六：宏命令 - 一键离家模式 <<<" << endl;

    vector<shared_ptr<Command>> leaveModeCommands = {
        livingRoomLightOff,
        bedroomLightOff,
        acOff,
        tvOff
    };
    auto leaveMode = make_shared<MacroCommand>("🚪 一键离家模式", leaveModeCommands);

    // 先把设备都打开，模拟在家状态
    cout << endl;
    cout << "  [模拟] 先打开所有设备..." << endl;
    livingRoomLight->on();
    bedroomLight->on();
    livingRoomAC->on();
    livingRoomTV->on();

    printSection("按下宏命令按钮 - 一键离家模式");
    remote.setCommand(leaveMode);
    remote.pressButton();
    printSectionEnd();

    // ---------- 7. 打印历史记录 ----------
    cout << endl;
    cout << ">>> 步骤七：查看命令执行历史 <<<" << endl;
    cout << "  已执行的命令记录:" << endl;
    remote.printHistory();

    // ---------- 8. 命令模式总结 ----------
    cout << endl;
    cout << ">>> 命令模式优势总结 <<<" << endl;
    cout << "  ├─ 解耦：调用者（遥控器）与接收者（设备）完全解耦" << endl;
    cout << "  │  （遥控器不知道具体控制的是什么设备）" << endl;
    cout << "  ├─ 可撤销：通过 undo 操作轻松实现撤销功能" << endl;
    cout << "  │  （每条命令都知道如何撤销自己）" << endl;
    cout << "  ├─ 宏命令：可以将多个命令组合成一个复合命令" << endl;
    cout << "  │  （一键回家/离家模式）" << endl;
    cout << "  ├─ 可扩展：新增设备只需新增命令类，无需修改已有代码" << endl;
    cout << "  │  （开闭原则）" << endl;
    cout << "  └─ 命令队列：可以将命令存储在队列中，延迟或批量执行" << endl;
    cout << "     （支持日志记录、事务回滚等高级功能）" << endl;

    cout << endl;
    cout << "============================================" << endl;
    cout << "                演示结束                     " << endl;
    cout << "============================================" << endl;

    return 0;
}