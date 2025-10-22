#pragma once

#include "../include/Event.h"
#include "Window.h"

namespace fk {

/**
 * @brief 应用程序主类（极简实现）
 *
 * 负责应用程序生命周期管理（Run / Shutdown）、全局事件（Startup/Exit）以及窗口容器的管理。
 * 当前实现为演示用途：Run 实现了一个简单的循环并在没有窗口时自动调用 Shutdown。
 * 真正的框架应在 Run 中集成平台消息泵和渲染循环。
 */
class Application {
public:
    /**
     * @brief 构造函数，会设置当前单例实例（Application::Current() 返回）
     */
    Application();

    /**
     * @brief 析构函数，会清理单例引用
     */
    virtual ~Application();

    /**
     * @brief 获取当前应用实例（单例访问）
     * @return 当前 Application 指针或 nullptr
     */
    static Application* Current();

    /**
     * @brief 启动应用主循环（阻塞调用，直到 Shutdown 被触发）
     */
    void Run();

    /**
     * @brief 请求关闭应用程序，触发 Exit 事件
     */
    void Shutdown();

    /**
     * @brief 将窗口添加到应用的窗口集合并自动显示（示例行为）
     * @param w 要添加的窗口智能指针
     */
    void AddWindow(const WindowPtr& w);

    /**
     * @brief 从集合中移除窗口（不会自动销毁智能指针）
     * @param w 要移除的窗口智能指针
     */
    void RemoveWindow(const WindowPtr& w);

    // 全局事件
    Event<> Startup;    /**< 应用启动时触发 */
    Event<> Exit;       /**< 应用退出时触发 */
    Event<> Activated;  /**< 应用激活事件（占位） */
    Event<> Deactivated;/**< 应用失活事件（占位） */

    /**
     * @brief 获取当前注册的窗口列表（只读）
     */
    const std::vector<WindowPtr>& Windows() const { return windows_; }

private:
    static Application* instance_; /**< 单例指针 */
    std::vector<WindowPtr> windows_; /**< 管理的窗口集合 */
    bool isRunning_;                 /**< 主循环运行标记 */
};

} // namespace fk
