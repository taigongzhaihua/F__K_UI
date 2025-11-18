#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/DrawCommand.h"
#include "fk/ui/Brush.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/animation/VisualStateManager.h"
#include <memory>

// 前向声明
namespace fk::animation {
    class VisualState;
}

namespace fk::ui {

/**
 * @brief 按钮控件
 * 
 * 职责：
 * - 响应点击事件
 * - 显示内容
 * 
 * WPF 对应：Button
 */
class Button : public ContentControl<Button> {
public:
    Button();
    virtual ~Button() = default;

    // ========== 事件 ==========
    
    core::Event<> Click;
    
    // ========== 依赖属性 ==========
    
    /// 鼠标悬停时的背景画刷依赖属性
    static const binding::DependencyProperty& MouseOverBackgroundProperty();
    
    /// 鼠标按下时的背景画刷依赖属性
    static const binding::DependencyProperty& PressedBackgroundProperty();
    
    /// 是否只使用主鼠标按钮(Left)触发 Click 的依赖属性访问器声明
    static const binding::DependencyProperty& PrimaryClickOnlyProperty();
    
    // ========== 属性访问器（使用宏生成） ==========
    
    /// 鼠标悬停背景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(MouseOverBackground, Brush*, Button)
    
    /// 鼠标按下背景属性（Get/Set/链式调用/绑定）
    FK_PROPERTY_VISUAL(PressedBackground, Brush*, Button)
    
    /// 便捷方法：接受颜色参数的链式调用
    Button* MouseOverBackground(const Color& color) { 
        SetMouseOverBackground(new SolidColorBrush(color)); 
        return this; 
    }
    Button* PressedBackground(const Color& color) { 
        SetPressedBackground(new SolidColorBrush(color)); 
        return this; 
    }
    
    // ========== 视觉状态管理 ==========
    
    /// 初始化视觉状态（public 以便属性变化回调访问）
    /// 如果模板中定义了视觉状态，将使用模板中的状态；否则使用默认状态
    void InitializeVisualStates();
    // 是否只使用主鼠标按钮(Left)触发 Click。默认 true。
    FK_PROPERTY_SIMPLE_NO_ACTION(PrimaryClickOnly, bool, Button)
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

private:
    bool isPressed_{false};
    MouseButton pressedButton_{MouseButton::None};
    
    // 辅助方法：同步 Background 到模板中的 Border
    // 注意：虽然模板使用了 TemplateBinding，但当前实现还需要手动触发同步
    void SyncBackgroundToBorder();
    
    // 更新视觉状态（根据当前状态切换到相应的视觉状态）
    void UpdateVisualState(bool useTransitions);
    
    // 创建各个状态的辅助方法（用于默认状态）
    std::shared_ptr<animation::VisualState> CreateNormalState();
    std::shared_ptr<animation::VisualState> CreateMouseOverState();
    std::shared_ptr<animation::VisualState> CreatePressedState();
    std::shared_ptr<animation::VisualState> CreateDisabledState();
    
    // 从模板加载视觉状态（如果模板中定义了状态）
    bool LoadVisualStatesFromTemplate();
    
    // 解析视觉状态中的TargetName并设置实际的动画目标
    void ResolveVisualStateTargets();
};

} // namespace fk::ui
