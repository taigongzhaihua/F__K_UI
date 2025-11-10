#pragma once

#include <string>
#include <unordered_map>

#include "fk/core/Event.h"
#include "fk/ui/Window.h"

namespace fk {

class Application {
public:
    Application();
    virtual ~Application();

    static Application* Current();

    /**
     * @brief 创建窗口（由 Application 管理生命周期）
     * @return 返回原始指针供使用，窗口所有权由 Application 管理
     */
    ui::Window* CreateWindow();
    
    /**
     * @brief 运行消息循环，直到主窗口关闭
     * @param mainWindow 主窗口
     */
    void Run(ui::WindowPtr mainWindow);
    
    /**
     * @brief 关闭应用程序
     */
    void Shutdown();

    bool IsRunning() const;

    void AddWindow(const ui::WindowPtr& window, const std::string& name);
    void RemoveWindow(const ui::WindowPtr& window);
    void RemoveWindow(const std::string& name);

    core::Event<> Startup;
    core::Event<> Exit;
    core::Event<> Activated;
    core::Event<> Deactivated;

    const std::unordered_map<std::string, ui::WindowPtr>& Windows() const { return windows_; }
    ui::WindowPtr GetWindow(const std::string& name) const;
    ui::WindowPtr GetMainWindow() const { return mainWindow_; }

private:
    static Application* instance_;
    std::unordered_map<std::string, ui::WindowPtr> windows_;
    ui::WindowPtr mainWindow_;
    bool isRunning_;
};

} // namespace fk
