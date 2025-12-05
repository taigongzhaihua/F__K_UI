/**
 * @file popuproot_test.cpp
 * @brief PopupRoot 基础功能测试
 */

#include "fk/ui/window/PopupRoot.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/graphics/Brush.h"

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <thread>
#include <chrono>

using namespace fk::ui;

int main() {
    std::cout << "=== PopupRoot Test ===" << std::endl;
    
#ifdef FK_HAS_GLFW
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "GLFW initialized successfully" << std::endl;
    
    // 创建 PopupRoot
    PopupRoot popupRoot;
    
    // 初始化
    popupRoot.Initialize();
    std::cout << "PopupRoot initialized: " << popupRoot.IsInitialized() << std::endl;
    
    // 创建窗口
    popupRoot.Create(300, 200);
    std::cout << "PopupRoot created: " << popupRoot.IsCreated() << std::endl;
    
    // 设置允许透明
    popupRoot.SetAllowsTransparency(false);
    
    // 创建内容
    auto border = new Border();
    border->SetBackground(new SolidColorBrush(Color(0.9f, 0.9f, 0.9f, 1.0f)));
    border->SetBorderThickness(Thickness(2));
    border->SetBorderBrush(new SolidColorBrush(Color(0.3f, 0.5f, 0.8f, 1.0f)));
    border->SetCornerRadius(CornerRadius(5));
    border->SetPadding(Thickness(10));
    
    auto stackPanel = new StackPanel();
    
    auto title = new TextBlock();
    title->SetText("PopupRoot Test");
    title->SetFontSize(16);
    title->SetForeground(new SolidColorBrush(Color(0.0f, 0.0f, 0.0f, 1.0f)));
    stackPanel->AddChild(title);
    
    auto text1 = new TextBlock();
    text1->SetText("This is a PopupRoot window");
    text1->SetFontSize(12);
    text1->SetForeground(new SolidColorBrush(Color(0.2f, 0.2f, 0.2f, 1.0f)));
    stackPanel->AddChild(text1);
    
    auto text2 = new TextBlock();
    text2->SetText("It's independent and frameless");
    text2->SetFontSize(12);
    text2->SetForeground(new SolidColorBrush(Color(0.2f, 0.2f, 0.2f, 1.0f)));
    stackPanel->AddChild(text2);
    
    border->SetChild(stackPanel);
    
    // 设置内容
    popupRoot.SetContent(border);
    std::cout << "Content set" << std::endl;
    
    // 显示窗口（在屏幕中心）
    popupRoot.Show(Point(100, 100));
    std::cout << "PopupRoot shown" << std::endl;
    
    // 渲染循环（5秒）
    std::cout << "Rendering for 5 seconds..." << std::endl;
    auto startTime = std::chrono::steady_clock::now();
    int frameCount = 0;
    
    while (true) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        if (elapsed >= 5) {
            break;
        }
        
        // 渲染一帧
        popupRoot.RenderFrame();
        frameCount++;
        
        // 处理事件
        glfwPollEvents();
        
        // 控制帧率
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
    
    std::cout << "Total frames rendered: " << frameCount << std::endl;
    
    // 隐藏窗口
    popupRoot.Hide();
    std::cout << "PopupRoot hidden" << std::endl;
    
    // 关闭
    popupRoot.Shutdown();
    std::cout << "PopupRoot shutdown" << std::endl;
    
    // 清理 GLFW
    glfwTerminate();
    std::cout << "GLFW terminated" << std::endl;
    
#else
    std::cout << "GLFW not available, test skipped" << std::endl;
#endif
    
    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}
