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
    if (const auto content = GetContent()) {
        content->Measure(availableSize);
        return content->DesiredSize();
    }
    return Size{};
}

Size ControlBase::ArrangeOverride(const Size& finalSize) {
    if (const auto content = GetContent()) {
        content->Arrange(Rect{ 0.0f, 0.0f, finalSize.width, finalSize.height });
    }
    return finalSize;
}

void ControlBase::OnContentChanged(UIElement*, UIElement*) {}

void ControlBase::OnIsFocusedChanged(bool, bool) {}

void ControlBase::OnTabIndexChanged(int, int) {}

void ControlBase::OnCursorChanged(const std::string&, const std::string&) {}

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
