/**
 * 原型模式 (Prototype Pattern)
 * 
 * 用原型实例指定创建对象的种类，并且通过拷贝（克隆）这些原型创建新的对象。
 * 原型模式允许在不依赖类构造函数的情况下创建对象，特别适用于：
 *   - 创建对象成本较大（如涉及网络、数据库操作）
 *   - 需要大量相似对象的场景
 *   - 需要避免构造函数的复杂初始化流程
 * 
 * 本例以游戏中的怪物/角色系统为场景，演示原型模式的核心结构：
 *   - Prototype（抽象原型，声明克隆接口）
 *   - ConcretePrototype（具体原型，实现克隆方法）
 *   - PrototypeRegistry（原型注册表，管理预定义的原型实例）
 */

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// ======================== 技能 ========================
struct Skill {
    string name;
    int damage;
    float cooldown;

    void show() const {
        cout << "    技能: " << name
             << " | 伤害: " << damage
             << " | 冷却: " << cooldown << "s" << endl;
    }
};

// ======================== 抽象原型 ========================
class Monster {
public:
    virtual ~Monster() {}
    virtual shared_ptr<Monster> clone() const = 0;
    virtual void show() const = 0;
    virtual void setName(const string& name) = 0;
};

// ======================== 具体原型：哥布林 ========================
class Goblin : public Monster {
public:
    Goblin() = default;
    Goblin(const string& name, int hp, int atk, int def, vector<Skill> skills)
        : name_(name), hp_(hp), atk_(atk), def_(def), skills_(skills) {}

    // 深拷贝克隆
    shared_ptr<Monster> clone() const override {
        // 调用拷贝构造函数，vector 和 string 会自动深拷贝
        return make_shared<Goblin>(*this);
    }

    void setName(const string& name) override { name_ = name; }

    void show() const override {
        cout << "  ┌──────────────────────────────────┐" << endl;
        cout << "  │ 哥布林: " << name_ << endl;
        cout << "  ├──────────────────────────────────┤" << endl;
        cout << "  │ HP: " << hp_ << " | ATK: " << atk_ << " | DEF: " << def_ << endl;
        for (const auto& skill : skills_) {
            skill.show();
        }
        cout << "  └──────────────────────────────────┘" << endl;
    }

private:
    string name_ = "哥布林";
    int hp_ = 100;
    int atk_ = 15;
    int def_ = 5;
    vector<Skill> skills_ = {{"砍击", 20, 2.0f}, {"逃跑", 0, 0.5f}};
};

// ======================== 具体原型：龙 ========================
class Dragon : public Monster {
public:
    Dragon() = default;
    Dragon(const string& name, int hp, int atk, int def, vector<Skill> skills)
        : name_(name), hp_(hp), atk_(atk), def_(def), skills_(skills) {}

    shared_ptr<Monster> clone() const override {
        return make_shared<Dragon>(*this);
    }

    void setName(const string& name) override { name_ = name; }

    void show() const override {
        cout << "  ┌──────────────────────────────────┐" << endl;
        cout << "  │ 巨龙: " << name_ << endl;
        cout << "  ├──────────────────────────────────┤" << endl;
        cout << "  │ HP: " << hp_ << " | ATK: " << atk_ << " | DEF: " << def_ << endl;
        for (const auto& skill : skills_) {
            skill.show();
        }
        cout << "  └──────────────────────────────────┘" << endl;
    }

private:
    string name_ = "远古巨龙";
    int hp_ = 5000;
    int atk_ = 300;
    int def_ = 200;
    vector<Skill> skills_ = {
        {"火焰吐息", 500, 5.0f},
        {"尾扫", 200, 3.0f},
        {"飞扑", 350, 8.0f}
    };
};

// ======================== 具体原型：骷髅兵 ========================
class Skeleton : public Monster {
public:
    Skeleton() = default;
    Skeleton(const string& name, int hp, int atk, int def, vector<Skill> skills)
        : name_(name), hp_(hp), atk_(atk), def_(def), skills_(skills) {}

    shared_ptr<Monster> clone() const override {
        return make_shared<Skeleton>(*this);//这里为什么是深拷贝？因为vector和string在C++中默认是深拷贝的，所以直接使用拷贝构造函数即可实现深拷贝。
    }

    void setName(const string& name) override { name_ = name; }

    void show() const override {
        cout << "  ┌──────────────────────────────────┐" << endl;
        cout << "  │ 骷髅兵: " << name_ << endl;
        cout << "  ├──────────────────────────────────┤" << endl;
        cout << "  │ HP: " << hp_ << " | ATK: " << atk_ << " | DEF: " << def_ << endl;
        for (const auto& skill : skills_) {
            skill.show();
        }
        cout << "  └──────────────────────────────────┘" << endl;
    }

private:
    string name_ = "骷髅兵";
    int hp_ = 80;
    int atk_ = 20;
    int def_ = 2;
    vector<Skill> skills_ = {{"骨剑斩", 25, 1.5f}};
};

// ======================== 原型注册表（管理预定义原型） ========================
class PrototypeRegistry {
public:
    // 注册原型
    void registerPrototype(const string& key, shared_ptr<Monster> prototype) {
        prototypes_[key] = prototype;
        cout << "[注册表] 注册原型: " << key << endl;
    }

    // 通过 key 克隆对象
    shared_ptr<Monster> create(const string& key) const {
        auto it = prototypes_.find(key);
        if (it != prototypes_.end()) {
            return it->second->clone();
        }
        cout << "[注册表] 错误：未找到原型 \"" << key << "\"" << endl;
        return nullptr;
    }

    // 列出所有已注册的原型
    void listPrototypes() const {
        cout << "[注册表] 已注册的原型类型:" << endl;
        for (const auto& pair : prototypes_) {
            cout << "  - " << pair.first << endl;
        }
    }

private:
    unordered_map<string, shared_ptr<Monster>> prototypes_;
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     原型模式演示 - 游戏怪物系统         " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建原型注册表并注册原型 ----------
    cout << endl;
    cout << ">>> 步骤一：注册原型 <<<" << endl;
    PrototypeRegistry registry;

    // 创建原始原型实例（这里使用自定义参数，实际场景中可能从配置/数据库加载）
    auto goblin = make_shared<Goblin>(
        "哥布林战士", 100, 15, 5,
        vector<Skill>{{"砍击", 20, 2.0f}, {"逃跑", 0, 0.5f}}
    );
    auto dragon = make_shared<Dragon>(
        "远古巨龙", 5000, 300, 200,
        vector<Skill>{{"火焰吐息", 500, 5.0f}, {"尾扫", 200, 3.0f}, {"飞扑", 350, 8.0f}}
    );
    auto skeleton = make_shared<Skeleton>(
        "骷髅兵", 80, 20, 2,
        vector<Skill>{{"骨剑斩", 25, 1.5f}}
    );

    registry.registerPrototype("goblin", goblin);
    registry.registerPrototype("dragon", dragon);
    registry.registerPrototype("skeleton", skeleton);
    cout << endl;
    registry.listPrototypes();

    // ---------- 2. 展示原始原型 ----------
    cout << endl;
    cout << ">>> 步骤二：展示原始原型 <<<" << endl;
    goblin->show();
    dragon->show();
    skeleton->show();

    // ---------- 3. 通过克隆创建新对象 ----------
    cout << endl;
    cout << ">>> 步骤三：通过克隆创建新怪物 <<<" << endl;

    // 克隆哥布林并修改名字（用于不同的战斗场景）
    auto goblin2 = registry.create("goblin");
    goblin2->setName("哥布林斥候");

    auto goblin3 = registry.create("goblin");
    goblin3->setName("哥布林首领");

    // 克隆龙
    auto dragon2 = registry.create("dragon");
    dragon2->setName("暗影巨龙");

    // 克隆骷髅兵
    auto skeleton2 = registry.create("skeleton");
    skeleton2->setName("骷髅弓箭手");

    cout << endl;
    cout << "--- 克隆出的新怪物 ---" << endl;
    goblin2->show();
    goblin3->show();
    dragon2->show();
    skeleton2->show();

    // ---------- 4. 验证深拷贝：修改克隆对象不影响原型 ----------
    cout << endl;
    cout << ">>> 步骤四：验证深拷贝（原型不受影响） <<<" << endl;
    cout << "修改了克隆体 \"哥布林斥候\" 的名字后，原始原型依然保持不变:" << endl;
    cout << endl;
    cout << "[原始原型]";
    goblin->show();
    cout << "[克隆体]";
    goblin2->show();

    // ---------- 5. 实际应用场景：批量生成怪物 ----------
    cout << endl;
    cout << ">>> 步骤五：批量生成骷髅军团 <<<" << endl;
    for (int i = 1; i <= 5; ++i) {
        auto sk = registry.create("skeleton");
        sk->setName("骷髅兵 #" + to_string(i));
        sk->show();
    }

    cout << endl;

    return 0;
}