#pragma once

#include "../include/Event.h"

namespace fk {

/**
 * @brief 表示应用程序窗口的基础类（极简实现）
 *
 * 该类提供窗口的基本属性（标题、宽度、高度、可见性）以及常见事件（打开、关闭、大小改变等）。
 * 注意：此实现为演示用，未集成任何平台窗口系统（Win32/GLFW 等），仅用于内部逻辑与事件流演示。
 */
class Window : public std::enable_shared_from_this<Window> {
public:
    /**
     * @brief 构造函数，设置默认标题和尺寸
     */
    Window();

    /**
     * @brief 析构函数
     */
    virtual ~Window();

    // ---------- 属性访问器 ----------
    /**
     * @brief 设置窗口标题
     * @param t 新标题
     */
    WindowPtr Title(const std::string& t);

    /**
     * @brief 获取窗口标题
     * @return 当前标题字符串
     */
    std::string Title() const;

    /**
     * @brief 设置窗口宽度（像素）并触发 Resized 事件
     * @param w 新宽度
     */
    WindowPtr Width(int w);

    /**
     * @brief 获取窗口宽度
     * @return 当前宽度
     */
    int Width() const;

    /**
     * @brief 设置窗口高度（像素）并触发 Resized 事件
     * @param h 新高度
     */
    WindowPtr Height(int h);

    /**
     * @brief 获取窗口高度
     * @return 当前高度
     */
    int Height() const;

    /**
     * @brief 显示窗口（示例实现：标记为可见并触发 Opened 事件）
     */
    void Show();

    /**
     * @brief 隐藏窗口（示例实现：标记为不可见并触发 Closed 事件）
     */
    void Hide();

    /**
     * @brief 查询窗口是否可见
     * @return true 表示可见
     */
    bool IsVisible() const;

    // ---------- 事件（订阅/触发） ----------
    /**
     * @brief 窗口关闭事件（当 Hide/Close 触发时调用）
     */
    Event<> Closed;

    /**
     * @brief 窗口打开事件（当 Show/Opened 触发时调用）
     */
    Event<> Opened;

    /**
     * @brief 窗口尺寸改变事件，回调参数为 (width, height)
     */
    Event<int,int> Resized;

    /**
     * @brief 窗口激活事件（占位）
     */
    Event<> Activated;

    /**
     * @brief 窗口失去激活事件（占位）
     */
    Event<> Deactivated;

private:
    // 私有字段：存储窗口状态
    std::string title_; /**< 窗口标题 */
    int width_;         /**< 窗口宽度（像素） */
    int height_;        /**< 窗口高度（像素） */
    bool visible_;      /**< 可见性标记 */
};

inline WindowPtr window(){
    return std::make_shared<Window>();}

} // namespace fk
