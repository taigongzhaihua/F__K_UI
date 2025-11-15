#pragma once

#include "fk/ui/ContentControl.h"

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
    
    // 辅助方法：手动同步 Background 到模板中的 Border
    void SyncBackgroundToBorder();
};

} // namespace fk::ui
