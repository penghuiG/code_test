#include <iostream>
#include <memory>
#include <string>
using namespace std;

class Product {
public:
    virtual void show() const = 0;
    virtual std::shared_ptr<Product> clone() const = 0;
    virtual void setNumber(const string& number) = 0;
    virtual ~Product() {}
private:
    // string number_;

};

class ConcreteProduct : public Product {
public:
    ConcreteProduct(const string& number) {number_ = number;}
    void show() const override {
        cout << "ConcreteProduct: " << number_ << endl;
    }
    std::shared_ptr<Product> clone() const override {
        return std::make_shared<ConcreteProduct>(*this);
    }
    void setNumber(const string& number) { number_ = number; }
private:
    string number_;
};

class A{
public:
    A(const int& x_){x = x_;}
    int x;
    std::shared_ptr<A> clone(){return std::make_shared<A>(*this);}
};

int main() {

    std::shared_ptr<A> a = std::make_shared<A>(10);
    std::shared_ptr<A> b = a;
    std::cout << "a = " << a->x << "; b = " << b->x <<std::endl;
    b->x = 20;
    std::cout << "a = " << a->x << "; b = " << b->x <<std::endl;

    std::shared_ptr<A> ca = std::make_shared<A>(10);
    std::shared_ptr<A> cb = ca->clone();
    std::cout << "ca = " << ca->x << "; cb = " << cb->x << std::endl;
    cb->x = 20;
    std::cout << "ca = " << ca->x << "; cb = " << cb->x << std::endl;

    std::weak_ptr<A> wa = a;
    if(auto wp= wa.lock())//wp是什么类型？
    {
        wp->x = 30;
    }
    // std::cout << wa->x<< std::endl;//无法编译通过
    if(auto wp = wa.lock())
    {
        std::cout << "wa = " << wp->x << std::endl;
    }

    std::cout << "a = " << a->x << "; b = " << b->x <<std::endl;
    return 0;
}