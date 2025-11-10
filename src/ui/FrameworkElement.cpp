#include "fk/ui/FrameworkElement.h"
#include <limits>

namespace fk::ui {

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::WidthProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Width",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {-1.0f}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::HeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Height",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {-1.0f}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::MinWidthProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "MinWidth",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {0.0f}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::MaxWidthProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "MaxWidth",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {std::numeric_limits<float>::infinity()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::MinHeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "MinHeight",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {0.0f}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::MaxHeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "MaxHeight",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        {std::numeric_limits<float>::infinity()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::DataContextProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "DataContext",
        typeid(std::any),
        typeid(FrameworkElement<Derived>),
        {std::any()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::MarginProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Margin",
        typeid(Thickness),
        typeid(FrameworkElement<Derived>),
        {Thickness(0)}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::HorizontalAlignmentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "HorizontalAlignment",
        typeid(ui::HorizontalAlignment),
        typeid(FrameworkElement<Derived>),
        {ui::HorizontalAlignment::Stretch}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::VerticalAlignmentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "VerticalAlignment",
        typeid(ui::VerticalAlignment),
        typeid(FrameworkElement<Derived>),
        {ui::VerticalAlignment::Stretch}
    );
    return property;
}

// 显式实例化常用的模板版本（避免链接错误）
// 注意：这些实例化应该在使用这些类型时才需要

} // namespace fk::ui
