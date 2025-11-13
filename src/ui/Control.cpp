#include "fk/ui/Control.h"
#include "fk/ui/ControlTemplate.h"

namespace fk::ui {

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::ForegroundProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Foreground",
        typeid(Brush*),
        typeid(Control<Derived>),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::BackgroundProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Background",
        typeid(Brush*),
        typeid(Control<Derived>),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::BorderBrushProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "BorderBrush",
        typeid(Brush*),
        typeid(Control<Derived>),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::BorderThicknessProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "BorderThickness",
        typeid(Thickness),
        typeid(Control<Derived>),
        {Thickness(0)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::PaddingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Padding",
        typeid(Thickness),
        typeid(Control<Derived>),
        {Thickness(0)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::FontFamilyProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontFamily",
        typeid(std::string),
        typeid(Control<Derived>),
        {std::string("Arial")}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::FontSizeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontSize",
        typeid(float),
        typeid(Control<Derived>),
        {14.0f}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::FontWeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontWeight",
        typeid(ui::FontWeight),
        typeid(Control<Derived>),
        {ui::FontWeight::Normal}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::StyleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Style",
        typeid(fk::ui::Style*),
        typeid(Control<Derived>),
        binding::PropertyMetadata{
            std::any(static_cast<fk::ui::Style*>(nullptr)),
            &Control<Derived>::OnStyleChanged
        }
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Control<Derived>::TemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Template",
        typeid(ControlTemplate*),
        typeid(Control<Derived>),
        binding::PropertyMetadata{
            std::any(static_cast<ControlTemplate*>(nullptr)),
            &Control<Derived>::OnTemplateChanged
        }
    );
    return property;
}

template<typename Derived>
void Control<Derived>::OnStyleChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* control = dynamic_cast<Control<Derived>*>(&d);
    if (!control) {
        return;
    }
    
    // 撤销旧样式
    if (oldValue.has_value() && oldValue.type() == typeid(fk::ui::Style*)) {
        auto* oldStyle = std::any_cast<fk::ui::Style*>(oldValue);
        if (oldStyle) {
            oldStyle->Unapply(control);
        }
    }
    
    // 应用新样式
    if (newValue.has_value() && newValue.type() == typeid(fk::ui::Style*)) {
        auto* newStyle = std::any_cast<fk::ui::Style*>(newValue);
        if (newStyle) {
            newStyle->Apply(control);
        }
    }
    
    // 触发重新渲染
    control->InvalidateVisual();
    control->InvalidateMeasure();
}

template<typename Derived>
void Control<Derived>::OnTemplateChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* control = dynamic_cast<Control<Derived>*>(&d);
    if (!control) {
        return;
    }
    
    // 应用新模板
    control->ApplyTemplate();
    
    // 触发重新布局和渲染
    control->InvalidateMeasure();
    control->InvalidateVisual();
}

} // namespace fk::ui
