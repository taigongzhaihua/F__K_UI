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

    // Width - Getter/Setter 重载
    [[nodiscard]] float Width() const { return Base::GetWidth(); }
    Ptr Width(float value) {
        Base::SetWidth(value);
        return Self();
    }

    // Height - Getter/Setter 重载
    [[nodiscard]] float Height() const { return Base::GetHeight(); }
    Ptr Height(float value) {
        Base::SetHeight(value);
        return Self();
    }

    // MinWidth - Getter/Setter 重载
    [[nodiscard]] float MinWidth() const { return Base::GetMinWidth(); }
    Ptr MinWidth(float value) {
        Base::SetMinWidth(value);
        return Self();
    }

    // MinHeight - Getter/Setter 重载
    [[nodiscard]] float MinHeight() const { return Base::GetMinHeight(); }
    Ptr MinHeight(float value) {
        Base::SetMinHeight(value);
        return Self();
    }

    // MaxWidth - Getter/Setter 重载
    [[nodiscard]] float MaxWidth() const { return Base::GetMaxWidth(); }
    Ptr MaxWidth(float value) {
        Base::SetMaxWidth(value);
        return Self();
    }

    // MaxHeight - Getter/Setter 重载
    [[nodiscard]] float MaxHeight() const { return Base::GetMaxHeight(); }
    Ptr MaxHeight(float value) {
        Base::SetMaxHeight(value);
        return Self();
    }

    // HorizontalAlignment - Getter/Setter 重载
    [[nodiscard]] ui::HorizontalAlignment HorizontalAlignment() const { return Base::GetHorizontalAlignment(); }
    Ptr HorizontalAlignment(ui::HorizontalAlignment alignment) {
        Base::SetHorizontalAlignment(alignment);
        return Self();
    }

    // VerticalAlignment - Getter/Setter 重载
    [[nodiscard]] ui::VerticalAlignment VerticalAlignment() const { return Base::GetVerticalAlignment(); }
    Ptr VerticalAlignment(ui::VerticalAlignment alignment) {
        Base::SetVerticalAlignment(alignment);
        return Self();
    }

    // Margin - Getter/Setter 重载
    [[nodiscard]] Thickness Margin() const { return Base::GetMargin(); }
    Ptr Margin(const Thickness& margin) {
        Base::SetMargin(margin);
        return Self();
    }

    // Visibility - Getter/Setter 重载
    [[nodiscard]] ui::Visibility Visibility() const { return Base::GetVisibility(); }
    Ptr Visibility(ui::Visibility visibility) {
        Base::SetVisibility(visibility);
        return Self();
    }

    // IsEnabled - Getter/Setter 重载
    [[nodiscard]] bool IsEnabled() const { return Base::GetIsEnabled(); }
    Ptr IsEnabled(bool enabled) {
        Base::SetIsEnabled(enabled);
        return Self();
    }

    // Opacity - Getter/Setter 重载
    [[nodiscard]] float Opacity() const { return Base::GetOpacity(); }
    Ptr Opacity(float opacity) {
        Base::SetOpacity(opacity);
        return Self();
    }

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
