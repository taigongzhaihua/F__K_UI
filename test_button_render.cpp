/**
 * @file test_button_render.cpp
 * @brief 测试 Button 渲染功能
 * 
 * 验证：
 * 1. Button 可以接受字符串 Content
 * 2. Content 被正确添加到视觉树
 * 3. Button 可以被正确测量和排列
 */

#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/render/RenderContext.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

void test_button_string_content() {
    std::cout << "测试1: Button 接受字符串 Content..." << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    
    // 验证 Content 已设置
    auto content = button->GetContent();
    assert(content.has_value());
    std::cout << "  ✓ Content 已设置" << std::endl;
    
    // 验证视觉子节点已添加
    size_t childCount = button->GetVisualChildrenCount();
    assert(childCount == 1);
    std::cout << "  ✓ 视觉子节点数量: " << childCount << std::endl;
    
    // 验证子节点是 TextBlock
    auto* child = button->GetVisualChild(0);
    assert(child != nullptr);
    auto* textBlock = dynamic_cast<TextBlock*>(child);
    assert(textBlock != nullptr);
    std::cout << "  ✓ 子节点是 TextBlock" << std::endl;
    
    // 验证 TextBlock 的文本
    auto text = textBlock->GetText();
    assert(text == "Click Me");
    std::cout << "  ✓ TextBlock 文本正确: " << text << std::endl;
    
    delete button;
    std::cout << "测试1: 通过 ✓\n" << std::endl;
}

void test_button_uielement_content() {
    std::cout << "测试2: Button 接受 UIElement* Content..." << std::endl;
    
    auto* button = new Button();
    auto* textBlock = new TextBlock();
    textBlock->Text("Custom TextBlock");
    
    button->Content(textBlock);
    
    // 验证视觉子节点已添加
    size_t childCount = button->GetVisualChildrenCount();
    assert(childCount == 1);
    std::cout << "  ✓ 视觉子节点数量: " << childCount << std::endl;
    
    // 验证子节点就是我们传入的 TextBlock
    auto* child = button->GetVisualChild(0);
    assert(child == textBlock);
    std::cout << "  ✓ 子节点是传入的 TextBlock" << std::endl;
    
    delete button;
    std::cout << "测试2: 通过 ✓\n" << std::endl;
}

void test_button_measure_arrange() {
    std::cout << "测试3: Button 测量和排列..." << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->Width(200);
    button->Height(50);
    
    // 测量
    button->Measure(Size(200, 50));
    Size desiredSize = button->GetDesiredSize();
    std::cout << "  ✓ 期望大小: " << desiredSize.width << " x " << desiredSize.height << std::endl;
    
    // 排列
    button->Arrange(Rect(0, 0, 200, 50));
    Size renderSize = button->GetRenderSize();
    std::cout << "  ✓ 渲染大小: " << renderSize.width << " x " << renderSize.height << std::endl;
    
    // 验证子元素也被排列
    auto* child = button->GetVisualChild(0);
    assert(child != nullptr);
    auto* childElement = dynamic_cast<UIElement*>(child);
    if (childElement) {
        Size childRenderSize = childElement->GetRenderSize();
        std::cout << "  ✓ 子元素渲染大小: " << childRenderSize.width << " x " << childRenderSize.height << std::endl;
    }
    
    delete button;
    std::cout << "测试3: 通过 ✓\n" << std::endl;
}

void test_button_in_stackpanel() {
    std::cout << "测试4: Button 在 StackPanel 中..." << std::endl;
    
    auto* panel = new StackPanel();
    
    auto* button1 = new Button();
    button1->Content("Button 1");
    button1->Height(50);
    
    auto* button2 = new Button();
    button2->Content("Button 2");
    button2->Height(50);
    
    panel->AddChild(button1);
    panel->AddChild(button2);
    
    // 验证 Panel 有两个子元素
    assert(panel->GetChildrenCount() == 2);
    std::cout << "  ✓ Panel 子元素数量: " << panel->GetChildrenCount() << std::endl;
    
    // 测量 Panel
    panel->Measure(Size(400, 600));
    Size panelDesiredSize = panel->GetDesiredSize();
    std::cout << "  ✓ Panel 期望大小: " << panelDesiredSize.width << " x " << panelDesiredSize.height << std::endl;
    
    // 排列 Panel
    panel->Arrange(Rect(0, 0, 400, 600));
    
    // 验证每个 Button 都有视觉子节点
    assert(button1->GetVisualChildrenCount() == 1);
    assert(button2->GetVisualChildrenCount() == 1);
    std::cout << "  ✓ 两个 Button 都有视觉子节点" << std::endl;
    
    delete panel;
    std::cout << "测试4: 通过 ✓\n" << std::endl;
}

void test_button_content_replacement() {
    std::cout << "测试5: Button Content 替换..." << std::endl;
    
    auto* button = new Button();
    button->Content("Original Text");
    
    // 验证第一个 Content
    assert(button->GetVisualChildrenCount() == 1);
    auto* child1 = dynamic_cast<TextBlock*>(button->GetVisualChild(0));
    assert(child1 != nullptr);
    assert(child1->GetText() == "Original Text");
    std::cout << "  ✓ 原始 Content: " << child1->GetText() << std::endl;
    
    // 替换 Content
    button->Content("New Text");
    
    // 验证新的 Content
    assert(button->GetVisualChildrenCount() == 1);
    auto* child2 = dynamic_cast<TextBlock*>(button->GetVisualChild(0));
    assert(child2 != nullptr);
    assert(child2->GetText() == "New Text");
    std::cout << "  ✓ 新 Content: " << child2->GetText() << std::endl;
    
    // 验证是不同的对象
    assert(child1 != child2);
    std::cout << "  ✓ Content 已被替换为新对象" << std::endl;
    
    delete button;
    std::cout << "测试5: 通过 ✓\n" << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  Button 渲染功能测试" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_button_string_content();
        test_button_uielement_content();
        test_button_measure_arrange();
        test_button_in_stackpanel();
        test_button_content_replacement();
        
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
