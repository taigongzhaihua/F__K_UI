/**
 * @file main.cpp
 * @brief Hello F__K_UI - 独立示例项目
 * 
 * 本项目演示如何将 F__K_UI 作为外部库引用到独立项目中
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"

#include <iostream>
#include <memory>

int main() {
    using namespace fk;
    
    try {
        std::cout << "=== Hello F__K_UI Sample ===" << std::endl;
        std::cout << "This is an independent project using F__K_UI framework" << std::endl;
        
        // 创建应用程序
        Application app;
        
        // 订阅应用程序事件
        app.Startup += [] {
            std::cout << "Application started!" << std::endl;
        };
        
        app.Exit += [] {
            std::cout << "Application exiting..." << std::endl;
        };
        
        // 创建主窗口
        auto mainWindow = ui::window()
            ->Title("Hello F__K_UI!")
            ->Width(600)
            ->Height(400);
        
        // 创建 UI 布局
        auto panel = ui::stackPanel()
            ->Orientation(ui::Orientation::Vertical)
            ->Spacing(15.0f);
        
        // 标题文本
        auto titleText = ui::textBlock()
            ->Text("Welcome to F__K_UI!")
            ->FontSize(28.0f)
            ->Foreground("#2C3E50");
        
        // 描述文本
        auto descText = ui::textBlock()
            ->Text("This is an independent sample project")
            ->FontSize(16.0f)
            ->Foreground("#7F8C8D");
        
        // 计数器
        int clickCount = 0;
        auto counterText = ui::textBlock()
            ->Text("Button clicks: 0")
            ->FontSize(18.0f)
            ->Foreground("#34495E");
        
        // 点击按钮
        auto clickButton = ui::button()
            ->Content("Click Me!")
            ->Background("#3498DB")
            ->HoveredBackground("#2980B9")
            ->PressedBackground("#1F5F8B")
            ->CornerRadius(5.0f)
            ->Width(150.0f)
            ->Height(40.0f);
        
        clickButton->Click += [counterText, &clickCount](ui::detail::ButtonBase& btn) {
            clickCount++;
            counterText->SetText("Button clicks: " + std::to_string(clickCount));
            std::cout << "Button clicked " << clickCount << " times!" << std::endl;
            std::cout << "  IsMouseOver: " << btn.IsMouseOver() << std::endl;
            std::cout << "  IsPressed: " << btn.IsPressed() << std::endl;
        };
        
        // 退出按钮
        auto exitButton = ui::button()
            ->Content("Exit Application")
            ->Background("#E74C3C")
            ->HoveredBackground("#C0392B")
            ->PressedBackground("#A93226")
            ->CornerRadius(5.0f)
            ->Width(150.0f)
            ->Height(40.0f);
        
        exitButton->Click += [](ui::detail::ButtonBase& btn) {
            std::cout << "Exit button clicked!" << std::endl;
            std::cout << "  IsMouseOver: " << btn.IsMouseOver() << std::endl;
            std::cout << "  IsPressed: " << btn.IsPressed() << std::endl;
            Application::Current()->Shutdown();
        };
        
        // 添加所有元素到面板
        panel->AddChild(titleText);
        panel->AddChild(descText);
        panel->AddChild(counterText);
        panel->AddChild(clickButton);
        panel->AddChild(exitButton);
        
        // 设置窗口内容
        mainWindow->Content(panel);
        
        // 窗口关闭事件
        mainWindow->Closed += [] {
            std::cout << "Window closed!" << std::endl;
        };
        
        // 运行应用程序
        app.Run(mainWindow);
        
        std::cout << "Application finished successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
