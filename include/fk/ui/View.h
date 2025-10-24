#pragma once

#include "fk/ui/FrameworkElement.h"

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

    Ptr Width(float value) {
        Base::SetWidth(value);
        return Self();
    }

    Ptr Height(float value) {
        Base::SetHeight(value);
        return Self();
    }

    Ptr MinWidth(float value) {
        Base::SetMinWidth(value);
        return Self();
    }

    Ptr MinHeight(float value) {
        Base::SetMinHeight(value);
        return Self();
    }

    Ptr MaxWidth(float value) {
        Base::SetMaxWidth(value);
        return Self();
    }

    Ptr MaxHeight(float value) {
        Base::SetMaxHeight(value);
        return Self();
    }

    Ptr HorizontalAlignment(HorizontalAlignment alignment) {
        Base::SetHorizontalAlignment(alignment);
        return Self();
    }

    Ptr VerticalAlignment(VerticalAlignment alignment) {
        Base::SetVerticalAlignment(alignment);
        return Self();
    }

    Ptr Margin(const Thickness& margin) {
        Base::SetMargin(margin);
        return Self();
    }

    Ptr Visibility(Visibility visibility) {
        Base::SetVisibility(visibility);
        return Self();
    }

    Ptr IsEnabled(bool enabled) {
        Base::SetIsEnabled(enabled);
        return Self();
    }

    Ptr Opacity(float opacity) {
        Base::SetOpacity(opacity);
        return Self();
    }

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
