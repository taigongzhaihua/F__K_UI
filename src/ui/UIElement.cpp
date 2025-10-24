#include "fk/ui/UIElement.h"

#include <algorithm>
#include <stdexcept>

namespace fk::ui {

using binding::DependencyProperty;

UIElement::UIElement() = default;

UIElement::~UIElement() = default;

const DependencyProperty& UIElement::VisibilityProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "Visibility",
        typeid(Visibility),
        typeid(UIElement),
        BuildVisibilityMetadata());
    return property;
}

const DependencyProperty& UIElement::IsEnabledProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "IsEnabled",
        typeid(bool),
        typeid(UIElement),
        BuildIsEnabledMetadata());
    return property;
}

const DependencyProperty& UIElement::OpacityProperty() {
    static const DependencyProperty& property = DependencyProperty::Register(
        "Opacity",
        typeid(float),
        typeid(UIElement),
        BuildOpacityMetadata());
    return property;
}

void UIElement::SetVisibility(Visibility visibility) {
    VerifyAccess();
    SetValue(VisibilityProperty(), visibility);
}

Visibility UIElement::GetVisibility() const {
    return GetValue<Visibility>(VisibilityProperty());
}

void UIElement::SetIsEnabled(bool enabled) {
    VerifyAccess();
    SetValue(IsEnabledProperty(), enabled);
}

bool UIElement::IsEnabled() const {
    return GetValue<bool>(IsEnabledProperty());
}

void UIElement::SetOpacity(float value) {
    VerifyAccess();
    SetValue(OpacityProperty(), value);
}

float UIElement::Opacity() const {
    return GetValue<float>(OpacityProperty());
}

Size UIElement::Measure(const Size& availableSize) {
    VerifyAccess();

    Size finalAvailable = availableSize;
    const auto visibility = GetVisibility();
    if (visibility == Visibility::Collapsed) {
        desiredSize_ = Size{};
        isMeasureValid_ = true;
        return desiredSize_;
    }

    desiredSize_ = MeasureCore(finalAvailable);
    isMeasureValid_ = true;
    return desiredSize_;
}

void UIElement::Arrange(const Rect& finalRect) {
    VerifyAccess();

    layoutSlot_ = finalRect;

    if (GetVisibility() == Visibility::Collapsed) {
        layoutSlot_.width = 0;
        layoutSlot_.height = 0;
        isArrangeValid_ = true;
        return;
    }

    ArrangeCore(finalRect);
    isArrangeValid_ = true;
}

void UIElement::InvalidateMeasure() {
    VerifyAccess();
    if (!isMeasureValid_) {
        return;
    }
    isMeasureValid_ = false;
    MeasureInvalidated(*this);
}

void UIElement::InvalidateArrange() {
    VerifyAccess();
    if (!isArrangeValid_) {
        return;
    }
    isArrangeValid_ = false;
    ArrangeInvalidated(*this);
}

void UIElement::OnAttachedToLogicalTree() {
    DependencyObject::OnAttachedToLogicalTree();
    InvalidateMeasure();
    InvalidateArrange();
}

void UIElement::OnDetachedFromLogicalTree() {
    DependencyObject::OnDetachedFromLogicalTree();
    InvalidateMeasure();
    InvalidateArrange();
}

Size UIElement::MeasureCore(const Size& availableSize) {
    return Size{
        std::max(0.0f, availableSize.width),
        std::max(0.0f, availableSize.height)};
}

void UIElement::ArrangeCore(const Rect& finalRect) {
    layoutSlot_ = finalRect;
}

void UIElement::OnVisibilityChanged(Visibility, Visibility newValue) {
    if (newValue == Visibility::Collapsed) {
        InvalidateMeasure();
        InvalidateArrange();
    } else {
        InvalidateMeasure();
    }
}

void UIElement::OnIsEnabledChanged(bool, bool) {}

void UIElement::OnOpacityChanged(float, float) {}

binding::PropertyMetadata UIElement::BuildVisibilityMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = Visibility::Visible;
    metadata.propertyChangedCallback = &UIElement::VisibilityPropertyChanged;
    return metadata;
}

binding::PropertyMetadata UIElement::BuildIsEnabledMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = true;
    metadata.propertyChangedCallback = &UIElement::IsEnabledPropertyChanged;
    return metadata;
}

binding::PropertyMetadata UIElement::BuildOpacityMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 1.0f;
    metadata.propertyChangedCallback = &UIElement::OpacityPropertyChanged;
    metadata.validateCallback = &UIElement::ValidateOpacity;
    return metadata;
}

void UIElement::VisibilityPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldVisibility = std::any_cast<Visibility>(oldValue);
    const auto newVisibility = std::any_cast<Visibility>(newValue);
    element->OnVisibilityChanged(oldVisibility, newVisibility);
}

void UIElement::IsEnabledPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldEnabled = std::any_cast<bool>(oldValue);
    const auto newEnabled = std::any_cast<bool>(newValue);
    element->OnIsEnabledChanged(oldEnabled, newEnabled);
}

void UIElement::OpacityPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* element = dynamic_cast<UIElement*>(&sender);
    if (!element) {
        return;
    }
    const auto oldOpacity = std::any_cast<float>(oldValue);
    const auto newOpacity = std::any_cast<float>(newValue);
    element->OnOpacityChanged(oldOpacity, newOpacity);
}

bool UIElement::ValidateOpacity(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    const auto opacity = std::any_cast<float>(value);
    return opacity >= 0.0f && opacity <= 1.0f;
}

} // namespace fk::ui
