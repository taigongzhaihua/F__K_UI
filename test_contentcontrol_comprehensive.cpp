/**
 * @file test_contentcontrol_comprehensive.cpp
 * @brief 综合测试 ContentControl 功能
 * 
 * 验证：
 * 1. Button 的 Content 功能
 * 2. Window 的 Content 功能（确保没有被破坏）
 * 3. 各种 Content 类型的处理
 */

#include "fk/ui/Button.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

void test_window_with_textblock() {
    std::cout << "测试1: Window 接受 TextBlock Content..." << std::endl;
    
    auto* window = new Window();
    auto* textBlock = new TextBlock();
    textBlock->Text("Window Content");
    
    window->Content(textBlock);
    
    // 验证 Content 已设置
    assert(window->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ Window 有视觉子节点" << std::endl;
    
    auto* child = window->GetVisualChild(0);
    assert(child == textBlock);
    std::cout << "  ✓ 子节点是传入的 TextBlock" << std::endl;
    
    delete window;
    std::cout << "测试1: 通过 ✓\n" << std::endl;
}

void test_window_with_panel() {
    std::cout << "测试2: Window 接受 Panel Content..." << std::endl;
    
    auto* window = new Window();
    auto* panel = new StackPanel();
    
    auto* text1 = new TextBlock();
    text1->Text("Text 1");
    auto* text2 = new TextBlock();
    text2->Text("Text 2");
    
    panel->AddChild(text1);
    panel->AddChild(text2);
    
    window->Content(panel);
    
    // 验证 Window 有 Panel 作为子节点
    assert(window->GetVisualChildrenCount() == 1);
    auto* child = window->GetVisualChild(0);
    assert(child == panel);
    std::cout << "  ✓ Window 的子节点是 Panel" << std::endl;
    
    // 验证 Panel 有两个子元素
    assert(panel->GetChildrenCount() == 2);
    std::cout << "  ✓ Panel 有两个子元素" << std::endl;
    
    delete window;
    std::cout << "测试2: 通过 ✓\n" << std::endl;
}

void test_button_string_types() {
    std::cout << "测试3: Button 接受不同类型的字符串..." << std::endl;
    
    // C 字符串
    auto* button1 = new Button();
    button1->Content("C String");
    assert(button1->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ C 字符串 Content" << std::endl;
    
    // std::string
    auto* button2 = new Button();
    std::string str = "std::string";
    button2->Content(std::any(str));
    assert(button2->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ std::string Content" << std::endl;
    
    delete button1;
    delete button2;
    std::cout << "测试3: 通过 ✓\n" << std::endl;
}

void test_nested_contentcontrols() {
    std::cout << "测试4: 嵌套的 ContentControl（Window 包含 Button）..." << std::endl;
    
    auto* window = new Window();
    auto* panel = new StackPanel();
    
    auto* button1 = new Button();
    button1->Content("Button 1");
    
    auto* button2 = new Button();
    button2->Content("Button 2");
    
    panel->AddChild(button1);
    panel->AddChild(button2);
    
    window->Content(panel);
    
    // 验证结构
    assert(window->GetVisualChildrenCount() == 1);
    assert(panel->GetChildrenCount() == 2);
    assert(button1->GetVisualChildrenCount() == 1);
    assert(button2->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ 嵌套结构正确" << std::endl;
    
    // 测量和排列整个树
    window->Width(400);
    window->Height(300);
    window->Measure(Size(400, 300));
    window->Arrange(Rect(0, 0, 400, 300));
    
    std::cout << "  ✓ 测量和排列成功" << std::endl;
    
    delete window;
    std::cout << "测试4: 通过 ✓\n" << std::endl;
}

void test_content_change_updates_visual_tree() {
    std::cout << "测试5: Content 更改更新视觉树..." << std::endl;
    
    auto* button = new Button();
    
    // 第一个 Content
    button->Content("First Content");
    assert(button->GetVisualChildrenCount() == 1);
    auto* child1 = button->GetVisualChild(0);
    std::cout << "  ✓ 第一个 Content 已添加" << std::endl;
    
    // 第二个 Content（字符串）
    button->Content("Second Content");
    assert(button->GetVisualChildrenCount() == 1);
    auto* child2 = button->GetVisualChild(0);
    assert(child1 != child2);
    std::cout << "  ✓ 第二个 Content 已替换" << std::endl;
    
    // 第三个 Content（UIElement）
    auto* customText = new TextBlock();
    customText->Text("Custom TextBlock");
    button->Content(customText);
    assert(button->GetVisualChildrenCount() == 1);
    auto* child3 = button->GetVisualChild(0);
    assert(child3 == customText);
    std::cout << "  ✓ UIElement Content 已替换" << std::endl;
    
    delete button;
    std::cout << "测试5: 通过 ✓\n" << std::endl;
}

void test_empty_content() {
    std::cout << "测试6: 空 Content 处理..." << std::endl;
    
    auto* button = new Button();
    button->Content("Some Content");
    assert(button->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ Button 有 Content" << std::endl;
    
    // 设置空 Content
    button->SetContent(std::any());
    assert(button->GetVisualChildrenCount() == 0);
    std::cout << "  ✓ 空 Content 清除了视觉子节点" << std::endl;
    
    delete button;
    std::cout << "测试6: 通过 ✓\n" << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  ContentControl 综合测试" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_window_with_textblock();
        test_window_with_panel();
        test_button_string_types();
        test_nested_contentcontrols();
        test_content_change_updates_visual_tree();
        test_empty_content();
        
        std::cout << "==================================" << std::endl;
        std::cout << "  所有测试通过！ ✓" << std::endl;
        std::cout << "==================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "测试失败: 未知错误" << std::endl;
        return 1;
    }
}
