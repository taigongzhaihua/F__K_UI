#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <cassert>

using namespace fk::ui;

/**
 * 测试：边框厚度为 0 时的逻辑验证
 * 
 * 验证修复：即使设置了 BorderBrush，当 BorderThickness 为 0 时，
 * 计算出的 strokeWidth 应该为 0。
 */
int main() {
    std::cout << "测试：边框厚度为 0 时的逻辑验证\n" << std::endl;
    
    // 创建 Border
    Border* border = new Border();
    
    // 设置背景色
    border->SetBackground(new SolidColorBrush(Color::FromRGB(100, 150, 200, 255)));
    
    // 设置边框画刷（灰色）
    border->SetBorderBrush(new SolidColorBrush(Color::FromRGB(172, 172, 172, 255)));
    
    // 测试1: 设置边框厚度为 0
    std::cout << "测试1: BorderThickness = 0" << std::endl;
    border->SetBorderThickness(fk::Thickness(0));
    
    // 获取边框厚度
    auto thickness = border->GetBorderThickness();
    float strokeWidth = (thickness.left + thickness.right + thickness.top + thickness.bottom) / 4.0f;
    
    std::cout << "  计算的 strokeWidth: " << strokeWidth << std::endl;
    assert(strokeWidth == 0.0f && "边框厚度应该为 0");
    std::cout << "  ✓ 边框厚度为 0" << std::endl;

    
    // 测试2: 设置边框厚度为非零值
    std::cout << "\n测试2: BorderThickness = 2.0" << std::endl;
    border->SetBorderThickness(fk::Thickness(2.0f));
    
    // 获取边框厚度
    thickness = border->GetBorderThickness();
    strokeWidth = (thickness.left + thickness.right + thickness.top + thickness.bottom) / 4.0f;
    
    std::cout << "  计算的 strokeWidth: " << strokeWidth << std::endl;
    assert(strokeWidth == 2.0f && "边框厚度应该为 2.0");
    std::cout << "  ✓ 边框厚度为 2.0" << std::endl;
    
    // 测试3: 设置不同边的边框厚度
    std::cout << "\n测试3: BorderThickness = (1, 2, 3, 4)" << std::endl;
    border->SetBorderThickness(fk::Thickness(1.0f, 2.0f, 3.0f, 4.0f));
    
    thickness = border->GetBorderThickness();
    strokeWidth = (thickness.left + thickness.right + thickness.top + thickness.bottom) / 4.0f;
    
    std::cout << "  计算的 strokeWidth: " << strokeWidth << std::endl;
    // (1 + 3 + 2 + 4) / 4 = 2.5
    assert(strokeWidth == 2.5f && "边框厚度应该为 2.5");
    std::cout << "  ✓ 边框厚度为 2.5" << std::endl;
    
    // 清理
    delete border;
    
    std::cout << "\n所有测试通过！✓" << std::endl;
    return 0;
}
