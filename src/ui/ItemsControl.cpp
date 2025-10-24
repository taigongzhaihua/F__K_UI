#include "fk/ui/ItemsControl.h"

#include "fk/ui/ContentControl.h"
#include "fk/ui/StackPanel.h"

#include <algorithm>
#include <utility>

namespace fk::ui {

using binding::DependencyProperty;

namespace {

constexpr bool IsVectorAnyType(const std::any& value) {
    return value.has_value() && value.type() == typeid(std::vector<std::any>);
}

constexpr bool IsTemplateFuncType(const std::any& value) {
    return value.has_value() && value.type() == typeid(ItemsControl::ItemTemplateFunc);
}

} // namespace

const DependencyProperty& ItemsControl::ItemsSourceProperty() {
    static const auto& property = DependencyProperty::Register(
        "ItemsSource",
        typeid(std::vector<std::any>),
        typeid(ItemsControl),
        BuildItemsSourceMetadata());
    return property;
}

const DependencyProperty& ItemsControl::ItemTemplateProperty() {
    static const auto& property = DependencyProperty::Register(
        "ItemTemplate",
        typeid(ItemTemplateFunc),
        typeid(ItemsControl),
        BuildItemTemplateMetadata());
    return property;
}

ItemsControl::ItemsControl() {
    itemsPanel_ = std::make_shared<StackPanel>();
    EnsurePanelContent();
}

ItemsControl::~ItemsControl() = default;

void ItemsControl::SetItemsPanel(std::shared_ptr<Panel> panel) {
    if (itemsPanel_ == panel) {
        return;
    }

    if (!panel) {
        panel = std::make_shared<StackPanel>();
    }

    itemsPanel_ = std::move(panel);
    EnsurePanelContent();
    RebuildItems();
}

void ItemsControl::AddItem(std::shared_ptr<UIElement> item) {
    if (HasItemsSource()) {
        ClearItemsSourceInternal();
    }

    if (!item) {
        return;
    }

    const auto alreadyPresent = std::any_of(items_.begin(), items_.end(), [raw = item.get()](const auto& existing) {
        return existing.get() == raw;
    });
    if (alreadyPresent) {
        return;
    }

    items_.push_back(std::move(item));
    AttachItem(items_.back());
}

bool ItemsControl::RemoveItem(UIElement* item) {
    if (HasItemsSource()) {
        return false;
    }

    if (!item) {
        return false;
    }

    const auto it = std::find_if(items_.begin(), items_.end(), [item](const auto& existing) {
        return existing.get() == item;
    });
    if (it == items_.end()) {
        return false;
    }

    DetachItem(it->get());
    items_.erase(it);
    return true;
}

void ItemsControl::ClearItems() {
    if (HasItemsSource()) {
        ClearItemsSource();
        return;
    }

    if (itemsPanel_) {
        itemsPanel_->ClearChildren();
    }
    items_.clear();
}

std::span<const std::shared_ptr<UIElement>> ItemsControl::Items() const noexcept {
    return { items_.data(), items_.size() };
}

void ItemsControl::SetItemsSource(std::vector<std::any> source) {
    SetValue(ItemsSourceProperty(), std::move(source));
}

const std::vector<std::any>& ItemsControl::GetItemsSource() const {
    return GetValue<const std::vector<std::any>&>(ItemsSourceProperty());
}

void ItemsControl::ClearItemsSource() {
    ClearItemsSourceInternal();
}

bool ItemsControl::HasItemsSource() const noexcept {
    return GetValueSource(ItemsSourceProperty()) != binding::ValueSource::Default;
}

void ItemsControl::SetItemTemplate(ItemTemplateFunc templateFunc) {
    SetValue(ItemTemplateProperty(), std::move(templateFunc));
}

ItemsControl::ItemTemplateFunc ItemsControl::GetItemTemplate() const {
    return GetValue<ItemTemplateFunc>(ItemTemplateProperty());
}

void ItemsControl::ClearItemTemplate() {
    ClearValue(ItemTemplateProperty());
}

void ItemsControl::OnAttachedToLogicalTree() {
    Base::OnAttachedToLogicalTree();
    EnsurePanelContent();
    if (HasItemsSource()) {
        ApplyItemsSource(GetItemsSource());
    } else {
        RebuildItems();
    }
}

void ItemsControl::OnDetachedFromLogicalTree() {
    if (itemsPanel_) {
        itemsPanel_->ClearChildren();
    }
    Base::OnDetachedFromLogicalTree();
}

void ItemsControl::EnsurePanelContent() {
    if (!itemsPanel_) {
        itemsPanel_ = std::make_shared<StackPanel>();
    }
    if (GetContent() != itemsPanel_) {
        SetContent(itemsPanel_);
    }
}

void ItemsControl::AttachItem(const std::shared_ptr<UIElement>& item) {
    if (!item || !itemsPanel_) {
        return;
    }
    itemsPanel_->AddChild(item);
}

void ItemsControl::DetachItem(UIElement* item) {
    if (!item || !itemsPanel_) {
        return;
    }
    itemsPanel_->RemoveChild(item);
}

void ItemsControl::RebuildItems() {
    if (!itemsPanel_) {
        return;
    }
    itemsPanel_->ClearChildren();
    for (const auto& item : items_) {
        if (!item) {
            continue;
        }
        itemsPanel_->AddChild(item);
    }
}

void ItemsControl::ClearItemsSourceInternal() {
    if (!HasItemsSource()) {
        return;
    }
    ClearValue(ItemsSourceProperty());
    items_.clear();
    if (itemsPanel_) {
        itemsPanel_->ClearChildren();
    }
}

void ItemsControl::ApplyItemsSource(const std::vector<std::any>& source) {
    if (itemsPanel_) {
        itemsPanel_->ClearChildren();
    }
    items_.clear();
    items_.reserve(source.size());

    for (const auto& entry : source) {
        auto element = RealizeItem(entry);
        if (!element) {
            continue;
        }
        items_.push_back(element);
        AttachItem(items_.back());
    }
}

void ItemsControl::OnItemTemplateChanged() {
    if (HasItemsSource()) {
        ApplyItemsSource(GetItemsSource());
    }
}

std::shared_ptr<UIElement> ItemsControl::RealizeItem(const std::any& value) {
    auto templateFunc = GetItemTemplate();
    if (templateFunc) {
        if (auto generated = templateFunc(value)) {
            return generated;
        }
    }

    if (value.type() == typeid(std::shared_ptr<UIElement>)) {
        return std::any_cast<std::shared_ptr<UIElement>>(value);
    }

    return nullptr;
}

binding::PropertyMetadata ItemsControl::BuildItemsSourceMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::vector<std::any>{};
    metadata.propertyChangedCallback = &ItemsControl::ItemsSourcePropertyChanged;
    metadata.validateCallback = &ItemsControl::ValidateItemsSource;
    return metadata;
}

binding::PropertyMetadata ItemsControl::BuildItemTemplateMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = ItemTemplateFunc{};
    metadata.propertyChangedCallback = &ItemsControl::ItemTemplatePropertyChanged;
    metadata.validateCallback = &ItemsControl::ValidateItemTemplate;
    return metadata;
}

void ItemsControl::ItemsSourcePropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any&, const std::any& newValue) {
    auto* control = dynamic_cast<ItemsControl*>(&sender);
    if (!control) {
        return;
    }
    if (!IsVectorAnyType(newValue)) {
        return;
    }
    const auto& source = std::any_cast<const std::vector<std::any>&>(newValue);
    control->ApplyItemsSource(source);
}

void ItemsControl::ItemTemplatePropertyChanged(binding::DependencyObject& sender, const DependencyProperty&, const std::any&, const std::any& newValue) {
    auto* control = dynamic_cast<ItemsControl*>(&sender);
    if (!control) {
        return;
    }
    if (!IsTemplateFuncType(newValue)) {
        return;
    }
    control->OnItemTemplateChanged();
}

bool ItemsControl::ValidateItemsSource(const std::any& value) {
    return !value.has_value() || value.type() == typeid(std::vector<std::any>);
}

bool ItemsControl::ValidateItemTemplate(const std::any& value) {
    return !value.has_value() || value.type() == typeid(ItemTemplateFunc);
}

} // namespace fk::ui
