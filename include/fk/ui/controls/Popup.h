/**
 * @file Popup.h
 * @brief Popup 控件 - 弹出式内容容器
 * 
 * Popup 提供在独立窗口中显示内容的能力，可以超出父窗口边界显示。
 * 对应 WPF 的 Popup 控件。
 */

#pragma once

#include "fk/ui/base/FrameworkElement.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"
#include <memory>

namespace fk::ui {

// 前向声明
class PopupRoot;

/**
 * @brief Popup 定位模式
 * 
 * 定义 Popup 相对于 PlacementTarget 的位置关系
 */
enum class PlacementMode {
    Absolute,       ///< 绝对屏幕坐标（使用 Offset）
    Relative,       ///< 相对于 Target 左上角 + Offset
    Bottom,         ///< Target 左下角（常用于下拉菜单）
    Top,            ///< Target 左上角上方
    Right,          ///< Target 右上角（常用于工具提示）
    Left,           ///< Target 左上角左侧
    Center,         ///< Target 中心
    Mouse           ///< 当前鼠标位置
};

/**
 * @brief Popup 控件
 * 
 * 职责：
 * - 管理 PopupRoot 生命周期
 * - 计算 Popup 屏幕位置
 * - 处理打开/关闭逻辑
 * - 与 PopupService 集成
 * 
 * 特点：
 * - 使用独立的原生窗口（PopupRoot）显示内容
 * - 可以超出父窗口边界
 * - 支持多种定位模式
 * - 支持外部点击自动关闭（StaysOpen=false）
 */
class Popup : public FrameworkElement<Popup> {
public:
    // ========== 依赖属性 ==========
    
    /// IsOpen 属性：是否打开
    static const binding::DependencyProperty& IsOpenProperty();
    
    /// Child 属性：弹出内容
    static const binding::DependencyProperty& ChildProperty();
    
    /// PlacementTarget 属性：定位参考元素
    static const binding::DependencyProperty& PlacementTargetProperty();
    
    /// Placement 属性：定位模式
    static const binding::DependencyProperty& PlacementProperty();
    
    /// HorizontalOffset 属性：水平偏移
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /// VerticalOffset 属性：垂直偏移
    static const binding::DependencyProperty& VerticalOffsetProperty();
    
    /// StaysOpen 属性：点击外部是否保持打开
    static const binding::DependencyProperty& StaysOpenProperty();

public:
    Popup();
    virtual ~Popup();

    // ========== IsOpen 属性 ==========
    
    bool GetIsOpen() const { return GetValue<bool>(IsOpenProperty()); }
    void SetIsOpen(bool value) { SetValue(IsOpenProperty(), value); }
    
    Popup* IsOpen(bool value) {
        SetIsOpen(value);
        return this;
    }

    // ========== Child 属性 ==========
    
    UIElement* GetChild() const { return GetValue<UIElement*>(ChildProperty()); }
    void SetChild(UIElement* child) { SetValue(ChildProperty(), child); }
    
    Popup* Child(UIElement* child) {
        SetChild(child);
        return this;
    }

    // ========== PlacementTarget 属性 ==========
    
    UIElement* GetPlacementTarget() const { return GetValue<UIElement*>(PlacementTargetProperty()); }
    void SetPlacementTarget(UIElement* target) { SetValue(PlacementTargetProperty(), target); }
    
    Popup* PlacementTarget(UIElement* target) {
        SetPlacementTarget(target);
        return this;
    }

    // ========== Placement 属性 ==========
    
    PlacementMode GetPlacement() const { return GetValue<PlacementMode>(PlacementProperty()); }
    void SetPlacement(PlacementMode mode) { SetValue(PlacementProperty(), mode); }
    
    Popup* Placement(PlacementMode mode) {
        SetPlacement(mode);
        return this;
    }

    // ========== Offset 属性 ==========
    
    float GetHorizontalOffset() const { return GetValue<float>(HorizontalOffsetProperty()); }
    void SetHorizontalOffset(float value) { SetValue(HorizontalOffsetProperty(), value); }
    
    Popup* HorizontalOffset(float value) {
        SetHorizontalOffset(value);
        return this;
    }
    
    float GetVerticalOffset() const { return GetValue<float>(VerticalOffsetProperty()); }
    void SetVerticalOffset(float value) { SetValue(VerticalOffsetProperty(), value); }
    
    Popup* VerticalOffset(float value) {
        SetVerticalOffset(value);
        return this;
    }

    // ========== StaysOpen 属性 ==========
    
    bool GetStaysOpen() const { return GetValue<bool>(StaysOpenProperty()); }
    void SetStaysOpen(bool value) { SetValue(StaysOpenProperty(), value); }
    
    Popup* StaysOpen(bool value) {
        SetStaysOpen(value);
        return this;
    }
    
    // 注：Width/Height 继承自 FrameworkElement<Popup>

    // ========== 事件 ==========
    
    core::Event<> Opened;   ///< Popup 打开时触发
    core::Event<> Closed;   ///< Popup 关闭时触发

    // ========== 核心方法 ==========
    
    /**
     * @brief 打开 Popup
     * 
     * 创建 PopupRoot（如果需要），计算位置，显示窗口，注册到 PopupService
     */
    void Open();
    
    /**
     * @brief 关闭 Popup
     * 
     * 隐藏窗口，从 PopupService 注销
     */
    void Close();
    
    /**
     * @brief 获取 PopupRoot 实例
     * @return PopupRoot 指针，如果未创建返回 nullptr
     */
    PopupRoot* GetPopupRoot() const { return popupRoot_.get(); }

    // ========== 内部方法（供 PopupService 调用） ==========
    
    /**
     * @brief 渲染 Popup 内容
     * 
     * 由 PopupService::RenderAll() 调用
     */
    void RenderPopup();
    
    /**
     * @brief 更新 Popup 状态
     * 
     * 由 PopupService::Update() 调用
     * 处理：位置更新、外部点击检测等
     */
    void UpdatePopup();

protected:
    /**
     * @brief 计算 Popup 在屏幕上的位置
     * @return 屏幕坐标点
     */
    Point CalculateScreenPosition();
    
    /**
     * @brief 应用边界检测，确保 Popup 不超出屏幕
     * @param position 原始位置
     * @param popupWidth Popup 宽度
     * @param popupHeight Popup 高度
     * @return 调整后的位置
     */
    Point ApplyBoundaryConstraints(Point position, float popupWidth, float popupHeight);
    
    /**
     * @brief 获取屏幕工作区域
     * @return 屏幕工作区矩形
     */
    Rect GetScreenWorkArea();
    
    /**
     * @brief 获取鼠标的屏幕坐标位置
     * @return 鼠标屏幕坐标
     */
    Point GetMouseScreenPosition();

private:
    // 依赖属性变更回调
    static void OnIsOpenChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnChildChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );

    std::unique_ptr<PopupRoot> popupRoot_;  ///< 独立窗口实例
    bool isPopupRootCreated_{false};        ///< PopupRoot 是否已创建
};

} // namespace fk::ui
