/**
 * 流畅 API 演示程序
 * 展示改进后的声明式 UI 编写方式
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         F__K_UI 流畅 API 演示程序                        ║\n";
    std::cout << "║          Fluent API Demonstration                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        // 创建应用程序
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application 创建成功\n";

        // 使用流畅的 API 创建 UI 层次结构
        // 注意：现在可以直接传递 UIElement* 指针，无需手动包装成 std::any
        auto window = app->CreateWindow();
        
        // 创建一个 StackPanel 作为容器
        auto* panel = new ui::StackPanel();
        
        // 添加标题文本
        auto* title = new ui::TextBlock();
        title->Text("欢迎使用 F__K_UI!")
            ->FontFamily("Arial")
            ->FontSize(32.0f)
            ->FontWeight(ui::FontWeight::Bold)
            ->TextAlignment(ui::TextAlignment::Center)
            ->Foreground(new ui::SolidColorBrush(ui::Color(0, 120, 215, 255)));
        
        // 添加描述文本
        auto* description = new ui::TextBlock();
        description->Text("这是一个展示改进后流畅 API 的示例程序。\n现在可以直接使用 UIElement* 指针，无需手动包装成 std::any！")
            ->FontFamily("Arial")
            ->FontSize(16.0f)
            ->TextAlignment(ui::TextAlignment::Center)
            ->TextWrapping(ui::TextWrapping::Wrap)
            ->Foreground(new ui::SolidColorBrush(ui::Color(100, 100, 100, 255)));
        
        // 注意：StackPanel 的 Children 添加需要使用现有的 API
        // 这里我们只是展示 Content 属性的改进
        
        // 配置窗口 - 使用流畅的链式调用
        // 最重要的改进：Content() 现在可以直接接受 UIElement* 指针！
        window->Title("F__K_UI - 流畅 API 演示")
            ->Width(600)
            ->Height(400)
            ->Content(title);  // 直接传递 UIElement*，不需要 std::any 包装！
        
        std::cout << "✓ UI 结构创建成功\n";
        std::cout << "✓ 使用了改进后的流畅 API\n";
        std::cout << "✓ Content() 方法现在直接接受 UIElement* 指针\n";

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
        std::cout << "\n✓ 流畅 API 工作正常\n";
        std::cout << "✓ 渲染了 " << frameCount << " 帧\n";
        std::cout << "✓ 声明式 UI 编写更加简洁直观\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
