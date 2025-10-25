#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"

using namespace fk;

/**
 * Phase 2 演示：自动失效追踪
 * 
 * 展示：
 * 1. 属性变更自动触发 InvalidateVisual
 * 2. 布局变更自动触发重绘
 * 3. 不需要手动调用 RequestRender
 */
int main()
{
    try {
        std::cout << "\n=== Phase 2 Demo: Auto Invalidation Tracking ===" << std::endl;
        std::cout << "This demo shows automatic render invalidation:" << std::endl;
        std::cout << "- Property changes auto-trigger InvalidateVisual()" << std::endl;
        std::cout << "- Layout changes auto-trigger rendering" << std::endl;
        std::cout << "- NO manual RequestRender() calls needed!" << std::endl;
        std::cout << std::endl;

        // 创建 UI 树 (使用流式API)
        auto child1 = std::make_shared<ui::ContentControl>();
        auto child2 = std::make_shared<ui::ContentControl>();
        auto child3 = std::make_shared<ui::ContentControl>();
        
        auto panel = std::make_shared<ui::StackPanel>();
        panel->Orientation(ui::Orientation::Vertical)
             ->AddChild(child1)
             ->AddChild(child2)
             ->AddChild(child3);

        // 创建窗口
        auto window = ui::window()
            ->Title("Phase 2 Demo - Auto Invalidation")
            ->Width(600)
            ->Height(400);

        window->Content(panel);

        // 订阅事件
        int resizeCount = 0;
        window->Resized += [&resizeCount](int w, int h) {
            resizeCount++;
            std::cout << "[Event] Window resized #" << resizeCount 
                      << " to " << w << "x" << h << std::endl;
        };

        // 演示：在后台线程修改属性（会自动触发 InvalidateVisual）
        std::thread([child1, child2, child3]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::cout << "\n[Thread] Modifying opacity (auto triggers InvalidateVisual)..." << std::endl;
            child1->SetOpacity(0.5f);
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::cout << "[Thread] Changing visibility (auto triggers InvalidateVisual)..." << std::endl;
            child2->SetVisibility(ui::Visibility::Hidden);
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            std::cout << "[Thread] Disabling element (auto triggers InvalidateVisual)..." << std::endl;
            child3->SetIsEnabled(false);
            
            std::cout << "[Thread] All property changes complete!" << std::endl;
            std::cout << "Notice: NO manual RequestRender() was called!" << std::endl;
            
        }).detach();

        // 运行应用
        std::cout << "\nStarting application..." << std::endl;
        std::cout << "Try resizing the window - layout auto-triggers rendering!" << std::endl;
        std::cout << "Close window to exit.\n" << std::endl;
        
        Application app;
        app.Run(window);

        std::cout << "\n=== Phase 2 Demo completed ===" << std::endl;
        std::cout << "Summary:" << std::endl;
        std::cout << "- Property changes automatically invalidated visuals" << std::endl;
        std::cout << "- Layout changes automatically triggered rendering" << std::endl;
        std::cout << "- Total resize events: " << resizeCount << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
