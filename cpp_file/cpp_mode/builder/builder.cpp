/**
 * 建造者模式 (Builder Pattern)
 * 
 * 将一个复杂对象的构建与其表示分离，使得同样的构建过程可以创建不同的表示。
 * 建造者模式一步步构建一个复杂对象，允许用户只通过指定复杂对象的类型和内容
 * 就可以构建它们，而不需要知道内部的具体构建细节。
 * 
 * 本例以组装电脑为场景，演示建造者模式的核心结构：
 *   - Product（产品，被构建的复杂对象）
 *   - Builder（抽象建造者，定义构建步骤的接口）
 *   - ConcreteBuilder（具体建造者，实现构建步骤）
 *   - Director（指挥者，控制构建过程的顺序）
 */

#include <iostream>
#include <memory>
#include <string>
using namespace std;

// ======================== 产品：电脑 ========================
class Computer {
public:
    void setCPU(const string& cpu) { cpu_ = cpu; }
    void setGPU(const string& gpu) { gpu_ = gpu; }
    void setRAM(const string& ram) { ram_ = ram; }
    void setStorage(const string& storage) { storage_ = storage; }
    void setMainboard(const string& mainboard) { mainboard_ = mainboard; }
    void setPowerSupply(const string& power) { powerSupply_ = power; }

    void show() const {
        cout << "  ┌─────────────────────────────────┐" << endl;
        cout << "  │         电脑配置清单             │" << endl;
        cout << "  ├─────────────────────────────────┤" << endl;
        cout << "  │ 主板: " << mainboard_ << endl;
        cout << "  │ CPU:  " << cpu_ << endl;
        cout << "  │ GPU:  " << gpu_ << endl;
        cout << "  │ 内存: " << ram_ << endl;
        cout << "  │ 硬盘: " << storage_ << endl;
        cout << "  │ 电源: " << powerSupply_ << endl;
        cout << "  └─────────────────────────────────┘" << endl;
    }

private:
    string cpu_;
    string gpu_;
    string ram_;
    string storage_;
    string mainboard_;
    string powerSupply_;
};

// ======================== 抽象建造者 ========================
class ComputerBuilder {
public:
    virtual ~ComputerBuilder() {}
    virtual void buildCPU() = 0;
    virtual void buildGPU() = 0;
    virtual void buildRAM() = 0;
    virtual void buildStorage() = 0;
    virtual void buildMainboard() = 0;
    virtual void buildPowerSupply() = 0;

    // 获取构建好的产品
    shared_ptr<Computer> getResult() { return computer_; }

protected:
    shared_ptr<Computer> computer_ = make_shared<Computer>();
};

// ======================== 具体建造者：游戏电脑 ========================
class GamingComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer_->setCPU("Intel Core i9-14900K");
        cout << "  [游戏电脑] 安装高性能 CPU" << endl;
    }

    void buildGPU() override {
        computer_->setGPU("NVIDIA RTX 4090 24GB");
        cout << "  [游戏电脑] 安装旗舰级显卡" << endl;
    }

    void buildRAM() override {
        computer_->setRAM("DDR5 64GB (32GB x 2) 6000MHz");
        cout << "  [游戏电脑] 安装大容量高频内存" << endl;
    }

    void buildStorage() override {
        computer_->setStorage("2TB NVMe SSD (PCIe 5.0)");
        cout << "  [游戏电脑] 安装高速固态硬盘" << endl;
    }

    void buildMainboard() override {
        computer_->setMainboard("ASUS ROG MAXIMUS Z790 HERO");
        cout << "  [游戏电脑] 安装高端电竞主板" << endl;
    }

    void buildPowerSupply() override {
        computer_->setPowerSupply("1200W 80Plus 金牌全模组");
        cout << "  [游戏电脑] 安装大功率电源" << endl;
    }
};

// ======================== 具体建造者：办公电脑 ========================
class OfficeComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer_->setCPU("Intel Core i5-14400");
        cout << "  [办公电脑] 安装主流级 CPU" << endl;
    }

    void buildGPU() override {
        computer_->setGPU("Intel UHD Graphics 730 (集成显卡)");
        cout << "  [办公电脑] 使用集成显卡" << endl;
    }

    void buildRAM() override {
        computer_->setRAM("DDR5 16GB (8GB x 2) 4800MHz");
        cout << "  [办公电脑] 安装够用的内存" << endl;
    }

    void buildStorage() override {
        computer_->setStorage("512GB NVMe SSD");
        cout << "  [办公电脑] 安装主流固态硬盘" << endl;
    }

    void buildMainboard() override {
        computer_->setMainboard("MSI PRO B760M-A WIFI");
        cout << "  [办公电脑] 安装实用型主板" << endl;
    }

    void buildPowerSupply() override {
        computer_->setPowerSupply("550W 80Plus 铜牌");
        cout << "  [办公电脑] 安装节能电源" << endl;
    }
};

// ======================== 具体建造者：工作站电脑 ========================
class WorkstationBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer_->setCPU("AMD Ryzen Threadripper PRO 7995WX (96核)");
        cout << "  [工作站] 安装专业级多核 CPU" << endl;
    }

    void buildGPU() override {
        computer_->setGPU("NVIDIA RTX A6000 48GB");
        cout << "  [工作站] 安装专业图形卡" << endl;
    }

    void buildRAM() override {
        computer_->setRAM("DDR5 ECC 256GB (32GB x 8) 5600MHz");
        cout << "  [工作站] 安装超大容量 ECC 内存" << endl;
    }

    void buildStorage() override {
        computer_->setStorage("4TB NVMe SSD + 16TB HDD RAID1");
        cout << "  [工作站] 安装混合存储方案" << endl;
    }

    void buildMainboard() override {
        computer_->setMainboard("ASUS WRX90E SAGE SE");
        cout << "  [工作站] 安装服务器级主板" << endl;
    }

    void buildPowerSupply() override {
        computer_->setPowerSupply("1600W 80Plus 铂金全模组");
        cout << "  [工作站] 安装高可靠电源" << endl;
    }
};

// ======================== 指挥者：电脑组装师 ========================
class ComputerAssembler {
public:
    // 设置建造者
    void setBuilder(shared_ptr<ComputerBuilder> builder) {
        builder_ = builder;
    }

    // 按照固定流程组装电脑（构建过程相同，但结果取决于具体建造者）
    shared_ptr<Computer> assemble() {
        if (!builder_) {
            cout << "[组装师] 错误：未设置建造者！" << endl;
            return nullptr;
        }

        cout << "[组装师] 开始组装电脑..." << endl;
        cout << "  ------ 步骤 1: 安装主板 ------" << endl;
        builder_->buildMainboard();
        cout << "  ------ 步骤 2: 安装 CPU ------" << endl;
        builder_->buildCPU();
        cout << "  ------ 步骤 3: 安装内存 ------" << endl;
        builder_->buildRAM();
        cout << "  ------ 步骤 4: 安装显卡 ------" << endl;
        builder_->buildGPU();
        cout << "  ------ 步骤 5: 安装硬盘 ------" << endl;
        builder_->buildStorage();
        cout << "  ------ 步骤 6: 安装电源 ------" << endl;
        builder_->buildPowerSupply();
        cout << "[组装师] 组装完成！" << endl;

        return builder_->getResult();
    }

private:
    shared_ptr<ComputerBuilder> builder_;
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "       建造者模式演示 - 电脑组装         " << endl;
    cout << "========================================" << endl;

    // 创建指挥者
    auto assembler = make_shared<ComputerAssembler>();

    // ---------- 1. 组装游戏电脑 ----------
    cout << endl;
    cout << ">>> 场景一：组装一台游戏电脑 <<<" << endl;
    auto gamingBuilder = make_shared<GamingComputerBuilder>();
    assembler->setBuilder(gamingBuilder);
    auto gamingPC = assembler->assemble();
    cout << endl;
    gamingPC->show();

    // ---------- 2. 组装办公电脑 ----------
    cout << endl;
    cout << ">>> 场景二：组装一台办公电脑 <<<" << endl;
    auto officeBuilder = make_shared<OfficeComputerBuilder>();
    assembler->setBuilder(officeBuilder);
    auto officePC = assembler->assemble();
    cout << endl;
    officePC->show();

    // ---------- 3. 组装工作站 ----------
    cout << endl;
    cout << ">>> 场景三：组装一台工作站 <<<" << endl;
    auto workstationBuilder = make_shared<WorkstationBuilder>();
    assembler->setBuilder(workstationBuilder);
    auto workstation = assembler->assemble();
    cout << endl;
    workstation->show();

    // ---------- 4. 演示不使用指挥者，直接使用建造者 ----------
    cout << endl;
    cout << ">>> 场景四：客户自己组装（不经过指挥者） <<<" << endl;
    cout << "[客户] 我要自己选配件组装一台混合型电脑！" << endl;
    auto customBuilder = make_shared<GamingComputerBuilder>();
    customBuilder->buildMainboard();
    customBuilder->buildCPU();
    customBuilder->buildRAM();
    customBuilder->buildGPU();
    customBuilder->buildStorage();
    customBuilder->buildPowerSupply();
    auto customPC = customBuilder->getResult();
    cout << endl;
    customPC->show();

    cout << endl;

    return 0;
}