#include "fk/ui/Control.h"

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

} // namespace fk::ui
