/**
 * @file test_button_comprehensive_render.cpp
 * @brief Button 渲染综合测试
 * 
 * 验证 Button 在各种场景下的渲染功能：
 * 1. 不同类型的 Content（字符串、UIElement、数据+模板）
 * 2. 自定义模板
 * 3. 在不同容器中
 * 4. 渲染命令生成
 * 5. 视觉树结构
 */

#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/DataTemplate.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

// 辅助函数：打印视觉树
void printVisualTree(Visual* node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- " << typeid(*node).name();
    
    auto* elem = dynamic_cast<UIElement*>(node);
    if (elem) {
        auto renderSize = elem->GetRenderSize();
        std::cout << " [" << renderSize.width << "x" << renderSize.height << "]";
    }
    std::cout << std::endl;
    
    for (size_t i = 0; i < node->GetVisualChildrenCount(); ++i) {
        printVisualTree(node->GetVisualChild(i), depth + 1);
    }
}

// 测试 1: Button 带字符串 Content
void test_button_string_content() {
    std::cout << "\n=== 测试 1: Button 带字符串 Content ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Click Me");
    button->ApplyTemplate();
    
    // 验证视觉树结构
    std::cout << "视觉树结构:" << std::endl;
    printVisualTree(button);
    
    // 测量和排列
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    assert(renderList.GetCommandCount() > 0);
    std::cout << "✓ Button 带字符串 Content 渲染正常\n" << std::endl;
    
    delete button;
}

// 测试 2: Button 带 UIElement Content
void test_button_uielement_content() {
    std::cout << "\n=== 测试 2: Button 带 UIElement Content ===" << std::endl;
    
    auto* button = new Button();
    auto* textBlock = new TextBlock();
    textBlock->Text("Custom TextBlock")->FontSize(16);
    
    button->Content(textBlock);
    button->ApplyTemplate();
    
    std::cout << "视觉树结构:" << std::endl;
    printVisualTree(button);
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    assert(renderList.GetCommandCount() > 0);
    std::cout << "✓ Button 带 UIElement Content 渲染正常\n" << std::endl;
    
    delete button;
}

// 测试 3: Button 带自定义模板
void test_button_custom_template() {
    std::cout << "\n=== 测试 3: Button 带自定义模板 ===" << std::endl;
    
    // 创建自定义模板
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        border->Background(new SolidColorBrush(Color(100, 100, 200, 255)));
        border->BorderBrush(new SolidColorBrush(Color(50, 50, 150, 255)));
        border->BorderThickness(2.0f);
        border->Padding(15.0f, 8.0f, 15.0f, 8.0f);
        border->CornerRadius(5.0f);
        
        auto* presenter = new ContentPresenter<>();
        presenter->SetHorizontalAlignment(HorizontalAlignment::Center);
        presenter->SetVerticalAlignment(VerticalAlignment::Center);
        
        border->Child(presenter);
        return border;
    });
    
    auto* button = new Button();
    button->SetTemplate(tmpl);
    button->Content("Custom Styled Button");
    button->ApplyTemplate();
    
    std::cout << "视觉树结构:" << std::endl;
    printVisualTree(button);
    
    button->Measure(Size(250, 60));
    button->Arrange(Rect(0, 0, 250, 60));
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    assert(renderList.GetCommandCount() > 0);
    std::cout << "✓ Button 带自定义模板渲染正常\n" << std::endl;
    
    delete button;
}

// 测试 4: Button 在 StackPanel 中
void test_button_in_stackpanel() {
    std::cout << "\n=== 测试 4: 多个 Button 在 StackPanel 中 ===" << std::endl;
    
    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical);
    
    // 创建 3 个不同的 Button
    auto* button1 = new Button();
    button1->Content("Button 1")->Height(40);
    
    auto* button2 = new Button();
    auto* text = new TextBlock();
    text->Text("Button 2 (Rich)")->FontSize(14);
    button2->Content(text)->Height(40);
    
    auto* button3 = new Button();
    button3->Content("Button 3")->Height(40);
    
    panel->AddChild(button1);
    panel->AddChild(button2);
    panel->AddChild(button3);
    
    std::cout << "StackPanel 子元素数量: " << panel->GetChildrenCount() << std::endl;
    
    std::cout << "视觉树结构:" << std::endl;
    printVisualTree(panel);
    
    // 布局
    panel->Measure(Size(400, 600));
    panel->Arrange(Rect(0, 0, 400, 600));
    
    // 渲染
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    panel->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    assert(renderList.GetCommandCount() > 0);
    
    // 验证每个 Button 都有正确的布局
    auto rect1 = button1->GetLayoutRect();
    auto rect2 = button2->GetLayoutRect();
    auto rect3 = button3->GetLayoutRect();
    
    std::cout << "Button1 layoutRect: (" << rect1.x << ", " << rect1.y 
              << ", " << rect1.width << ", " << rect1.height << ")" << std::endl;
    std::cout << "Button2 layoutRect: (" << rect2.x << ", " << rect2.y 
              << ", " << rect2.width << ", " << rect2.height << ")" << std::endl;
    std::cout << "Button3 layoutRect: (" << rect3.x << ", " << rect3.y 
              << ", " << rect3.width << ", " << rect3.height << ")" << std::endl;
    
    // 验证 Button 不重叠
    assert(rect1.y + rect1.height <= rect2.y);
    assert(rect2.y + rect2.height <= rect3.y);
    
    std::cout << "✓ Button 在 StackPanel 中布局和渲染正常\n" << std::endl;
    
    delete panel;
}

// 测试 5: Button Content 动态更新
void test_button_content_update() {
    std::cout << "\n=== 测试 5: Button Content 动态更新 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Initial Content");
    button->ApplyTemplate();
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 第一次渲染
    render::RenderList renderList1;
    render::TextRenderer textRenderer;
    render::RenderContext context1(&renderList1, &textRenderer);
    button->CollectDrawCommands(context1);
    
    size_t count1 = renderList1.GetCommandCount();
    std::cout << "初始内容渲染命令数量: " << count1 << std::endl;
    
    // 更新 Content
    button->Content("Updated Content");
    button->InvalidateMeasure();
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 第二次渲染
    render::RenderList renderList2;
    render::RenderContext context2(&renderList2, &textRenderer);
    button->CollectDrawCommands(context2);
    
    size_t count2 = renderList2.GetCommandCount();
    std::cout << "更新后渲染命令数量: " << count2 << std::endl;
    
    // 验证视觉树没有重复创建
    assert(button->GetVisualChildrenCount() == 1);
    std::cout << "视觉子节点数量: " << button->GetVisualChildrenCount() << std::endl;
    
    std::cout << "✓ Button Content 动态更新正常\n" << std::endl;
    
    delete button;
}

// 测试 6: Button 延迟模板应用
void test_button_lazy_template_apply() {
    std::cout << "\n=== 测试 6: Button 延迟模板应用 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Lazy Template");
    
    // 不显式调用 ApplyTemplate
    std::cout << "创建 Button 后，未调用 ApplyTemplate" << std::endl;
    std::cout << "视觉子节点数量（应用模板前）: " << button->GetVisualChildrenCount() << std::endl;
    
    // 测量和排列（可能触发模板应用）
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    std::cout << "视觉子节点数量（Measure/Arrange 后）: " << button->GetVisualChildrenCount() << std::endl;
    
    // 渲染（应该触发模板应用）
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "视觉子节点数量（渲染后）: " << button->GetVisualChildrenCount() << std::endl;
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    
    assert(button->GetVisualChildrenCount() == 1);
    assert(renderList.GetCommandCount() > 0);
    
    std::cout << "✓ Button 延迟模板应用正常\n" << std::endl;
    
    delete button;
}

// 测试 7: Button 带 ContentTemplate
void test_button_with_contenttemplate() {
    std::cout << "\n=== 测试 7: Button 带 ContentTemplate ===" << std::endl;
    
    // 创建 DataTemplate
    auto* dataTemplate = new DataTemplate();
    dataTemplate->SetFactory([](const std::any& data) -> UIElement* {
        if (data.type() == typeid(std::string)) {
            std::string text = std::any_cast<std::string>(data);
            
            auto* border = new Border();
            border->Background(Brushes::Yellow());
            border->Padding(5, 3, 5, 3);
            
            auto* textBlock = new TextBlock();
            textBlock->Text("★ " + text + " ★");
            
            border->Child(textBlock);
            return border;
        }
        return nullptr;
    });
    
    auto* button = new Button();
    button->Content(std::string("Fancy Button"));
    button->SetContentTemplate(dataTemplate);
    button->ApplyTemplate();
    
    std::cout << "视觉树结构:" << std::endl;
    printVisualTree(button);
    
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    assert(renderList.GetCommandCount() > 0);
    
    std::cout << "✓ Button 带 ContentTemplate 渲染正常\n" << std::endl;
    
    delete button;
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "  Button 渲染综合测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        test_button_string_content();
        test_button_uielement_content();
        test_button_custom_template();
        test_button_in_stackpanel();
        test_button_content_update();
        test_button_lazy_template_apply();
        test_button_with_contenttemplate();
        
        std::cout << "\n==========================================" << std::endl;
        std::cout << "  所有测试通过！ ✓✓✓" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 测试失败: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\n❌ 测试失败: 未知错误" << std::endl;
        return 1;
    }
}
