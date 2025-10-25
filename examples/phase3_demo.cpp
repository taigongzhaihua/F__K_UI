#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/ui/ThreadingConfig.h"

using namespace fk;

/**
 * Phase 3 演示：线程安全检查
 * 
 * 展示不同的线程检查模式：
 * 1. Disabled - 禁用检查（最快但不安全）
 * 2. WarnOnly - 只警告不抛异常（调试用）
 * 3. ThrowException - 抛出异常（默认，推荐）
 * 4. AutoDispatch - 自动调度到UI线程（未实现）
 */

void TestThreadSafety(ui::ThreadCheckMode mode, const char* modeName, 
                      std::shared_ptr<ui::UIElement> element) {
    std::cout << "\n--- Testing " << modeName << " mode ---" << std::endl;
    ui::ThreadingConfig::Instance().SetThreadCheckMode(mode);
    
    try {
        // 在后台线程尝试修改 UI 元素
        std::thread([element, modeName]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "[Background Thread] Attempting to modify UI element..." << std::endl;
            
            try {
                element->SetOpacity(0.5f);  // 跨线程访问！
                std::cout << "[Background Thread] Success - opacity changed" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "[Background Thread] Exception caught: " << e.what() << std::endl;
            }
        }).join();
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
    }
}

int main()
{
    try {
        std::cout << "\n=== Phase 3 Demo: Thread Safety Checks ===" << std::endl;
        std::cout << "This demo shows configurable thread safety:" << std::endl;
        std::cout << "1. Disabled - No checks (fast but unsafe)" << std::endl;
        std::cout << "2. WarnOnly - Warns but doesn't throw" << std::endl;
        std::cout << "3. ThrowException - Throws exception (default)" << std::endl;
        std::cout << "4. AutoDispatch - Auto-dispatch to UI thread (future)" << std::endl;
        std::cout << std::endl;

        // 创建 UI 元素（主线程作为 UI 线程）
        auto child = std::make_shared<ui::ContentControl>();
        
        // 创建 Dispatcher 并在当前线程运行
        auto dispatcher = std::make_shared<core::Dispatcher>("UIThread");
        child->SetDispatcher(dispatcher);
        
        std::cout << "UI element attached to Dispatcher" << std::endl;
        std::cout << "Main thread ID (UI thread): " << std::this_thread::get_id() << std::endl;

        std::cout << "\nSetup complete. Testing different thread check modes...\n" << std::endl;

        // 测试 1: 禁用检查
        TestThreadSafety(ui::ThreadCheckMode::Disabled, "Disabled", child);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // 测试 2: 只警告
        TestThreadSafety(ui::ThreadCheckMode::WarnOnly, "WarnOnly", child);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // 测试 3: 抛出异常（默认）
        TestThreadSafety(ui::ThreadCheckMode::ThrowException, "ThrowException", child);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::cout << "\n=== Phase 3 Demo completed ===" << std::endl;
        std::cout << "\nSummary:" << std::endl;
        std::cout << "- Disabled mode: No checks, fastest but unsafe" << std::endl;
        std::cout << "- WarnOnly mode: Warns but allows cross-thread access" << std::endl;
        std::cout << "- ThrowException mode: Prevents cross-thread bugs (recommended)" << std::endl;
        std::cout << "\nRecommendation: Use ThrowException in development," << std::endl;
        std::cout << "                Disabled in production for performance" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}
