#include <iostream>
#include <emscripten/emscripten.h>
using namespace std;

// 这是一个普通的 C++ 函数
void sayHello() {
    cout << "Hello from C++ inside the browser!" << endl;
    
    // 使用 Emscripten 特有的宏调用 JavaScript
    EM_ASM({
        console.log("JavaScript received message from C++!");
        alert("成功！C++ 代码已经在浏览器中运行了。");
    });
}

int main() {
    cout << "程序开始运行..." << std::endl;
    sayHello();
    cout << "程序结束。" << std::endl;
    return 0;
}
