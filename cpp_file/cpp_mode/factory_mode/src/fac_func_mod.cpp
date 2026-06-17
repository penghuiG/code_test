#include <iostream>
#include <memory>

// 产品接口
class Button {
public:
    virtual void render() = 0;
    virtual ~Button() = default;
};

// 具体产品
class WindowsButton : public Button {
public:
    void render() override { std::cout << "Windows Button\n"; }
    ~WindowsButton() {
        std::cout << "WindowsButton destructor called\n";
    }
};

class MacButton : public Button {
public:
    void render() override { std::cout << "Mac Button\n"; }
    ~MacButton() {
        std::cout << "MacButton destructor called\n";
    }
};

// 工厂接口
class Dialog {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    void render() {
        auto button = createButton();
        button->render();
    }
    virtual ~Dialog() = default;
};

// 具体工厂
class WindowsDialog : public Dialog {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
};

class MacDialog : public Dialog {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
};

// 使用示例
int main() {
    std::unique_ptr<Dialog> winDialog = std::make_unique<WindowsDialog>();
    winDialog->render();  // Windows Button

    std::unique_ptr<Dialog> macDialog = std::make_unique<MacDialog>();
    macDialog->render();  // Mac Button
}