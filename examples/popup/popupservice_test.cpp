/**
 * @file popupservice_test.cpp
 * @brief 测试 PopupService 基础功能 (Day 3)
 * 
 * 测试内容：
 * 1. 单例模式验证
 * 2. 注册/注销功能（使用 nullptr 模拟 Popup*）
 * 3. Update/RenderAll 调用（空操作验证）
 * 4. 集成到 Window::ProcessEvents
 */

#include "fk/ui/PopupService.h"
#include "fk/ui/Window.h"
#include <iostream>

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

using namespace fk::ui;

int main() {
    std::cout << "========== PopupService 测试 ==========" << std::endl;
    
#ifdef FK_HAS_GLFW
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    std::cout << "\n=== 测试 1: 单例模式 ===" << std::endl;
    PopupService& service1 = PopupService::Instance();
    PopupService& service2 = PopupService::Instance();
    
    std::cout << "Service1 address: " << &service1 << std::endl;
    std::cout << "Service2 address: " << &service2 << std::endl;
    std::cout << "Same instance: " << (&service1 == &service2 ? "✓ Pass" : "✗ Fail") << std::endl;
    
    std::cout << "\n=== 测试 2: 注册/注销功能 ===" << std::endl;
    std::cout << "Initial active popups count: " << service1.GetActivePopups().size() << std::endl;
    
    // 使用虚拟指针模拟 Popup（实际使用中不应该这样做）
    Popup* dummyPopup1 = reinterpret_cast<Popup*>(0x1000);
    Popup* dummyPopup2 = reinterpret_cast<Popup*>(0x2000);
    
    service1.RegisterPopup(dummyPopup1);
    std::cout << "After register popup1: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    std::cout << "Is popup1 registered: " << (service1.IsRegistered(dummyPopup1) ? "✓ Yes" : "✗ No") << std::endl;
    
    service1.RegisterPopup(dummyPopup2);
    std::cout << "After register popup2: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    
    // 测试重复注册
    service1.RegisterPopup(dummyPopup1);
    std::cout << "After duplicate register popup1: " << service1.GetActivePopups().size() << " popup(s) (should be 2)" << std::endl;
    
    service1.UnregisterPopup(dummyPopup1);
    std::cout << "After unregister popup1: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    std::cout << "Is popup1 still registered: " << (service1.IsRegistered(dummyPopup1) ? "✗ Yes" : "✓ No") << std::endl;
    
    service1.UnregisterPopup(dummyPopup2);
    std::cout << "After unregister popup2: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    
    std::cout << "\n=== 测试 3: Update/RenderAll 调用 ===" << std::endl;
    // 这些方法当前为空实现，只是验证可以调用
    service1.Update();
    std::cout << "Update() called successfully ✓" << std::endl;
    
    service1.RenderAll();
    std::cout << "RenderAll() called successfully ✓" << std::endl;
    
    std::cout << "\n=== 测试 4: 集成到 Window::ProcessEvents ===" << std::endl;
    Window* window = new Window();
    window->SetWidth(400);
    window->SetHeight(300);
    window->SetTitle("PopupService Test Window");
    window->Show();
    
    std::cout << "Processing 5 event loops (PopupService::Update will be called)..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        if (!window->ProcessEvents()) {
            std::cout << "Window closed early" << std::endl;
            break;
        }
        std::cout << "  Frame " << (i + 1) << " processed" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "\n=== 测试 5: CloseAll 功能 ===" << std::endl;
    service1.RegisterPopup(dummyPopup1);
    service1.RegisterPopup(dummyPopup2);
    std::cout << "Before CloseAll: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    
    service1.CloseAll();
    std::cout << "After CloseAll: " << service1.GetActivePopups().size() << " popup(s)" << std::endl;
    
    // 清理
    delete window;
    glfwTerminate();
    
    std::cout << "\n========== 所有测试完成 ==========" << std::endl;
    
    return 0;
#else
    std::cerr << "GLFW is not available" << std::endl;
    return -1;
#endif
}
