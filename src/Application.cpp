#include "Application.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

namespace fk {

Application* Application::instance_ = nullptr;

Application::Application()
    : isRunning_(false) {
    instance_ = this;
}

Application::~Application() {
    if (instance_ == this) instance_ = nullptr;
}

Application* Application::Current() { return instance_; }

void Application::Run() {
    // 如果已经在运行则忽略二次调用
    if (isRunning_) return;
    isRunning_ = true;

    // 触发 Startup 事件，用户可在此初始化资源
    Startup();
    std::cout << "Application started." << std::endl;

    // 简化的主循环：实际框架应集成平台消息泵、计时与渲染
    while (isRunning_) {
        // 模拟每帧操作的延迟
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // 无窗口则自动退出（示例策略）
        if (windows_.empty()) {
            Shutdown();
        }
    }
}

void Application::Shutdown() {
    if (!isRunning_) return;
    isRunning_ = false;
    for (auto& [_, window] : windows_) {
        if (window && window->IsVisible()) {
            window->Hide();
        }
    }
    windows_.clear();
    // 触发 Exit 事件，用户可在此释放资源
    Exit();
    std::cout << "Application exiting." << std::endl;
}

bool Application::IsRunning() const {
    return isRunning_;
}

void Application::AddWindow(const WindowPtr& window, const std::string& name) {
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

void Application::RemoveWindow(const WindowPtr& window) {
    if (!window) return;

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
    if (it == windows_.end()) return;

    if (it->second && it->second->IsVisible()) {
        it->second->Hide();
    }
    windows_.erase(it);
}

WindowPtr Application::GetWindow(const std::string& name) const {
    auto it = windows_.find(name);
    if (it != windows_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace fk
