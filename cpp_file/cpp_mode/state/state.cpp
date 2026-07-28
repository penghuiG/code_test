/**
 * 状态模式 (State Pattern)
 * 
 * 允许一个对象在其内部状态改变时改变它的行为，对象看起来似乎修改了它的类。
 * 将每一个具体状态封装成独立的类，将状态转换逻辑分散到各个状态类中。
 * 
 * 本例以订单处理系统为场景，演示状态模式的核心结构及状态流转：
 *   - State（抽象状态）：IOrderState，定义订单在各状态下可执行的操作接口
 *   - ConcreteState（具体状态）：
 *     · NewOrderState（新建状态）：订单刚创建，可以支付或取消
 *     · PaidState（已支付状态）：订单已付款，可以发货或退款
 *     · ShippedState（已发货状态）：商品已发出，可以确认收货
 *     · DeliveredState（已收货状态）：交易完成，可以申请售后
 *     · CancelledState（已取消状态）：订单已取消，终态
 *   - Context（上下文）：Order，持有当前状态引用，将操作委托给当前状态对象
 * 
 * 场景说明：
 *   一个电商订单在其生命周期中会经历多个状态：新建 → 已支付 → 已发货 → 已收货。
 *   每个状态下可执行的操作不同，且状态之间的转换有严格的规则。
 *   使用状态模式，将每个状态的行为封装到独立的状态类中，避免了大量的
 *   if-else 或 switch-case 判断，使代码更易于维护和扩展。
 */

#include <iostream>
#include <string>
#include <memory>
using namespace std;

// ======================== 抽象状态：订单状态接口 ========================
class IOrderState {
public:
    virtual ~IOrderState() {}
    virtual void pay(class Order& order) = 0;
    virtual void ship(class Order& order) = 0;
    virtual void deliver(class Order& order) = 0;
    virtual void cancel(class Order& order) = 0;
    virtual void refund(class Order& order) = 0;
    virtual void afterSale(class Order& order) = 0;
    virtual string getStateName() const = 0;
};

// ======================== 上下文：订单 ========================
class Order {
public:
    Order(const string& orderId, const string& productName, double amount)
        : orderId_(orderId), productName_(productName), amount_(amount) {
        cout << "    [订单] 创建订单 " << orderId_
             << " | 商品: " << productName_
             << " | 金额: ¥" << amount_ << endl;
    }

    void setState(shared_ptr<IOrderState> state) {
        cout << "    [订单 " << orderId_ << "] 状态变更: "
             << (currentState_ ? currentState_->getStateName() : "无")
             << " → " << state->getStateName() << endl;
        currentState_ = state;
    }

    void pay()        { if (currentState_) currentState_->pay(*this); }
    void ship()       { if (currentState_) currentState_->ship(*this); }
    void deliver()    { if (currentState_) currentState_->deliver(*this); }
    void cancel()     { if (currentState_) currentState_->cancel(*this); }
    void refund()     { if (currentState_) currentState_->refund(*this); }
    void afterSale()  { if (currentState_) currentState_->afterSale(*this); }

    string getOrderId() const { return orderId_; }
    string getProductName() const { return productName_; }
    double getAmount() const { return amount_; }
    string getCurrentStateName() const {
        return currentState_ ? currentState_->getStateName() : "未初始化";
    }

    void showInfo() const {
        cout << "    ┌─ 订单详情 ─────────────────────────" << endl;
        cout << "    │ 订单号: " << orderId_ << endl;
        cout << "    │ 商品:   " << productName_ << endl;
        cout << "    │ 金额:   ¥" << amount_ << endl;
        cout << "    │ 状态:   " << getCurrentStateName() << endl;
        cout << "    └──────────────────────────────────────" << endl;
    }

private:
    string orderId_;
    string productName_;
    double amount_;
    shared_ptr<IOrderState> currentState_;
};

// ======================== 具体状态类声明 ========================
// 声明在前，方法体定义在所有类之后（解决循环依赖）

class NewOrderState : public IOrderState {
public:
    void pay(Order& order) override;
    void ship(Order& order) override;
    void deliver(Order& order) override;
    void cancel(Order& order) override;
    void refund(Order& order) override;
    void afterSale(Order& order) override;
    string getStateName() const override { return "新建"; }
};

class PaidState : public IOrderState {
public:
    void pay(Order& order) override;
    void ship(Order& order) override;
    void deliver(Order& order) override;
    void cancel(Order& order) override;
    void refund(Order& order) override;
    void afterSale(Order& order) override;
    string getStateName() const override { return "已支付"; }
};

class ShippedState : public IOrderState {
public:
    void pay(Order& order) override;
    void ship(Order& order) override;
    void deliver(Order& order) override;
    void cancel(Order& order) override;
    void refund(Order& order) override;
    void afterSale(Order& order) override;
    string getStateName() const override { return "已发货"; }
};

class DeliveredState : public IOrderState {
public:
    void pay(Order& order) override;
    void ship(Order& order) override;
    void deliver(Order& order) override;
    void cancel(Order& order) override;
    void refund(Order& order) override;
    void afterSale(Order& order) override;
    string getStateName() const override { return "已收货"; }
};

class CancelledState : public IOrderState {
public:
    void pay(Order& order) override;
    void ship(Order& order) override;
    void deliver(Order& order) override;
    void cancel(Order& order) override;
    void refund(Order& order) override;
    void afterSale(Order& order) override;
    string getStateName() const override { return "已取消"; }
};

// ======================== NewOrderState 方法实现 ========================
void NewOrderState::pay(Order& order) {
    cout << "    [新建状态] 订单 " << order.getOrderId()
         << " 支付成功，金额: ¥" << order.getAmount() << endl;
    order.setState(make_shared<PaidState>());
}
void NewOrderState::ship(Order& order) {
    cout << "    [新建状态] ✗ 操作失败 —— 订单尚未支付，无法发货" << endl;
}
void NewOrderState::deliver(Order& order) {
    cout << "    [新建状态] ✗ 操作失败 —— 订单尚未支付，无法确认收货" << endl;
}
void NewOrderState::cancel(Order& order) {
    cout << "    [新建状态] 订单 " << order.getOrderId() << " 已取消" << endl;
    order.setState(make_shared<CancelledState>());
}
void NewOrderState::refund(Order& order) {
    cout << "    [新建状态] ✗ 操作失败 —— 订单尚未支付，无法退款" << endl;
}
void NewOrderState::afterSale(Order& order) {
    cout << "    [新建状态] ✗ 操作失败 —— 订单尚未完成，无法申请售后" << endl;
}

// ======================== PaidState 方法实现 ========================
void PaidState::pay(Order& order) {
    cout << "    [已支付状态] ✗ 操作失败 —— 订单已支付，请勿重复支付" << endl;
}
void PaidState::ship(Order& order) {
    cout << "    [已支付状态] 订单 " << order.getOrderId() << " 已发货，快递正在路上..." << endl;
    order.setState(make_shared<ShippedState>());
}
void PaidState::deliver(Order& order) {
    cout << "    [已支付状态] ✗ 操作失败 —— 订单尚未发货，无法确认收货" << endl;
}
void PaidState::cancel(Order& order) {
    cout << "    [已支付状态] ✗ 操作失败 —— 订单已支付，如需取消请申请退款" << endl;
}
void PaidState::refund(Order& order) {
    cout << "    [已支付状态] 订单 " << order.getOrderId()
         << " 退款成功，金额 ¥" << order.getAmount() << " 已原路返回" << endl;
    order.setState(make_shared<CancelledState>());
}
void PaidState::afterSale(Order& order) {
    cout << "    [已支付状态] ✗ 操作失败 —— 订单尚未完成，无法申请售后" << endl;
}

// ======================== ShippedState 方法实现 ========================
void ShippedState::pay(Order& order) {
    cout << "    [已发货状态] ✗ 操作失败 —— 订单已支付" << endl;
}
void ShippedState::ship(Order& order) {
    cout << "    [已发货状态] ✗ 操作失败 —— 订单已发货，请勿重复操作" << endl;
}
void ShippedState::deliver(Order& order) {
    cout << "    [已发货状态] 订单 " << order.getOrderId() << " 已确认收货，交易完成！" << endl;
    order.setState(make_shared<DeliveredState>());
}
void ShippedState::cancel(Order& order) {
    cout << "    [已发货状态] ✗ 操作失败 —— 订单已发货，无法取消" << endl;
}
void ShippedState::refund(Order& order) {
    cout << "    [已发货状态] 订单 " << order.getOrderId()
         << " 申请退款成功，请退回商品，金额 ¥" << order.getAmount() << " 将原路返回" << endl;
    order.setState(make_shared<CancelledState>());
}
void ShippedState::afterSale(Order& order) {
    cout << "    [已发货状态] ✗ 操作失败 —— 订单尚未完成，请先确认收货" << endl;
}

// ======================== DeliveredState 方法实现 ========================
void DeliveredState::pay(Order& order) {
    cout << "    [已收货状态] ✗ 操作失败 —— 订单已完成" << endl;
}
void DeliveredState::ship(Order& order) {
    cout << "    [已收货状态] ✗ 操作失败 —— 订单已完成" << endl;
}
void DeliveredState::deliver(Order& order) {
    cout << "    [已收货状态] ✗ 操作失败 —— 订单已确认收货" << endl;
}
void DeliveredState::cancel(Order& order) {
    cout << "    [已收货状态] ✗ 操作失败 —— 订单已完成，无法取消" << endl;
}
void DeliveredState::refund(Order& order) {
    cout << "    [已收货状态] ✗ 操作失败 —— 订单已完成，无法直接退款" << endl;
}
void DeliveredState::afterSale(Order& order) {
    cout << "    [已收货状态] 订单 " << order.getOrderId()
         << " 已提交售后申请，客服将尽快处理" << endl;
}

// ======================== CancelledState 方法实现 ========================
void CancelledState::pay(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消" << endl;
}
void CancelledState::ship(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消" << endl;
}
void CancelledState::deliver(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消" << endl;
}
void CancelledState::cancel(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消，请勿重复操作" << endl;
}
void CancelledState::refund(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消" << endl;
}
void CancelledState::afterSale(Order& order) {
    cout << "    [已取消状态] ✗ 操作失败 —— 订单已取消，无法申请售后" << endl;
}

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
    cout << "       状态模式演示 - 订单处理系统        " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 正常订单流程：新建 → 支付 → 发货 → 收货 ----------
    cout << endl;
    cout << ">>> 场景一：正常订单流程（新建 → 支付 → 发货 → 收货） <<<" << endl;

    printSection("创建订单");
    Order order1("ORD-20260001", "MacBook Pro 16寸", 19999.0);
    order1.setState(make_shared<NewOrderState>());
    order1.showInfo();
    printSectionEnd();

    printSection("支付订单");
    order1.pay();
    order1.showInfo();
    printSectionEnd();

    printSection("发货");
    order1.ship();
    order1.showInfo();
    printSectionEnd();

    printSection("确认收货");
    order1.deliver();
    order1.showInfo();
    printSectionEnd();

    printSection("申请售后");
    order1.afterSale();
    printSectionEnd();

    // ---------- 2. 取消订单流程：新建 → 取消 ----------
    cout << endl;
    cout << ">>> 场景二：新建订单后取消 <<<" << endl;

    printSection("创建订单");
    Order order2("ORD-20260002", "AirPods Pro", 1899.0);
    order2.setState(make_shared<NewOrderState>());
    order2.showInfo();
    printSectionEnd();

    printSection("取消订单");
    order2.cancel();
    order2.showInfo();
    printSectionEnd();

    printSection("尝试操作已取消的订单");
    order2.pay();
    order2.ship();
    printSectionEnd();

    // ---------- 3. 退款流程：新建 → 支付 → 退款 ----------
    cout << endl;
    cout << ">>> 场景三：支付后退款 <<<" << endl;

    printSection("创建并支付订单");
    Order order3("ORD-20260003", "iPhone 16 Pro", 8999.0);
    order3.setState(make_shared<NewOrderState>());
    order3.pay();
    order3.showInfo();
    printSectionEnd();

    printSection("申请退款");
    order3.refund();
    order3.showInfo();
    printSectionEnd();

    // ---------- 4. 非法操作演示：状态约束 ----------
    cout << endl;
    cout << ">>> 场景四：非法操作演示（状态约束） <<<" << endl;

    printSection("新建订单直接发货（非法）");
    Order order4("ORD-20260004", "iPad Air", 4799.0);
    order4.setState(make_shared<NewOrderState>());
    order4.ship();
    order4.deliver();
    printSectionEnd();

    printSection("已支付状态重复支付（非法）");
    order4.pay();
    order4.pay();
    printSectionEnd();

    printSection("已发货状态取消（非法）");
    order4.ship();
    order4.cancel();
    printSectionEnd();

    // ---------- 5. 发货后退款流程 ----------
    cout << endl;
    cout << ">>> 场景五：发货后退款 <<<" << endl;

    printSection("创建、支付、发货");
    Order order5("ORD-20260005", "Magic Keyboard", 2999.0);
    order5.setState(make_shared<NewOrderState>());
    order5.pay();
    order5.ship();
    order5.showInfo();
    printSectionEnd();

    printSection("发货后申请退款");
    order5.refund();
    order5.showInfo();
    printSectionEnd();

    // ---------- 6. 展示状态模式的价值 ----------
    cout << endl;
    cout << ">>> 状态模式优势总结 <<<" << endl;
    cout << "  ├─ 消除条件分支：用多态替代大量的 if-else/switch-case" << endl;
    cout << "  ├─ 单一职责：每个状态类只关注自己状态下的行为" << endl;
    cout << "  ├─ 开闭原则：新增状态只需添加新类，不修改已有代码" << endl;
    cout << "  ├─ 状态转换明确：状态转换逻辑分散在各状态类中，清晰可见" << endl;
    cout << "  ├─ 行为约束：每个状态下只能执行合法操作，非法操作被自动拦截" << endl;
    cout << "  └─ 可维护性：状态越多，相比条件分支的优势越明显" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}