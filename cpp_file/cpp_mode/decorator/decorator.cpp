/**
 * 装饰模式 (Decorator Pattern)
 * 
 * 动态地给一个对象添加额外的职责。就增加功能来说，装饰模式比生成子类
 * 更为灵活。装饰模式可以在不改变原有对象的基础上，通过包装（装饰）的
 * 方式为对象添加新行为。
 * 
 * 本例以咖啡店为场景，演示装饰模式的核心结构：
 *   - Component（组件接口）：饮料（Beverage），定义所有饮料的公共接口
 *   - ConcreteComponent（具体组件）：基础饮料（浓缩咖啡、黑咖啡等），是可以被装饰的原始对象
 *   - Decorator（装饰器基类）：调料装饰器（CondimentDecorator），持有被装饰对象的引用，并实现Component接口
 *   - ConcreteDecorator（具体装饰器）：具体调料（牛奶、糖、奶油等），在调用被装饰对象方法的基础上添加新行为
 * 
 * 场景说明：
 *   咖啡店有多种基础饮料（浓缩咖啡、黑咖啡），顾客可以在任意饮料基础上
 *   添加各种调料（牛奶、糖、摩卡、奶油等），每种调料都有额外的价格。
 *   通过装饰模式，可以自由组合任意数量和种类的调料，无需为每种组合创建子类。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ======================== 组件接口：饮料 ========================
class Beverage {
public:
    virtual ~Beverage() {}
    virtual string getDescription() const = 0;
    virtual double cost() const = 0;
};

// ======================== 具体组件：浓缩咖啡 ========================
class Espresso : public Beverage {
public:
    string getDescription() const override {
        return "☕ 浓缩咖啡 (Espresso)";
    }

    double cost() const override {
        return 15.0;
    }
};

// ======================== 具体组件：黑咖啡 ========================
class HouseBlend : public Beverage {
public:
    string getDescription() const override {
        return "☕ 黑咖啡 (House Blend)";
    }

    double cost() const override {
        return 12.0;
    }
};

// ======================== 具体组件：脱因咖啡 ========================
class Decaf : public Beverage {
public:
    string getDescription() const override {
        return "☕ 脱因咖啡 (Decaf)";
    }

    double cost() const override {
        return 18.0;
    }
};

// ======================== 装饰器基类：调料装饰器 ========================
class CondimentDecorator : public Beverage {
public:
    CondimentDecorator(shared_ptr<Beverage> beverage) : beverage_(beverage) {}

    string getDescription() const override {
        return beverage_->getDescription();
    }

    double cost() const override {
        return beverage_->cost();
    }

protected:
    shared_ptr<Beverage> beverage_;
};

// ======================== 具体装饰器：牛奶 ========================
class Milk : public CondimentDecorator {
public:
    Milk(shared_ptr<Beverage> beverage) : CondimentDecorator(beverage) {}

    string getDescription() const override {
        return beverage_->getDescription() + " + 🥛 牛奶";
    }

    double cost() const override {
        return beverage_->cost() + 3.0;
    }
};

// ======================== 具体装饰器：糖 ========================
class Sugar : public CondimentDecorator {
public:
    Sugar(shared_ptr<Beverage> beverage) : CondimentDecorator(beverage) {}

    string getDescription() const override {
        return beverage_->getDescription() + " + 🍬 糖";
    }

    double cost() const override {
        return beverage_->cost() + 1.5;
    }
};

// ======================== 具体装饰器：摩卡 ========================
class Mocha : public CondimentDecorator {
public:
    Mocha(shared_ptr<Beverage> beverage) : CondimentDecorator(beverage) {}

    string getDescription() const override {
        return beverage_->getDescription() + " + 🍫 摩卡";
    }

    double cost() const override {
        return beverage_->cost() + 5.0;
    }
};

// ======================== 具体装饰器：奶油 ========================
class Whip : public CondimentDecorator {
public:
    Whip(shared_ptr<Beverage> beverage) : CondimentDecorator(beverage) {}

    string getDescription() const override {
        return beverage_->getDescription() + " + 🍦 奶油";
    }

    double cost() const override {
        return beverage_->cost() + 4.0;
    }
};

// ======================== 辅助函数：打印订单详情 ========================
void printOrder(const shared_ptr<Beverage>& beverage, int orderNum) {
    cout << "  ┌─ 订单 " << orderNum << " ─────────────────────────────" << endl;
    cout << "  │ 描述: " << beverage->getDescription() << endl;
    cout << "  │ 价格: " << beverage->cost() << " 元" << endl;
    cout << "  └─────────────────────────────────────────" << endl;
}

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     装饰模式演示 - 咖啡店点单系统       " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 基础饮料（未装饰的组件） ----------
    cout << endl;
    cout << ">>> 步骤一：基础饮料（未装饰的组件）<<<" << endl;

    auto espresso = make_shared<Espresso>();
    auto houseBlend = make_shared<HouseBlend>();
    auto decaf = make_shared<Decaf>();

    printOrder(espresso, 1);
    printOrder(houseBlend, 2);
    printOrder(decaf, 3);

    // ---------- 2. 单层装饰：给浓缩咖啡加一份牛奶 ----------
    cout << endl;
    cout << ">>> 步骤二：单层装饰 - 浓缩咖啡 + 牛奶 <<<" << endl;

    auto espressoWithMilk = make_shared<Milk>(espresso);
    printOrder(espressoWithMilk, 4);

    // ---------- 3. 多层装饰：给黑咖啡加糖和摩卡 ----------
    cout << endl;
    cout << ">>> 步骤三：多层装饰 - 黑咖啡 + 糖 + 摩卡 <<<" << endl;

    // 装饰顺序：先加糖，再加摩卡（可任意组合）
    shared_ptr<Beverage> order5 = make_shared<HouseBlend>();
    order5 = make_shared<Sugar>(order5);
    order5 = make_shared<Mocha>(order5);
    printOrder(order5, 5);

    // ---------- 4. 复杂组合装饰：多重叠加 ----------
    cout << endl;
    cout << ">>> 步骤四：复杂组合装饰 - 脱因咖啡 + 牛奶 + 摩卡 + 奶油 <<<" << endl;

    shared_ptr<Beverage> order6 = make_shared<Decaf>();
    order6 = make_shared<Milk>(order6);
    order6 = make_shared<Mocha>(order6);
    order6 = make_shared<Whip>(order6);
    printOrder(order6, 6);

    // ---------- 5. 同一基础饮料的不同装饰组合 ----------
    cout << endl;
    cout << ">>> 步骤五：同一基础饮料的不同装饰组合 <<<" << endl;

    // 浓缩咖啡 + 双份摩卡 + 奶油
    shared_ptr<Beverage> order7 = make_shared<Espresso>();
    order7 = make_shared<Mocha>(order7);    // 第一份摩卡
    order7 = make_shared<Mocha>(order7);    // 第二份摩卡（可叠加相同装饰）
    order7 = make_shared<Whip>(order7);
    printOrder(order7, 7);

    // 浓缩咖啡 + 糖 + 牛奶
    shared_ptr<Beverage> order8 = make_shared<Espresso>();
    order8 = make_shared<Sugar>(order8);
    order8 = make_shared<Milk>(order8);
    printOrder(order8, 8);

    // ---------- 6. 批量点单展示 ----------
    cout << endl;
    cout << ">>> 步骤六：批量点单汇总 <<<" << endl;

    vector<shared_ptr<Beverage>> orders = { espresso, houseBlend, decaf, 
                                             espressoWithMilk, order5, order6, 
                                             order7, order8 };
    double totalCost = 0;
    for (size_t i = 0; i < orders.size(); ++i) {
        cout << endl;
        printOrder(orders[i], i + 1);
        totalCost += orders[i]->cost();
    }

    cout << endl;
    cout << "  ═══════════════════════════════════════" << endl;
    cout << "  总计 " << orders.size() << " 杯，合计: " << totalCost << " 元" << endl;
    cout << "  ═══════════════════════════════════════" << endl;

    // ---------- 7. 展示装饰模式的价值 ----------
    cout << endl;
    cout << ">>> 装饰模式优势总结 <<<" << endl;
    cout << "  ├─ 动态扩展：运行时可以自由地为对象添加或移除职责" << endl;
    cout << "  │  （如随时给咖啡加糖或去糖）" << endl;
    cout << "  ├─ 灵活组合：多个装饰器可以任意嵌套组合" << endl;
    cout << "  │  （如双份摩卡 + 牛奶 + 奶油）" << endl;
    cout << "  ├─ 替代继承：避免为每种组合创建子类" << endl;
    cout << "  │  （若有 N 种调料，继承方式需要 2^N 个子类）" << endl;
    cout << "  ├─ 单一职责：每种调料装饰器只关注自己的新增行为" << endl;
    cout << "  └─ 开闭原则：新增调料无需修改已有代码，只需新增装饰器类" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}