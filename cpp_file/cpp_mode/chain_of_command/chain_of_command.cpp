/**
 * 责任链模式 (Chain of Responsibility Pattern)
 * 
 * 使多个对象都有机会处理请求，从而避免请求的发送者和接收者之间的
 * 耦合关系。将这些对象连成一条链，并沿着这条链传递请求，直到有一
 * 个对象处理它为止。
 * 
 * 本例以公司报销审批系统为场景，演示责任链模式的核心结构：
 *   - Handler（抽象处理者）：Approver，定义处理请求的接口，并持有下一个处理者的引用
 *   - ConcreteHandler（具体处理者）：
 *     · TeamLeader（组长）：可审批 0~1000 元的报销
 *     · DepartmentManager（部门经理）：可审批 1000~5000 元的报销
 *     · Director（总监）：可审批 5000~20000 元的报销
 *     · CEO（CEO）：可审批 20000~100000 元的报销
 * 
 * 场景说明：
 *   公司员工提交报销申请，根据金额大小由不同级别的领导审批。
 *   如果当前处理者的权限不够，则将请求传递给下一个处理者，
 *   直到请求被处理或整条链都无法处理。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// ======================== 报销请求 ========================
struct ExpenseRequest {
    string employee;     // 申请人
    string description;  // 报销描述
    double amount;       // 金额

    ExpenseRequest(const string& emp, const string& desc, double amt)
        : employee(emp), description(desc), amount(amt) {}
};

// ======================== 审批结果 ========================
struct ApprovalResult {
    bool approved;
    string approver;
    string message;

    ApprovalResult(bool a, const string& approver, const string& msg)
        : approved(a), approver(approver), message(msg) {}
};

// ======================== 抽象处理者：审批者 ========================
class Approver {
public:
    Approver(const string& name) : name_(name), next_(nullptr) {}
    virtual ~Approver() {}

    // 设置下一个处理者，返回下一个处理者以支持链式调用
    shared_ptr<Approver> setNext(shared_ptr<Approver> next) {
        next_ = next;
        return next;
    }

    // 处理报销请求
    virtual ApprovalResult handleRequest(const ExpenseRequest& request) = 0;

    string getName() const { return name_; }

protected:
    string name_;
    shared_ptr<Approver> next_;
};

// ======================== 具体处理者：组长 ========================
class TeamLeader : public Approver {
public:
    TeamLeader(const string& name) : Approver(name) {}

    ApprovalResult handleRequest(const ExpenseRequest& request) override {
        if (request.amount <= 1000) {
            return ApprovalResult(true, name_,
                "✅ [" + name_ + "] (组长) 审批通过: " + request.description
                + "，金额: " + to_string((int)request.amount) + " 元");
        }
        // 权限不够，传递给下一个处理者
        if (next_) {
            cout << "    [TeamLeader] " << name_ << " 权限不足 (限额1000元)，转交给上级..." << endl;
            return next_->handleRequest(request);
        }
        return ApprovalResult(false, name_, "❌ 无人可审批此报销请求");
    }
};

// ======================== 具体处理者：部门经理 ========================
class DepartmentManager : public Approver {
public:
    DepartmentManager(const string& name) : Approver(name) {}

    ApprovalResult handleRequest(const ExpenseRequest& request) override {
        if (request.amount <= 5000) {
            return ApprovalResult(true, name_,
                "✅ [" + name_ + "] (部门经理) 审批通过: " + request.description
                + "，金额: " + to_string((int)request.amount) + " 元");
        }
        if (next_) {
            cout << "    [DeptManager] " << name_ << " 权限不足 (限额5000元)，转交给上级..." << endl;
            return next_->handleRequest(request);
        }
        return ApprovalResult(false, name_, "❌ 无人可审批此报销请求");
    }
};

// ======================== 具体处理者：总监 ========================
class Director : public Approver {
public:
    Director(const string& name) : Approver(name) {}

    ApprovalResult handleRequest(const ExpenseRequest& request) override {
        if (request.amount <= 20000) {
            return ApprovalResult(true, name_,
                "✅ [" + name_ + "] (总监) 审批通过: " + request.description
                + "，金额: " + to_string((int)request.amount) + " 元");
        }
        if (next_) {
            cout << "    [Director] " << name_ << " 权限不足 (限额20000元)，转交给上级..." << endl;
            return next_->handleRequest(request);
        }
        return ApprovalResult(false, name_, "❌ 无人可审批此报销请求");
    }
};

// ======================== 具体处理者：CEO ========================
class CEO : public Approver {
public:
    CEO(const string& name) : Approver(name) {}

    ApprovalResult handleRequest(const ExpenseRequest& request) override {
        if (request.amount <= 100000) {
            return ApprovalResult(true, name_,
                "✅ [" + name_ + "] (CEO) 审批通过: " + request.description
                + "，金额: " + to_string((int)request.amount) + " 元");
        }
        // CEO 也无法处理
        return ApprovalResult(false, name_,
            "❌ [" + name_ + "] (CEO) 拒绝: 金额 " + to_string((int)request.amount)
            + " 元超出公司报销上限 (100000元)");
    }
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
    cout << "   责任链模式演示 - 公司报销审批系统          " << endl;
    cout << "============================================" << endl;

    // ---------- 1. 构建责任链 ----------
    cout << endl;
    cout << ">>> 步骤一：构建审批责任链 <<<" << endl;

    auto teamLeader = make_shared<TeamLeader>("赵组长");
    auto deptManager = make_shared<DepartmentManager>("钱经理");
    auto director = make_shared<Director>("孙总监");
    auto ceo = make_shared<CEO>("李CEO");

    // 组装责任链：组长 → 部门经理 → 总监 → CEO
    teamLeader->setNext(deptManager)->setNext(director)->setNext(ceo);

    cout << "  审批链: " << teamLeader->getName()
         << " → " << deptManager->getName()
         << " → " << director->getName()
         << " → " << ceo->getName() << endl;
    cout << endl;
    cout << "  审批权限:" << endl;
    cout << "    赵组长(组长)    : 0 ~ 1,000 元" << endl;
    cout << "    钱经理(部门经理): 0 ~ 5,000 元" << endl;
    cout << "    孙总监(总监)    : 0 ~ 20,000 元" << endl;
    cout << "    李CEO(CEO)      : 0 ~ 100,000 元" << endl;

    // ---------- 2. 提交报销请求 ----------
    cout << endl;
    cout << ">>> 步骤二：提交报销请求 <<<" << endl;

    // 构造一系列报销请求，金额从小到大
    vector<ExpenseRequest> requests = {
        ExpenseRequest("小王", "购买办公文具",           200),
        ExpenseRequest("小张", "团队聚餐费用",           800),
        ExpenseRequest("小李", "客户招待费用",           3500),
        ExpenseRequest("小陈", "部门团建活动",           8000),
        ExpenseRequest("小刘", "参加行业大会差旅费",     15000),
        ExpenseRequest("小赵", "购买服务器设备",         68000),
        ExpenseRequest("小周", "并购项目咨询费",         500000),
    };

    // 所有请求都从链头（组长）开始处理
    cout << endl;
    cout << "  所有请求统一提交给链头 [" << teamLeader->getName() << "] 处理" << endl;

    for (size_t i = 0; i < requests.size(); ++i) {
        const auto& req = requests[i];
        cout << endl;
        printSection("报销申请 #" + to_string(i + 1) + " —— " + req.employee
                     + " 申请 " + to_string((int)req.amount) + " 元");

        ApprovalResult result = teamLeader->handleRequest(req);

        cout << "    " << result.message << endl;
        printSectionEnd();
    }

    // ---------- 3. 展示责任链模式的价值 ----------
    cout << endl;
    cout << ">>> 责任链模式优势总结 <<<" << endl;
    cout << "  ├─ 解耦：发送者不需要知道具体由谁处理请求" << endl;
    cout << "  │  （小王提交报销时无需了解审批流程）" << endl;
    cout << "  ├─ 灵活：可以动态调整链的结构和顺序" << endl;
    cout << "  │  （如临时加入副总审批环节）" << endl;
    cout << "  ├─ 单一职责：每个处理者只关注自己能处理的请求" << endl;
    cout << "  │  （组长只管小额、CEO只管大额）" << endl;
    cout << "  ├─ 开闭原则：新增处理者只需接入链中，无需修改已有代码" << endl;
    cout << "  └─ 请求可能不被处理：链尾未处理的请求有明确的兜底逻辑" << endl;

    cout << endl;
    cout << "============================================" << endl;
    cout << "                演示结束                     " << endl;
    cout << "============================================" << endl;

    return 0;
}