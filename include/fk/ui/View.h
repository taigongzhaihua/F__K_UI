#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/BindingMacros.h"

#include <any>
#include <memory>
#include <utility>

namespace fk::ui {

template <typename Derived, typename Base = FrameworkElement>
class View : public Base, public std::enable_shared_from_this<Derived> {
public:
    using BaseType = Base;
    using Ptr = std::shared_ptr<Derived>;

    using Base::Base;

    template <typename... Args>
    static Ptr Create(Args&&... args) {
        return Ptr(new Derived(std::forward<Args>(args)...));
    }

    // 🎯 使用宏简化属性绑定支持
    // FrameworkElement 属性
    FK_BINDING_PROPERTY_VIEW_VALUE(Width, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(Height, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(MinWidth, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(MinHeight, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(MaxWidth, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(MaxHeight, float, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_ENUM(HorizontalAlignment, ui::HorizontalAlignment, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW_ENUM(VerticalAlignment, ui::VerticalAlignment, FrameworkElement)
    FK_BINDING_PROPERTY_VIEW(Margin, Thickness, FrameworkElement)

    // UIElement 属性
    FK_BINDING_PROPERTY_VIEW_ENUM(Visibility, ui::Visibility, UIElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(IsEnabled, bool, UIElement)
    FK_BINDING_PROPERTY_VIEW_VALUE(Opacity, float, UIElement)

    // DataContext - Getter/Setter 重载
    [[nodiscard]] const std::any& DataContext() const { return Base::GetDataContext(); }
    Ptr DataContext(std::any value) {
        Base::SetDataContext(std::move(value));
        return Self();
    }

    template <typename T>
    Ptr DataContext(T&& value) {
        Base::SetDataContext(std::forward<T>(value));
        return Self();
    }

    Ptr ClearDataContextValue() {
        Base::ClearDataContext();
        return Self();
    }

protected:
    Ptr Self() {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }

    std::shared_ptr<const Derived> Self() const {
        return std::static_pointer_cast<const Derived>(this->shared_from_this());
    }
};

} // namespace fk::ui
