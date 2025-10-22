#include "Application.h"
#include <iostream>
#include <thread>
#include <chrono>

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
    // 触发 Exit 事件，用户可在此释放资源
    Exit();
    std::cout << "Application exiting." << std::endl;
}

void Application::AddWindow(const WindowPtr& w) {
    if (!w) return;
    windows_.push_back(w);
    // 演示策略：添加后自动显示窗口并触发相应事件
    w->Show();
}

void Application::RemoveWindow(const WindowPtr& w) {
    if (!w) return;
    auto it = std::find(windows_.begin(), windows_.end(), w);
    if (it != windows_.end()) {
        windows_.erase(it);
    }
}

} // namespace fk
