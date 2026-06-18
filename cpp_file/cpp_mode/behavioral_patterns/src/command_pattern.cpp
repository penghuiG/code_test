/**
 * 命令模式 (Command Pattern)
 * 
 * 将请求封装为对象，从而使你可以用不同的请求对客户进行参数化，
 * 对请求排队或记录日志，以及支持可撤销的操作。
 * 
 * 本例以智能家居遥控器为场景，演示命令模式的核心结构：
 *   - Command（命令接口）
 *   - ConcreteCommand（具体命令）
 *   - Receiver（接收者，真正执行操作的对象）
 *   - Invoker（调用者，持有命令并触发执行）
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stack>
using namespace std;

// ======================== 命令接口 ========================
class Command {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual string description() const = 0;
    virtual ~Command() {}
};

// ======================== 接收者：电灯 ========================
class Light {
public:
    Light(const string& location) : location_(location), isOn_(false) {}

    void on() {
        isOn_ = true;
        cout << "  [" << location_ << " 灯] 已打开" << endl;
    }

    void off() {
        isOn_ = false;
        cout << "  [" << location_ << " 灯] 已关闭" << endl;
    }

    bool isOn() const { return isOn_; }

private:
    string location_;
    bool isOn_;
};

// ======================== 接收者：电视 ========================
class TV {
public:
    TV(const string& location) : location_(location), isOn_(false), channel_(1) {}

    void on() {
        isOn_ = true;
        cout << "  [" << location_ << " 电视] 已打开" << endl;
    }

    void off() {
        isOn_ = false;
        cout << "  [" << location_ << " 电视] 已关闭" << endl;
    }

    void setChannel(int ch) {
        channel_ = ch;
        cout << "  [" << location_ << " 电视] 切换到频道 " << channel_ << endl;
    }

    bool isOn() const { return isOn_; }
    int getChannel() const { return channel_; }

private:
    string location_;
    bool isOn_;
    int channel_;
};

// ======================== 具体命令：开灯 ========================
class LightOnCommand : public Command {
public:
    LightOnCommand(shared_ptr<Light> light) : light_(light) {}

    void execute() override {
        light_->on();
    }

    void undo() override {
        light_->off();
    }

    string description() const override {
        return "开灯";
    }

private:
    shared_ptr<Light> light_;
};

// ======================== 具体命令：关灯 ========================
class LightOffCommand : public Command {
public:
    LightOffCommand(shared_ptr<Light> light) : light_(light) {}

    void execute() override {
        light_->off();
    }

    void undo() override {
        light_->on();
    }

    string description() const override {
        return "关灯";
    }

private:
    shared_ptr<Light> light_;
};

// ======================== 具体命令：开电视 ========================
class TVOnCommand : public Command {
public:
    TVOnCommand(shared_ptr<TV> tv) : tv_(tv) {}

    void execute() override {
        tv_->on();
    }

    void undo() override {
        tv_->off();
    }

    string description() const override {
        return "开电视";
    }

private:
    shared_ptr<TV> tv_;
};

// ======================== 具体命令：关电视 ========================
class TVOffCommand : public Command {
public:
    TVOffCommand(shared_ptr<TV> tv) : tv_(tv) {}

    void execute() override {
        tv_->off();
    }

    void undo() override {
        tv_->on();
    }

    string description() const override {
        return "关电视";
    }

private:
    shared_ptr<TV> tv_;
};

// ======================== 具体命令：换台 ========================
class TVSetChannelCommand : public Command {
public:
    TVSetChannelCommand(shared_ptr<TV> tv, int channel)
        : tv_(tv), newChannel_(channel), prevChannel_(1) {}

    void execute() override {
        prevChannel_ = tv_->getChannel();
        tv_->setChannel(newChannel_);
    }

    void undo() override {
        tv_->setChannel(prevChannel_);
    }

    string description() const override {
        return "电视换台到 " + to_string(newChannel_);
    }

private:
    shared_ptr<TV> tv_;
    int newChannel_;
    int prevChannel_;
};

// ======================== 空命令（空对象模式，避免空指针检查） ========================
class NoCommand : public Command {
public:
    void execute() override {
        cout << "  [无操作]" << endl;
    }
    void undo() override {
        cout << "  [无操作]" << endl;
    }
    string description() const override {
        return "无操作";
    }
};

// ======================== 调用者：遥控器 ========================
class RemoteControl {
public:
    RemoteControl(int slotCount) : slots_(slotCount, make_shared<NoCommand>()) {}

    // 设置某个插槽的命令
    void setCommand(int slot, shared_ptr<Command> cmd) {
        if (slot >= 0 && slot < static_cast<int>(slots_.size())) {
            slots_[slot] = cmd;
        }
    }

    // 按下按钮执行命令
    void pressButton(int slot) {
        if (slot >= 0 && slot < static_cast<int>(slots_.size())) {
            cout << "[遥控器] 按下按钮 " << slot
                 << " (" << slots_[slot]->description() << "):" << endl;
            slots_[slot]->execute();
            undoStack_.push(slots_[slot]);
        }
    }

    // 按下撤销按钮
    void pressUndo() {
        if (!undoStack_.empty()) {
            cout << "[遥控器] 撤销 (" << undoStack_.top()->description() << "):" << endl;
            undoStack_.top()->undo();
            undoStack_.pop();
        } else {
            cout << "[遥控器] 没有可撤销的操作" << endl;
        }
    }

    // 显示当前配置
    void showConfig() const {
        cout << "[遥控器配置]" << endl;
        for (int i = 0; i < static_cast<int>(slots_.size()); ++i) {
            cout << "  按钮 " << i << ": " << slots_[i]->description() << endl;
        }
    }

private:
    vector<shared_ptr<Command>> slots_;
    stack<shared_ptr<Command>> undoStack_;
};

// ======================== 宏命令：一次执行多个命令 ========================
class MacroCommand : public Command {
public:
    MacroCommand(const string& name, vector<shared_ptr<Command>> cmds)
        : name_(name), commands_(cmds) {}

    void execute() override {
        cout << "  [宏命令: " << name_ << "] 开始执行:" << endl;
        for (auto& cmd : commands_) {
            cmd->execute();
        }
    }

    void undo() override {
        cout << "  [宏命令: " << name_ << "] 撤销:" << endl;
        // 按逆序撤销
        for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
            (*it)->undo();
        }
    }

    string description() const override {
        return "宏命令: " + name_;
    }

private:
    string name_;
    vector<shared_ptr<Command>> commands_;
};

// ======================== 客户端 ========================
int main() {
    // 创建接收者
    auto livingRoomLight = make_shared<Light>("客厅");
    auto bedroomLight = make_shared<Light>("卧室");
    auto livingRoomTV = make_shared<TV>("客厅");

    // 创建具体命令
    auto livingRoomLightOn = make_shared<LightOnCommand>(livingRoomLight);
    auto livingRoomLightOff = make_shared<LightOffCommand>(livingRoomLight);
    auto bedroomLightOn = make_shared<LightOnCommand>(bedroomLight);
    auto bedroomLightOff = make_shared<LightOffCommand>(bedroomLight);
    auto tvOn = make_shared<TVOnCommand>(livingRoomTV);
    auto tvOff = make_shared<TVOffCommand>(livingRoomTV);
    auto tvSetCh5 = make_shared<TVSetChannelCommand>(livingRoomTV, 5);

    // 创建遥控器（4个按钮）
    RemoteControl remote(4);

    // 配置遥控器
    remote.setCommand(0, livingRoomLightOn);
    remote.setCommand(1, livingRoomLightOff);
    remote.setCommand(2, tvOn);
    remote.setCommand(3, tvSetCh5);

    cout << "========================================" << endl;
    cout << "       命令模式演示 - 智能家居遥控器      " << endl;
    cout << "========================================" << endl;

    // 显示配置
    cout << endl;
    remote.showConfig();

    // 执行命令
    cout << endl;
    cout << "--- 执行命令 ---" << endl;
    remote.pressButton(0);  // 开客厅灯
    remote.pressButton(2);  // 开电视
    remote.pressButton(3);  // 换台到5
    remote.pressButton(1);  // 关客厅灯

    // 撤销操作
    cout << endl;
    cout << "--- 撤销操作 ---" << endl;
    remote.pressUndo();  // 撤销关灯
    remote.pressUndo();  // 撤销换台
    remote.pressUndo();  // 撤销开电视

    // 宏命令演示
    cout << endl;
    cout << "--- 宏命令演示：睡前模式 ---" << endl;
    vector<shared_ptr<Command>> sleepCommands = {
        make_shared<LightOffCommand>(livingRoomLight),
        make_shared<LightOffCommand>(bedroomLight),
        make_shared<TVOffCommand>(livingRoomTV)
    };
    auto sleepMacro = make_shared<MacroCommand>("睡前模式", sleepCommands);

    // 先开灯和电视
    cout << endl;
    cout << "--- 当前状态 ---" << endl;
    livingRoomLight->on();
    bedroomLight->on();
    livingRoomTV->on();

    cout << endl;
    cout << "--- 执行睡前宏命令 ---" << endl;
    sleepMacro->execute();

    cout << endl;
    cout << "--- 撤销睡前宏命令 ---" << endl;
    sleepMacro->undo();

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}