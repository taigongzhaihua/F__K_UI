#include "fk/ui/controls/Control.h"
#include "fk/ui/styling/ControlTemplate.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/Window.h"

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

// 注意：StyleProperty、TemplateProperty 及其回调函数已在 Control.h 中实�?
// 因为 Control 是模板类，所有方法实现都应该在头文件�?

} // namespace fk::ui

// 显式实例化需要的额外头文�?
#include "fk/ui/lists/ListBox.h"
#include "fk/ui/lists/ComboBox.h"
#include "fk/ui/buttons/ToggleButton.h"
#include "fk/ui/scrolling/Thumb.h"
#include "fk/ui/buttons/RepeatButton.h"
#include "fk/ui/scrolling/ScrollBar.h"
#include "fk/ui/scrolling/ScrollViewer.h"

// 显式实例�?Control 模板（必须在命名空间之外�?
template class fk::ui::Control<fk::ui::Button>;
template class fk::ui::Control<fk::ui::Window>;
template class fk::ui::Control<fk::ui::ListBox<>>;
template class fk::ui::Control<fk::ui::ComboBox<>>;
template class fk::ui::Control<fk::ui::ToggleButton>;
template class fk::ui::Control<fk::ui::Thumb>;
template class fk::ui::Control<fk::ui::RepeatButton>;
template class fk::ui::Control<fk::ui::ScrollBar>;
template class fk::ui::Control<fk::ui::ScrollViewer>;
