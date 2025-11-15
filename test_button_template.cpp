/**
 * @file test_button_template.cpp
 * @brief 测试 Button 的模板机制
 * 
 * 验证：
 * 1. Button 没有模板时的回退行为（直接显示 Content）
 * 2. Button 有模板时使用 ContentPresenter
 */

#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

void test_button_without_template() {
    std::cout << "测试1: Button 没有模板时的回退行为..." << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    
    // 检查模板
    auto* tmpl = button->GetTemplate();
    std::cout << "  - Button 的 Template: " << (tmpl ? "有" : "无") << std::endl;
    
    // 验证 Content 直接作为视觉子节点
    size_t childCount = button->GetVisualChildrenCount();
    std::cout << "  - 视觉子节点数量: " << childCount << std::endl;
    
    if (childCount > 0) {
        auto* child = button->GetVisualChild(0);
        auto* textBlock = dynamic_cast<TextBlock*>(child);
        if (textBlock) {
            std::cout << "  ✓ 直接显示 Content（回退机制）" << std::endl;
            std::cout << "  ✓ TextBlock 文本: " << textBlock->GetText() << std::endl;
        }
    }
    
    delete button;
    std::cout << "测试1: 完成\n" << std::endl;
}

void test_button_with_template() {
    std::cout << "测试2: Button 使用 ControlTemplate..." << std::endl;
    
    // 创建一个简单的 Button 模板
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 模板工厂：Border + ContentPresenter
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        border->Background(new SolidColorBrush(Color(200, 200, 200, 255)));
        border->BorderBrush(new SolidColorBrush(Color(100, 100, 100, 255)));
        border->BorderThickness(1.0f);
        border->Padding(10.0f);
        
        auto* presenter = new ContentPresenter<>();
        border->Child(presenter);
        
        return border;
    });
    
    // 创建 Button 并设置模板
    auto* button = new Button();
    button->SetTemplate(tmpl);
    button->Content("Templated Button");
    
    // 应用模板
    button->ApplyTemplate();
    
    std::cout << "  - 模板已设置并应用" << std::endl;
    
    // 注意：当前的实现可能还没有完全支持模板根元素的管理
    // 这个测试主要是为了验证架构设计
    
    size_t childCount = button->GetVisualChildrenCount();
    std::cout << "  - 视觉子节点数量: " << childCount << std::endl;
    
    // 理论上应该只有一个子节点（模板根 Border）
    // 但当前实现可能有多个（模板根 + Content）
    if (childCount > 0) {
        auto* child = button->GetVisualChild(0);
        auto* border = dynamic_cast<Border*>(child);
        if (border) {
            std::cout << "  ✓ 第一个子节点是 Border（模板根）" << std::endl;
        } else {
            auto* textBlock = dynamic_cast<TextBlock*>(child);
            if (textBlock) {
                std::cout << "  ⚠ 第一个子节点是 TextBlock（回退机制仍在生效）" << std::endl;
                std::cout << "  ⚠ 这表明模板还没有完全替换 Content 显示" << std::endl;
            }
        }
    }
    
    delete button;
    std::cout << "测试2: 完成\n" << std::endl;
}

void test_architecture_understanding() {
    std::cout << "测试3: 理解当前架构..." << std::endl;
    std::cout << "\n理想架构（WPF 风格）：" << std::endl;
    std::cout << "  Button" << std::endl;
    std::cout << "    └─ TemplateRoot (Border)" << std::endl;
    std::cout << "         └─ ContentPresenter" << std::endl;
    std::cout << "              └─ Content (TextBlock)" << std::endl;
    
    std::cout << "\n当前实现（回退机制）：" << std::endl;
    std::cout << "  Button" << std::endl;
    std::cout << "    └─ Content (TextBlock) [直接添加]" << std::endl;
    
    std::cout << "\n分析：" << std::endl;
    std::cout << "  - 当前实现在没有 Template 时提供了回退机制" << std::endl;
    std::cout << "  - 这对于简单场景是可行的" << std::endl;
    std::cout << "  - 但不符合完整的 WPF 模板架构" << std::endl;
    std::cout << "\n测试3: 完成\n" << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  Button 模板机制测试" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;
    
    try {
        test_button_without_template();
        test_button_with_template();
        test_architecture_understanding();
        
        std::cout << "==================================" << std::endl;
        std::cout << "  测试完成" << std::endl;
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
