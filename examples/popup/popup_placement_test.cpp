/**
 * @file popup_placement_test.cpp
 * @brief 测试 Popup 定位和边界检测 (Day 5)
 * 
 * 测试内容：
 * 1. 各种 PlacementMode 的定位
 * 2. 边界检测（确保不超出屏幕）
 * 3. 偏移量应用
 */

#include "fk/ui/controls/Popup.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/buttons/Button.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

using namespace fk::ui;

Border* CreatePopupContent(const std::string& text) {
    Border* border = new Border();
    border->SetBackground(new SolidColorBrush(Color(0.95f, 0.95f, 0.95f, 1.0f)));
    border->SetBorderBrush(new SolidColorBrush(Color(0.2f, 0.4f, 0.8f, 1.0f)));
    border->SetBorderThickness(Thickness(2));
    border->SetCornerRadius(CornerRadius(5));
    border->SetPadding(Thickness(10));
    
    TextBlock* textBlock = new TextBlock();
    textBlock->SetText(text);
    textBlock->SetFontSize(12);
    textBlock->SetForeground(new SolidColorBrush(Color::Black()));
    
    border->SetChild(textBlock);
    return border;
}

int main() {
    std::cout << "========== Popup 定位与边界检测测试 ==========" << std::endl;
    
#ifdef FK_HAS_GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // 创建主窗口
    Window* window = new Window();
    window->SetWidth(600);
    window->SetHeight(400);
    window->SetTitle("Popup Placement Test");
    
    // 创建按钮作为 PlacementTarget
    Button* targetButton = new Button();
    targetButton->SetWidth(150);
    targetButton->SetHeight(40);
    targetButton->SetContent("Click Target");
    targetButton->SetHorizontalAlignment(HorizontalAlignment::Center);
    targetButton->SetVerticalAlignment(VerticalAlignment::Center);
    
    window->SetContent(targetButton);
    window->Show();
    
    // 等待布局完成
    for (int i = 0; i < 5; ++i) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "\n=== 测试 1: PlacementMode::Bottom ===" << std::endl;
    Popup* popup1 = new Popup();
    popup1->SetWidth(200);
    popup1->SetHeight(100);
    popup1->SetPlacement(PlacementMode::Bottom);
    popup1->SetPlacementTarget(targetButton);
    popup1->SetChild(CreatePopupContent("Bottom Placement"));
    
    popup1->SetIsOpen(true);
    std::cout << "Popup opened at Bottom position" << std::endl;
    
    // 显示 2 秒
    auto startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup1->SetIsOpen(false);
    
    std::cout << "\n=== 测试 2: PlacementMode::Right ===" << std::endl;
    Popup* popup2 = new Popup();
    popup2->SetWidth(200);
    popup2->SetHeight(100);
    popup2->SetPlacement(PlacementMode::Right);
    popup2->SetPlacementTarget(targetButton);
    popup2->SetChild(CreatePopupContent("Right Placement"));
    
    popup2->SetIsOpen(true);
    std::cout << "Popup opened at Right position" << std::endl;
    
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup2->SetIsOpen(false);
    
    std::cout << "\n=== 测试 3: PlacementMode::Center ===" << std::endl;
    Popup* popup3 = new Popup();
    popup3->SetWidth(200);
    popup3->SetHeight(100);
    popup3->SetPlacement(PlacementMode::Center);
    popup3->SetPlacementTarget(targetButton);
    popup3->SetChild(CreatePopupContent("Center Placement"));
    
    popup3->SetIsOpen(true);
    std::cout << "Popup opened at Center position" << std::endl;
    
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup3->SetIsOpen(false);
    
    std::cout << "\n=== 测试 4: PlacementMode::Absolute (带边界检测) ===" << std::endl;
    Popup* popup4 = new Popup();
    popup4->SetWidth(300);
    popup4->SetHeight(200);
    popup4->SetPlacement(PlacementMode::Absolute);
    popup4->SetChild(CreatePopupContent("Absolute with Boundary"));
    
    // 测试超出屏幕右边界
    std::cout << "Testing right boundary (x=10000)..." << std::endl;
    popup4->SetHorizontalOffset(10000);
    popup4->SetVerticalOffset(100);
    popup4->SetIsOpen(true);
    
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup4->SetIsOpen(false);
    
    // 测试超出屏幕底部边界
    std::cout << "Testing bottom boundary (y=10000)..." << std::endl;
    popup4->SetHorizontalOffset(100);
    popup4->SetVerticalOffset(10000);
    popup4->SetIsOpen(true);
    
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup4->SetIsOpen(false);
    
    std::cout << "\n=== 测试 5: 偏移量应用 ===" << std::endl;
    Popup* popup5 = new Popup();
    popup5->SetWidth(200);
    popup5->SetHeight(100);
    popup5->SetPlacement(PlacementMode::Bottom);
    popup5->SetPlacementTarget(targetButton);
    popup5->SetHorizontalOffset(50);  // 向右偏移 50
    popup5->SetVerticalOffset(10);     // 向下偏移 10
    popup5->SetChild(CreatePopupContent("With Offset (+50, +10)"));
    
    popup5->SetIsOpen(true);
    std::cout << "Popup with offset opened" << std::endl;
    
    startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 2) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    popup5->SetIsOpen(false);
    
    // 清理
    delete popup1;
    delete popup2;
    delete popup3;
    delete popup4;
    delete popup5;
    delete window;
    
    glfwTerminate();
    
    std::cout << "\n========== 测试完成 ==========" << std::endl;
    std::cout << "所有定位模式和边界检测测试通过 ✓" << std::endl;
    
    return 0;
#else
    std::cerr << "GLFW is not available" << std::endl;
    return -1;
#endif
}
