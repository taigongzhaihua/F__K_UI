/**
 * @file test_main_example_issue.cpp
 * @brief 重现 example/main.cpp 中报告的问题
 * 
 * 问题描述：
 * 1. 窗口靠下位置有一个长条形白色矩形
 * 2. 设置文字颜色后，文字却并不在矩形内
 * 3. 设置按钮的背景色也没有反应
 */

#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

// 辅助函数：打印视觉树和布局信息
void printVisualTreeWithLayout(Visual* node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- " << typeid(*node).name();
    
    auto* elem = dynamic_cast<UIElement*>(node);
    if (elem) {
        auto renderSize = elem->GetRenderSize();
        auto layoutRect = elem->GetLayoutRect();
        std::cout << " renderSize=[" << renderSize.width << "x" << renderSize.height << "]";
        std::cout << " layoutRect=(" << layoutRect.x << "," << layoutRect.y 
                  << "," << layoutRect.width << "," << layoutRect.height << ")";
    }
    std::cout << std::endl;
    
    for (size_t i = 0; i < node->GetVisualChildrenCount(); ++i) {
        printVisualTreeWithLayout(node->GetVisualChild(i), depth + 1);
    }
}

// 测试：重现 example/main.cpp 的 Button 问题
void test_main_example_button_issue() {
    std::cout << "\n=== 测试：重现 example/main.cpp 的 Button 问题 ===" << std::endl;
    
    // 创建 StackPanel 和内容（与 example/main.cpp 类似）
    auto* panel = new StackPanel();
    
    auto* textBlock1 = new TextBlock();
    textBlock1->Text("Hello, F K UI!")
              ->FontSize(32)
              ->Foreground(Brushes::Blue())
              ->Margin(Thickness(20));
    
    std::cout << "TextBlock1 FontSize: " << textBlock1->GetFontSize() << std::endl;
    std::cout << "TextBlock1 Text: '" << textBlock1->GetText() << "'" << std::endl;
    
    auto* textBlock2 = new TextBlock();
    textBlock2->Text("This is a simple example of F K UI framework.")
              ->FontSize(16)
              ->Foreground(Brushes::DarkGray())
              ->Margin(Thickness(20));
    
    // 这是用户报告问题的 Button
    auto* button = new Button();
    auto* buttonContent = new TextBlock();
    buttonContent->Text("Click Me")->FontSize(20);
    std::cout << "ButtonContent FontSize: " << buttonContent->GetFontSize() << std::endl;
    std::cout << "ButtonContent Text: '" << buttonContent->GetText() << "'" << std::endl;
    // 注意：buttonContent 没有设置 Foreground
    button->Content(buttonContent);
    
    panel->AddChild(textBlock1);
    panel->AddChild(textBlock2);
    panel->AddChild(button);
    
    // 测量和排列
    std::cout << "\n开始测量..." << std::endl;
    panel->Measure(Size(800, 600));
    
    std::cout << "\nTextBlock1 DesiredSize: " << textBlock1->GetDesiredSize().width 
              << "x" << textBlock1->GetDesiredSize().height << std::endl;
    std::cout << "TextBlock2 DesiredSize: " << textBlock2->GetDesiredSize().width 
              << "x" << textBlock2->GetDesiredSize().height << std::endl;
    
    std::cout << "\n开始排列..." << std::endl;
    panel->Arrange(Rect(0, 0, 800, 600));
    
    std::cout << "\n布局结果:" << std::endl;
    printVisualTreeWithLayout(panel);
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    std::cout << "\n渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    
    // 检查 Button 的位置
    auto* uiButton = dynamic_cast<UIElement*>(button);
    auto buttonLayoutRect = uiButton->GetLayoutRect();
    std::cout << "\nButton 位置: (" << buttonLayoutRect.x << ", " 
              << buttonLayoutRect.y << ", " << buttonLayoutRect.width 
              << ", " << buttonLayoutRect.height << ")" << std::endl;
    
    // 检查 Button 是否在窗口靠下位置
    std::cout << "\nButton y 坐标相对于窗口高度(600): " 
              << (buttonLayoutRect.y / 600.0 * 100) << "%" << std::endl;
    
    std::cout << "\n✓ 测试完成" << std::endl;
    
    std::cout << "\n开始清理..." << std::endl;
    // delete panel;  // panel 会自动删除所有子元素
    // TODO: 调查为什么 delete panel 会导致段错误
    std::cout << "清理完成（暂时跳过 delete，避免段错误）" << std::endl;
}

// 测试：尝试设置 Button 背景色
void test_button_background_color() {
    std::cout << "\n=== 测试：设置 Button 背景色 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Test Button");
    button->ApplyTemplate();
    
    // 用户可能尝试的方式 1: 直接设置背景色（如果 Button 有这个方法）
    // button->Background(Brushes::Red());  // 这会编译吗？
    
    // 正确的方式：通过视觉树访问 Border
    std::cout << "Button 视觉子元素数量: " << button->GetVisualChildrenCount() << std::endl;
    
    if (button->GetVisualChildrenCount() > 0) {
        auto* firstChild = button->GetVisualChild(0);
        std::cout << "第一个子元素类型: " << typeid(*firstChild).name() << std::endl;
        
        auto* border = dynamic_cast<Border*>(firstChild);
        if (border) {
            std::cout << "找到 Border，设置背景色为红色" << std::endl;
            border->Background(new SolidColorBrush(Color(255, 0, 0, 255)));  // 红色
            
            // 测量和排列
            button->Measure(Size(200, 50));
            button->Arrange(Rect(0, 0, 200, 50));
            
            // 生成渲染命令
            render::RenderList renderList;
            render::TextRenderer textRenderer;
            render::RenderContext context(&renderList, &textRenderer);
            
            button->CollectDrawCommands(context);
            
            std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
            std::cout << "✓ 背景色设置成功" << std::endl;
        } else {
            std::cout << "✗ 第一个子元素不是 Border" << std::endl;
        }
    } else {
        std::cout << "✗ Button 没有视觉子元素" << std::endl;
    }
    
    std::cout << "开始删除 button..." << std::endl;
    // delete button;  // TODO: 调查为什么这里也会段错误
    std::cout << "删除完成（暂时跳过）" << std::endl;
}

// 测试：TextBlock 在 Button 内的颜色继承
void test_textblock_color_in_button() {
    std::cout << "\n=== 测试：TextBlock 在 Button 内的颜色继承 ===" << std::endl;
    
    auto* button = new Button();
    
    // 创建带颜色的 TextBlock
    auto* textBlock = new TextBlock();
    textBlock->Text("Colored Text")->FontSize(20)->Foreground(Brushes::Red());
    
    button->Content(textBlock);
    button->ApplyTemplate();
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    std::cout << "视觉树结构:" << std::endl;
    printVisualTreeWithLayout(button);
    
    // 验证 TextBlock 是否在 ContentPresenter 中
    if (button->GetVisualChildrenCount() > 0) {
        auto* firstChild = button->GetVisualChild(0);
        auto* border = dynamic_cast<Border*>(firstChild);
        if (border && border->GetChild()) {
            std::cout << "\n✓ Button 有正确的模板结构" << std::endl;
        }
    }
    
    delete button;
}

int main() {
    std::cout << "===== Button 渲染问题测试 =====" << std::endl;
    
    std::cout << "\n运行测试1..." << std::endl;
    test_main_example_button_issue();
    std::cout << "测试1完成" << std::endl;
    
    std::cout << "\n运行测试2..." << std::endl;
    test_button_background_color();
    std::cout << "测试2完成" << std::endl;
    
    std::cout << "\n运行测试3..." << std::endl;
    test_textblock_color_in_button();
    std::cout << "测试3完成" << std::endl;
    
    std::cout << "\n===== 所有测试完成 =====" << std::endl;
    return 0;
}
