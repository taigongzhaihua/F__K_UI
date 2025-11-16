/**
 * 重复名称测试程序
 * 演示当出现两个同名控件时的行为
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
    std::cout << "║         重复名称行为测试                                 ║\n";
    std::cout << "║         测试当出现两个同名控件时的查找行为               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        auto window = app->CreateWindow();
        window->Title("重复名称测试")
              ->Width(600)
              ->Height(400);
        
        // 创建主容器
        auto* mainPanel = new ui::StackPanel();
        mainPanel->Name("mainPanel");
        
        // 标题
        auto* title = new ui::TextBlock();
        title->Text("重复名称行为测试");
        title->FontSize(28.0f);
        title->FontWeight(ui::FontWeight::Bold);
        title->TextAlignment(ui::TextAlignment::Center);
        title->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        mainPanel->AddChild(title);
        
        // 说明
        auto* desc = new ui::TextBlock();
        desc->Text("当有多个同名控件时，FindName返回第一个找到的（深度优先）");
        desc->FontSize(14.0f);
        desc->TextAlignment(ui::TextAlignment::Center);
        desc->Margin(0, 10, 0, 20);
        mainPanel->AddChild(desc);
        
        // 创建第一个同名按钮
        auto* button1 = new ui::Button();
        button1->Name("duplicateName");  // 相同的名称
        button1->Content("第一个按钮（duplicateName）");
        button1->Width(300);
        button1->Height(40);
        button1->Margin(0, 5, 0, 5);
        mainPanel->AddChild(button1);
        
        // 创建一个中间面板
        auto* middlePanel = new ui::StackPanel();
        middlePanel->Name("middlePanel");
        
        // 在中间面板中创建第二个同名按钮
        auto* button2 = new ui::Button();
        button2->Name("duplicateName");  // 相同的名称！
        button2->Content("第二个按钮（duplicateName）");
        button2->Width(300);
        button2->Height(40);
        button2->Margin(0, 5, 0, 5);
        middlePanel->AddChild(button2);
        
        mainPanel->AddChild(middlePanel);
        
        // 创建第三个同名按钮（在更深的层次）
        auto* innerPanel = new ui::StackPanel();
        innerPanel->Name("innerPanel");
        
        auto* button3 = new ui::Button();
        button3->Name("duplicateName");  // 还是相同的名称！
        button3->Content("第三个按钮（duplicateName）");
        button3->Width(300);
        button3->Height(40);
        button3->Margin(0, 5, 0, 5);
        innerPanel->AddChild(button3);
        
        middlePanel->AddChild(innerPanel);
        
        // 结果文本
        auto* resultText = new ui::TextBlock();
        resultText->Name("resultText");
        resultText->FontSize(14.0f);
        resultText->TextAlignment(ui::TextAlignment::Center);
        resultText->Foreground(new ui::SolidColorBrush(ui::Color(0, 150, 0, 255)));
        resultText->Margin(0, 20, 0, 0);
        mainPanel->AddChild(resultText);
        
        window->Content(mainPanel);
        
        std::cout << "✓ UI 结构创建成功\n";
        std::cout << "  - 创建了3个名为 'duplicateName' 的按钮\n";
        std::cout << "  - 按钮1在主面板中\n";
        std::cout << "  - 按钮2在中间面板中\n";
        std::cout << "  - 按钮3在内部面板中（更深层次）\n\n";
        
        // ========== 测试重复名称的查找行为 ==========
        
        std::cout << "开始测试重复名称的查找行为：\n";
        std::cout << "========================================\n\n";
        
        // 测试1：从窗口查找
        std::cout << "测试1：从窗口查找 'duplicateName'\n";
        auto* found1 = window->FindName("duplicateName");
        if (found1) {
            auto* btn = dynamic_cast<ui::Button*>(found1);
            if (btn) {
                auto content = btn->GetContent();
                if (content.type() == typeid(std::string)) {
                    std::string text = std::any_cast<std::string>(content);
                    std::cout << "  ✓ 找到按钮，内容: " << text << "\n";
                    if (found1 == button1) {
                        std::cout << "  ✓ 确认：返回的是第一个按钮\n";
                    } else if (found1 == button2) {
                        std::cout << "  ⚠ 返回的是第二个按钮\n";
                    } else if (found1 == button3) {
                        std::cout << "  ⚠ 返回的是第三个按钮\n";
                    }
                }
            }
        } else {
            std::cout << "  ✗ 未找到按钮\n";
        }
        
        // 测试2：从中间面板查找
        std::cout << "\n测试2：从中间面板查找 'duplicateName'\n";
        auto* found2 = middlePanel->FindName("duplicateName");
        if (found2) {
            auto* btn = dynamic_cast<ui::Button*>(found2);
            if (btn) {
                auto content = btn->GetContent();
                if (content.type() == typeid(std::string)) {
                    std::string text = std::any_cast<std::string>(content);
                    std::cout << "  ✓ 找到按钮，内容: " << text << "\n";
                    if (found2 == button2) {
                        std::cout << "  ✓ 确认：返回的是第二个按钮（中间面板的直接子元素）\n";
                    } else if (found2 == button3) {
                        std::cout << "  ⚠ 返回的是第三个按钮\n";
                    }
                }
            }
        } else {
            std::cout << "  ✗ 未找到按钮\n";
        }
        
        // 测试3：从内部面板查找
        std::cout << "\n测试3：从内部面板查找 'duplicateName'\n";
        auto* found3 = innerPanel->FindName("duplicateName");
        if (found3) {
            auto* btn = dynamic_cast<ui::Button*>(found3);
            if (btn) {
                auto content = btn->GetContent();
                if (content.type() == typeid(std::string)) {
                    std::string text = std::any_cast<std::string>(content);
                    std::cout << "  ✓ 找到按钮，内容: " << text << "\n";
                    if (found3 == button3) {
                        std::cout << "  ✓ 确认：返回的是第三个按钮（内部面板的直接子元素）\n";
                    }
                }
            }
        } else {
            std::cout << "  ✗ 未找到按钮\n";
        }
        
        std::cout << "\n========================================\n";
        std::cout << "测试结论：\n";
        std::cout << "  1. FindName 使用深度优先搜索（DFS）\n";
        std::cout << "  2. 遇到第一个匹配的元素就立即返回\n";
        std::cout << "  3. 从不同的起始点查找，会返回不同的结果\n";
        std::cout << "  4. 建议：在同一逻辑树中使用唯一的名称\n\n";
        
        std::cout << "最佳实践：\n";
        std::cout << "  ✓ 为每个需要查找的元素分配唯一的名称\n";
        std::cout << "  ✓ 使用有意义的前缀，如 'header_', 'footer_'\n";
        std::cout << "  ✓ 在设计阶段就规划好命名规范\n";
        std::cout << "  ✗ 避免在同一逻辑树中使用重复的名称\n\n";
        
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
        std::cout << "║                   测试完成！                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
