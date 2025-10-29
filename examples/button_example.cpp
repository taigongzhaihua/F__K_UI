#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include <iostream>

using namespace fk::ui;

/**
 * Button 控件示例
 * 
 * 演示如何创建和使用圆角按钮，包括：
 * - 设置圆角半径
 * - 自定义背景色和前景色
 * - 添加边框
 * - 处理点击事件
 */

int main() {
    std::cout << "=== Button 控件示例 ===" << std::endl;
    
    // 1. 创建基础按钮
    auto basicButton = Button<>::Create()
        ->Width(120)
        ->Height(40);
    
    std::cout << "✓ 创建基础按钮" << std::endl;
    std::cout << "  - 宽度: " << basicButton->GetWidth() << std::endl;
    std::cout << "  - 高度: " << basicButton->GetHeight() << std::endl;
    
    // 2. 创建圆角按钮（Material Design 风格）
    auto materialButton = Button<>::Create()
        ->CornerRadius(8.0f)
        ->Background("#4CAF50")    // 绿色
        ->Foreground("#FFFFFF")    // 白色文字
        ->BorderThickness(0.0f)    // 无边框
        ->Width(150)
        ->Height(45);
    
    std::cout << "\n✓ 创建圆角按钮（Material Design）" << std::endl;
    std::cout << "  - 已设置圆角半径: 8.0px" << std::endl;
    std::cout << "  - 已设置背景色: #4CAF50" << std::endl;
    std::cout << "  - 已设置前景色: #FFFFFF" << std::endl;
    
    // 3. 创建带边框的按钮
    auto borderedButton = Button<>::Create()
        ->CornerRadius(12.0f)
        ->Background("#FFFFFF")     // 白色背景
        ->Foreground("#2196F3")     // 蓝色文字
        ->BorderBrush("#2196F3")    // 蓝色边框
        ->BorderThickness(2.0f)
        ->Width(140)
        ->Height(42);
    
    std::cout << "\n✓ 创建带边框按钮" << std::endl;
    std::cout << "  - 圆角: 12.0px" << std::endl;
    std::cout << "  - 边框颜色: #2196F3" << std::endl;
    std::cout << "  - 边框粗细: 2.0px" << std::endl;
    
    // 4. 订阅点击事件
    int clickCount = 0;
    materialButton->OnClick([&clickCount](auto& button) {
        clickCount++;
        std::cout << "\n[事件] 按钮被点击！点击次数: " << clickCount << std::endl;
    });
    
    std::cout << "\n✓ 订阅点击事件" << std::endl;
    
    // 5. 动态修改样式
    std::cout << "\n=== 动态修改样式 ===" << std::endl;
    std::cout << "\n修改按钮为红色（危险按钮）..." << std::endl;
    materialButton->Background("#F44336");
    std::cout << "  - 新背景色已设置: #F44336" << std::endl;
    
    std::cout << "\n增大圆角半径..." << std::endl;
    materialButton->CornerRadius(20.0f);
    std::cout << "  - 新圆角已设置: 20.0px" << std::endl;
    
    // 6. 创建按钮组（在 StackPanel 中）
    std::cout << "\n=== 创建按钮组 ===" << std::endl;
    
    auto panel = StackPanel::Create()
        ->Orientation(Orientation::Vertical)
        ->Width(200)
        ->Height(300);
    
    auto btn1 = Button<>::Create()
        ->CornerRadius(8.0f)
        ->Background("#4CAF50")
        ->Width(180)
        ->Height(40);
    
    auto btn2 = Button<>::Create()
        ->CornerRadius(8.0f)
        ->Background("#2196F3")
        ->Width(180)
        ->Height(40);
    
    auto btn3 = Button<>::Create()
        ->CornerRadius(8.0f)
        ->Background("#FF9800")
        ->Width(180)
        ->Height(40);
    
    panel->AddChild(btn1);
    panel->AddChild(btn2);
    panel->AddChild(btn3);
    
    std::cout << "✓ 创建包含 3 个按钮的 StackPanel" << std::endl;
    std::cout << "  - 按钮 1: 绿色 (#4CAF50)" << std::endl;
    std::cout << "  - 按钮 2: 蓝色 (#2196F3)" << std::endl;
    std::cout << "  - 按钮 3: 橙色 (#FF9800)" << std::endl;
    
    // 7. 创建复合按钮示例
    std::cout << "\n=== 复合按钮示例 ===" << std::endl;
    
    auto saveButton = Button<>::Create()
        ->CornerRadius(4.0f)
        ->Background("#4CAF50")
        ->Foreground("#FFFFFF")
        ->BorderThickness(0.0f)
        ->Width(100)
        ->Height(35)
        ->OnClick([](auto& btn) {
            std::cout << "[保存] 按钮被点击" << std::endl;
        });
    
    auto cancelButton = Button<>::Create()
        ->CornerRadius(4.0f)
        ->Background("#F44336")
        ->Foreground("#FFFFFF")
        ->BorderThickness(0.0f)
        ->Width(100)
        ->Height(35)
        ->OnClick([](auto& btn) {
            std::cout << "[取消] 按钮被点击" << std::endl;
        });
    
    std::cout << "✓ 创建保存/取消按钮对" << std::endl;
    
    std::cout << "\n=== 示例完成 ===" << std::endl;
    std::cout << "\nButton 控件特性:" << std::endl;
    std::cout << "  ✓ 支持圆角设置" << std::endl;
    std::cout << "  ✓ 支持自定义背景色/前景色" << std::endl;
    std::cout << "  ✓ 支持边框样式" << std::endl;
    std::cout << "  ✓ 支持点击事件" << std::endl;
    std::cout << "  ✓ 支持链式调用" << std::endl;
    std::cout << "  ✓ 支持动态样式修改" << std::endl;
    
    return 0;
}
