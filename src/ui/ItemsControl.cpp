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

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/ListBox.h"

// 显式实例化常用类型
template class fk::ui::ItemsControl<fk::ui::ListBox<>>;
