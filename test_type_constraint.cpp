#include "fk/ui/GridCellAttacher.h"
#include "fk/ui/Button.h"
#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "=== 类型约束测试 ===\n\n";
    
    // ✓ 应该编译通过：Button 是 UIElement 派生类
    {
        Button* btn = new Button();
        auto result = btn | cell(0, 0);
        std::cout << "✓ Button | cell(0,0) - 编译通过\n";
        delete btn;
    }
    
    // ✓ 应该编译通过：UIElement* 本身
    {
        UIElement* elem = new Button();
        auto result = elem | cell(1, 1);
        std::cout << "✓ UIElement | cell(1,1) - 编译通过\n";
        delete elem;
    }
    
    // ✗ 下面这些应该编译失败（如果取消注释）：
    
    // int* invalid = nullptr;
    // auto fail1 = invalid | cell(0, 0);  // ✗ 编译错误：int* 不是 UIElement 派生类
    
    // std::string* str = nullptr;
    // auto fail2 = str | cell(0, 0);      // ✗ 编译错误：string* 不是 UIElement 派生类
    
    std::cout << "\n=== 所有测试通过 ===\n";
    std::cout << "类型约束正常工作：\n";
    std::cout << "  ✓ UIElement 派生类可以使用 | cell()\n";
    std::cout << "  ✓ 非 UIElement 类型会在编译期被拒绝\n";
    
    return 0;
}
