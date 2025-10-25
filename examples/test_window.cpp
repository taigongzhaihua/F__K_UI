#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include <iostream>

using namespace fk;

int main() {
    std::cout << "=== F__K_UI Window Class Test ===" << std::endl;
    std::cout << std::endl;

    // 1. 创建窗口
    std::cout << "1. Creating window..." << std::endl;
    auto window = ui::window()
        ->Title("Test Window")
        ->Width(800)
        ->Height(600);
    
    std::cout << "   Title: " << window->Title() << std::endl;
    std::cout << "   Size: " << window->Width() << "x" << window->Height() << std::endl;
    std::cout << std::endl;

    // 2. 创建内容 (使用流式API)
    std::cout << "2. Creating content..." << std::endl;
    auto stackPanel = std::make_shared<ui::StackPanel>();
    stackPanel->Orientation(ui::Orientation::Vertical);
    
    for (int i = 0; i < 3; ++i) {
        auto content = std::make_shared<ui::ContentControl>();
        stackPanel->AddChild(content);
    }
    
    window->SetContent(stackPanel);
    std::cout << "   Content set: StackPanel with " << stackPanel->GetChildren().size() << " children" << std::endl;
    std::cout << std::endl;

    // 3. 订阅事件
    std::cout << "3. Subscribing to events..." << std::endl;
    
    window->Opened += []() {
        std::cout << "   [Event] Window opened!" << std::endl;
    };
    
    window->Closed += []() {
        std::cout << "   [Event] Window closed!" << std::endl;
    };
    
    window->Resized += [](int w, int h) {
        std::cout << "   [Event] Window resized to " << w << "x" << h << std::endl;
    };
    
    std::cout << "   Events subscribed" << std::endl;
    std::cout << std::endl;

    // 4. 显示窗口
    std::cout << "4. Showing window..." << std::endl;
    window->Show();
    std::cout << "   Window is visible: " << (window->IsVisible() ? "yes" : "no") << std::endl;
    std::cout << "   Native handle: " << window->GetNativeHandle() << std::endl;
    std::cout << std::endl;

    // 5. 测试窗口属性修改
    std::cout << "5. Testing property changes..." << std::endl;
    window->Title("Modified Title");
    window->Width(1024)->Height(768);
    std::cout << "   New title: " << window->Title() << std::endl;
    std::cout << "   New size: " << window->Width() << "x" << window->Height() << std::endl;
    std::cout << std::endl;

    // 6. 等待一段时间
    std::cout << "6. Window will remain visible for a moment..." << std::endl;
    std::cout << "   (Window lifecycle managed by GLFW)" << std::endl;
    
    // 由于我们没有消息循环，这里只是演示 Window 类的功能
    // 在实际应用中，需要 glfwPollEvents() 循环

    std::cout << std::endl;
    std::cout << "=== Test Complete ===" << std::endl;
    std::cout << "Note: In a real app, you'd need a message loop (glfwPollEvents)" << std::endl;

    return 0;
}
