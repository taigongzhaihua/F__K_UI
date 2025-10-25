#include "fk/ui/Window.h"
#include <iostream>
#include <type_traits>

int main() {
    std::cout << "=== Testing Window Width/Height Method Resolution ===" << std::endl;
    
    auto window = std::make_shared<fk::ui::Window>();
    
    // 测试 Window 的 Width/Height 方法
    window->Width(800)->Height(600);
    
    // 检查返回类型
    auto w = window->Width();
    auto h = window->Height();
    
    std::cout << "Window Width type: " << typeid(w).name() << std::endl;
    std::cout << "Window Height type: " << typeid(h).name() << std::endl;
    
    std::cout << "Width: " << w << std::endl;
    std::cout << "Height: " << h << std::endl;
    
    // 检查是否是 int 类型
    if constexpr (std::is_same_v<decltype(w), int>) {
        std::cout << "✅ Width() returns int (Window's version)" << std::endl;
    } else if constexpr (std::is_same_v<decltype(w), float>) {
        std::cout << "⚠️ Width() returns float (View's version)" << std::endl;
    }
    
    return 0;
}
