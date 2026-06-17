/*
    简单工厂模式（Simple Factory Pattern）是一种创建型设计模式
    ，它提供一个工厂类来创建对象，而不是直接使用构造函数。工厂类
    根据输入参数决定创建哪个具体的对象实例。这种模式可以将对象的
    创建逻辑集中在一个地方，简化代码并提高可维护性。
*/

#include <iostream>
#include <memory>

// 产品接口
class Shape {
public:
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

// 具体产品
class Circle : public Shape {
public:
    void draw() override { std::cout << "Drawing Circle\n"; }
};

class Rectangle : public Shape {
public:
    void draw() override { std::cout << "Drawing Rectangle\n"; }
};

// 简单工厂
class ShapeFactory {
public:
    static std::unique_ptr<Shape> createShape(const std::string& type) {//这里为什么用智能指针 ？因为工厂模式通常涉及到动态分配对象，使用智能指针可以自动管理内存，避免内存泄漏。
        if (type == "Circle") return std::make_unique<Circle>();
        if (type == "Rectangle") return std::make_unique<Rectangle>();
        return nullptr;
    }
};

// 使用示例
int main() {
    auto circle = ShapeFactory::createShape("Circle");
    auto rect = ShapeFactory::createShape("Rectangle");
    
    circle->draw();  // Drawing Circle
    rect->draw();    // Drawing Rectangle
}