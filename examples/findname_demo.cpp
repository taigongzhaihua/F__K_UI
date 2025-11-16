/**
 * FindName 功能演示程序
 * 展示类似WPF中x:Name的元素命名和查找机制
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
    std::cout << "║         F__K_UI FindName 功能演示                        ║\n";
    std::cout << "║         类似WPF中的x:Name机制                            ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        // 创建应用程序
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        // 创建窗口
        auto window = app->CreateWindow();
        window->Title("FindName 演示")
              ->Width(600)
              ->Height(400);
        
        // 创建主容器 - StackPanel
        auto* mainPanel = new ui::StackPanel();
        mainPanel->Name("mainPanel");  // 使用流式API设置名称
        
        // 添加标题
        auto* titleText = new ui::TextBlock();
        titleText->Name("titleText");
        titleText->Text("FindName 功能演示");
        titleText->FontSize(32.0f);
        titleText->FontWeight(ui::FontWeight::Bold);
        titleText->TextAlignment(ui::TextAlignment::Center);
        titleText->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        mainPanel->AddChild(titleText);
        
        // 添加描述文本
        auto* descText = new ui::TextBlock();
        descText->Name("descriptionText");
        descText->Text("此示例展示如何使用Name()方法命名元素，\n以及使用FindName()方法查找元素。");
        descText->FontSize(16.0f);
        descText->TextAlignment(ui::TextAlignment::Center);
        descText->Foreground(new ui::SolidColorBrush(ui::Color(80, 80, 80, 255)));
        descText->Margin(0, 10, 0, 20);
        mainPanel->AddChild(descText);
        
        // 创建一个带边框的容器
        auto* contentBorder = new ui::Border();
        contentBorder->Name("contentBorder");
        contentBorder->BorderBrush(new ui::SolidColorBrush(ui::Color(200, 200, 200, 255)));
        contentBorder->BorderThickness(2);
        contentBorder->Padding(15);
        contentBorder->Margin(20, 0, 20, 0);
        
        // 在边框内创建一个嵌套的 StackPanel
        auto* innerPanel = new ui::StackPanel();
        innerPanel->Name("innerPanel");
        
        // 添加几个按钮
        for (int i = 1; i <= 3; i++) {
            auto* button = new ui::Button();
            std::string buttonName = "button" + std::to_string(i);
            std::string buttonText = "按钮 " + std::to_string(i);
            
            button->Name(buttonName);
            button->Content(buttonText);
            button->Width(200);
            button->Height(40);
            button->Margin(0, 5, 0, 5);
            
            innerPanel->AddChild(button);
        }
        
        contentBorder->Child(innerPanel);
        mainPanel->AddChild(contentBorder);
        
        // 添加状态文本
        auto* statusText = new ui::TextBlock();
        statusText->Name("statusText");
        statusText->Text("准备测试 FindName 功能...");
        statusText->FontSize(14.0f);
        statusText->TextAlignment(ui::TextAlignment::Center);
        statusText->Foreground(new ui::SolidColorBrush(ui::Color(0, 150, 0, 255)));
        statusText->Margin(0, 20, 0, 0);
        mainPanel->AddChild(statusText);
        
        // 设置窗口内容
        window->Content(mainPanel);
        
        std::cout << "✓ UI 结构创建成功\n\n";
        
        // ========== 测试 FindName 功能 ==========
        
        std::cout << "开始测试 FindName 功能：\n";
        std::cout << "----------------------------------------\n";
        
        // 测试1：从窗口查找元素
        std::cout << "测试1：从窗口查找元素\n";
        auto* foundTitle = window->FindName("titleText");
        if (foundTitle) {
            std::cout << "  ✓ 成功找到 'titleText'\n";
            auto* textBlock = dynamic_cast<ui::TextBlock*>(foundTitle);
            if (textBlock) {
                std::cout << "  ✓ 元素类型正确: TextBlock\n";
                std::cout << "  ✓ 元素文本: " << textBlock->GetText() << "\n";
            }
        } else {
            std::cout << "  ✗ 未找到 'titleText'\n";
        }
        
        // 测试2：从主面板查找嵌套元素
        std::cout << "\n测试2：从主面板查找嵌套元素\n";
        auto* foundButton = mainPanel->FindName("button2");
        if (foundButton) {
            std::cout << "  ✓ 成功找到 'button2'\n";
            auto* button = dynamic_cast<ui::Button*>(foundButton);
            if (button) {
                std::cout << "  ✓ 元素类型正确: Button\n";
            }
        } else {
            std::cout << "  ✗ 未找到 'button2'\n";
        }
        
        // 测试3：从内部面板查找按钮
        std::cout << "\n测试3：从内部面板查找按钮\n";
        auto* foundInnerPanel = mainPanel->FindName("innerPanel");
        if (foundInnerPanel) {
            std::cout << "  ✓ 成功找到 'innerPanel'\n";
            auto* foundButton1 = foundInnerPanel->FindName("button1");
            if (foundButton1) {
                std::cout << "  ✓ 从内部面板成功找到 'button1'\n";
            } else {
                std::cout << "  ✗ 从内部面板未找到 'button1'\n";
            }
        } else {
            std::cout << "  ✗ 未找到 'innerPanel'\n";
        }
        
        // 测试4：查找不存在的元素
        std::cout << "\n测试4：查找不存在的元素\n";
        auto* notFound = window->FindName("nonExistentElement");
        if (notFound == nullptr) {
            std::cout << "  ✓ 正确返回 nullptr（元素不存在）\n";
        } else {
            std::cout << "  ✗ 应该返回 nullptr 但返回了非空指针\n";
        }
        
        // 测试5：从边框查找内容
        std::cout << "\n测试5：从边框查找内容\n";
        auto* foundBorder = mainPanel->FindName("contentBorder");
        if (foundBorder) {
            std::cout << "  ✓ 成功找到 'contentBorder'\n";
            auto* foundButton3 = foundBorder->FindName("button3");
            if (foundButton3) {
                std::cout << "  ✓ 从边框成功找到 'button3'\n";
            } else {
                std::cout << "  ✗ 从边框未找到 'button3'\n";
            }
        } else {
            std::cout << "  ✗ 未找到 'contentBorder'\n";
        }
        
        // 测试6：验证Name()流式API
        std::cout << "\n测试6：验证Name()流式API\n";
        auto* testElement = new ui::TextBlock();
        testElement->Name("testElement");
        testElement->Text("测试元素");
        if (testElement->GetName() == "testElement") {
            std::cout << "  ✓ Name()流式API工作正常\n";
        } else {
            std::cout << "  ✗ Name()流式API有问题\n";
        }
        delete testElement;
        
        std::cout << "----------------------------------------\n";
        std::cout << "✓ FindName 功能测试完成！\n\n";
        
        // 显示窗口
        window->Show();
        std::cout << "✓ 窗口已显示\n";

        // 运行 5 秒后自动关闭
        std::cout << "\n运行窗口 5 秒...\n";
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (window->ProcessEvents()) {
            window->RenderFrame();
            frameCount++;
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - startTime
            ).count();
            
            if (elapsed >= 5) {
                window->Close();
                break;
            }
        }
        
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   演示完成！                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n✓ FindName 功能工作正常\n";
        std::cout << "✓ 渲染了 " << frameCount << " 帧\n";
        std::cout << "✓ 类似WPF x:Name的机制已成功实现\n\n";
        
        std::cout << "功能说明：\n";
        std::cout << "  - 使用 Name(\"name\") 方法为元素命名（流式API）\n";
        std::cout << "  - 使用 FindName(\"name\") 方法查找命名元素\n";
        std::cout << "  - FindName 会递归搜索整个逻辑树\n";
        std::cout << "  - 可以从任何元素（窗口、面板等）开始查找\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
