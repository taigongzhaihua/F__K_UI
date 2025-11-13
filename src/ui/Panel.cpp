#include "fk/ui/Panel.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"

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

} // namespace fk::ui

// 显式实例化 Panel 模板（必须在命名空间之外）
template class fk::ui::Panel<fk::ui::StackPanel>;
template class fk::ui::Panel<fk::ui::Grid>;
