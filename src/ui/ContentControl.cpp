#include "fk/ui/ContentControl.h"
#include "fk/binding/DependencyProperty.h"

namespace fk::ui {

// 为 ContentControl 模板类注册依赖属性

template<typename Derived>
const binding::DependencyProperty& ContentControl<Derived>::ContentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Content",
        typeid(std::any),
        typeid(ContentControl<Derived>),
        binding::PropertyMetadata{std::any()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ContentControl<Derived>::ContentTemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ContentTemplate",
        typeid(DataTemplate*),
        typeid(ContentControl<Derived>),
        binding::PropertyMetadata{static_cast<DataTemplate*>(nullptr)}
    );
    return property;
}

} // namespace fk::ui
