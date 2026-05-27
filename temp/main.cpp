#include <iostream>
#include <functional>
int a = 10;
int main() {
    int b = 20;
    std::vector<std::function<void()>> fs;
    for(int i = 0; i < 10; i++) {
        fs.push_back([i]() {
            std::cout << i << std::endl;
        });
    }
    for(auto& f : fs) {
        f();
    }
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    return 0;
}
