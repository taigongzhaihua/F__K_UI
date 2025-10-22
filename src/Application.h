#pragma once

#include "../include/Event.h"
#include "Window.h"

#include <string>
#include <unordered_map>

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
     * @brief 查询应用是否处于运行状态
     */
    bool IsRunning() const;

    /**
     * @brief 将窗口添加到应用的窗口集合并自动显示（示例行为）
     * @param window 要添加的窗口智能指针
     * @param name 用于索引窗口的唯一名称
     */
    void AddWindow(const WindowPtr& window, const std::string& name);

    /**
     * @brief 从集合中移除窗口（不会自动销毁智能指针）
     * @param window 要移除的窗口智能指针
     */
    void RemoveWindow(const WindowPtr& window);

    /**
     * @brief 通过名称移除窗口
     * @param name 要移除的窗口名称
     */
    void RemoveWindow(const std::string& name);

    // 全局事件
    Event<> Startup;    /**< 应用启动时触发 */
    Event<> Exit;       /**< 应用退出时触发 */
    Event<> Activated;  /**< 应用激活事件（占位） */
    Event<> Deactivated;/**< 应用失活事件（占位） */

    /**
     * @brief 获取当前注册的窗口列表（只读）
     */
    /**
     * @brief 获取当前注册的窗口映射（名称 -> 窗口）
     */
    const std::unordered_map<std::string, WindowPtr>& Windows() const { return windows_; }

    /**
     * @brief 根据名称查找窗口
     * @param name 窗口名称
     * @return 找到则返回对应窗口指针，否则返回 nullptr
     */
    WindowPtr GetWindow(const std::string& name) const;

private:
    static Application* instance_; /**< 单例指针 */
    std::unordered_map<std::string, WindowPtr> windows_; /**< 管理的窗口映射 */
    bool isRunning_;                 /**< 主循环运行标记 */
};

} // namespace fk
