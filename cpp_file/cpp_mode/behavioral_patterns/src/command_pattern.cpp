#include <iostream>
#include <memory>
#include <vector>
using namespace std;

//命令接口
class Command {
public:    
    virtual void execute() = 0;
    virtual ~Command() {}
};


int main() {
    //示例代码
    return 0;
}