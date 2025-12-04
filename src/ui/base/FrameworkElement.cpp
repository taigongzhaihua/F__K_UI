#include "fk/ui/base/FrameworkElement.h"
#include <limits>

// 显式实例化所需的头文件（必须在命名空间之外�?
#include "fk/ui/controls/Border.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Image.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/graphics/Shape.h"

namespace fk::ui {

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::WidthProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Width",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        binding::PropertyMetadata(
            -1.0f,
            [](binding::DependencyObject& d, const binding::DependencyProperty& prop, const std::any& oldValue, const std::any& newValue) {
                if (auto* element = dynamic_cast<FrameworkElement<Derived>*>(&d)) {
                    element->InvalidateMeasure();
                }
            }
        )
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::HeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Height",
        typeid(float),
        typeid(FrameworkElement<Derived>),
        binding::PropertyMetadata(
            -1.0f,
            [](binding::DependencyObject& d, const binding::DependencyProperty& prop, const std::any& oldValue, const std::any& newValue) {
                if (auto* element = dynamic_cast<FrameworkElement<Derived>*>(&d)) {
                    element->InvalidateMeasure();
                }
            }
        )
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

// DataContext 不再作为 DependencyProperty 注册，改�?BindingContext 管理

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
const binding::DependencyProperty& FrameworkElement<Derived>::PaddingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Padding",
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
#include "fk/ui/controls/ContentPresenter.h"
#include "fk/ui/lists/ListBox.h"
#include "fk/ui/buttons/ToggleButton.h"
#include "fk/ui/scrolling/Thumb.h"
#include "fk/ui/buttons/RepeatButton.h"
#include "fk/ui/scrolling/Track.h"
#include "fk/ui/scrolling/ScrollBar.h"
#include "fk/ui/scrolling/ScrollContentPresenter.h"
#include "fk/ui/scrolling/ScrollViewer.h"

// 显式实例�?FrameworkElement 模板（必须在命名空间之外�?
// 注意：Rectangle、Ellipse、Line、Path、Polygon 等继承自 Shape<T>，Shape<T> 继承�?FrameworkElement<T>
template class fk::ui::FrameworkElement<fk::ui::Border>;
template class fk::ui::FrameworkElement<fk::ui::TextBlock>;
template class fk::ui::FrameworkElement<fk::ui::Image>;
template class fk::ui::FrameworkElement<fk::ui::Button>;
template class fk::ui::FrameworkElement<fk::ui::Window>;
template class fk::ui::FrameworkElement<fk::ui::StackPanel>;
template class fk::ui::FrameworkElement<fk::ui::Grid>;
// Shape 现在是模板类，需要实例化具体�?Shape 子类
template class fk::ui::FrameworkElement<fk::ui::Rectangle>;
template class fk::ui::FrameworkElement<fk::ui::Ellipse>;
template class fk::ui::FrameworkElement<fk::ui::Line>;
template class fk::ui::FrameworkElement<fk::ui::Polygon>;
template class fk::ui::FrameworkElement<fk::ui::Path>;
template class fk::ui::FrameworkElement<fk::ui::ContentPresenter<>>;
template class fk::ui::FrameworkElement<fk::ui::ListBox<>>;
template class fk::ui::FrameworkElement<fk::ui::ToggleButton>;
template class fk::ui::FrameworkElement<fk::ui::Thumb>;
template class fk::ui::FrameworkElement<fk::ui::RepeatButton>;
template class fk::ui::FrameworkElement<fk::ui::Track>;
template class fk::ui::FrameworkElement<fk::ui::ScrollBar>;
template class fk::ui::FrameworkElement<fk::ui::ScrollContentPresenter<>>;
template class fk::ui::FrameworkElement<fk::ui::ScrollViewer>;
