/**
 * @file coordinate_test.cpp
 * @brief 测试坐标转换系统 (Day 2)
 * 
 * 测试内容：
 * 1. UIElement::PointToScreen / PointFromScreen
 * 2. UIElement::GetBoundsOnScreen
 * 3. UIElement::TransformToRoot / TransformFromRoot
 * 4. UIElement::GetRootWindow
 * 5. Window::ClientToScreen / ScreenToClient
 * 6. Window::GetWindowBoundsOnScreen
 */

#include "fk/ui/Window.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/graphics/Brush.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

using namespace fk::ui;

void PrintPoint(const char* label, const Point& point) {
    std::cout << label << ": (" << point.x << ", " << point.y << ")" << std::endl;
}

void PrintRect(const char* label, const Rect& rect) {
    std::cout << label << ": (" << rect.x << ", " << rect.y 
              << ", " << rect.width << "x" << rect.height << ")" << std::endl;
}

int main() {
    std::cout << "========== 坐标转换测试 ==========" << std::endl;
    
#ifdef FK_HAS_GLFW
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // 创建窗口 (800x600)
    Window* window = new Window();
    window->SetWidth(800);
    window->SetHeight(600);
    window->SetTitle("Coordinate Test Window");
    
    // 创建布局：Grid 根容器
    Grid* rootGrid = new Grid();
    
    // 在 Grid 中添加一个 Border，位置 (200, 150), 大小 300x200
    Border* border = new Border();
    border->SetWidth(300);
    border->SetHeight(200);
    border->SetHorizontalAlignment(HorizontalAlignment::Left);
    border->SetVerticalAlignment(VerticalAlignment::Top);
    border->SetMargin(Thickness(200, 150, 0, 0));
    border->SetBackground(new SolidColorBrush(Color::Blue())); // 蓝色
    
    rootGrid->AddChild(border);
    window->SetContent(rootGrid);
    
    // 显示窗口
    window->Show();
    
    // 等待窗口布局完成（处理更多帧事件以确保布局完成）
    std::cout << "Waiting for layout..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        window->ProcessEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "Layout rect after 10 frames: (" << border->GetLayoutRect().x << ", " 
              << border->GetLayoutRect().y << ", " 
              << border->GetLayoutRect().width << "x" 
              << border->GetLayoutRect().height << ")" << std::endl;
    
    std::cout << "\n========== 测试 Window 坐标方法 ==========" << std::endl;
    
    // 获取窗口在屏幕上的边界
    Rect windowBounds = window->GetWindowBoundsOnScreen();
    PrintRect("Window bounds on screen", windowBounds);
    
    // 测试 ClientToScreen
    Point clientOrigin(0, 0);
    Point screenOrigin = window->ClientToScreen(clientOrigin);
    PrintPoint("Client (0,0) -> Screen", screenOrigin);
    
    Point clientCenter(400, 300);
    Point screenCenter = window->ClientToScreen(clientCenter);
    PrintPoint("Client (400,300) -> Screen", screenCenter);
    
    // 测试 ScreenToClient
    Point backToClient = window->ScreenToClient(screenCenter);
    PrintPoint("Screen -> Client (should be 400,300)", backToClient);
    
    std::cout << "\n========== 测试 UIElement 坐标方法 ==========" << std::endl;
    
    // 调试：检查 visual parent 链
    std::cout << "\nDebug: Visual parent chain:" << std::endl;
    std::cout << "  Border: " << border << std::endl;
    std::cout << "  Border->GetVisualParent(): " << border->GetVisualParent() << std::endl;
    if (border->GetVisualParent()) {
        std::cout << "  Parent->GetVisualParent(): " << border->GetVisualParent()->GetVisualParent() << std::endl;
    }
    std::cout << "  Window: " << window << std::endl;
    std::cout << "  RootGrid: " << rootGrid << std::endl;
    
    // 调试：检查布局矩形
    auto borderRect = border->GetLayoutRect();
    std::cout << "\nDebug: Layout rectangles:" << std::endl;
    PrintRect("  Border layout rect", borderRect);
    
    // 测试 GetRootWindow
    auto rootWindow = border->GetRootWindow();
    std::cout << "\nBorder's root window: " << (rootWindow == window ? "✓ Correct" : "✗ Wrong") << std::endl;
    if (rootWindow) {
        std::cout << "  Found root window: " << rootWindow << std::endl;
    } else {
        std::cout << "  No root window found!" << std::endl;
    }
    
    // 测试 TransformToRoot
    Point borderLocal(10, 20);
    Point borderInRoot = border->TransformToRoot(borderLocal);
    PrintPoint("Border local (10,20) -> Root", borderInRoot);
    std::cout << "  Expected: (210, 170) due to margin" << std::endl;
    
    // 测试 TransformFromRoot
    Point backToLocal = border->TransformFromRoot(borderInRoot);
    PrintPoint("Root -> Border local (should be 10,20)", backToLocal);
    
    // 测试 PointToScreen
    Point borderScreenPoint = border->PointToScreen(borderLocal);
    PrintPoint("Border local (10,20) -> Screen", borderScreenPoint);
    
    // 测试 PointFromScreen
    Point backToBorderLocal = border->PointFromScreen(borderScreenPoint);
    PrintPoint("Screen -> Border local (should be 10,20)", backToBorderLocal);
    
    // 测试 GetBoundsOnScreen
    Rect borderBounds = border->GetBoundsOnScreen();
    PrintRect("Border bounds on screen", borderBounds);
    std::cout << "  Expected: window position + (200, 150) with size 300x200" << std::endl;
    
    std::cout << "\n========== 验证坐标一致性 ==========" << std::endl;
    
    // 验证 TransformToRoot -> ClientToScreen = PointToScreen
    Point manualScreen = window->ClientToScreen(border->TransformToRoot(borderLocal));
    bool consistent1 = (manualScreen.x == borderScreenPoint.x && manualScreen.y == borderScreenPoint.y);
    std::cout << "TransformToRoot + ClientToScreen = PointToScreen: " 
              << (consistent1 ? "✓ Pass" : "✗ Fail") << std::endl;
    
    // 验证逆变换
    bool roundTrip1 = (backToClient.x == clientCenter.x && backToClient.y == clientCenter.y);
    std::cout << "ClientToScreen -> ScreenToClient round-trip: " 
              << (roundTrip1 ? "✓ Pass" : "✗ Fail") << std::endl;
    
    bool roundTrip2 = (backToBorderLocal.x == borderLocal.x && backToBorderLocal.y == borderLocal.y);
    std::cout << "PointToScreen -> PointFromScreen round-trip: " 
              << (roundTrip2 ? "✓ Pass" : "✗ Fail") << std::endl;
    
    bool roundTrip3 = (backToLocal.x == borderLocal.x && backToLocal.y == borderLocal.y);
    std::cout << "TransformToRoot -> TransformFromRoot round-trip: " 
              << (roundTrip3 ? "✓ Pass" : "✗ Fail") << std::endl;
    
    std::cout << "\n========== 测试完成 ==========" << std::endl;
    std::cout << "窗口将保持显示 3 秒..." << std::endl;
    
    // 显示 3 秒
    auto startTime = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count() < 3) {
        
        if (!window->ProcessEvents()) {
            break; // 窗口关闭
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60fps
    }
    
    // 清理
    delete window;
    glfwTerminate();
    
    std::cout << "测试结束" << std::endl;
    
    return 0;
#else
    std::cerr << "GLFW is not available" << std::endl;
    return -1;
#endif
}
