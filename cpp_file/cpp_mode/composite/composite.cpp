/**
 * 组合模式 (Composite Pattern)
 * 
 * 将对象组合成树形结构以表示"部分-整体"的层次结构，
 * 使得用户对单个对象和组合对象的使用具有一致性。
 * 
 * 本例以公司组织架构为场景，演示组合模式的核心结构：
 *   - Component（组件接口）：组织单元（OrganizationUnit），定义叶子和容器的公共接口
 *   - Leaf（叶子节点）：员工（Employee），是树形结构中的最小单位，没有子节点
 *   - Composite（容器节点）：部门（Department），可以包含叶子节点或其他容器节点
 *   - Client（客户端）：统一接口操作叶子和容器，无需区分
 * 
 * 场景说明：
 *   某公司有多个层级的组织结构：总部下设技术部、市场部等，
 *   技术部又下设前端组、后端组、测试组等，每个组有多名员工。
 *   通过组合模式，可以用统一的接口来计算薪资总和、打印组织架构树等操作，
 *   无需关心当前操作的是单个员工还是整个部门。
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ======================== 组件接口：组织单元 ========================
class OrganizationUnit {
public:
    virtual ~OrganizationUnit() {}
    virtual string getName() const = 0;
    virtual double getSalary() const = 0;
    virtual int getHeadCount() const = 0;
    virtual void print(int depth = 0) const = 0;

    // 默认实现：叶子节点不支持添加/删除子节点
    virtual void add(shared_ptr<OrganizationUnit> unit) {
        throw runtime_error("叶子节点不支持添加子节点");
    }
    virtual void remove(shared_ptr<OrganizationUnit> unit) {
        throw runtime_error("叶子节点不支持删除子节点");
    }
};

// ======================== 叶子节点：员工 ========================
class Employee : public OrganizationUnit {
public:
    Employee(const string& name, const string& title, double salary)
        : name_(name), title_(title), salary_(salary) {}

    string getName() const override { return name_; }
    double getSalary() const override { return salary_; }
    int getHeadCount() const override { return 1; }

    void print(int depth = 0) const override {
        string indent(depth * 4, ' ');
        cout << indent << "├─ 👤 " << name_ << " (" << title_ 
             << ") - 月薪: " << salary_ << " 元" << endl;
    }

private:
    string name_;
    string title_;
    double salary_;
};

// ======================== 容器节点：部门 ========================
class Department : public OrganizationUnit {
public:
    Department(const string& name, const string& managerName = "")
        : name_(name), managerName_(managerName) {}

    string getName() const override { return name_; }

    // 递归计算部门薪资总和
    double getSalary() const override {
        double total = 0;
        for (const auto& child : children_) {
            total += child->getSalary();
        }
        return total;
    }

    // 递归计算部门总人数
    int getHeadCount() const override {
        int count = 0;
        for (const auto& child : children_) {
            count += child->getHeadCount();
        }
        return count;
    }

    // 添加子节点
    void add(shared_ptr<OrganizationUnit> unit) override {
        children_.push_back(unit);
    }

    // 删除子节点
    void remove(shared_ptr<OrganizationUnit> unit) override {
        auto it = find(children_.begin(), children_.end(), unit);
        if (it != children_.end()) {
            children_.erase(it);
        }
    }

    // 递归打印组织架构树
    void print(int depth = 0) const override {
        string indent(depth * 4, ' ');
        cout << indent << "📂 " << name_;
        if (!managerName_.empty()) {
            cout << " [负责人: " << managerName_ << "]";
        }
        cout << " — 人数: " << getHeadCount()
             << ", 月薪总计: " << getSalary() << " 元" << endl;
        for (const auto& child : children_) {
            child->print(depth + 1);
        }
    }

private:
    string name_;
    string managerName_;
    vector<shared_ptr<OrganizationUnit>> children_;
};

// ======================== 客户端 ========================
int main() {
    cout << "========================================" << endl;
    cout << "     组合模式演示 - 公司组织架构         " << endl;
    cout << "========================================" << endl;

    // ---------- 1. 创建叶子节点：员工 ----------
    cout << endl;
    cout << ">>> 步骤一：创建员工（叶子节点）<<<" << endl;

    // 技术部 - 前端组
    auto fe1 = make_shared<Employee>("张三", "高级前端工程师", 25000);
    auto fe2 = make_shared<Employee>("李四", "前端工程师", 18000);

    // 技术部 - 后端组
    auto be1 = make_shared<Employee>("王五", "架构师", 35000);
    auto be2 = make_shared<Employee>("赵六", "高级后端工程师", 28000);
    auto be3 = make_shared<Employee>("钱七", "后端工程师", 20000);

    // 技术部 - 测试组
    auto qa1 = make_shared<Employee>("孙八", "测试主管", 22000);
    auto qa2 = make_shared<Employee>("周九", "测试工程师", 15000);

    // 市场部
    auto mkt1 = make_shared<Employee>("吴十", "市场总监", 30000);
    auto mkt2 = make_shared<Employee>("郑十一", "市场专员", 12000);

    // 总经理
    auto ceo = make_shared<Employee>("林总", "总经理", 50000);

    cout << "  ✓ 已创建 " << "11 名员工" << endl;

    // ---------- 2. 创建容器节点：部门，并构建树形结构 ----------
    cout << endl;
    cout << ">>> 步骤二：构建组织架构树 <<<" << endl;

    // 技术部 - 前端组
    auto frontendGroup = make_shared<Department>("前端组", "张三");
    frontendGroup->add(fe1);
    frontendGroup->add(fe2);

    // 技术部 - 后端组
    auto backendGroup = make_shared<Department>("后端组", "王五");
    backendGroup->add(be1);
    backendGroup->add(be2);
    backendGroup->add(be3);

    // 技术部 - 测试组
    auto qaGroup = make_shared<Department>("测试组", "孙八");
    qaGroup->add(qa1);
    qaGroup->add(qa2);

    // 技术部
    auto techDept = make_shared<Department>("技术部", "王五");
    techDept->add(frontendGroup);
    techDept->add(backendGroup);
    techDept->add(qaGroup);

    // 市场部
    auto mktDept = make_shared<Department>("市场部", "吴十");
    mktDept->add(mkt1);
    mktDept->add(mkt2);

    // 公司总部（树根）
    auto company = make_shared<Department>("XX科技有限公司", "林总");
    company->add(ceo);
    company->add(techDept);
    company->add(mktDept);

    cout << "  ✓ 组织架构树构建完成" << endl;

    // ---------- 3. 统一接口操作：打印组织架构 ----------
    cout << endl;
    cout << ">>> 步骤三：打印完整组织架构 <<<" << endl;
    cout << endl;
    company->print(1);

    // ---------- 4. 统一接口操作：计算薪资 ----------
    cout << endl;
    cout << ">>> 步骤四：薪资统计（统一接口，透明访问）<<<" << endl;

    cout << endl;
    cout << "  【叶子节点 - 单个员工】" << endl;
    cout << "  " << fe1->getName() << " 月薪: " << fe1->getSalary() << " 元"
         << ", 人数: " << fe1->getHeadCount() << endl;

    cout << endl;
    cout << "  【组合节点 - 后端组】" << endl;
    cout << "  " << backendGroup->getName() << " 月薪总计: " 
         << backendGroup->getSalary() << " 元"
         << ", 人数: " << backendGroup->getHeadCount() << endl;

    cout << endl;
    cout << "  【组合节点 - 技术部】" << endl;
    cout << "  " << techDept->getName() << " 月薪总计: " 
         << techDept->getSalary() << " 元"
         << ", 人数: " << techDept->getHeadCount() << endl;

    cout << endl;
    cout << "  【组合节点 - 公司总部】" << endl;
    cout << "  " << company->getName() << " 月薪总计: " 
         << company->getSalary() << " 元"
         << ", 总人数: " << company->getHeadCount() << endl;

    // ---------- 5. 动态调整：部门重组 ----------
    cout << endl;
    cout << ">>> 步骤五：动态调整 - 将测试组从技术部移至公司直属 <<<" << endl;

    techDept->remove(qaGroup);
    company->add(qaGroup);

    cout << endl;
    cout << "  [重组后] 技术部人数: " << techDept->getHeadCount()
         << ", 月薪总计: " << techDept->getSalary() << " 元" << endl;
    cout << "  [重组后] 公司总人数: " << company->getHeadCount()
         << ", 月薪总计: " << company->getSalary() << " 元" << endl;

    // ---------- 6. 打印重组后的架构 ----------
    cout << endl;
    cout << ">>> 重组后的组织架构 <<<" << endl;
    cout << endl;
    company->print(1);

    // ---------- 7. 展示组合模式的价值 ----------
    cout << endl;
    cout << ">>> 组合模式优势总结 <<<" << endl;
    cout << "  ├─ 统一接口：叶子节点（Employee）和容器节点（Department）" << endl;
    cout << "  │  实现相同的 OrganizationUnit 接口" << endl;
    cout << "  ├─ 透明操作：客户端无需区分叶子和容器，" << endl;
    cout << "  │  getSalary()、getHeadCount() 等操作自动递归" << endl;
    cout << "  ├─ 树形结构：自然表达" << endl;
    cout << "  │  部门-子部门-员工" << endl;
    cout << "  │  的层级关系" << endl;
    cout << "  └─ 动态组合：运行时可自由添加、移除节点，灵活调整结构" << endl;

    cout << endl;
    cout << "========================================" << endl;
    cout << "              演示结束                   " << endl;
    cout << "========================================" << endl;

    return 0;
}