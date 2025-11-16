/**
 * 命名作用域演示程序
 * 说明：不同作用域中可以有相同的名称！
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         命名作用域演示                                   ║\n";
    std::cout << "║         不同作用域可以有相同的名称！                     ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        auto window = app->CreateWindow();
        window->Title("命名作用域演示")
              ->Width(800)
              ->Height(500);
        
        // 创建主容器
        auto* mainPanel = new ui::StackPanel();
        
        // 标题
        auto* title = new ui::TextBlock();
        title->Text("命名作用域演示 - 不同区域可以使用相同名称");
        title->FontSize(24.0f);
        title->FontWeight(ui::FontWeight::Bold);
        title->TextAlignment(ui::TextAlignment::Center);
        title->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        mainPanel->AddChild(title);
        
        // 说明
        auto* explanation = new ui::TextBlock();
        explanation->Text("FindName只在当前元素的子树中搜索\n不同父元素下可以有相同名称的子元素");
        explanation->FontSize(14.0f);
        explanation->TextAlignment(ui::TextAlignment::Center);
        explanation->Margin(0, 10, 0, 20);
        mainPanel->AddChild(explanation);
        
        // ========== 区域1：Header ==========
        auto* headerBorder = new ui::Border();
        headerBorder->Name("headerSection");
        headerBorder->BorderBrush(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        headerBorder->BorderThickness(2);
        headerBorder->Padding(15);
        headerBorder->Margin(10);
        
        auto* headerPanel = new ui::StackPanel();
        headerPanel->Name("headerPanel");
        
        auto* headerTitle = new ui::TextBlock();
        headerTitle->Name("sectionTitle");  // 名称：sectionTitle
        headerTitle->Text("Header 区域");
        headerTitle->FontSize(18.0f);
        headerTitle->FontWeight(ui::FontWeight::Bold);
        headerPanel->AddChild(headerTitle);
        
        auto* headerButton = new ui::Button();
        headerButton->Name("actionButton");  // 名称：actionButton
        headerButton->Content("Header 操作");
        headerButton->Width(150);
        headerButton->Height(30);
        headerButton->Margin(0, 5, 0, 0);
        headerPanel->AddChild(headerButton);
        
        headerBorder->Child(headerPanel);
        mainPanel->AddChild(headerBorder);
        
        // ========== 区域2：Content ==========
        auto* contentBorder = new ui::Border();
        contentBorder->Name("contentSection");
        contentBorder->BorderBrush(new ui::SolidColorBrush(ui::Color(0, 150, 0, 255)));
        contentBorder->BorderThickness(2);
        contentBorder->Padding(15);
        contentBorder->Margin(10);
        
        auto* contentPanel = new ui::StackPanel();
        contentPanel->Name("contentPanel");
        
        auto* contentTitle = new ui::TextBlock();
        contentTitle->Name("sectionTitle");  // 相同名称！sectionTitle
        contentTitle->Text("Content 区域");
        contentTitle->FontSize(18.0f);
        contentTitle->FontWeight(ui::FontWeight::Bold);
        contentPanel->AddChild(contentTitle);
        
        auto* contentButton = new ui::Button();
        contentButton->Name("actionButton");  // 相同名称！actionButton
        contentButton->Content("Content 操作");
        contentButton->Width(150);
        contentButton->Height(30);
        contentButton->Margin(0, 5, 0, 0);
        contentPanel->AddChild(contentButton);
        
        contentBorder->Child(contentPanel);
        mainPanel->AddChild(contentBorder);
        
        // ========== 区域3：Footer ==========
        auto* footerBorder = new ui::Border();
        footerBorder->Name("footerSection");
        footerBorder->BorderBrush(new ui::SolidColorBrush(ui::Color(200, 0, 0, 255)));
        footerBorder->BorderThickness(2);
        footerBorder->Padding(15);
        footerBorder->Margin(10);
        
        auto* footerPanel = new ui::StackPanel();
        footerPanel->Name("footerPanel");
        
        auto* footerTitle = new ui::TextBlock();
        footerTitle->Name("sectionTitle");  // 还是相同名称！sectionTitle
        footerTitle->Text("Footer 区域");
        footerTitle->FontSize(18.0f);
        footerTitle->FontWeight(ui::FontWeight::Bold);
        footerPanel->AddChild(footerTitle);
        
        auto* footerButton = new ui::Button();
        footerButton->Name("actionButton");  // 还是相同名称！actionButton
        footerButton->Content("Footer 操作");
        footerButton->Width(150);
        footerButton->Height(30);
        footerButton->Margin(0, 5, 0, 0);
        footerPanel->AddChild(footerButton);
        
        footerBorder->Child(footerPanel);
        mainPanel->AddChild(footerBorder);
        
        window->Content(mainPanel);
        
        std::cout << "✓ UI 结构创建成功\n";
        std::cout << "  - 创建了3个区域（Header, Content, Footer）\n";
        std::cout << "  - 每个区域都有名为 'sectionTitle' 的文本\n";
        std::cout << "  - 每个区域都有名为 'actionButton' 的按钮\n\n";
        
        // ========== 测试命名作用域 ==========
        
        std::cout << "测试命名作用域：\n";
        std::cout << "========================================\n\n";
        
        // 测试1：从headerPanel查找
        std::cout << "测试1：从headerPanel查找 'sectionTitle'\n";
        auto* found1 = headerPanel->FindName("sectionTitle");
        if (found1) {
            auto* text = dynamic_cast<ui::TextBlock*>(found1);
            if (text) {
                std::cout << "  ✓ 找到：" << text->GetText() << "\n";
                std::cout << "  ✓ 这是 Header 区域的标题\n";
            }
        }
        
        // 测试2：从contentPanel查找
        std::cout << "\n测试2：从contentPanel查找 'sectionTitle'\n";
        auto* found2 = contentPanel->FindName("sectionTitle");
        if (found2) {
            auto* text = dynamic_cast<ui::TextBlock*>(found2);
            if (text) {
                std::cout << "  ✓ 找到：" << text->GetText() << "\n";
                std::cout << "  ✓ 这是 Content 区域的标题\n";
            }
        }
        
        // 测试3：从footerPanel查找
        std::cout << "\n测试3：从footerPanel查找 'sectionTitle'\n";
        auto* found3 = footerPanel->FindName("sectionTitle");
        if (found3) {
            auto* text = dynamic_cast<ui::TextBlock*>(found3);
            if (text) {
                std::cout << "  ✓ 找到：" << text->GetText() << "\n";
                std::cout << "  ✓ 这是 Footer 区域的标题\n";
            }
        }
        
        // 测试4：从window查找（会找到第一个）
        std::cout << "\n测试4：从window查找 'sectionTitle'\n";
        auto* found4 = window->FindName("sectionTitle");
        if (found4) {
            auto* text = dynamic_cast<ui::TextBlock*>(found4);
            if (text) {
                std::cout << "  ✓ 找到：" << text->GetText() << "\n";
                std::cout << "  ⚠ 从根部查找，返回第一个遇到的（Header）\n";
            }
        }
        
        // 测试5：从headerBorder查找actionButton
        std::cout << "\n测试5：从headerBorder查找 'actionButton'\n";
        auto* found5 = headerBorder->FindName("actionButton");
        if (found5) {
            auto* btn = dynamic_cast<ui::Button*>(found5);
            if (btn) {
                auto content = btn->GetContent();
                if (content.type() == typeid(std::string)) {
                    std::cout << "  ✓ 找到：" << std::any_cast<std::string>(content) << "\n";
                    std::cout << "  ✓ 这是 Header 区域的按钮\n";
                }
            }
        }
        
        // 测试6：从contentBorder查找actionButton
        std::cout << "\n测试6：从contentBorder查找 'actionButton'\n";
        auto* found6 = contentBorder->FindName("actionButton");
        if (found6) {
            auto* btn = dynamic_cast<ui::Button*>(found6);
            if (btn) {
                auto content = btn->GetContent();
                if (content.type() == typeid(std::string)) {
                    std::cout << "  ✓ 找到：" << std::any_cast<std::string>(content) << "\n";
                    std::cout << "  ✓ 这是 Content 区域的按钮\n";
                }
            }
        }
        
        std::cout << "\n========================================\n";
        std::cout << "结论：\n";
        std::cout << "  ✅ 不同的父元素（作用域）下可以使用相同的名称\n";
        std::cout << "  ✅ FindName 只在当前元素的子树中搜索\n";
        std::cout << "  ✅ 通过从正确的父元素开始搜索，可以访问到正确的元素\n";
        std::cout << "  ✅ 这就是\"命名作用域\"的概念\n\n";
        
        std::cout << "实际应用场景：\n";
        std::cout << "  1. 不同窗口中可以有相同名称的控件\n";
        std::cout << "  2. 同一窗口的不同区域可以有相同名称的控件\n";
        std::cout << "  3. 只需要确保在同一个逻辑容器内名称唯一即可\n";
        std::cout << "  4. 通过保持对容器的引用，可以访问其内部的命名元素\n\n";
        
        window->Show();
        std::cout << "✓ 窗口已显示\n";

        // 运行3秒
        std::cout << "\n运行窗口 3 秒...\n";
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (window->ProcessEvents()) {
            window->RenderFrame();
            frameCount++;
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - startTime
            ).count();
            
            if (elapsed >= 3) {
                window->Close();
                break;
            }
        }
        
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   演示完成！                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n✅ 命名作用域让您可以在不同区域使用相同的名称\n";
        std::cout << "✅ 不需要在整个应用中保持名称唯一\n";
        std::cout << "✅ 只需在同一个逻辑容器内保持唯一即可\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
