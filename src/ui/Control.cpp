#include "fk/ui/Control.h"

#include <utility>

namespace fk::ui::detail {

using binding::DependencyProperty;

ControlBase::ControlBase() = default;

ControlBase::~ControlBase() = default;

const DependencyProperty& ControlBase::IsFocusedProperty() {
    static const auto& property = DependencyProperty::Register(
        "IsFocused",
        typeid(bool),
        typeid(ControlBase),
        BuildIsFocusedMetadata());
    return property;
}

const DependencyProperty& ControlBase::TabIndexProperty() {
    static const auto& property = DependencyProperty::Register(
        "TabIndex",
        typeid(int),
        typeid(ControlBase),
        BuildTabIndexMetadata());
    return property;
}

const DependencyProperty& ControlBase::CursorProperty() {
    static const auto& property = DependencyProperty::Register(
        "Cursor",
        typeid(std::string),
        typeid(ControlBase),
        BuildCursorMetadata());
    return property;
}

const DependencyProperty& ControlBase::ContentProperty() {
    static const auto& property = DependencyProperty::Register(
        "Content",
        typeid(std::shared_ptr<UIElement>),
        typeid(ControlBase),
        BuildContentMetadata());
    return property;
}

const DependencyProperty& ControlBase::PaddingProperty() {
    static const auto& property = DependencyProperty::Register(
        "Padding",
        typeid(fk::Thickness),
        typeid(ControlBase),
        BuildPaddingMetadata());
    return property;
}

void ControlBase::SetIsFocused(bool value) {
    VerifyAccess();
    SetValue(IsFocusedProperty(), value);
}

bool ControlBase::IsFocused() const {
    return GetValue<bool>(IsFocusedProperty());
}

void ControlBase::SetTabIndex(int value) {
    VerifyAccess();
    SetValue(TabIndexProperty(), value);
}

int ControlBase::GetTabIndex() const {
    return GetValue<int>(TabIndexProperty());
}

void ControlBase::SetCursor(std::string cursor) {
    VerifyAccess();
    SetValue(CursorProperty(), std::move(cursor));
}

const std::string& ControlBase::GetCursor() const {
    return GetValue<const std::string&>(CursorProperty());
}

void ControlBase::SetContent(std::shared_ptr<UIElement> content) {
    VerifyAccess();
    SetValue(ContentProperty(), std::move(content));
}

void ControlBase::ClearContent() {
    VerifyAccess();
    ClearValue(ContentProperty());
}

std::shared_ptr<UIElement> ControlBase::GetContent() const {
    return GetValue<std::shared_ptr<UIElement>>(ContentProperty());
}

void ControlBase::SetPadding(const fk::Thickness& padding) {
    VerifyAccess();
    SetValue(PaddingProperty(), padding);
}

const fk::Thickness& ControlBase::GetPadding() const {
    return GetValue<const fk::Thickness&>(PaddingProperty());
}

void ControlBase::OnAttachedToLogicalTree() {
    FrameworkElement::OnAttachedToLogicalTree();
    SyncContentAttachment();
}

void ControlBase::OnDetachedFromLogicalTree() {
    if (auto content = GetContent()) {
        DetachContent(content.get());
    }
    FrameworkElement::OnDetachedFromLogicalTree();
}

Size ControlBase::MeasureOverride(const Size& availableSize) {
    const auto& padding = GetPadding();
    const float paddingWidth = padding.left + padding.right;
    const float paddingHeight = padding.top + padding.bottom;

    if (const auto content = GetContent()) {
        // 测量内容时减去 Padding
        const Size childAvailable{
            std::max(0.0f, availableSize.width - paddingWidth),
            std::max(0.0f, availableSize.height - paddingHeight)
        };
        content->Measure(childAvailable);
        
        // 返回内容大小加上 Padding
        const auto& childDesired = content->DesiredSize();
        return Size{
            childDesired.width + paddingWidth,
            childDesired.height + paddingHeight
        };
    }
    
    // 没有内容时,只返回 Padding 大小
    return Size{paddingWidth, paddingHeight};
}

Size ControlBase::ArrangeOverride(const Size& finalSize) {
    if (const auto content = GetContent()) {
        const auto& padding = GetPadding();
        
        // 计算内容区域(减去 Padding)
        const float contentWidth = std::max(0.0f, finalSize.width - padding.left - padding.right);
        const float contentHeight = std::max(0.0f, finalSize.height - padding.top - padding.bottom);
        
        // 在 Padding 区域内排列内容
        content->Arrange(Rect{
            padding.left,
            padding.top,
            contentWidth,
            contentHeight
        });
    }
    return finalSize;
}

std::vector<Visual*> ControlBase::GetVisualChildren() const {
    std::vector<Visual*> children;
    if (auto content = GetContent()) {
        children.push_back(content.get());
    }
    return children;
}

void ControlBase::OnContentChanged(UIElement*, UIElement*) {}

void ControlBase::OnIsFocusedChanged(bool, bool) {}

void ControlBase::OnTabIndexChanged(int, int) {}

void ControlBase::OnCursorChanged(const std::string&, const std::string&) {}

void ControlBase::OnPaddingChanged(const fk::Thickness&, const fk::Thickness&) {}

binding::PropertyMetadata ControlBase::BuildIsFocusedMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = false;
    metadata.propertyChangedCallback = &ControlBase::IsFocusedPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildTabIndexMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &ControlBase::TabIndexPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateTabIndex;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildCursorMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string("arrow");
    metadata.propertyChangedCallback = &ControlBase::CursorPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateCursor;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildContentMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::shared_ptr<UIElement>{};
    metadata.propertyChangedCallback = &ControlBase::ContentPropertyChanged;
    metadata.validateCallback = &ControlBase::ValidateContent;
    return metadata;
}

binding::PropertyMetadata ControlBase::BuildPaddingMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = fk::Thickness{};
    metadata.propertyChangedCallback = &ControlBase::PaddingPropertyChanged;
    return metadata;
}

void ControlBase::IsFocusedPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }
    if (!oldValue.has_value() || !newValue.has_value()) {
        return;
    }
    const bool oldFocused = std::any_cast<bool>(oldValue);
    const bool newFocused = std::any_cast<bool>(newValue);
    if (oldFocused == newFocused) {
        return;
    }
    control->OnIsFocusedChanged(oldFocused, newFocused);
}

void ControlBase::TabIndexPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }
    if (!oldValue.has_value() || !newValue.has_value()) {
        return;
    }
    const int oldIndex = std::any_cast<int>(oldValue);
    const int newIndex = std::any_cast<int>(newValue);
    if (oldIndex == newIndex) {
        return;
    }
    control->OnTabIndexChanged(oldIndex, newIndex);
}

void ControlBase::CursorPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }
    if (!oldValue.has_value() || !newValue.has_value()) {
        return;
    }
    const auto& oldCursor = std::any_cast<const std::string&>(oldValue);
    const auto& newCursor = std::any_cast<const std::string&>(newValue);
    if (oldCursor == newCursor) {
        return;
    }
    control->OnCursorChanged(oldCursor, newCursor);
}

void ControlBase::ContentPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }

    const auto oldContent = ToElement(oldValue);
    const auto newContent = ToElement(newValue);

    if (oldContent && newContent && oldContent.get() == newContent.get()) {
        return;
    }

    if (oldContent) {
        control->DetachContent(oldContent.get());
    }

    if (newContent) {
        control->AttachContent(newContent.get());
    }

    control->OnContentChanged(oldContent.get(), newContent.get());
    control->InvalidateMeasure();
    control->InvalidateArrange();
}

void ControlBase::PaddingPropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
    auto* control = dynamic_cast<ControlBase*>(&sender);
    if (!control) {
        return;
    }
    if (!oldValue.has_value() || !newValue.has_value()) {
        return;
    }
    const auto& oldPadding = std::any_cast<const fk::Thickness&>(oldValue);
    const auto& newPadding = std::any_cast<const fk::Thickness&>(newValue);
    control->OnPaddingChanged(oldPadding, newPadding);
    control->InvalidateMeasure();
    control->InvalidateArrange();
}

bool ControlBase::ValidateTabIndex(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(int)) {
        return false;
    }
    return std::any_cast<int>(value) >= 0;
}

bool ControlBase::ValidateCursor(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(std::string);
}

bool ControlBase::ValidateContent(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(std::shared_ptr<UIElement>);
}

std::shared_ptr<UIElement> ControlBase::ToElement(const std::any& value) {
    if (!value.has_value()) {
        return {};
    }
    if (value.type() != typeid(std::shared_ptr<UIElement>)) {
        return {};
    }
    return std::any_cast<std::shared_ptr<UIElement>>(value);
}

void ControlBase::AttachContent(UIElement* content) {
    if (!content) {
        return;
    }
    if (content->GetLogicalParent() == this) {
        if (IsAttachedToLogicalTree() && !content->IsAttachedToLogicalTree()) {
            AddLogicalChild(content);
        }
        return;
    }
    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(content);
    } else {
        binding::DependencyObject::AddLogicalChild(content);
    }
}

void ControlBase::DetachContent(UIElement* content) {
    if (!content) {
        return;
    }
    if (content->GetLogicalParent() != this) {
        return;
    }
    RemoveLogicalChild(content);
}

void ControlBase::SyncContentAttachment() {
    if (const auto content = GetContent()) {
        AttachContent(content.get());
    }
}

} // namespace fk::ui::detail
