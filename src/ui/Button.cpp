#include "fk/ui/Button.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/ui/Control.h"

namespace fk::ui {

// 创建 Button 的默认 ControlTemplate
static ControlTemplate* CreateDefaultButtonTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 模板工厂：创建 Border + ContentPresenter
    tmpl->SetFactory([]() -> UIElement* {
        auto* border = new Border();
        
        // 使用 TemplateBinding 绑定 Button 的 Background 到 Border 的 Background
        // 这样设置 Button 的 Background 属性时，Border 的背景色会自动更新
        border->SetBinding(
            Border::BackgroundProperty(),
            binding::TemplateBinding(Control<Button>::BackgroundProperty())
        );
        
        // 设置默认背景色（当 Button 的 Background 为空时使用）
        // 注意：由于使用了 TemplateBinding，如果 Button 没有设置 Background，
        // 这里设置的默认值可能不会生效。需要在 Button 构造函数中设置默认值。
        // border->Background(new SolidColorBrush(Color(240, 240, 240, 255)));  // 浅灰色背景
        
        border->BorderBrush(new SolidColorBrush(Color::FromRGB(172, 172, 172, 255))); // 灰色边框
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
    
    // 设置默认背景色（浅灰色）
    // 用户可以通过 Background() 方法覆盖此默认值
    if (!GetBackground()) {
        SetBackground(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));
    }
    
    // 设置默认模板
    if (!GetTemplate()) {
        SetTemplate(CreateDefaultButtonTemplate());
    }
}

void Button::OnTemplateApplied() {
    ContentControl<Button>::OnTemplateApplied();
    
    // 手动同步 Background 属性到模板中的 Border
    // 这是因为 TemplateBinding 还没有完全实现
    SyncBackgroundToBorder();
}

void Button::SyncBackgroundToBorder() {
    // 获取模板根（应该是 Border）
    if (GetVisualChildrenCount() > 0) {
        auto* firstChild = GetVisualChild(0);
        auto* border = dynamic_cast<Border*>(firstChild);
        if (border) {
            // 将 Button 的 Background 同步到 Border
            auto* bg = GetBackground();
            if (bg) {
                border->Background(bg);
            }
        }
    }
}

void Button::OnPropertyChanged(const binding::DependencyProperty& property, 
                               const std::any& oldValue, 
                               const std::any& newValue,
                               binding::ValueSource oldSource,
                               binding::ValueSource newSource) {
    ContentControl<Button>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    
    // 当 Background 属性改变时，同步到模板中的 Border
    if (property.Name() == "Background") {
        SyncBackgroundToBorder();
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

