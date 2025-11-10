#include "fk/ui/ItemsControl.h"

namespace fk::ui {

template<typename Derived>
const binding::DependencyProperty& ItemsControl<Derived>::ItemsSourceProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ItemsSource",
        typeid(std::any),
        typeid(ItemsControl<Derived>),
        {std::any()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ItemsControl<Derived>::ItemTemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ItemTemplate",
        typeid(DataTemplate*),
        typeid(ItemsControl<Derived>),
        {static_cast<DataTemplate*>(nullptr)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ItemsControl<Derived>::ItemsPanelProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ItemsPanel",
        typeid(UIElement*),
        typeid(ItemsControl<Derived>),
        {static_cast<UIElement*>(nullptr)}
    );
    return property;
}

// 显式实例化常用类型（如果需要）
// template class ItemsControl<ListBox>;
// 等等...

} // namespace fk::ui
