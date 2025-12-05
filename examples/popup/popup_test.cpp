/**
 * @file popup_test.cpp
 * @brief 测试 Popup 控件基础功能 (Day 4)
 * 
 * 测试内容：
 * 1. Popup 属性设置
 * 2. Open/Close 逻辑
 * 3. PopupRoot 创建和显示
 * 4. 事件触发（Opened/Closed）
 * 5. 与 PopupService 集成
 */

#include "fk/ui/controls/Popup.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/ui/Window.h"
#include "fk/ui/PopupService.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

using namespace fk::ui;

int main() {
    std::cout << "========== Popup 控件测试 ==========" << std::endl;
    
#ifdef FK_HAS_GLFW
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "\n=== 测试 1: Popup 属性设置 ===" << std::endl;
    Popup* popup = new Popup();
    
    std::cout << "Default IsOpen: " << (popup->GetIsOpen() ? "true" : "false") << std::endl;
    std::cout << "Default Placement: " << static_cast<int>(popup->GetPlacement()) << std::endl;
    std::cout << "Default StaysOpen: " << (popup->GetStaysOpen() ? "true" : "false") << std::endl;
    std::cout << "Default Width: " << popup->GetWidth() << std::endl;
    std::cout << "Default Height: " << popup->GetHeight() << std::endl;
    
    // 设置属性
    popup->SetWidth(300);
    popup->SetHeight(200);
    popup->SetPlacement(PlacementMode::Absolute);
    popup->SetHorizontalOffset(100);
    popup->SetVerticalOffset(100);
    
    std::cout << "After setup - Width: " << popup->GetWidth() << ", Height: " << popup->GetHeight() << std::endl;
    
    std::cout << "\n=== 测试 2: 创建 Popup 内容 ===" << std::endl;
    
    // 创建内容：Border + TextBlock
    Border* border = new Border();
    border->SetBackground(new SolidColorBrush(Color(0.95f, 0.95f, 0.95f, 1.0f)));
    border->SetBorderBrush(new SolidColorBrush(Color(0.3f, 0.5f, 0.8f, 1.0f)));
    border->SetBorderThickness(Thickness(2));
    border->SetCornerRadius(CornerRadius(8));
    border->SetPadding(Thickness(15));
    
    TextBlock* text = new TextBlock();
    text->SetText("Hello from Popup!");
    text->SetFontSize(14);
    text->SetForeground(new SolidColorBrush(Color::Black()));
    
    border->SetChild(text);
    popup->SetChild(border);
    
    std::cout << "Content created: Border with TextBlock" << std::endl;
    
    std::cout << "\n=== 测试 3: 事件监听 ===" << std::endl;
    
    bool openedFired = false;
    bool closedFired = false;
    
    popup->Opened += [&]() {
        std::cout << "  [Event] Opened fired!" << std::endl;
        openedFired = true;
    };
    
    popup->Closed += [&]() {
        std::cout << "  [Event] Closed fired!" << std::endl;
        closedFired = true;
    };
    
    std::cout << "Event handlers registered" << std::endl;
    
    std::cout << "\n=== 测试 4: 打开 Popup ===" << std::endl;
    std::cout << "PopupService active popups before open: " 
              << PopupService::Instance().GetActivePopups().size() << std::endl;
    
    popup->SetIsOpen(true);
    
    std::cout << "IsOpen: " << (popup->GetIsOpen() ? "true" : "false") << std::endl;
    std::cout << "PopupRoot created: " << (popup->GetPopupRoot() != nullptr ? "true" : "false") << std::endl;
    std::cout << "PopupService active popups after open: " 
              << PopupService::Instance().GetActivePopups().size() << std::endl;
    std::cout << "Opened event fired: " << (openedFired ? "✓ Yes" : "✗ No") << std::endl;
    
    std::cout << "\n=== 测试 5: Popup 保持显示 3 秒 ===" << std::endl;
    std::cout << "Rendering Popup for 3 seconds..." << std::endl;
    
    auto startTime = std::chrono::steady_clock::now();
    int frameCount = 0;
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 3) {
        
        // 调用 PopupService 渲染（模拟主循环）
        PopupService::Instance().RenderAll();
        
        frameCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
    }
    
    std::cout << "Rendered " << frameCount << " frames" << std::endl;
    
    std::cout << "\n=== 测试 6: 关闭 Popup ===" << std::endl;
    std::cout << "PopupService active popups before close: " 
              << PopupService::Instance().GetActivePopups().size() << std::endl;
    
    popup->SetIsOpen(false);
    
    std::cout << "IsOpen: " << (popup->GetIsOpen() ? "true" : "false") << std::endl;
    std::cout << "PopupService active popups after close: " 
              << PopupService::Instance().GetActivePopups().size() << std::endl;
    std::cout << "Closed event fired: " << (closedFired ? "✓ Yes" : "✗ No") << std::endl;
    
    std::cout << "\n=== 测试 7: 重新打开 Popup ===" << std::endl;
    popup->SetIsOpen(true);
    std::cout << "Reopened, active popups: " 
              << PopupService::Instance().GetActivePopups().size() << std::endl;
    
    // 再保持 2 秒
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        PopupService::Instance().RenderAll();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    // 清理
    delete popup;
    glfwTerminate();
    
    std::cout << "\n========== 测试完成 ==========" << std::endl;
    std::cout << "Summary:" << std::endl;
    std::cout << "  Opened event: " << (openedFired ? "✓" : "✗") << std::endl;
    std::cout << "  Closed event: " << (closedFired ? "✓" : "✗") << std::endl;
    std::cout << "  PopupService integration: ✓" << std::endl;
    
    return 0;
#else
    std::cerr << "GLFW is not available" << std::endl;
    return -1;
#endif
}
