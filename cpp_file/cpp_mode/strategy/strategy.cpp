/**
 * 策略模式 (Strategy Pattern)
 * 
 * 定义一系列算法，把它们一个个封装起来，并且使它们可以互相替换。
 * 策略模式使得算法可以独立于使用它的客户端而变化。
 * 
 * 本例以电商购物结算为场景，演示策略模式的核心结构及运行时算法切换：
 *   - Strategy（抽象策略）：IPaymentStrategy，定义支付算法的统一接口
 *   - ConcreteStrategy（具体策略）：
 *     · CreditCardStrategy（信用卡支付）：验证卡号、有效期，扣款并生成签账单
 *     · AlipayStrategy（支付宝支付）：校验支付宝账号，扫码扣款
 *     · WeChatPayStrategy（微信支付）：校验微信账号，扫码扣款
 *     · BankTransferStrategy（银行转账）：校验银行卡号与开户行，发起转账
 *     · CryptoStrategy（加密货币支付）：校验钱包地址，广播交易上链
 *   - Context（上下文）：ShoppingCart，持有策略引用，将支付行为委托给当前策略
 * 
 * 场景说明：
 *   用户在电商平台上选购商品后进入结算环节，可以选择不同的支付方式。
 *   每种支付方式对应一套独立的算法（校验逻辑、扣款流程、凭证生成）。
 *   使用策略模式，将每种支付算法封装到独立的策略类中，客户端可以在
 *   运行时自由切换支付方式，无需修改购物车代码，符合开闭原则。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
using namespace std;

// ======================== 商品数据结构 ========================
struct Product {
    string name;
    double price;
    int    quantity;
};

// ======================== 支付结果 ========================
struct PaymentResult {
    bool   success;
    string transactionId;
    string message;
};

// ======================== 抽象策略：支付算法接口 ========================
class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() {}
    virtual string getPaymentName() const = 0;
    virtual bool validate() const = 0;
    virtual PaymentResult pay(double amount) = 0;
    virtual void printReceipt(const string& transactionId, double amount) const = 0;
};

// ======================== 上下文：购物车 ========================
class ShoppingCart {
public:
    void addProduct(const string& name, double price, int quantity = 1) {
        products_.push_back({name, price, quantity});
        cout << "    [购物车] 添加商品: " << name
             << " × " << quantity
             << "，单价: ¥" << fixed << setprecision(2) << price << endl;
    }

    double getTotalAmount() const {
        double total = 0;
        for (const auto& p : products_) {
            total += p.price * p.quantity;
        }
        return total;
    }

    void setPaymentStrategy(shared_ptr<IPaymentStrategy> strategy) {
        paymentStrategy_ = strategy;
        cout << "    [购物车] 已选择支付方式: " << strategy->getPaymentName() << endl;
    }

    string getPaymentName() const {
        return paymentStrategy_ ? paymentStrategy_->getPaymentName() : "未选择";
    }

    PaymentResult checkout() {
        cout << endl;
        cout << "    ┌─ 结算清单 ─────────────────────────────────────" << endl;
        for (const auto& p : products_) {
            double subtotal = p.price * p.quantity;
            cout << "    │ " << p.name << " × " << p.quantity
                 << "  = ¥" << fixed << setprecision(2) << subtotal << endl;
        }
        cout << "    │ ─────────────────────────────────────────────" << endl;
        cout << "    │ 合计: ¥" << fixed << setprecision(2) << getTotalAmount() << endl;
        cout << "    │ 支付方式: " << getPaymentName() << endl;
        cout << "    └──────────────────────────────────────────────────" << endl;

        if (!paymentStrategy_) {
            cout << "    [错误] 未选择支付方式！" << endl;
            return {false, "", "未选择支付方式"};
        }

        if (!paymentStrategy_->validate()) {
            cout << "    [错误] 支付信息校验失败！" << endl;
            return {false, "", "校验失败"};
        }

        double amount = getTotalAmount();
        PaymentResult result = paymentStrategy_->pay(amount);

        if (result.success) {
            paymentStrategy_->printReceipt(result.transactionId, amount);
            products_.clear();
        }

        return result;
    }

    void showCart() const {
        cout << "    [购物车] 当前商品数量: " << products_.size()
             << "，总额: ¥" << fixed << setprecision(2) << getTotalAmount() << endl;
    }

private:
    vector<Product> products_;
    shared_ptr<IPaymentStrategy> paymentStrategy_;
};

// ======================== 具体策略A：信用卡支付 ========================
class CreditCardStrategy : public IPaymentStrategy {
public:
    CreditCardStrategy(const string& cardNumber, const string& holderName,
                       const string& expiryDate, const string& cvv)
        : cardNumber_(cardNumber), holderName_(holderName),
          expiryDate_(expiryDate), cvv_(cvv) {}

    string getPaymentName() const override { return "信用卡支付"; }

    bool validate() const override {
        cout << "    [信用卡] 验证卡号 " << maskCardNumber(cardNumber_) << " ..." << endl;
        if (cardNumber_.length() != 16) {
            cout << "    [信用卡] ✗ 卡号格式错误（需要16位）" << endl;
            return false;
        }
        cout << "    [信用卡] 验证有效期 " << expiryDate_ << " ..." << endl;
        cout << "    [信用卡] 验证CVV ..." << endl;
        cout << "    [信用卡] ✓ 验证通过" << endl;
        return true;
    }

    PaymentResult pay(double amount) override {
        cout << "    [信用卡] 向银行发起扣款请求，金额: ¥"
             << fixed << setprecision(2) << amount << " ..." << endl;
        cout << "    [信用卡] 银行授权成功" << endl;
        string txnId = "CC-" + cardNumber_.substr(12, 4) + "-20260728";
        cout << "    [信用卡] ✓ 扣款成功，交易号: " << txnId << endl;
        return {true, txnId, "信用卡支付成功"};
    }

    void printReceipt(const string& transactionId, double amount) const override {
        cout << endl;
        cout << "    ╔══════════════════════════════════════╗" << endl;
        cout << "    ║          信用卡签账单                ║" << endl;
        cout << "    ╠══════════════════════════════════════╣" << endl;
        cout << "    ║ 持卡人: " << holderName_ << endl;
        cout << "    ║ 卡  号: **** **** **** " << cardNumber_.substr(12, 4) << endl;
        cout << "    ║ 金  额: ¥" << fixed << setprecision(2) << amount << endl;
        cout << "    ║ 交易号: " << transactionId << endl;
        cout << "    ║ 签  名: ________________________" << endl;
        cout << "    ╚══════════════════════════════════════╝" << endl;
    }

private:
    string maskCardNumber(const string& card) const {
        if (card.length() < 4) return "****";
        return "**** **** **** " + card.substr(12, 4);
    }

    string cardNumber_;
    string holderName_;
    string expiryDate_;
    string cvv_;
};

// ======================== 具体策略B：支付宝支付 ========================
class AlipayStrategy : public IPaymentStrategy {
public:
    AlipayStrategy(const string& account) : account_(account) {}

    string getPaymentName() const override { return "支付宝支付"; }

    bool validate() const override {
        cout << "    [支付宝] 校验账号 " << account_ << " ..." << endl;
        if (account_.find('@') == string::npos && account_.length() != 11) {
            cout << "    [支付宝] ✗ 账号格式错误" << endl;
            return false;
        }
        cout << "    [支付宝] ✓ 账号校验通过" << endl;
        return true;
    }

    PaymentResult pay(double amount) override {
        cout << "    [支付宝] 生成付款二维码 ..." << endl;
        cout << "    [支付宝] 用户扫码确认付款 ..." << endl;
        cout << "    [支付宝] 从余额/花呗/银行卡扣款 ..." << endl;
        string txnId = "ALI-20260728-" + to_string(rand() % 900000 + 100000);
        cout << "    [支付宝] ✓ 支付成功，交易号: " << txnId << endl;
        return {true, txnId, "支付宝支付成功"};
    }

    void printReceipt(const string& transactionId, double amount) const override {
        cout << endl;
        cout << "    ╔══════════════════════════════════════╗" << endl;
        cout << "    ║          支付宝交易凭证              ║" << endl;
        cout << "    ╠══════════════════════════════════════╣" << endl;
        cout << "    ║ 付 款 方: " << account_ << endl;
        cout << "    ║ 金  额: ¥" << fixed << setprecision(2) << amount << endl;
        cout << "    ║ 交易号: " << transactionId << endl;
        cout << "    ║ 状  态: 交易成功" << endl;
        cout << "    ╚══════════════════════════════════════╝" << endl;
    }

private:
    string account_;
};

// ======================== 具体策略C：微信支付 ========================
class WeChatPayStrategy : public IPaymentStrategy {
public:
    WeChatPayStrategy(const string& openId) : openId_(openId) {}

    string getPaymentName() const override { return "微信支付"; }

    bool validate() const override {
        cout << "    [微信支付] 校验OpenID " << openId_.substr(0, 8) << "..." << endl;
        if (openId_.length() < 10) {
            cout << "    [微信支付] ✗ OpenID无效" << endl;
            return false;
        }
        cout << "    [微信支付] ✓ 校验通过" << endl;
        return true;
    }

    PaymentResult pay(double amount) override {
        cout << "    [微信支付] 唤起微信支付 ..." << endl;
        cout << "    [微信支付] 用户确认支付 ..." << endl;
        string txnId = "WX-20260728-" + to_string(rand() % 900000 + 100000);
        cout << "    [微信支付] ✓ 支付成功，交易号: " << txnId << endl;
        return {true, txnId, "微信支付成功"};
    }

    void printReceipt(const string& transactionId, double amount) const override {
        cout << endl;
        cout << "    ╔══════════════════════════════════════╗" << endl;
        cout << "    ║          微信支付凭证                ║" << endl;
        cout << "    ╠══════════════════════════════════════╣" << endl;
        cout << "    ║ 用户ID: " << openId_.substr(0, 12) << "..." << endl;
        cout << "    ║ 金  额: ¥" << fixed << setprecision(2) << amount << endl;
        cout << "    ║ 交易号: " << transactionId << endl;
        cout << "    ║ 商户单号: MERCHANT-" << rand() % 9000 + 1000 << endl;
        cout << "    ╚══════════════════════════════════════╝" << endl;
    }

private:
    string openId_;
};

// ======================== 具体策略D：银行转账 ========================
class BankTransferStrategy : public IPaymentStrategy {
public:
    BankTransferStrategy(const string& bankCard, const string& bankName, const string& accountHolder)
        : bankCard_(bankCard), bankName_(bankName), accountHolder_(accountHolder) {}

    string getPaymentName() const override { return "银行转账"; }

    bool validate() const override {
        cout << "    [银行转账] 校验银行卡号 " << bankCard_.substr(0, 4) << "****" << bankCard_.substr(bankCard_.length() - 4) << " ..." << endl;
        cout << "    [银行转账] 校验开户行: " << bankName_ << " ..." << endl;
        cout << "    [银行转账] 校验户名: " << accountHolder_ << " ..." << endl;
        cout << "    [银行转账] ✓ 信息校验通过" << endl;
        return true;
    }

    PaymentResult pay(double amount) override {
        cout << "    [银行转账] 向" << bankName_ << "发起转账请求 ..." << endl;
        cout << "    [银行转账] 银行处理中（大额转账可能需要人工审核）..." << endl;
        string txnId = "BT-" + to_string(rand() % 900000 + 100000);
        cout << "    [银行转账] ✓ 转账成功，流水号: " << txnId << endl;
        return {true, txnId, "银行转账成功"};
    }

    void printReceipt(const string& transactionId, double amount) const override {
        cout << endl;
        cout << "    ╔══════════════════════════════════════╗" << endl;
        cout << "    ║          银行转账回单                ║" << endl;
        cout << "    ╠══════════════════════════════════════╣" << endl;
        cout << "    ║ 收款银行: " << bankName_ << endl;
        cout << "    ║ 收款账号: " << bankCard_.substr(0, 4) << "****" << bankCard_.substr(bankCard_.length() - 4) << endl;
        cout << "    ║ 收款户名: " << accountHolder_ << endl;
        cout << "    ║ 金  额: ¥" << fixed << setprecision(2) << amount << endl;
        cout << "    ║ 流水号: " << transactionId << endl;
        cout << "    ║ 状  态: 转账成功" << endl;
        cout << "    ╚══════════════════════════════════════╝" << endl;
    }

private:
    string bankCard_;
    string bankName_;
    string accountHolder_;
};

// ======================== 具体策略E：加密货币支付 ========================
class CryptoStrategy : public IPaymentStrategy {
public:
    CryptoStrategy(const string& walletAddress, const string& cryptoType)
        : walletAddress_(walletAddress), cryptoType_(cryptoType) {}

    string getPaymentName() const override { return cryptoType_ + "支付"; }

    bool validate() const override {
        cout << "    [" << cryptoType_ << "] 校验钱包地址 " << walletAddress_.substr(0, 8) << "..." << endl;
        if (walletAddress_.length() < 20) {
            cout << "    [" << cryptoType_ << "] ✗ 钱包地址无效" << endl;
            return false;
        }
        cout << "    [" << cryptoType_ << "] 校验账户余额 ..." << endl;
        cout << "    [" << cryptoType_ << "] ✓ 校验通过" << endl;
        return true;
    }

    PaymentResult pay(double amount) override {
        cout << "    [" << cryptoType_ << "] 构造交易，金额: ¥"
             << fixed << setprecision(2) << amount << " ..." << endl;
        cout << "    [" << cryptoType_ << "] 使用私钥签名 ..." << endl;
        cout << "    [" << cryptoType_ << "] 广播交易到区块链网络 ..." << endl;
        cout << "    [" << cryptoType_ << "] 等待区块确认 ..." << endl;
        string txnId = cryptoType_.substr(0, 3) + "-0x" + to_string(rand() % 0xFFFF);
        cout << "    [" << cryptoType_ << "] ✓ 交易已上链，TxHash: " << txnId << endl;
        return {true, txnId, cryptoType_ + "支付成功"};
    }

    void printReceipt(const string& transactionId, double amount) const override {
        cout << endl;
        cout << "    ╔══════════════════════════════════════╗" << endl;
        cout << "    ║          链上交易凭证                ║" << endl;
        cout << "    ╠══════════════════════════════════════╣" << endl;
        cout << "    ║ 币  种: " << cryptoType_ << endl;
        cout << "    ║ 钱包地址: " << walletAddress_.substr(0, 16) << "..." << endl;
        cout << "    ║ 金额(法币): ¥" << fixed << setprecision(2) << amount << endl;
        cout << "    ║ TxHash: " << transactionId << endl;
        cout << "    ║ 区块确认: 已确认" << endl;
        cout << "    ╚══════════════════════════════════════╝" << endl;
    }

private:
    string walletAddress_;
    string cryptoType_;
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
    cout << "      策略模式演示 - 电商支付系统        " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 信用卡支付 ----------
    cout << endl;
    cout << ">>> 场景一：使用信用卡支付 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart1;
    cart1.addProduct("MacBook Pro 16寸", 19999.00);
    cart1.addProduct("AirPods Pro", 1899.00, 2);
    cart1.addProduct("USB-C 转接头", 149.00, 3);
    cart1.showCart();
    printSectionEnd();

    printSection("选择信用卡支付");
    auto creditCard = make_shared<CreditCardStrategy>(
        "6225880112345678", "张三", "12/28", "123");
    cart1.setPaymentStrategy(creditCard);
    printSectionEnd();

    printSection("结算");
    PaymentResult result1 = cart1.checkout();
    cout << "    [结果] " << result1.message << endl;
    printSectionEnd();

    // ---------- 2. 支付宝支付 ----------
    cout << endl;
    cout << ">>> 场景二：使用支付宝支付 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart2;
    cart2.addProduct("iPhone 16 Pro", 8999.00);
    cart2.addProduct("MagSafe 充电器", 399.00);
    cart2.showCart();
    printSectionEnd();

    printSection("选择支付宝支付");
    auto alipay = make_shared<AlipayStrategy>("zhangsan@example.com");
    cart2.setPaymentStrategy(alipay);
    printSectionEnd();

    printSection("结算");
    PaymentResult result2 = cart2.checkout();
    cout << "    [结果] " << result2.message << endl;
    printSectionEnd();

    // ---------- 3. 微信支付 ----------
    cout << endl;
    cout << ">>> 场景三：使用微信支付 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart3;
    cart3.addProduct("iPad Air", 4799.00);
    cart3.addProduct("Apple Pencil", 999.00);
    cart3.showCart();
    printSectionEnd();

    printSection("选择微信支付");
    auto wechat = make_shared<WeChatPayStrategy>("oXyz1234567890abcdefghijklmn");
    cart3.setPaymentStrategy(wechat);
    printSectionEnd();

    printSection("结算");
    PaymentResult result3 = cart3.checkout();
    cout << "    [结果] " << result3.message << endl;
    printSectionEnd();

    // ---------- 4. 银行转账 ----------
    cout << endl;
    cout << ">>> 场景四：使用银行转账 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart4;
    cart4.addProduct("Dell 4K 显示器", 3999.00, 2);
    cart4.addProduct("机械键盘", 1299.00);
    cart4.showCart();
    printSectionEnd();

    printSection("选择银行转账");
    auto bankTransfer = make_shared<BankTransferStrategy>(
        "6222021234567890123", "中国工商银行", "张三");
    cart4.setPaymentStrategy(bankTransfer);
    printSectionEnd();

    printSection("结算");
    PaymentResult result4 = cart4.checkout();
    cout << "    [结果] " << result4.message << endl;
    printSectionEnd();

    // ---------- 5. 加密货币支付 ----------
    cout << endl;
    cout << ">>> 场景五：使用加密货币支付 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart5;
    cart5.addProduct("RTX 5090 显卡", 16999.00);
    cart5.showCart();
    printSectionEnd();

    printSection("选择比特币支付");
    auto crypto = make_shared<CryptoStrategy>(
        "bc1qxy2kgdygjrsqtzq2n0yrf2493p83kkfjhx0wlh", "BTC");
    cart5.setPaymentStrategy(crypto);
    printSectionEnd();

    printSection("结算");
    PaymentResult result5 = cart5.checkout();
    cout << "    [结果] " << result5.message << endl;
    printSectionEnd();

    // ---------- 6. 运行时切换支付策略 ----------
    cout << endl;
    cout << ">>> 场景六：运行时动态切换支付方式 <<<" << endl;

    printSection("选购商品");
    ShoppingCart cart6;
    cart6.addProduct("Sony WH-1000XM6 耳机", 2999.00);
    cart6.showCart();
    printSectionEnd();

    printSection("先选择信用卡支付");
    auto creditCard2 = make_shared<CreditCardStrategy>(
        "6225880198765432", "李四", "06/29", "456");
    cart6.setPaymentStrategy(creditCard2);
    cout << "    [客户端] 犹豫中...还是换一种方式吧" << endl;
    printSectionEnd();

    printSection("切换为微信支付");
    auto wechat2 = make_shared<WeChatPayStrategy>("oAbc9876543210zyxwvutsrqponml");
    cart6.setPaymentStrategy(wechat2);
    printSectionEnd();

    printSection("结算");
    PaymentResult result6 = cart6.checkout();
    cout << "    [结果] " << result6.message << endl;
    printSectionEnd();

    // ---------- 7. 展示策略模式的价值 ----------
    cout << endl;
    cout << ">>> 策略模式优势总结 <<<" << endl;
    cout << "  ├─ 算法独立：每种支付算法封装在独立的策略类中" << endl;
    cout << "  ├─ 运行时切换：客户端可在运行时自由选择/切换策略" << endl;
    cout << "  ├─ 开闭原则：新增支付方式只需添加新策略类" << endl;
    cout << "  ├─ 消除条件分支：购物车无需 if-else 判断支付类型" << endl;
    cout << "  ├─ 单一职责：每种策略只关注自己的支付逻辑" << endl;
    cout << "  └─ 可测试性：每种策略可独立测试" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}