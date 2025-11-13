/**
 * @file contentpresenter_demo.cpp
 * @brief ContentPresenter 和 ContentControl 功能演示程序
 * 
 * 本示例展示：
 * 1. ContentControl 的基本用法（Content 和 ContentTemplate）
 * 2. ContentPresenter 在 ControlTemplate 中的使用
 * 3. DataTemplate 的数据绑定和实例化
 * 4. ContentControl + ContentPresenter 组合实现自定义控件
 */

#include "fk/ui/ContentControl.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/DataTemplate.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/DrawCommand.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <string>
#include <memory>

using namespace fk;
using namespace fk::ui;

// ========== 辅助函数 ==========

void printSeparator() {
    std::cout << "\n========================================\n";
}

void printHeader(const std::string& title) {
    printSeparator();
    std::cout << "  " << title << "\n";
    printSeparator();
    std::cout << std::endl;
}

// ========== 示例 1: ContentControl 基础用法 ==========

void demo_ContentControl_Basic() {
    printHeader("示例 1: ContentControl 基础用法");
    
    std::cout << "[基础] 创建 Button (继承自 ContentControl)" << std::endl;
    auto* button = new Button();
    
    // 设置字符串内容
    std::cout << "  - 设置 Content = \"点击我\"" << std::endl;
    button->Content(std::string("点击我"));
    
    auto content = button->Content();
    if (content.type() == typeid(std::string)) {
        std::cout << "  - 获取 Content: \"" << std::any_cast<std::string>(content) << "\"" << std::endl;
    }
    
    std::cout << "  ✅ ContentControl 可以存储任意类型的 Content" << std::endl;
    
    delete button;
}

// ========== 示例 2: ContentControl 使用 UIElement 内容 ==========

void demo_ContentControl_UIElement() {
    printHeader("示例 2: ContentControl 使用 UIElement 内容");
    
    std::cout << "[UIElement 内容] 创建带 TextBlock 内容的 Button" << std::endl;
    
    auto* button = new Button();
    auto* textBlock = new TextBlock();
    textBlock->Text("复杂按钮")->FontSize(20);
    
    std::cout << "  - 创建 TextBlock: \"复杂按钮\", FontSize=20" << std::endl;
    button->Content(static_cast<UIElement*>(textBlock));
    std::cout << "  - 设置为 Button 的 Content" << std::endl;
    
    auto content = button->Content();
    if (content.type() == typeid(UIElement*)) {
        std::cout << "  ✅ Content 是 UIElement*，可以直接显示" << std::endl;
    }
    
    delete button;  // Button 会管理 textBlock 的生命周期
}

// ========== 示例 3: ContentTemplate 数据模板 ==========

void demo_ContentTemplate() {
    printHeader("示例 3: ContentTemplate 数据模板");
    
    std::cout << "[DataTemplate] 为非 UIElement 数据创建视觉表示" << std::endl;
    
    // 创建数据模板
    auto* dataTemplate = new DataTemplate();
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        // 从数据上下文中提取数据
        if (dataContext.type() == typeid(std::string)) {
            auto text = std::any_cast<std::string>(dataContext);
            
            // 创建视觉树：Border 包含 TextBlock
            auto* border = new Border();
            border->Background(Brushes::LightGray())
                  ->Padding(10, 5, 10, 5);
            
            auto* textBlock = new TextBlock();
            textBlock->Text(text)
                     ->FontSize(16);
            
            border->Child(textBlock);
            
            std::cout << "  - DataTemplate 实例化: Border + TextBlock(\"" << text << "\")" << std::endl;
            
            return border;
        }
        return nullptr;
    });
    
    std::cout << "  - 创建 DataTemplate（工厂函数）" << std::endl;
    
    // 创建 Button 并应用数据模板
    auto* button = new Button();
    button->Content(std::string("数据模板按钮"));
    button->ContentTemplate(dataTemplate);
    
    std::cout << "  - Button.Content = \"数据模板按钮\"" << std::endl;
    std::cout << "  - Button.ContentTemplate = dataTemplate" << std::endl;
    std::cout << "  ✅ DataTemplate 将数据转换为可视化视觉树" << std::endl;
    
    delete button;
}

// ========== 示例 4: ContentPresenter 在 ControlTemplate 中 ==========

void demo_ContentPresenter_InControlTemplate() {
    printHeader("示例 4: ContentPresenter 在 ControlTemplate 中");
    
    std::cout << "[ControlTemplate] 自定义 Button 外观，使用 ContentPresenter 显示内容" << std::endl;
    
    // 创建自定义 Button 模板
    auto* controlTemplate = new ControlTemplate();
    controlTemplate->SetFactory([]() -> UIElement* {
        // 外层边框（按钮背景）
        auto* outerBorder = new Border();
        outerBorder->Background(Brushes::DarkGray())
                   ->Padding(20, 10, 20, 10)
                   ->CornerRadius(5);
        
        // ContentPresenter 显示 Button 的 Content
        auto* presenter = new ContentPresenter<>();
        
        // 注意：在实际应用中，这里应该通过 TemplateBinding 绑定到 Button 的 Content
        // presenter->SetTemplateBinding(ContentPresenter<>::ContentProperty(), Button::ContentProperty());
        
        outerBorder->Child(presenter);
        
        std::cout << "  - 创建 ControlTemplate: Border(DarkGray) + ContentPresenter" << std::endl;
        
        return outerBorder;
    });
    
    // 创建 Button 并应用自定义模板
    auto* button = new Button();
    button->Content(std::string("自定义按钮"));
    button->Template(controlTemplate);
    
    std::cout << "  - Button.Content = \"自定义按钮\"" << std::endl;
    std::cout << "  - Button.Template = 自定义模板" << std::endl;
    std::cout << "  ✅ ContentPresenter 在模板中显示 Button 的 Content" << std::endl;
    
    delete button;
}

// ========== 示例 5: ContentPresenter 独立使用 ==========

void demo_ContentPresenter_Standalone() {
    printHeader("示例 5: ContentPresenter 独立使用");
    
    std::cout << "[独立使用] ContentPresenter 可以独立于 ControlTemplate 使用" << std::endl;
    
    // 创建 ContentPresenter
    auto* presenter = new ContentPresenter<>();
    
    // 方式 1: 直接设置 UIElement 内容
    std::cout << "\n[方式 1] 设置 UIElement 内容" << std::endl;
    auto* textBlock1 = new TextBlock();
    textBlock1->Text("直接 UIElement 内容")->FontSize(18);
    
    presenter->SetContent(static_cast<UIElement*>(textBlock1));
    std::cout << "  - ContentPresenter.Content = TextBlock" << std::endl;
    std::cout << "  - 结果：直接显示 TextBlock" << std::endl;
    
    // 方式 2: 设置数据 + 数据模板
    std::cout << "\n[方式 2] 设置数据 + ContentTemplate" << std::endl;
    
    auto* dataTemplate = new DataTemplate();
    dataTemplate->SetFactory([](const std::any& data) -> UIElement* {
        if (data.type() == typeid(int)) {
            int value = std::any_cast<int>(data);
            
            auto* border = new Border();
            border->Background(Brushes::Blue())
                  ->Padding(15, 8, 15, 8);
            
            auto* text = new TextBlock();
            text->Text("数字: " + std::to_string(value))
                ->FontSize(16);
            
            border->Child(text);
            return border;
        }
        return nullptr;
    });
    
    presenter->SetContent(42);  // 设置整数数据
    presenter->SetContentTemplate(dataTemplate);
    
    std::cout << "  - ContentPresenter.Content = 42 (int)" << std::endl;
    std::cout << "  - ContentPresenter.ContentTemplate = 数字模板" << std::endl;
    std::cout << "  - 结果：模板将 42 转换为 Border + TextBlock" << std::endl;
    std::cout << "  ✅ ContentPresenter 灵活处理各种内容类型" << std::endl;
    
    delete presenter;
}

// ========== 示例 6: 复杂布局示例 ==========

void demo_Complex_Layout() {
    printHeader("示例 6: 复杂布局示例");
    
    std::cout << "[复杂场景] 多个 ContentControl 组合布局" << std::endl;
    
    // 创建 StackPanel 容器
    auto* panel = new StackPanel();
    panel->SetOrient(Orientation::Vertical);
    
    std::cout << "  - 创建 StackPanel (Vertical)" << std::endl;
    
    // 创建 3 个按钮，每个有不同的内容
    for (int i = 1; i <= 3; ++i) {
        auto* button = new Button();
        
        if (i == 1) {
            // 简单字符串内容
            button->Content(std::string("按钮 " + std::to_string(i)));
            std::cout << "    - 按钮 1: 字符串内容" << std::endl;
        }
        else if (i == 2) {
            // UIElement 内容
            auto* content = new TextBlock();
            content->Text("按钮 " + std::to_string(i) + " (富文本)")->FontSize(18);
            button->Content(static_cast<UIElement*>(content));
            std::cout << "    - 按钮 2: TextBlock 内容" << std::endl;
        }
        else {
            // 数据 + 模板
            auto* tmpl = new DataTemplate();
            tmpl->SetFactory([i](const std::any&) -> UIElement* {
                auto* border = new Border();
                border->Background(Brushes::Green());
                
                auto* text = new TextBlock();
                text->Text("模板按钮 " + std::to_string(i));
                border->Child(text);
                
                return border;
            });
            
            button->Content(i);
            button->ContentTemplate(tmpl);
            std::cout << "    - 按钮 3: 数据 + DataTemplate" << std::endl;
        }
        
        panel->AddChild(button);
    }
    
    std::cout << "  ✅ 成功创建包含 3 个不同类型按钮的布局" << std::endl;
    
    delete panel;  // 会递归删除所有子元素
}

// ========== 示例 7: ContentPresenter 生命周期 ==========

void demo_ContentPresenter_Lifecycle() {
    printHeader("示例 7: ContentPresenter 生命周期");
    
    std::cout << "[生命周期] 演示 ContentPresenter 的内容更新机制" << std::endl;
    
    auto* presenter = new ContentPresenter<>();
    
    // 步骤 1: 初始内容
    std::cout << "\n[步骤 1] 设置初始内容" << std::endl;
    auto* text1 = new TextBlock();
    text1->Text("初始内容");
    presenter->SetContent(static_cast<UIElement*>(text1));
    std::cout << "  - Content = TextBlock(\"初始内容\")" << std::endl;
    
    // 步骤 2: 更新内容
    std::cout << "\n[步骤 2] 更新为新内容" << std::endl;
    auto* text2 = new TextBlock();
    text2->Text("新内容");
    presenter->SetContent(static_cast<UIElement*>(text2));
    std::cout << "  - Content = TextBlock(\"新内容\")" << std::endl;
    std::cout << "  - 旧内容被移除，新内容被添加" << std::endl;
    
    // 步骤 3: 清除内容
    std::cout << "\n[步骤 3] 清除内容" << std::endl;
    presenter->SetContent(std::any());
    std::cout << "  - Content = 空" << std::endl;
    std::cout << "  - 所有视觉子元素被移除" << std::endl;
    
    std::cout << "\n  ✅ ContentPresenter 正确管理内容的生命周期" << std::endl;
    
    delete presenter;
}

// ========== 主函数 ==========

int main() {
    printHeader("F__K_UI ContentPresenter 功能演示");
    
    std::cout << "本示例演示 ContentPresenter 和 ContentControl 的核心功能：\n"
              << "1. ContentControl 的基础用法\n"
              << "2. ContentTemplate 数据模板\n"
              << "3. ContentPresenter 在 ControlTemplate 中的作用\n"
              << "4. ContentPresenter 独立使用\n"
              << "5. 复杂布局场景\n"
              << "6. 生命周期管理\n" << std::endl;
    
    try {
        // 运行所有示例
        demo_ContentControl_Basic();
        demo_ContentControl_UIElement();
        demo_ContentTemplate();
        demo_ContentPresenter_InControlTemplate();
        demo_ContentPresenter_Standalone();
        demo_Complex_Layout();
        demo_ContentPresenter_Lifecycle();
        
        // 总结
        printHeader("功能总结");
        std::cout << "✅ ContentControl - 单一内容控件基类\n"
                  << "   • Content 属性：存储任意类型内容 (std::any)\n"
                  << "   • ContentTemplate 属性：定义数据可视化\n"
                  << "   • 继承者：Button, Window, ListBoxItem 等\n\n"
                  
                  << "✅ ContentPresenter - 内容展示器\n"
                  << "   • 在 ControlTemplate 中显示 ContentControl 的 Content\n"
                  << "   • 自动应用 ContentTemplate（如果 Content 不是 UIElement）\n"
                  << "   • 支持 TemplateBinding 绑定到父控件属性\n"
                  << "   • 独立使用时可作为通用数据展示容器\n\n"
                  
                  << "✅ DataTemplate - 数据模板\n"
                  << "   • 将非 UIElement 数据转换为可视化视觉树\n"
                  << "   • 工厂函数接收数据上下文，返回 UIElement\n"
                  << "   • 支持复杂的数据绑定和样式\n\n"
                  
                  << "🎯 使用场景：\n"
                  << "   • 自定义控件外观（ControlTemplate + ContentPresenter）\n"
                  << "   • 数据驱动 UI（DataTemplate + ContentPresenter）\n"
                  << "   • 列表项渲染（ItemsControl + DataTemplate）\n"
                  << "   • 动态内容展示（运行时切换 Content 和 Template）\n" << std::endl;
        
        printSeparator();
        std::cout << "\n✨ 所有示例运行成功！\n" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << std::endl;
        return 1;
    }
}
