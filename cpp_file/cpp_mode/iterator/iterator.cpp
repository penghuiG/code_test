/**
 * 迭代器模式 (Iterator Pattern)
 * 
 * 提供一种方法顺序访问一个聚合对象中的各个元素，而不暴露该对象的内部表示。
 * 迭代器模式将遍历行为从聚合对象中分离出来，使得聚合对象可以专注于数据存储，
 * 而迭代器负责遍历逻辑。
 * 
 * 本例以在线书店书架管理系统为场景，演示迭代器模式的核心结构：
 *   - Iterator（抽象迭代器）：Iterator<T>，定义 hasNext()、next() 接口
 *   - ConcreteIterator（具体迭代器）：
 *     · BookShelfIterator：书架迭代器（正序遍历）
 *     · BookShelfReverseIterator：书架逆序迭代器（逆序遍历）
 *     · MagazineRackIterator：杂志架迭代器
 *   - Aggregate（抽象聚合）：Collection<T>，定义 createIterator() 接口
 *   - ConcreteAggregate（具体聚合）：
 *     · BookShelf：书架，存储书籍的集合
 *     · MagazineRack：杂志架，存储杂志的集合
 * 
 * 场景说明：
 *   书店管理员需要遍历书架上的书籍和杂志架上的杂志。
 *   不同的集合（书架、杂志架）底层数据结构不同（数组、链表等），
 *   但通过统一的迭代器接口，客户端代码可以以相同的方式遍历它们。
 *   同时演示了正序迭代、逆序迭代、筛选迭代等多种迭代策略。
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
using namespace std;

// ======================== 数据元素：书籍 ========================
class Book {
public:
    Book(const string& title, const string& author, double price)
        : title_(title), author_(author), price_(price) {}

    string getTitle() const { return title_; }
    string getAuthor() const { return author_; }
    double getPrice() const { return price_; }

    void display() const {
        cout << "    📖 《" << title_ << "》 - " << author_ << "  ¥" << price_;
    }

private:
    string title_;
    string author_;
    double price_;
};

// ======================== 数据元素：杂志 ========================
class Magazine {
public:
    Magazine(const string& name, int issue, const string& publisher)
        : name_(name), issue_(issue), publisher_(publisher) {}

    string getName() const { return name_; }
    int getIssue() const { return issue_; }
    string getPublisher() const { return publisher_; }

    void display() const {
        cout << "    📰 " << name_ << " 第" << issue_ << "期 - " << publisher_;
    }

private:
    string name_;
    int issue_;
    string publisher_;
};

// ======================== 抽象迭代器：Iterator ========================
template <typename T>
class Iterator {
public:
    virtual ~Iterator() {}
    virtual bool hasNext() const = 0;
    virtual T next() = 0;
    virtual void reset() = 0;
    virtual string getDescription() const = 0;
};

// ======================== 抽象聚合：Collection ========================
template <typename T>
class Collection {
public:
    virtual ~Collection() {}
    virtual shared_ptr<Iterator<T>> createIterator() = 0;
    virtual int size() const = 0;
    virtual string getName() const = 0;
};

// ======================== 具体聚合：书架 ========================
class BookShelf : public Collection<Book> {
public:
    BookShelf(const string& name) : name_(name) {}

    void addBook(const Book& book) {
        books_.push_back(book);
    }

    Book getBookAt(int index) const {
        return books_[index];
    }

    int size() const override {
        return books_.size();
    }

    string getName() const override {
        return name_;
    }

    // 创建正序迭代器
    shared_ptr<Iterator<Book>> createIterator() override;

    // 创建逆序迭代器
    shared_ptr<Iterator<Book>> createReverseIterator();

    // 创建筛选迭代器（按价格筛选）
    shared_ptr<Iterator<Book>> createFilterIterator(function<bool(const Book&)> predicate);

private:
    string name_;
    vector<Book> books_;
};

// ======================== 具体聚合：杂志架 ========================
class MagazineRack : public Collection<Magazine> {
public:
    MagazineRack(const string& name) : name_(name) {}

    void addMagazine(const Magazine& magazine) {
        magazines_.push_back(magazine);
    }

    Magazine getMagazineAt(int index) const {
        return magazines_[index];
    }

    int size() const override {
        return magazines_.size();
    }

    string getName() const override {
        return name_;
    }

    // 创建正序迭代器
    shared_ptr<Iterator<Magazine>> createIterator() override;

private:
    string name_;
    vector<Magazine> magazines_;
};

// ======================== 具体迭代器：书架正序迭代器 ========================
class BookShelfIterator : public Iterator<Book> {
public:
    BookShelfIterator(shared_ptr<BookShelf> shelf) 
        : shelf_(shelf), index_(0) {}

    bool hasNext() const override {
        return index_ < shelf_->size();
    }

    Book next() override {
        Book book = shelf_->getBookAt(index_);
        index_++;
        return book;
    }

    void reset() override {
        index_ = 0;
    }

    string getDescription() const override {
        return shelf_->getName() + " 正序迭代器";
    }

private:
    shared_ptr<BookShelf> shelf_;
    int index_;
};

// ======================== 具体迭代器：书架逆序迭代器 ========================
class BookShelfReverseIterator : public Iterator<Book> {
public:
    BookShelfReverseIterator(shared_ptr<BookShelf> shelf) 
        : shelf_(shelf), index_(shelf->size() - 1) {}

    bool hasNext() const override {
        return index_ >= 0;
    }

    Book next() override {
        Book book = shelf_->getBookAt(index_);
        index_--;
        return book;
    }

    void reset() override {
        index_ = shelf_->size() - 1;
    }

    string getDescription() const override {
        return shelf_->getName() + " 逆序迭代器";
    }

private:
    shared_ptr<BookShelf> shelf_;
    int index_;
};

// ======================== 具体迭代器：书架筛选迭代器 ========================
class BookShelfFilterIterator : public Iterator<Book> {
public:
    BookShelfFilterIterator(shared_ptr<BookShelf> shelf, 
                            function<bool(const Book&)> predicate)
        : shelf_(shelf), predicate_(predicate), index_(0) {
        // 预先筛选出符合条件的书籍索引
        for (int i = 0; i < shelf_->size(); i++) {
            if (predicate_(shelf_->getBookAt(i))) {
                filteredIndices_.push_back(i);
            }
        }
    }

    bool hasNext() const override {
        return index_ < static_cast<int>(filteredIndices_.size());
    }

    Book next() override {
        Book book = shelf_->getBookAt(filteredIndices_[index_]);
        index_++;
        return book;
    }

    void reset() override {
        index_ = 0;
    }

    string getDescription() const override {
        return shelf_->getName() + " 筛选迭代器 (共" 
               + to_string(filteredIndices_.size()) + "本匹配)";
    }

private:
    shared_ptr<BookShelf> shelf_;
    function<bool(const Book&)> predicate_;
    vector<int> filteredIndices_;
    int index_;
};

// ======================== 具体迭代器：杂志架迭代器 ========================
class MagazineRackIterator : public Iterator<Magazine> {
public:
    MagazineRackIterator(shared_ptr<MagazineRack> rack) 
        : rack_(rack), index_(0) {}

    bool hasNext() const override {
        return index_ < rack_->size();
    }

    Magazine next() override {
        Magazine magazine = rack_->getMagazineAt(index_);
        index_++;
        return magazine;
    }

    void reset() override {
        index_ = 0;
    }

    string getDescription() const override {
        return rack_->getName() + " 正序迭代器";
    }

private:
    shared_ptr<MagazineRack> rack_;
    int index_;
};

// ======================== 实现 Collection 中的创建迭代器方法 ========================
shared_ptr<Iterator<Book>> BookShelf::createIterator() {
    return make_shared<BookShelfIterator>(shared_ptr<BookShelf>(this, [](BookShelf*){}));
}

shared_ptr<Iterator<Book>> BookShelf::createReverseIterator() {
    return make_shared<BookShelfReverseIterator>(shared_ptr<BookShelf>(this, [](BookShelf*){}));
}

shared_ptr<Iterator<Book>> BookShelf::createFilterIterator(function<bool(const Book&)> predicate) {
    return make_shared<BookShelfFilterIterator>(
        shared_ptr<BookShelf>(this, [](BookShelf*){}), predicate);
}

shared_ptr<Iterator<Magazine>> MagazineRack::createIterator() {
    return make_shared<MagazineRackIterator>(
        shared_ptr<MagazineRack>(this, [](MagazineRack*){}));
}

// ======================== 辅助函数 ========================
void printSection(const string& title) {
    cout << endl;
    cout << "  ┌─ " << title << " ─────────────────────────────" << endl;
}

void printSectionEnd() {
    cout << "  └──────────────────────────────────────────────" << endl;
}

// 通用遍历函数：客户端只需面向迭代器编程，无需知道集合的具体类型
template <typename T>
void traverseCollection(Iterator<T>& iter) {
    while (iter.hasNext()) {
        T item = iter.next();
        item.display();
        cout << endl;
    }
}

// ======================== 客户端 ========================
int main() {
    cout << "============================================" << endl;
    cout << "   迭代器模式演示 - 在线书店书架管理系统      " << endl;
    cout << "============================================" << endl;

    // ---------- 1. 创建聚合并填充数据 ----------
    cout << endl;
    cout << ">>> 步骤一：创建书架和杂志架，添加数据 <<<" << endl;

    auto shelf1 = make_shared<BookShelf>("科技书架");
    shelf1->addBook(Book("深入理解计算机系统", "Randal E. Bryant", 139.0));
    shelf1->addBook(Book("设计模式", "GoF", 59.0));
    shelf1->addBook(Book("C++ Primer", "Stanley B. Lippman", 128.0));
    shelf1->addBook(Book("算法导论", "Thomas H. Cormen", 128.0));
    shelf1->addBook(Book("代码整洁之道", "Robert C. Martin", 59.0));

    auto shelf2 = make_shared<BookShelf>("文学书架");
    shelf2->addBook(Book("百年孤独", "加西亚·马尔克斯", 55.0));
    shelf2->addBook(Book("活着", "余华", 39.5));
    shelf2->addBook(Book("三体", "刘慈欣", 68.0));

    auto rack1 = make_shared<MagazineRack>("时尚杂志架");
    rack1->addMagazine(Magazine("Vogue", 202406, "康泰纳仕"));
    rack1->addMagazine(Magazine("GQ", 202405, "康泰纳仕"));
    rack1->addMagazine(Magazine("时尚芭莎", 202406, "时尚集团"));

    cout << "  已创建: 科技书架(" << shelf1->size() << "本), 文学书架(" 
         << shelf2->size() << "本), 时尚杂志架(" << rack1->size() << "本)" << endl;

    // ---------- 2. 使用正序迭代器遍历书架 ----------
    printSection("遍历科技书架（正序）");
    cout << "  迭代器: " << endl;
    auto forwardIter = shelf1->createIterator();
    cout << "    [使用 " << forwardIter->getDescription() << "]" << endl;
    traverseCollection(*forwardIter);
    printSectionEnd();

    // ---------- 3. 使用逆序迭代器遍历书架 ----------
    printSection("遍历科技书架（逆序）");
    auto reverseIter = shelf1->createReverseIterator();
    cout << "    [使用 " << reverseIter->getDescription() << "]" << endl;
    traverseCollection(*reverseIter);
    printSectionEnd();

    // ---------- 4. 使用筛选迭代器：按价格筛选 ----------
    printSection("筛选科技书架（价格 > 100元）");
    auto filterIter = shelf1->createFilterIterator(
        [](const Book& book) { return book.getPrice() > 100.0; }
    );
    cout << "    [使用 " << filterIter->getDescription() << "]" << endl;
    traverseCollection(*filterIter);
    printSectionEnd();

    // ---------- 5. 使用筛选迭代器：按作者筛选 ----------
    printSection("筛选科技书架（作者含 'C' 的书籍）");
    auto authorFilterIter = shelf1->createFilterIterator(
        [](const Book& book) { 
            string author = book.getAuthor();
            return author.find('C') != string::npos || author.find('c') != string::npos;
        }
    );
    cout << "    [使用 " << authorFilterIter->getDescription() << "]" << endl;
    traverseCollection(*authorFilterIter);
    printSectionEnd();

    // ---------- 6. 遍历文学书架 ----------
    printSection("遍历文学书架（正序）");
    auto shelf2Iter = shelf2->createIterator();
    cout << "    [使用 " << shelf2Iter->getDescription() << "]" << endl;
    traverseCollection(*shelf2Iter);
    printSectionEnd();

    // ---------- 7. 使用迭代器遍历杂志架 ----------
    printSection("遍历时尚杂志架（正序）");
    auto magazineIter = rack1->createIterator();
    cout << "    [使用 " << magazineIter->getDescription() << "]" << endl;
    traverseCollection(*magazineIter);
    printSectionEnd();

    // ---------- 8. 演示迭代器的 reset 功能 ----------
    printSection("演示迭代器 Reset 功能");
    auto resetDemoIter = shelf1->createIterator();
    cout << "    [第一次遍历]" << endl;
    traverseCollection(*resetDemoIter);
    cout << "    [调用 reset() 后再次遍历]" << endl;
    resetDemoIter->reset();
    traverseCollection(*resetDemoIter);
    printSectionEnd();

    // ---------- 9. 统一接口遍历不同集合 ----------
    printSection("统一接口遍历演示");
    cout << "    客户端代码无需知道集合的具体类型，" << endl;
    cout << "    只需面向 Iterator 接口编程即可遍历任何集合。" << endl;
    cout << endl;

    // 将不同集合放入统一容器
    vector<shared_ptr<Collection<Book>>> collections = {shelf1, shelf2};
    for (size_t i = 0; i < collections.size(); i++) {
        cout << "    [" << collections[i]->getName() << "] 包含 " 
             << collections[i]->size() << " 本书:" << endl;
        auto iter = collections[i]->createIterator();
        int count = 1;
        while (iter->hasNext()) {
            Book book = iter->next();
            cout << "      " << count++ << ". " << book.getTitle() 
                 << " (¥" << book.getPrice() << ")" << endl;
        }
        cout << endl;
    }
    printSectionEnd();

    // ---------- 10. 迭代器模式总结 ----------
    cout << endl;
    cout << ">>> 迭代器模式优势总结 <<<" << endl;
    cout << "  ├─ 封装性：客户端无需了解聚合对象的内部结构" << endl;
    cout << "  │  （书架底层用 vector，但客户端通过迭代器访问）" << endl;
    cout << "  ├─ 统一接口：不同类型的聚合对象可以使用相同的遍历方式" << endl;
    cout << "  │  （BookShelf 和 MagazineRack 都用 hasNext/next 遍历）" << endl;
    cout << "  ├─ 多种遍历策略：同一个聚合对象可以有不同的迭代器" << endl;
    cout << "  │  （正序、逆序、筛选等多种迭代方式）" << endl;
    cout << "  ├─ 单一职责：聚合对象专注数据存储，迭代器专注遍历逻辑" << endl;
    cout << "  │  （BookShelf 管理书籍，Iterator 负责遍历）" << endl;
    cout << "  └─ 开闭原则：新增遍历方式只需新增迭代器类，无需修改聚合类" << endl;
    cout << "     （新增 FilterIterator 时无需修改 BookShelf）" << endl;

    cout << endl;
    cout << "============================================" << endl;
    cout << "                演示结束                     " << endl;
    cout << "============================================" << endl;

    return 0;
}