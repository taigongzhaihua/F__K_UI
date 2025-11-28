#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/Brush.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/animation/VisualStateManager.h"

// 实现部分需要的头文件
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/Shape.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/DoubleAnimation.h"
#include <iostream>

namespace fk::ui {

/**
 * @brief 按钮基类
 * 
 * 职责：
 * - 提供按钮的基本交互行为（点击、悬停、按下）
 * - 管理按钮的通用视觉状态（CommonStates）
 * - 作为 Button、ToggleButton 等控件的基类
 * 
 * WPF 对应：ButtonBase
 */
template<typename Derived>
class ButtonBase : public ContentControl<Derived> {
public:
    ButtonBase();
    virtual ~ButtonBase() = default;

    // ========== 事件 ==========
    
    /// 点击事件
    core::Event<> Click;
    
    // ========== 依赖属性 ==========
    
    /// 鼠标悬停时的背景画刷依赖属性
    static const binding::DependencyProperty& MouseOverBackgroundProperty();
    
    /// 鼠标按下时的背景画刷依赖属性
    static const binding::DependencyProperty& PressedBackgroundProperty();
    
    /// 是否只使用主鼠标按钮(Left)触发 Click 的依赖属性
    static const binding::DependencyProperty& PrimaryClickOnlyProperty();
    
    // ========== 属性访问器（使用宏生成） ==========
    
    /// 鼠标悬停背景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(MouseOverBackground, Brush*, Derived)
    
    /// 鼠标按下背景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(PressedBackground, Brush*, Derived)
    
    /// 是否只使用主鼠标按钮(Left)触发 Click。默认 true。
    FK_PROPERTY_SIMPLE_NO_ACTION(PrimaryClickOnly, bool, Derived)
    
    /// 便捷方法：接受颜色参数的链式调用
    Derived* MouseOverBackground(const Color& color) { 
        SetMouseOverBackground(new SolidColorBrush(color)); 
        return static_cast<Derived*>(this); 
    }
    Derived* PressedBackground(const Color& color) { 
        SetPressedBackground(new SolidColorBrush(color)); 
        return static_cast<Derived*>(this); 
    }
    
    // ========== 视觉状态管理 ==========
    
    /// 初始化视觉状态（public 以便属性变化回调访问）
    void InitializeVisualStates();

protected:
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnPointerEntered(PointerEventArgs& e) override;
    void OnPointerExited(PointerEventArgs& e) override;
    void OnTemplateApplied() override;
    void OnPropertyChanged(const binding::DependencyProperty& property, 
                          const std::any& oldValue, 
                          const std::any& newValue,
                          binding::ValueSource oldSource,
                          binding::ValueSource newSource) override;

    /// 更新视觉状态（根据当前状态切换到相应的视觉状态）
    /// 派生类可以重写此方法以添加额外的状态逻辑
    virtual void UpdateVisualState(bool useTransitions);
    
    /// 处理点击行为（派生类可以重写以自定义行为）
    virtual void OnClick();
    
    /// 按钮是否处于按下状态
    bool IsPressed() const { return isPressed_; }
    
    /// 从模板加载视觉状态（如果模板中定义了状态）
    bool LoadVisualStatesFromTemplate();
    
    /// 解析视觉状态中的TargetName并设置实际的动画目标
    void ResolveVisualStateTargets();

private:
    bool isPressed_;
    MouseButton pressedButton_;
};

// ========== 模板实现 ==========

template<typename Derived>
ButtonBase<Derived>::ButtonBase() : isPressed_(false), pressedButton_(MouseButton::None) {}

// 依赖属性定义需要在 .cpp 中实现（非模板部分）
// 其他成员函数实现
#include "ButtonBase.inl"

} // namespace fk::ui
