#include "fk/ui/layouts/Panel.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/layouts/Grid.h"

namespace fk::ui {

template<typename Derived>
const binding::DependencyProperty& Panel<Derived>::BackgroundProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Background",
        typeid(Brush*),
        typeid(Panel<Derived>),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Panel<Derived>::CornerRadiusProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "CornerRadius",
        typeid(ui::CornerRadius),
        typeid(Panel<Derived>),
        {ui::CornerRadius(0)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& Panel<Derived>::ClipToBoundsProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ClipToBounds",
        typeid(bool),
        typeid(Panel<Derived>),
        {false}  // 默认不裁�?
    );
    return property;
}

} // namespace fk::ui

// 显式实例�?Panel 模板（必须在命名空间之外�?
template class fk::ui::Panel<fk::ui::StackPanel>;
template class fk::ui::Panel<fk::ui::Grid>;
