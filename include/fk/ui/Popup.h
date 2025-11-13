/**
 * @file Popup.h
 * @brief Popup 弹出窗口控件
 * 
 * 职责：
 * - 提供弹出式内容容器
 * - 支持打开/关闭状态管理
 * - 弹出位置计算和定位
 * - 外部点击自动关闭
 * 
 * WPF 对应：Popup
 */

#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/core/Event.h"

namespace fk::ui {

/**
 * @brief Popup弹出位置模式
 */
enum class PlacementMode {
    Absolute,       // 绝对位置（相对于屏幕）
    Relative,       // 相对于PlacementTarget
    Bottom,         // 在PlacementTarget下方
    Top,            // 在PlacementTarget上方
    Left,           // 在PlacementTarget左侧
    Right,          // 在PlacementTarget右侧
    Center,         // 在PlacementTarget中心
    Mouse           // 在鼠标位置
};

/**
 * @brief Popup 弹出窗口控件
 * 
 * 提供浮动内容的容器，可以在其他UI元素之上显示内容。
 * 常用于实现下拉菜单、工具提示、上下文菜单等。
 */
template<typename Derived = void>
class Popup : public FrameworkElement<typename std::conditional<std::is_void_v<Derived>, Popup<>, Derived>::type> {
private:
    using Base = FrameworkElement<typename std::conditional<std::is_void_v<Derived>, Popup<>, Derived>::type>;
    using ActualDerived = typename std::conditional<std::is_void_v<Derived>, Popup<>, Derived>::type;
    
public:
    Popup() : Base() {
        // 默认不可见
        this->SetVisibility(Visibility::Collapsed);
    }
    
    virtual ~Popup() = default;
    
    // ========== 依赖属性 ==========
    
    /// IsOpen 属性：弹出窗口是否打开
    static const binding::DependencyProperty& IsOpenProperty();
    
    /// Child 属性：弹出窗口的子内容
    static const binding::DependencyProperty& ChildProperty();
    
    /// PlacementTarget 属性：定位目标元素
    static const binding::DependencyProperty& PlacementTargetProperty();
    
    /// Placement 属性：弹出位置模式
    static const binding::DependencyProperty& PlacementProperty();
    
    /// HorizontalOffset 属性：水平偏移
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /// VerticalOffset 属性：垂直偏移
    static const binding::DependencyProperty& VerticalOffsetProperty();
    
    /// StaysOpen 属性：是否保持打开（false时点击外部自动关闭）
    static const binding::DependencyProperty& StaysOpenProperty();
    
    // ========== IsOpen 属性 ==========
    
    bool GetIsOpen() const {
        return this->template GetValue<bool>(IsOpenProperty());
    }
    
    void SetIsOpen(bool value) {
        this->SetValue(IsOpenProperty(), value);
        UpdateVisibility();
    }
    
    ActualDerived* IsOpen(bool value) {
        SetIsOpen(value);
        return static_cast<ActualDerived*>(this);
    }
    bool IsOpen() const { return GetIsOpen(); }
    
    // ========== Child 属性 ==========
    
    UIElement* GetChild() const {
        return this->template GetValue<UIElement*>(ChildProperty());
    }
    
    void SetChild(UIElement* value) {
        this->SetValue(ChildProperty(), value);
    }
    
    ActualDerived* Child(UIElement* value) {
        SetChild(value);
        return static_cast<ActualDerived*>(this);
    }
    UIElement* Child() const { return GetChild(); }
    
    // ========== PlacementTarget 属性 ==========
    
    UIElement* GetPlacementTarget() const {
        return this->template GetValue<UIElement*>(PlacementTargetProperty());
    }
    
    void SetPlacementTarget(UIElement* value) {
        this->SetValue(PlacementTargetProperty(), value);
    }
    
    ActualDerived* PlacementTarget(UIElement* value) {
        SetPlacementTarget(value);
        return static_cast<ActualDerived*>(this);
    }
    UIElement* PlacementTarget() const { return GetPlacementTarget(); }
    
    // ========== Placement 属性 ==========
    
    PlacementMode GetPlacement() const {
        return this->template GetValue<PlacementMode>(PlacementProperty());
    }
    
    void SetPlacement(PlacementMode value) {
        this->SetValue(PlacementProperty(), value);
    }
    
    ActualDerived* Placement(PlacementMode value) {
        SetPlacement(value);
        return static_cast<ActualDerived*>(this);
    }
    PlacementMode Placement() const { return GetPlacement(); }
    
    // ========== HorizontalOffset 属性 ==========
    
    double GetHorizontalOffset() const {
        return this->template GetValue<double>(HorizontalOffsetProperty());
    }
    
    void SetHorizontalOffset(double value) {
        this->SetValue(HorizontalOffsetProperty(), value);
    }
    
    ActualDerived* HorizontalOffset(double value) {
        SetHorizontalOffset(value);
        return static_cast<ActualDerived*>(this);
    }
    double HorizontalOffset() const { return GetHorizontalOffset(); }
    
    // ========== VerticalOffset 属性 ==========
    
    double GetVerticalOffset() const {
        return this->template GetValue<double>(VerticalOffsetProperty());
    }
    
    void SetVerticalOffset(double value) {
        this->SetValue(VerticalOffsetProperty(), value);
    }
    
    ActualDerived* VerticalOffset(double value) {
        SetVerticalOffset(value);
        return static_cast<ActualDerived*>(this);
    }
    double VerticalOffset() const { return GetVerticalOffset(); }
    
    // ========== StaysOpen 属性 ==========
    
    bool GetStaysOpen() const {
        return this->template GetValue<bool>(StaysOpenProperty());
    }
    
    void SetStaysOpen(bool value) {
        this->SetValue(StaysOpenProperty(), value);
    }
    
    ActualDerived* StaysOpen(bool value) {
        SetStaysOpen(value);
        return static_cast<ActualDerived*>(this);
    }
    bool StaysOpen() const { return GetStaysOpen(); }
    
    // ========== 事件 ==========
    
    /// Opened 事件：弹出窗口打开时触发
    core::Event<> Opened;
    
    /// Closed 事件：弹出窗口关闭时触发
    core::Event<> Closed;
    
protected:
    /**
     * @brief 更新可见性
     */
    void UpdateVisibility() {
        if (GetIsOpen()) {
            this->SetVisibility(Visibility::Visible);
            CalculatePosition();
            Opened();
        } else {
            this->SetVisibility(Visibility::Collapsed);
            Closed();
        }
    }
    
    /**
     * @brief 计算弹出位置
     */
    virtual void CalculatePosition() {
        // TODO: 根据Placement模式计算实际位置
        // 需要获取PlacementTarget的位置和大小
        // 应用HorizontalOffset和VerticalOffset
    }
};

// ========== 依赖属性实现 ==========

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::IsOpenProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "IsOpen",
        typeid(bool),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{false}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::ChildProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Child",
        typeid(UIElement*),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{static_cast<UIElement*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::PlacementTargetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "PlacementTarget",
        typeid(UIElement*),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{static_cast<UIElement*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::PlacementProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Placement",
        typeid(PlacementMode),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{PlacementMode::Bottom}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::HorizontalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "HorizontalOffset",
        typeid(double),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{0.0}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::VerticalOffsetProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "VerticalOffset",
        typeid(double),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{0.0}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Popup<Derived>::StaysOpenProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "StaysOpen",
        typeid(bool),
        typeid(Popup<Derived>),
        binding::PropertyMetadata{true}
    );
    return property;
}

} // namespace fk::ui
