/**
 * @file PopupService.h
 * @brief 全局 Popup 管理服务
 * 
 * PopupService 负责管理所有活跃的 Popup 实例，协调它们的生命周期、
 * 事件处理和渲染。使用单例模式确保全局唯一。
 */

#pragma once

#include <vector>
#include <memory>

namespace fk::ui {

// 前向声明
class Popup;
class PopupRoot;

/**
 * @brief Popup 全局管理服务（单例）
 * 
 * 职责：
 * - 注册/注销活跃的 Popup 实例
 * - 协调 Popup 的事件处理（如点击外部关闭）
 * - 管理 Popup 的 Z-order（多个 Popup 重叠时的层级）
 * - 提供统一的更新/渲染接口
 */
class PopupService {
public:
    /**
     * @brief 获取 PopupService 单例
     */
    static PopupService& Instance();

    // 禁止拷贝和赋值
    PopupService(const PopupService&) = delete;
    PopupService& operator=(const PopupService&) = delete;

    /**
     * @brief 注册一个 Popup 实例
     * @param popup 要注册的 Popup 指针
     * 
     * 当 Popup::Open() 被调用时应该注册到服务中
     */
    void RegisterPopup(Popup* popup);

    /**
     * @brief 注销一个 Popup 实例
     * @param popup 要注销的 Popup 指针
     * 
     * 当 Popup::Close() 被调用或 Popup 被销毁时应该注销
     */
    void UnregisterPopup(Popup* popup);

    /**
     * @brief 检查指定 Popup 是否已注册
     * @param popup 要检查的 Popup 指针
     * @return true 如果已注册，false 否则
     */
    bool IsRegistered(const Popup* popup) const;

    /**
     * @brief 获取所有活跃的 Popup 列表
     * @return Popup 指针列表
     */
    const std::vector<Popup*>& GetActivePopups() const { return activePopups_; }

    /**
     * @brief 更新所有活跃 Popup 的状态
     * 
     * 应在主窗口的 ProcessEvents 中调用，用于：
     * - 处理 StaysOpen=false 的 Popup 的外部点击检测
     * - 更新 Popup 位置（如果 PlacementTarget 移动）
     * - 处理窗口失焦等事件
     */
    void Update();

    /**
     * @brief 渲染所有活跃的 Popup
     * 
     * 遍历所有 Popup，调用它们关联的 PopupRoot::RenderFrame()
     * 注意：PopupRoot 有独立的 GLFW 窗口和渲染上下文，
     * 因此不需要在主窗口的渲染管线中执行
     */
    void RenderAll();

    /**
     * @brief 关闭所有 Popup
     * 
     * 用于清理场景，如应用程序关闭时
     */
    void CloseAll();
    
    /**
     * @brief 处理全局鼠标按下事件
     * @param screenX 屏幕 X 坐标
     * @param screenY 屏幕 Y 坐标
     * 
     * 用于实现 StaysOpen=false 的 Popup 点击外部关闭逻辑。
     * 遍历所有活跃的 Popup，检查点击位置是否在 PopupRoot 窗口内：
     * - 如果在窗口内，不做处理（由 PopupRoot 的 InputManager 处理）
     * - 如果在窗口外且 StaysOpen=false，关闭该 Popup
     * 
     * 应在主窗口的鼠标按下事件中调用。
     */
    void HandleGlobalMouseDown(int screenX, int screenY);

private:
    PopupService() = default;
    ~PopupService() = default;

    std::vector<Popup*> activePopups_; ///< 当前活跃的 Popup 列表
};

} // namespace fk::ui
