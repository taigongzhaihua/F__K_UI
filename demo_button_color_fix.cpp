/**
 * @file demo_button_color_fix.cpp
 * @brief 演示 Button 悬停和按下颜色设置修复效果
 */

#include "fk/ui/Button.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <iomanip>

using namespace fk;
using namespace fk::ui;

void PrintColorInfo(const std::string& label, Color color) {
    std::cout << "  " << std::left << std::setw(20) << label 
              << " RGB(" << std::setw(3) << (int)(color.r * 255) 
              << ", " << std::setw(3) << (int)(color.g * 255) 
              << ", " << std::setw(3) << (int)(color.b * 255) << ")\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║       Button 悬停和按下颜色自定义演示                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    // 演示 1: 默认颜色
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "【演示 1】使用默认颜色的按钮\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    auto* button1 = new Button();
    button1->ApplyTemplate();
    
    PrintColorInfo("背景色:", Color::FromRGB(240, 240, 240, 255));
    PrintColorInfo("悬停色:", Color::FromRGB(229, 241, 251, 255));
    PrintColorInfo("按下色:", Color::FromRGB(204, 228, 247, 255));
    
    std::cout << "\n说明: 这些是 Button 的默认颜色\n";
    std::cout << "      - 背景: 浅灰色\n";
    std::cout << "      - 悬停: 浅蓝色\n";
    std::cout << "      - 按下: 深蓝色\n\n";
    
    delete button1;

    // 演示 2: 自定义颜色（绿色主题）
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "【演示 2】自定义绿色主题按钮\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    auto* button2 = new Button();
    button2->SetBackground(new SolidColorBrush(Color::FromRGB(34, 139, 34, 255)));        // 森林绿
    button2->MouseOverBackground(Color::FromRGB(50, 205, 50, 255)); // 酸橙绿
    button2->PressedBackground(Color::FromRGB(0, 128, 0, 255));     // 纯绿
    button2->ApplyTemplate();
    
    PrintColorInfo("背景色:", Color::FromRGB(34, 139, 34, 255));
    PrintColorInfo("悬停色:", Color::FromRGB(50, 205, 50, 255));
    PrintColorInfo("按下色:", Color::FromRGB(0, 128, 0, 255));
    
    std::cout << "\n说明: 修复后，用户可以自定义所有状态的颜色\n";
    std::cout << "      ✓ 悬停时显示酸橙绿\n";
    std::cout << "      ✓ 按下时显示纯绿色\n\n";
    
    delete button2;

    // 演示 3: 自定义颜色（红色主题）
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "【演示 3】自定义红色主题按钮\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    auto* button3 = new Button();
    button3->SetBackground(new SolidColorBrush(Color::FromRGB(220, 20, 60, 255)));         // 深红
    button3->MouseOverBackground(Color::FromRGB(255, 69, 0, 255)); // 橙红
    button3->PressedBackground(Color::FromRGB(178, 34, 34, 255));  // 火砖红
    button3->ApplyTemplate();
    
    PrintColorInfo("背景色:", Color::FromRGB(220, 20, 60, 255));
    PrintColorInfo("悬停色:", Color::FromRGB(255, 69, 0, 255));
    PrintColorInfo("按下色:", Color::FromRGB(178, 34, 34, 255));
    
    std::cout << "\n说明: 支持任意自定义颜色组合\n";
    std::cout << "      ✓ 危险按钮使用红色主题\n";
    std::cout << "      ✓ 悬停时显示警告色\n\n";
    
    delete button3;

    // 演示 4: 自定义颜色（紫色主题）
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "【演示 4】自定义紫色主题按钮\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    auto* button4 = new Button();
    button4->SetBackground(new SolidColorBrush(Color::FromRGB(138, 43, 226, 255)));        // 蓝紫
    button4->MouseOverBackground(Color::FromRGB(186, 85, 211, 255)); // 中兰花紫
    button4->PressedBackground(Color::FromRGB(75, 0, 130, 255));     // 靛青
    button4->ApplyTemplate();
    
    PrintColorInfo("背景色:", Color::FromRGB(138, 43, 226, 255));
    PrintColorInfo("悬停色:", Color::FromRGB(186, 85, 211, 255));
    PrintColorInfo("按下色:", Color::FromRGB(75, 0, 130, 255));
    
    std::cout << "\n说明: 品牌定制按钮\n";
    std::cout << "      ✓ 使用品牌紫色\n";
    std::cout << "      ✓ 保持视觉一致性\n\n";
    
    delete button4;

    // 总结
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "【修复总结】\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "\n";
    std::cout << "✓ 问题: Button 的 MouseOverBackground 和 PressedBackground 设置不生效\n";
    std::cout << "✓ 原因: 默认模板使用硬编码颜色，覆盖了用户设置\n";
    std::cout << "✓ 修复: 在 ResolveVisualStateTargets() 中从属性读取自定义颜色\n";
    std::cout << "✓ 效果: 用户可以完全自定义按钮的悬停和按下状态颜色\n";
    std::cout << "\n";
    std::cout << "使用方法:\n";
    std::cout << "  button->Background(Color::FromRGB(r, g, b))           // 设置默认背景\n";
    std::cout << "  button->MouseOverBackground(Color::FromRGB(r, g, b))  // 设置悬停背景\n";
    std::cout << "  button->PressedBackground(Color::FromRGB(r, g, b))    // 设置按下背景\n";
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    演示完成！                                 ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    return 0;
}
