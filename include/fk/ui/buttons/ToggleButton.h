#pragma once

#include "fk/ui/buttons/ButtonBase.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/ui/graphics/Brush.h"
#include <optional>

namespace fk::ui {

/**
 * @brief 切换按钮控件
 * 
 * 职责：
 * - 继承自 ButtonBase，响应点击事件
 * - 提供可切换的状态（IsChecked: true/false/null）
 * - 支持三态模式（IsThreeState）
 * 
 * WPF 对应：ToggleButton
 */
class ToggleButton : public ButtonBase<ToggleButton> {
public:
    ToggleButton();
    virtual ~ToggleButton() = default;

    // ========== 事件 ==========
    
    /// IsChecked 属性改变时触发
    core::Event<std::optional<bool>> Checked;
    core::Event<std::optional<bool>> Unchecked;
    core::Event<std::optional<bool>> Indeterminate;
    
    // ========== 依赖属性 ==========
    
    /// IsChecked 依赖属性
    static const binding::DependencyProperty& IsCheckedProperty();
    
    /// IsThreeState 依赖属性
    static const binding::DependencyProperty& IsThreeStateProperty();
    
    /// Checked 状态背景画刷依赖属性
    static const binding::DependencyProperty& CheckedBackgroundProperty();
    
    /// Checked 状态前景画刷依赖属性
    static const binding::DependencyProperty& CheckedForegroundProperty();
    
    // ========== 属性访问器（使用宏生成） ==========
    
    /// IsChecked 属性（Get/Set/链式调用/绑定）
    /// 三态：true (checked), false (unchecked), nullopt (indeterminate)
    FK_PROPERTY_VISUAL(IsChecked, std::optional<bool>, ToggleButton)
    
    /// IsThreeState 属性（Get/Set/链式调用/绑定）
    /// 为 true 时允许 Indeterminate 状态
    FK_PROPERTY_SIMPLE(IsThreeState, bool, ToggleButton, {})
    
    /// Checked 状态背景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(CheckedBackground, Brush*, ToggleButton)
    
    /// Checked 状态前景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(CheckedForeground, Brush*, ToggleButton)
    
    /// 便捷方法：接受颜色参数的链式调用
    ToggleButton* CheckedBackground(const Color& color) { 
        SetCheckedBackground(new SolidColorBrush(color)); 
        return this; 
    }
    ToggleButton* CheckedForeground(const Color& color) { 
        SetCheckedForeground(new SolidColorBrush(color)); 
        return this; 
    }
    
    /// 切换状态（在 unchecked/checked/indeterminate 之间循环）
    virtual void Toggle();
    
    // ========== 视觉状态管理 ==========
    
    /// 初始化 ToggleButton 的视觉状态
    void InitializeToggleVisualStates();
    
protected:
    void OnPointerReleased(PointerEventArgs& e) override;
    void OnClick() override;
    
    /// 重写基类方法以同时更新 CommonStates 和 CheckStates
    void UpdateVisualState(bool useTransitions) override;
    void OnTemplateApplied() override;
    void OnPropertyChanged(const binding::DependencyProperty& property, 
                          const std::any& oldValue, 
                          const std::any& newValue,
                          binding::ValueSource oldSource,
                          binding::ValueSource newSource) override;

private:
    // 更新视觉状态（根据当前 IsChecked 状态）
    void UpdateCheckVisualState(bool useTransitions);
    
    // 触发相应的事件
    void RaiseCheckedEvents(const std::optional<bool>& oldValue, const std::optional<bool>& newValue);
};

} // namespace fk::ui
