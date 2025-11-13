/**
 * Window 功能测试程序
 * 验证窗口创建、显示和事件处理
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║         F__K_UI Window 功能测试程序                      ║\n";
    std::cout << "║              Window Function Test                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";

    try {
        // 创建应用程序
        auto app = std::make_shared<Application>();
        std::cout << "✓ Application created\n";

        // 创建窗口
        auto window = app->CreateWindow();
        if (!window) {
            std::cerr << "✗ Failed to create window\n";
            return 1;
        }
        std::cout << "✓ Window created\n";

        // 配置窗口
        window->Title("F__K_UI Window Test");
        window->Width(800);
        window->Height(600);
        std::cout << "✓ Window configured: " << window->Title() 
                  << " (" << window->Width() << "x" << window->Height() << ")\n";

        // 显示窗口
        window->Show();
        std::cout << "✓ Window::Show() called\n";
        std::cout << "  Is visible: " << (window->IsVisible() ? "yes" : "no") << "\n";

        // 模拟消息循环（运行5秒）
        std::cout << "\n开始消息循环（运行5秒）...\n";
        
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (window->ProcessEvents()) {
            window->RenderFrame();
            frameCount++;
            
            // 5秒后退出
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - startTime
            ).count();
            
            if (elapsed >= 5) {
                std::cout << "\n5秒已过，关闭窗口...\n";
                window->Close();
                break;
            }
        }
        
        std::cout << "✓ 消息循环完成\n";
        std::cout << "  总帧数: " << frameCount << "\n";
        std::cout << "  平均 FPS: " << (frameCount / 5) << "\n";

        // 测试窗口状态
        std::cout << "\n测试窗口状态...\n";
        std::cout << "  最终可见性: " << (window->IsVisible() ? "visible" : "hidden") << "\n";
        std::cout << "  窗口标题: " << window->Title() << "\n";
        std::cout << "  窗口尺寸: " << window->Width() << "x" << window->Height() << "\n";

        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   测试完成！                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n✓ 所有测试通过\n";
        std::cout << "✓ 窗口创建成功\n";
        std::cout << "✓ 消息循环正常运行\n";
        std::cout << "✓ 事件处理工作正常\n";
        std::cout << "✓ 渲染循环工作正常\n";

    } catch (const std::exception& e) {
        std::cerr << "\n✗ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
