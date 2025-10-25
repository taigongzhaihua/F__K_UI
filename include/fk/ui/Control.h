#pragma once

#include "fk/ui/UIElement.h"
#include "fk/ui/View.h"

#include <any>
#include <memory>
#include <string>
#include <utility>

namespace fk::ui {

class UIElement;

namespace detail {

class ControlBase : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    ControlBase();
    ~ControlBase() override;

    static const binding::DependencyProperty& IsFocusedProperty();
    static const binding::DependencyProperty& TabIndexProperty();
    static const binding::DependencyProperty& CursorProperty();
    static const binding::DependencyProperty& ContentProperty();

    void SetIsFocused(bool value);
    [[nodiscard]] bool IsFocused() const;

    void SetTabIndex(int value);
    [[nodiscard]] int GetTabIndex() const;

    void SetCursor(std::string cursor);
    [[nodiscard]] const std::string& GetCursor() const;

    void SetContent(std::shared_ptr<UIElement> content);
    void ClearContent();
    [[nodiscard]] std::shared_ptr<UIElement> GetContent() const;
    [[nodiscard]] bool HasContent() const { return GetContent() != nullptr; }

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

    virtual void OnContentChanged(UIElement* oldContent, UIElement* newContent);
    virtual void OnIsFocusedChanged(bool oldValue, bool newValue);
    virtual void OnTabIndexChanged(int oldValue, int newValue);
    virtual void OnCursorChanged(const std::string& oldCursor, const std::string& newCursor);

private:
    static binding::PropertyMetadata BuildIsFocusedMetadata();
    static binding::PropertyMetadata BuildTabIndexMetadata();
    static binding::PropertyMetadata BuildCursorMetadata();
    static binding::PropertyMetadata BuildContentMetadata();

    static void IsFocusedPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void TabIndexPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void CursorPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void ContentPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);

    static bool ValidateTabIndex(const std::any& value);
    static bool ValidateCursor(const std::any& value);
    static bool ValidateContent(const std::any& value);

    static std::shared_ptr<UIElement> ToElement(const std::any& value);

    void AttachContent(UIElement* content);
    void DetachContent(UIElement* content);
    void SyncContentAttachment();
};

} // namespace detail

template <typename Derived>
class Control : public View<Derived, detail::ControlBase> {
public:
    using Base = View<Derived, detail::ControlBase>;
    using ControlBase = detail::ControlBase;
    using Ptr = typename Base::Ptr;
    using ContentPtr = std::shared_ptr<UIElement>;

    using Base::Base;

    Ptr IsFocused(bool value) {
        this->SetIsFocused(value);
        return this->Self();
    }

    Ptr TabIndex(int value) {
        this->SetTabIndex(value);
        return this->Self();
    }

    Ptr Cursor(std::string cursor) {
        this->SetCursor(std::move(cursor));
        return this->Self();
    }

    Ptr Content(ContentPtr content) {
        this->SetContent(std::move(content));
        return this->Self();
    }

    Ptr ClearContentValue() {
        static_cast<ControlBase*>(this)->ClearContent();
        return this->Self();
    }

};

} // namespace fk::ui
