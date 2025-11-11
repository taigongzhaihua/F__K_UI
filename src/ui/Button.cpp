#include "fk/ui/Button.h"

namespace fk::ui {

Button::Button() : isPressed_(false) {
}

void Button::OnPointerPressed(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerPressed(e);
    
    if (this->GetIsEnabled()) {
        isPressed_ = true;
        this->InvalidateVisual();
        e.handled = true;
    }
}

void Button::OnPointerReleased(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerReleased(e);
    
    if (isPressed_ && this->GetIsEnabled()) {
        isPressed_ = false;
        this->InvalidateVisual();
        
        // 触发 Click 事件
        Click();  // 使用 operator() 触发事件
        e.handled = true;
    }
}

void Button::OnPointerEntered(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerEntered(e);
    this->InvalidateVisual();
}

void Button::OnPointerExited(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerExited(e);
    
    if (isPressed_) {
        isPressed_ = false;
        this->InvalidateVisual();
    }
}

} // namespace fk::ui

