#include "fk/ui/FrameworkElement.h"
#include <limits>

// 显式实例化所需的头文件（必须在命名空间之外）
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Image.h"
#include "fk/ui/Button.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Shape.h"

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

} // namespace fk::ui

// 显式实例化需要的额外头文件
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/ScrollViewer.h"
#include "fk/ui/ListBox.h"

// 显式实例化 FrameworkElement 模板（必须在命名空间之外）
// 注意：Rectangle、Ellipse、Line 等继承自 Shape，不直接继承 FrameworkElement
template class fk::ui::FrameworkElement<fk::ui::Border>;
template class fk::ui::FrameworkElement<fk::ui::TextBlock>;
template class fk::ui::FrameworkElement<fk::ui::Image>;
template class fk::ui::FrameworkElement<fk::ui::Button>;
template class fk::ui::FrameworkElement<fk::ui::Window>;
template class fk::ui::FrameworkElement<fk::ui::StackPanel>;
template class fk::ui::FrameworkElement<fk::ui::Grid>;
template class fk::ui::FrameworkElement<fk::ui::Shape>;
template class fk::ui::FrameworkElement<fk::ui::ContentPresenter<>>;
template class fk::ui::FrameworkElement<fk::ui::ScrollBar>;
template class fk::ui::FrameworkElement<fk::ui::ScrollViewer>;
template class fk::ui::FrameworkElement<fk::ui::ListBox<>>;
