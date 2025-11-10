#include "fk/ui/Panel.h"

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

// 显式实例化常用类型（如果需要）
// template class Panel<StackPanel>;
// template class Panel<Grid>;
// 等等...

} // namespace fk::ui
