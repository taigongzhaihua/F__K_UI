#include "fk/ui/FrameworkElement.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace fk::ui {

namespace {
[[nodiscard]] float ThicknessWidth(const fk::Thickness& thickness) noexcept {
    return thickness.left + thickness.right;
}

[[nodiscard]] float ThicknessHeight(const fk::Thickness& thickness) noexcept {
    return thickness.top + thickness.bottom;
}

[[nodiscard]] constexpr float DefaultLengthUnset() noexcept {
    return std::numeric_limits<float>::quiet_NaN();
}

[[nodiscard]] constexpr float DefaultLengthMax() noexcept {
    return std::numeric_limits<float>::infinity();
}

[[nodiscard]] float CoerceAvailable(float available) noexcept {
    if (std::isnan(available)) {
        return 0.0f;
    }
    if (available < 0.0f) {
        return 0.0f;
    }
    return available;
}

[[nodiscard]] float LimitToFinite(float value, float limit) noexcept {
    if (!std::isfinite(limit)) {
        return value;
    }
    return std::min(value, limit);
}

} // namespace

FrameworkElement::FrameworkElement() = default;

FrameworkElement::~FrameworkElement() = default;

const binding::DependencyProperty& FrameworkElement::WidthProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Width",
        typeid(float),
        typeid(FrameworkElement),
        BuildWidthMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::HeightProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Height",
        typeid(float),
        typeid(FrameworkElement),
        BuildHeightMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::MinWidthProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "MinWidth",
        typeid(float),
        typeid(FrameworkElement),
        BuildMinWidthMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::MinHeightProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "MinHeight",
        typeid(float),
        typeid(FrameworkElement),
        BuildMinHeightMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::MaxWidthProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "MaxWidth",
        typeid(float),
        typeid(FrameworkElement),
        BuildMaxWidthMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::MaxHeightProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "MaxHeight",
        typeid(float),
        typeid(FrameworkElement),
        BuildMaxHeightMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::HorizontalAlignmentProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "HorizontalAlignment",
        typeid(HorizontalAlignment),
        typeid(FrameworkElement),
        BuildHorizontalAlignmentMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::VerticalAlignmentProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "VerticalAlignment",
        typeid(VerticalAlignment),
        typeid(FrameworkElement),
        BuildVerticalAlignmentMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::MarginProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Margin",
        typeid(fk::Thickness),
        typeid(FrameworkElement),
        BuildMarginMetadata());
    return property;
}

const binding::DependencyProperty& FrameworkElement::DataContextProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "DataContext",
        typeid(std::any),
        typeid(FrameworkElement),
        BuildDataContextMetadata());
    return property;
}

void FrameworkElement::SetWidth(float value) {
    VerifyAccess();
    SetValue(WidthProperty(), value);
}

float FrameworkElement::GetWidth() const {
    return GetValue<float>(WidthProperty());
}

void FrameworkElement::SetHeight(float value) {
    VerifyAccess();
    SetValue(HeightProperty(), value);
}

float FrameworkElement::GetHeight() const {
    return GetValue<float>(HeightProperty());
}

void FrameworkElement::SetMinWidth(float value) {
    VerifyAccess();
    SetValue(MinWidthProperty(), value);
}

float FrameworkElement::GetMinWidth() const {
    return GetValue<float>(MinWidthProperty());
}

void FrameworkElement::SetMinHeight(float value) {
    VerifyAccess();
    SetValue(MinHeightProperty(), value);
}

float FrameworkElement::GetMinHeight() const {
    return GetValue<float>(MinHeightProperty());
}

void FrameworkElement::SetMaxWidth(float value) {
    VerifyAccess();
    SetValue(MaxWidthProperty(), value);
}

float FrameworkElement::GetMaxWidth() const {
    return GetValue<float>(MaxWidthProperty());
}

void FrameworkElement::SetMaxHeight(float value) {
    VerifyAccess();
    SetValue(MaxHeightProperty(), value);
}

float FrameworkElement::GetMaxHeight() const {
    return GetValue<float>(MaxHeightProperty());
}

void FrameworkElement::SetHorizontalAlignment(HorizontalAlignment alignment) {
    VerifyAccess();
    SetValue(HorizontalAlignmentProperty(), alignment);
}

HorizontalAlignment FrameworkElement::GetHorizontalAlignment() const {
    return GetValue<HorizontalAlignment>(HorizontalAlignmentProperty());
}

void FrameworkElement::SetVerticalAlignment(VerticalAlignment alignment) {
    VerifyAccess();
    SetValue(VerticalAlignmentProperty(), alignment);
}

VerticalAlignment FrameworkElement::GetVerticalAlignment() const {
    return GetValue<VerticalAlignment>(VerticalAlignmentProperty());
}

void FrameworkElement::SetMargin(const fk::Thickness& margin) {
    VerifyAccess();
    SetValue(MarginProperty(), margin);
}

const fk::Thickness& FrameworkElement::GetMargin() const {
    return GetValue<const fk::Thickness&>(MarginProperty());
}

void FrameworkElement::SetDataContext(std::any value) {
    VerifyAccess();
    SetValue(DataContextProperty(), std::move(value));
}

void FrameworkElement::ClearDataContext() {
    VerifyAccess();
    ClearValue(DataContextProperty());
}

const std::any& FrameworkElement::GetDataContext() const noexcept {
    return BindingDependencyObject::GetDataContext();
}

bool FrameworkElement::ApplyTemplate() {
    VerifyAccess();
    if (isTemplateApplied_) {
        return false;
    }
    isTemplateApplied_ = true;
    OnApplyTemplate();
    TemplateApplied(*this);
    return true;
}

Size FrameworkElement::MeasureCore(const Size& availableSize) {
    const auto margin = GetMargin();
    const float marginWidth = ThicknessWidth(margin);
    const float marginHeight = ThicknessHeight(margin);

    const float minWidth = GetMinWidth();
    const float minHeight = GetMinHeight();
    float maxWidth = GetMaxWidth();
    float maxHeight = GetMaxHeight();

    if (maxWidth < minWidth) {
        maxWidth = minWidth;
    }
    if (maxHeight < minHeight) {
        maxHeight = minHeight;
    }

    Size constrainedAvailable{
        CoerceAvailable(availableSize.width - marginWidth),
        CoerceAvailable(availableSize.height - marginHeight)
    };

    float widthConstraint = constrainedAvailable.width;
    float heightConstraint = constrainedAvailable.height;

    const float width = GetWidth();
    const float height = GetHeight();

    if (!IsUnsetLength(width)) {
        widthConstraint = width;
    } else {
        widthConstraint = std::min(widthConstraint, maxWidth);
    }

    if (!IsUnsetLength(height)) {
        heightConstraint = height;
    } else {
        heightConstraint = std::min(heightConstraint, maxHeight);
    }

    widthConstraint = std::max(widthConstraint, minWidth);
    heightConstraint = std::max(heightConstraint, minHeight);

    const Size desired = MeasureOverride(Size{ widthConstraint, heightConstraint });

    float desiredWidth = std::max(desired.width, minWidth);
    float desiredHeight = std::max(desired.height, minHeight);

    if (!IsUnsetLength(width)) {
        desiredWidth = width;
    } else {
        desiredWidth = std::min(desiredWidth, maxWidth);
        desiredWidth = LimitToFinite(desiredWidth, constrainedAvailable.width);
    }

    if (!IsUnsetLength(height)) {
        desiredHeight = height;
    } else {
        desiredHeight = std::min(desiredHeight, maxHeight);
        desiredHeight = LimitToFinite(desiredHeight, constrainedAvailable.height);
    }

    Size result{
        std::max(0.0f, desiredWidth) + marginWidth,
        std::max(0.0f, desiredHeight) + marginHeight
    };

    if (std::isfinite(availableSize.width)) {
        result.width = std::min(result.width, availableSize.width);
    }
    if (std::isfinite(availableSize.height)) {
        result.height = std::min(result.height, availableSize.height);
    }

    return result;
}

void FrameworkElement::ArrangeCore(const Rect& finalRect) {
    const auto margin = GetMargin();
    const float marginWidth = ThicknessWidth(margin);
    const float marginHeight = ThicknessHeight(margin);

    Rect innerRect{
        finalRect.x + margin.left,
        finalRect.y + margin.top,
        CoerceAvailable(finalRect.width - marginWidth),
        CoerceAvailable(finalRect.height - marginHeight)
    };

    const float minWidth = GetMinWidth();
    const float minHeight = GetMinHeight();
    float maxWidth = GetMaxWidth();
    float maxHeight = GetMaxHeight();

    if (maxWidth < minWidth) {
        maxWidth = minWidth;
    }
    if (maxHeight < minHeight) {
        maxHeight = minHeight;
    }

    const float width = GetWidth();
    const float height = GetHeight();
    const auto horizontalAlignment = GetHorizontalAlignment();
    const auto verticalAlignment = GetVerticalAlignment();

    float targetWidth = DesiredSize().width - marginWidth;
    float targetHeight = DesiredSize().height - marginHeight;

    targetWidth = std::max(targetWidth, minWidth);
    targetHeight = std::max(targetHeight, minHeight);

    if (!IsUnsetLength(width)) {
        targetWidth = width;
    } else {
        targetWidth = std::min(targetWidth, maxWidth);
        if (horizontalAlignment == HorizontalAlignment::Stretch) {
            targetWidth = std::max(targetWidth, innerRect.width);
        }
        targetWidth = LimitToFinite(targetWidth, innerRect.width);
    }

    if (!IsUnsetLength(height)) {
        targetHeight = height;
    } else {
        targetHeight = std::min(targetHeight, maxHeight);
        if (verticalAlignment == VerticalAlignment::Stretch) {
            targetHeight = std::max(targetHeight, innerRect.height);
        }
        targetHeight = LimitToFinite(targetHeight, innerRect.height);
    }

    Size arrangedSize = ArrangeOverride(Size{ targetWidth, targetHeight });

    if (!IsUnsetLength(width)) {
        arrangedSize.width = width;
    }
    if (!IsUnsetLength(height)) {
        arrangedSize.height = height;
    }

    arrangedSize.width = std::max(arrangedSize.width, minWidth);
    arrangedSize.height = std::max(arrangedSize.height, minHeight);

    arrangedSize.width = std::min(arrangedSize.width, maxWidth);
    arrangedSize.height = std::min(arrangedSize.height, maxHeight);

    if (IsUnsetLength(width)) {
        arrangedSize.width = LimitToFinite(arrangedSize.width, innerRect.width);
        if (horizontalAlignment == HorizontalAlignment::Stretch) {
            arrangedSize.width = std::max(arrangedSize.width, innerRect.width);
            arrangedSize.width = LimitToFinite(arrangedSize.width, innerRect.width);
        }
    }

    if (IsUnsetLength(height)) {
        arrangedSize.height = LimitToFinite(arrangedSize.height, innerRect.height);
        if (verticalAlignment == VerticalAlignment::Stretch) {
            arrangedSize.height = std::max(arrangedSize.height, innerRect.height);
            arrangedSize.height = LimitToFinite(arrangedSize.height, innerRect.height);
        }
    }

    arrangedSize.width = std::max(arrangedSize.width, 0.0f);
    arrangedSize.height = std::max(arrangedSize.height, 0.0f);

    const float remainingWidth = std::max(0.0f, innerRect.width - arrangedSize.width);
    const float remainingHeight = std::max(0.0f, innerRect.height - arrangedSize.height);

    float offsetX = innerRect.x;
    float offsetY = innerRect.y;

    switch (horizontalAlignment) {
    case HorizontalAlignment::Center:
        offsetX += remainingWidth * 0.5f;
        break;
    case HorizontalAlignment::Right:
        offsetX += remainingWidth;
        break;
    case HorizontalAlignment::Stretch:
    case HorizontalAlignment::Left:
        break;
    }

    switch (verticalAlignment) {
    case VerticalAlignment::Center:
        offsetY += remainingHeight * 0.5f;
        break;
    case VerticalAlignment::Bottom:
        offsetY += remainingHeight;
        break;
    case VerticalAlignment::Stretch:
    case VerticalAlignment::Top:
        break;
    }

    renderSize_ = arrangedSize;
    SetLayoutSlot(Rect{ offsetX, offsetY, renderSize_.width, renderSize_.height });
}

Size FrameworkElement::MeasureOverride(const Size& availableSize) {
    return UIElement::MeasureCore(availableSize);
}

Size FrameworkElement::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

void FrameworkElement::OnInitialized() {}

void FrameworkElement::OnLoaded() {}

void FrameworkElement::OnUnloaded() {}

void FrameworkElement::OnApplyTemplate() {}

void FrameworkElement::OnAttachedToLogicalTree() {
    UIElement::OnAttachedToLogicalTree();

    if (!isInitialized_) {
        isInitialized_ = true;
        OnInitialized();
        Initialized(*this);
    }

    if (!isLoaded_) {
        isLoaded_ = true;
        OnLoaded();
        Loaded(*this);
    }
}

void FrameworkElement::OnDetachedFromLogicalTree() {
    UIElement::OnDetachedFromLogicalTree();

    if (isLoaded_) {
        isLoaded_ = false;
        OnUnloaded();
        Unloaded(*this);
    }
}

void FrameworkElement::OnDataContextChanged(const std::any& oldValue, const std::any& newValue) {
    UIElement::OnDataContextChanged(oldValue, newValue);
    UpdatePropertyFromLocalDataContext(newValue);
}

binding::PropertyMetadata FrameworkElement::BuildWidthMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = DefaultLengthUnset();
    metadata.propertyChangedCallback = &FrameworkElement::LengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateOptionalLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildHeightMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = DefaultLengthUnset();
    metadata.propertyChangedCallback = &FrameworkElement::LengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateOptionalLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildMinWidthMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0.0f;
    metadata.propertyChangedCallback = &FrameworkElement::MinMaxLengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateNonNegativeLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildMinHeightMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0.0f;
    metadata.propertyChangedCallback = &FrameworkElement::MinMaxLengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateNonNegativeLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildMaxWidthMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = DefaultLengthMax();
    metadata.propertyChangedCallback = &FrameworkElement::MinMaxLengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateNonNegativeOrInfiniteLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildMaxHeightMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = DefaultLengthMax();
    metadata.propertyChangedCallback = &FrameworkElement::MinMaxLengthPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateNonNegativeOrInfiniteLength;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildHorizontalAlignmentMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = HorizontalAlignment::Stretch;
    metadata.propertyChangedCallback = &FrameworkElement::AlignmentPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateHorizontalAlignment;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildVerticalAlignmentMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = VerticalAlignment::Stretch;
    metadata.propertyChangedCallback = &FrameworkElement::AlignmentPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateVerticalAlignment;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildMarginMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = fk::Thickness{};
    metadata.propertyChangedCallback = &FrameworkElement::MarginPropertyChanged;
    metadata.validateCallback = &FrameworkElement::ValidateThickness;
    return metadata;
}

binding::PropertyMetadata FrameworkElement::BuildDataContextMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::any{};
    metadata.propertyChangedCallback = &FrameworkElement::DataContextPropertyChanged;
    metadata.bindingOptions.inheritsDataContext = true;
    metadata.validateCallback = nullptr;
    return metadata;
}

void FrameworkElement::LengthPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty&,
    const std::any&, const std::any&) {
    auto* element = dynamic_cast<FrameworkElement*>(&sender);
    if (!element) {
        return;
    }
    element->InvalidateMeasure();
    element->InvalidateArrange();
}

void FrameworkElement::MinMaxLengthPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty&,
    const std::any&, const std::any&) {
    auto* element = dynamic_cast<FrameworkElement*>(&sender);
    if (!element) {
        return;
    }
    element->CoerceMaxToAtLeastMin();
    element->InvalidateMeasure();
    element->InvalidateArrange();
}

void FrameworkElement::AlignmentPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
    const std::any&, const std::any&) {
    auto* element = dynamic_cast<FrameworkElement*>(&sender);
    if (!element) {
        return;
    }
    if (&property == &HorizontalAlignmentProperty() || &property == &VerticalAlignmentProperty()) {
        element->InvalidateArrange();
    }
}

void FrameworkElement::MarginPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty&,
    const std::any&, const std::any&) {
    auto* element = dynamic_cast<FrameworkElement*>(&sender);
    if (!element) {
        return;
    }
    element->InvalidateMeasure();
    element->InvalidateArrange();
}

void FrameworkElement::DataContextPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty&,
    const std::any&, const std::any& newValue) {
    auto* element = dynamic_cast<FrameworkElement*>(&sender);
    if (!element) {
        return;
    }
    element->UpdateLocalDataContextFromProperty(newValue);
}

bool FrameworkElement::ValidateOptionalLength(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    const auto length = std::any_cast<float>(value);
    return IsUnsetLength(length) || length >= 0.0f;
}

bool FrameworkElement::ValidateNonNegativeLength(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    const auto length = std::any_cast<float>(value);
    return std::isfinite(length) && length >= 0.0f;
}

bool FrameworkElement::ValidateNonNegativeOrInfiniteLength(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    const auto length = std::any_cast<float>(value);
    return (std::isfinite(length) && length >= 0.0f) || std::isinf(length);
}

bool FrameworkElement::ValidateHorizontalAlignment(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(HorizontalAlignment);
}

bool FrameworkElement::ValidateVerticalAlignment(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(VerticalAlignment);
}

bool FrameworkElement::ValidateThickness(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(fk::Thickness);
}

bool FrameworkElement::IsUnsetLength(float value) {
    return std::isnan(value);
}

void FrameworkElement::UpdateLocalDataContextFromProperty(const std::any& value) {
    if (isUpdatingDataContext_) {
        return;
    }
    isUpdatingDataContext_ = true;
    if (value.has_value()) {
        BindingDependencyObject::SetDataContext(value);
    } else {
        BindingDependencyObject::ClearDataContext();
    }
    isUpdatingDataContext_ = false;
}

void FrameworkElement::UpdatePropertyFromLocalDataContext(const std::any& value) {
    if (isUpdatingDataContext_) {
        return;
    }
    isUpdatingDataContext_ = true;
    if (value.has_value()) {
        SetValue(DataContextProperty(), value);
    } else {
        ClearValue(DataContextProperty());
    }
    isUpdatingDataContext_ = false;
}

void FrameworkElement::CoerceMaxToAtLeastMin() {
    const float minWidth = GetMinWidth();
    const float minHeight = GetMinHeight();
    float maxWidth = GetMaxWidth();
    float maxHeight = GetMaxHeight();

    if (std::isfinite(maxWidth) && maxWidth < minWidth) {
        SetValue(MaxWidthProperty(), minWidth);
    }
    if (std::isfinite(maxHeight) && maxHeight < minHeight) {
        SetValue(MaxHeightProperty(), minHeight);
    }
}

} // namespace fk::ui
