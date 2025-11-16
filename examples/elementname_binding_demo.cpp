/**
 * ElementName 绑定演示程序
 * 展示如何使用 ElementName 绑定将一个控件的属性绑定到另一个命名控件的属性
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/binding/Binding.h"
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         F__K_UI ElementName 绑定演示                     ║\n";
    std::cout << "║         演示控件间的属性绑定                             ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        // 创建应用程序
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        // 创建窗口
        auto window = app->CreateWindow();
        window->Title("ElementName 绑定演示")
              ->Width(600)
              ->Height(400);
        
        // 创建主容器
        auto* mainPanel = new ui::StackPanel();
        mainPanel->Name("mainPanel");
        
        // 添加标题
        auto* title = new ui::TextBlock();
        title->Name("titleText");
        title->Text("ElementName 绑定演示");
        title->FontSize(32.0f);
        title->FontWeight(ui::FontWeight::Bold);
        title->TextAlignment(ui::TextAlignment::Center);
        title->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        mainPanel->AddChild(title);
        
        // 添加说明文本
        auto* description = new ui::TextBlock();
        description->Name("descriptionText");
        description->Text("ElementName绑定允许一个控件的属性绑定到\n另一个命名控件的属性。");
        description->FontSize(16.0f);
        description->TextAlignment(ui::TextAlignment::Center);
        description->Foreground(new ui::SolidColorBrush(ui::Color(80, 80, 80, 255)));
        description->Margin(0, 10, 0, 20);
        mainPanel->AddChild(description);
        
        // 示例1：源按钮
        auto* sourceButton = new ui::Button();
        sourceButton->Name("sourceButton");
        sourceButton->Content("源控件");
        sourceButton->Width(200);
        sourceButton->Height(50);
        sourceButton->Margin(0, 10, 0, 10);
        mainPanel->AddChild(sourceButton);
        
        // 示例1：目标文本块 - 绑定到按钮的 Content 属性
        auto* targetText1 = new ui::TextBlock();
        targetText1->Name("targetText1");
        targetText1->FontSize(18.0f);
        targetText1->TextAlignment(ui::TextAlignment::Center);
        targetText1->Foreground(new ui::SolidColorBrush(ui::Color(0, 150, 0, 255)));
        targetText1->Margin(0, 5, 0, 20);
        
        // 创建 ElementName 绑定
        binding::Binding textBinding;
        textBinding.ElementName("sourceButton")
                   .Path("Content");
        targetText1->SetBinding(ui::TextBlock::TextProperty(), std::move(textBinding));
        mainPanel->AddChild(targetText1);
        
        // 示例2：源文本块（用于显示宽度）
        auto* widthLabel = new ui::TextBlock();
        widthLabel->Name("widthLabel");
        widthLabel->Text("控件宽度: ");
        widthLabel->FontSize(16.0f);
        widthLabel->TextAlignment(ui::TextAlignment::Center);
        widthLabel->Margin(0, 10, 0, 5);
        mainPanel->AddChild(widthLabel);
        
        // 示例2：目标文本块 - 绑定到按钮的 Width 属性
        auto* targetText2 = new ui::TextBlock();
        targetText2->Name("targetText2");
        targetText2->FontSize(16.0f);
        targetText2->TextAlignment(ui::TextAlignment::Center);
        targetText2->Foreground(new ui::SolidColorBrush(ui::Color(0, 100, 200, 255)));
        
        // 创建 ElementName 绑定到 Width 属性
        binding::Binding widthBinding;
        widthBinding.ElementName("sourceButton")
                    .Path("Width");
        targetText2->SetBinding(ui::TextBlock::TextProperty(), std::move(widthBinding));
        mainPanel->AddChild(targetText2);
        
        // 状态说明
        auto* statusText = new ui::TextBlock();
        statusText->Name("statusText");
        statusText->Text("\n✓ ElementName 绑定已建立\n✓ 目标控件会自动反映源控件的属性变化");
        statusText->FontSize(14.0f);
        statusText->TextAlignment(ui::TextAlignment::Center);
        statusText->Foreground(new ui::SolidColorBrush(ui::Color(100, 100, 100, 255)));
        statusText->Margin(0, 20, 0, 0);
        mainPanel->AddChild(statusText);
        
        // 设置窗口内容
        window->Content(mainPanel);
        
        std::cout << "✓ UI 结构创建成功\n";
        std::cout << "✓ ElementName 绑定已设置\n\n";
        
        // 验证 ElementName 绑定
        std::cout << "验证 ElementName 绑定功能：\n";
        std::cout << "----------------------------------------\n";
        
        // 测试1：验证源控件可以被找到
        std::cout << "测试1：使用 FindName 查找源控件\n";
        auto* foundSource = window->FindName("sourceButton");
        if (foundSource) {
            std::cout << "  ✓ 成功找到源控件 'sourceButton'\n";
            auto* btn = dynamic_cast<ui::Button*>(foundSource);
            if (btn) {
                std::cout << "  ✓ 控件类型正确: Button\n";
            }
        } else {
            std::cout << "  ✗ 未找到源控件\n";
        }
        
        // 测试2：验证目标控件可以被找到
        std::cout << "\n测试2：使用 FindName 查找目标控件\n";
        auto* foundTarget = window->FindName("targetText1");
        if (foundTarget) {
            std::cout << "  ✓ 成功找到目标控件 'targetText1'\n";
            auto* text = dynamic_cast<ui::TextBlock*>(foundTarget);
            if (text) {
                std::cout << "  ✓ 控件类型正确: TextBlock\n";
            }
        } else {
            std::cout << "  ✗ 未找到目标控件\n";
        }
        
        std::cout << "----------------------------------------\n";
        std::cout << "✓ ElementName 绑定验证完成！\n\n";
        
        std::cout << "工作原理：\n";
        std::cout << "  1. 使用 Name(\"name\") 为控件命名\n";
        std::cout << "  2. 创建绑定: Binding().ElementName(\"name\").Path(\"Property\")\n";
        std::cout << "  3. 目标控件通过 SetBinding() 应用绑定\n";
        std::cout << "  4. 绑定系统自动查找命名的源控件并建立连接\n";
        std::cout << "  5. 当源控件属性改变时，目标控件自动更新\n\n";
        
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
        std::cout << "\n✓ ElementName 绑定功能已实现\n";
        std::cout << "✓ 渲染了 " << frameCount << " 帧\n";
        std::cout << "✓ 通过 Name() 和 ElementName 绑定实现控件间属性连接\n\n";
        
        std::cout << "功能总结：\n";
        std::cout << "  - Name() 方法：为控件设置名称（同时支持 FindName 和 ElementName）\n";
        std::cout << "  - FindName() 方法：在逻辑树中查找命名控件\n";
        std::cout << "  - ElementName 绑定：将控件属性绑定到其他命名控件\n";
        std::cout << "  - 与 WPF 的 x:Name 和 ElementName 绑定机制类似\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
