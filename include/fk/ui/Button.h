#pragma once

#include "fk/ui/ContentControl.h"
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
    
    // 辅助方法：同步 Background 到模板中的 Border
    // 注意：虽然模板使用了 TemplateBinding，但当前实现还需要手动触发同步
    void SyncBackgroundToBorder();
    
    // ========== 视觉状态管理 ==========
    
    // 更新视觉状态（根据当前状态切换到相应的视觉状态）
    void UpdateVisualState(bool useTransitions);
    
    // 初始化视觉状态
    void InitializeVisualStates();
    
    // 创建各个状态的辅助方法
    std::shared_ptr<animation::VisualState> CreateNormalState();
    std::shared_ptr<animation::VisualState> CreateMouseOverState();
    std::shared_ptr<animation::VisualState> CreatePressedState();
    std::shared_ptr<animation::VisualState> CreateDisabledState();
};

} // namespace fk::ui
