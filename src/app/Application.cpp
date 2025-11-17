#include "fk/app/Application.h"
#include "fk/animation/AnimationManager.h"

#ifdef FK_HAS_GLFW
#include <GLFW/glfw3.h>
#endif

#include <iostream>

namespace fk {

Application* Application::instance_ = nullptr;

Application::Application()
    : isRunning_(false) {
    instance_ = this;
}

Application::~Application() {
    if (instance_ == this) {
        instance_ = nullptr;
    }
}

Application* Application::Current() { return instance_; }

ui::Window* Application::CreateWindow() {
    auto window = std::make_shared<ui::Window>();
    auto* ptr = window.get();
    
    // 自动添加到窗口集合中，使用指针地址作为唯一名称
    std::string name = "Window_" + std::to_string(reinterpret_cast<uintptr_t>(ptr));
    windows_[name] = window;
    
    return ptr;
}

void Application::Run(ui::WindowPtr mainWindow) {
    if (isRunning_) {
        return;
    }
    
    if (!mainWindow) {
        std::cerr << "Cannot run application without a main window" << std::endl;
        return;
    }
    
    mainWindow_ = mainWindow;
    isRunning_ = true;

    Startup();
    std::cout << "Application started." << std::endl;

    // 显示主窗口
    mainWindow_->Show();
    
    // 消息循环
    std::cout << "Starting message loop..." << std::endl;
    
    auto lastFrameTime = std::chrono::steady_clock::now();
    
    while (isRunning_) {
#ifdef FK_HAS_GLFW
        glfwPollEvents();
#endif
        // 计算帧时间
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
        lastFrameTime = currentTime;
        
        // 更新所有活动动画
        animation::AnimationManager::Instance().Update(deltaTime);
        
        // 处理所有窗口的消息
        if (!mainWindow_->ProcessEvents()) {
            // 主窗口关闭，退出应用
            break;
        }
        
        // 渲染主窗口 (RenderFrame 内部会在需要时调用 glfwSwapBuffers)
        mainWindow_->RenderFrame();
    }
    
    std::cout << "Message loop ended" << std::endl;
    Shutdown();
}

void Application::Shutdown() {
    if (!isRunning_) {
        return;
    }
    isRunning_ = false;
    
    for (auto& [_, window] : windows_) {
        if (window && window->IsVisible()) {
            window->Hide();
        }
    }
    windows_.clear();
    mainWindow_.reset();
    
    Exit();
    std::cout << "Application exiting." << std::endl;
}

bool Application::IsRunning() const {
    return isRunning_;
}

void Application::AddWindow(const ui::WindowPtr& window, const std::string& name) {
    if (!window) {
        std::cerr << "AddWindow: window pointer is null" << std::endl;
        return;
    }

    auto [it, inserted] = windows_.emplace(name, window);
    if (!inserted) {
        std::cerr << "AddWindow: window name '" << name << "' already exists, replacing existing window" << std::endl;
        it->second = window;
    }

    if (!window->IsVisible()) {
        window->Show();
    }
}

void Application::RemoveWindow(const ui::WindowPtr& window) {
    if (!window) {
        return;
    }

    for (auto it = windows_.begin(); it != windows_.end(); ++it) {
        if (it->second == window) {
            if (window->IsVisible()) {
                window->Hide();
            }
            windows_.erase(it);
            break;
        }
    }
}

void Application::RemoveWindow(const std::string& name) {
    auto it = windows_.find(name);
    if (it == windows_.end()) {
        return;
    }

    if (it->second && it->second->IsVisible()) {
        it->second->Hide();
    }
    windows_.erase(it);
}

ui::WindowPtr Application::GetWindow(const std::string& name) const {
    auto it = windows_.find(name);
    if (it != windows_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace fk
