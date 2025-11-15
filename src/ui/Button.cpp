#include "fk/ui/Button.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"

namespace fk::ui {

// 创建 Button 的默认 ControlTemplate
static ControlTemplate* CreateDefaultButtonTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 模板工厂：创建 Border + ContentPresenter
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        
        // 默认样式
        border->Background(new SolidColorBrush(Color(240, 240, 240, 255)));  // 浅灰色背景
        border->BorderBrush(new SolidColorBrush(Color(172, 172, 172, 255))); // 灰色边框
        border->BorderThickness(1.0f);
        border->Padding(10.0f, 5.0f, 10.0f, 5.0f);
        border->CornerRadius(3.0f);  // 统一圆角
        
        // ContentPresenter 用于显示按钮的内容
        auto* presenter = new ContentPresenter<>();
        presenter->SetHorizontalAlignment(HorizontalAlignment::Center);
        presenter->SetVerticalAlignment(VerticalAlignment::Center);
        
        border->Child(presenter);
        
        return border;
    });
    
    return tmpl;
}

Button::Button() : isPressed_(false) {
    // 设置默认对齐方式：Button 不应该拉伸，而是根据内容大小决定
    SetHorizontalAlignment(HorizontalAlignment::Left);
    SetVerticalAlignment(VerticalAlignment::Top);
    
    // 设置默认模板
    if (!GetTemplate()) {
        SetTemplate(CreateDefaultButtonTemplate());
    }
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

