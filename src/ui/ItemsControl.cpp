#include "fk/ui/ItemsControl.h"

#include "fk/ui/ContentControl.h"
#include "fk/ui/Panel.h"
#include "fk/ui/StackPanel.h"

#include <algorithm>
#include <utility>

namespace fk::ui {

using binding::DependencyProperty;

namespace {

constexpr bool IsVectorAnyType(const std::any& value) {
    return value.has_value() && value.type() == typeid(std::vector<std::any>);
}

// 验证回调
bool ValidateItemsSource(const std::any& value) {
    return !value.has_value() || value.type() == typeid(std::vector<std::any>);
}

bool ValidateItemTemplate(const std::any& value) {
    return !value.has_value() || value.type() == typeid(ItemsControl::ItemTemplateFunc);
}

bool ValidateDisplayMemberPath(const std::any& value) {
    return !value.has_value() || value.type() == typeid(std::string);
}

bool ValidateAlternationCount(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(int)) {
        return false;
    }
    int count = std::any_cast<int>(value);
    return count >= 0; // AlternationCount 必须非负
}

} // namespace

// 依赖属性定义
FK_DEPENDENCY_PROPERTY_REGISTER(ItemsControl, ItemsSource, std::vector<std::any>);
FK_DEPENDENCY_PROPERTY_IMPL_REF(ItemsControl, ItemsSource, std::vector<std::any>);
FK_DEPENDENCY_PROPERTY_CALLBACK_REF(ItemsControl, ItemsSource, std::vector<std::any>);

FK_DEPENDENCY_PROPERTY_REGISTER(ItemsControl, ItemTemplate, ItemsControl::ItemTemplateFunc);
FK_DEPENDENCY_PROPERTY_IMPL_REF(ItemsControl, ItemTemplate, ItemsControl::ItemTemplateFunc);
FK_DEPENDENCY_PROPERTY_CALLBACK_REF(ItemsControl, ItemTemplate, ItemsControl::ItemTemplateFunc);

FK_DEPENDENCY_PROPERTY_REGISTER(ItemsControl, DisplayMemberPath, std::string);
FK_DEPENDENCY_PROPERTY_IMPL_REF(ItemsControl, DisplayMemberPath, std::string);
FK_DEPENDENCY_PROPERTY_CALLBACK_REF(ItemsControl, DisplayMemberPath, std::string);

FK_DEPENDENCY_PROPERTY_REGISTER(ItemsControl, AlternationCount, int);
FK_DEPENDENCY_PROPERTY_IMPL(ItemsControl, AlternationCount, int, 0);
FK_DEPENDENCY_PROPERTY_CALLBACK(ItemsControl, AlternationCount, int, 0);

// BuildMetadata 实现
binding::PropertyMetadata ItemsControl::BuildItemsSourceMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::vector<std::any>{};
    metadata.propertyChangedCallback = &ItemsControl::ItemsSourcePropertyChanged;
    metadata.validateCallback = &ValidateItemsSource;
    return metadata;
}

binding::PropertyMetadata ItemsControl::BuildItemTemplateMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = ItemsControl::ItemTemplateFunc{};
    metadata.propertyChangedCallback = &ItemsControl::ItemTemplatePropertyChanged;
    metadata.validateCallback = &ValidateItemTemplate;
    return metadata;
}

binding::PropertyMetadata ItemsControl::BuildDisplayMemberPathMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{};
    metadata.propertyChangedCallback = &ItemsControl::DisplayMemberPathPropertyChanged;
    metadata.validateCallback = &ValidateDisplayMemberPath;
    return metadata;
}

binding::PropertyMetadata ItemsControl::BuildAlternationCountMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &ItemsControl::AlternationCountPropertyChanged;
    metadata.validateCallback = &ValidateAlternationCount;
    return metadata;
}

ItemsControl::ItemsControl() {
    itemsPanel_ = std::make_shared<StackPanel>();
    EnsurePanelContent();
}

ItemsControl::~ItemsControl() = default;

void ItemsControl::SetItemsPanel(std::shared_ptr<PanelBase> panel) {
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

std::span<const std::shared_ptr<UIElement>> ItemsControl::Children() const noexcept {
    if (!itemsPanel_) {
        return {};
    }
    return itemsPanel_->GetChildren();
}

std::size_t ItemsControl::GetChildCount() const noexcept {
    if (!itemsPanel_) {
        return 0;
    }
    return itemsPanel_->GetChildCount();
}

bool ItemsControl::HasChildren() const noexcept {
    if (!itemsPanel_) {
        return false;
    }
    return itemsPanel_->HasChildren();
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

void ItemsControl::AppendItems(std::initializer_list<std::shared_ptr<UIElement>> newItems) {
    if (HasItemsSource()) {
        ClearItemsSourceInternal();
    }
    
    for (auto& item : newItems) {
        if (!item) {
            continue;
        }
        items_.push_back(item);
        AttachItem(item);
    }
    
    UpdateAlternationIndexes();
}

void ItemsControl::InsertItem(std::size_t index, std::shared_ptr<UIElement> item) {
    if (HasItemsSource()) {
        ClearItemsSourceInternal();
    }
    
    if (!item) {
        return;
    }
    
    if (index >= items_.size()) {
        AddItem(std::move(item));
        return;
    }
    
    items_.insert(items_.begin() + index, item);
    RebuildItems();
}

UIElement* ItemsControl::GetItemAt(std::size_t index) const {
    if (index >= items_.size()) {
        return nullptr;
    }
    return items_[index].get();
}

std::size_t ItemsControl::GetItemCount() const noexcept {
    return items_.size();
}

std::span<const std::shared_ptr<UIElement>> ItemsControl::Items() const noexcept {
    return { items_.data(), items_.size() };
}

void ItemsControl::ClearItemsSource() {
    ClearItemsSourceInternal();
}

bool ItemsControl::HasItemsSource() const noexcept {
    return GetValueSource(ItemsSourceProperty()) != binding::ValueSource::Default;
}

void ItemsControl::ClearItemTemplate() {
    ClearValue(ItemTemplateProperty());
}

void ItemsControl::ClearDisplayMemberPath() {
    ClearValue(DisplayMemberPathProperty());
}

int ItemsControl::GetAlternationIndex(DependencyObject* element) {
    if (!element) {
        return 0;
    }
    
    // AlternationIndex 作为附加属性存储在元素上
    static const auto& property = DependencyProperty::RegisterAttached(
        "AlternationIndex",
        typeid(int),
        typeid(ItemsControl),
        binding::PropertyMetadata(0));
    
    return element->GetValue<int>(property);
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
    
    // 更新交替索引
    UpdateAlternationIndexes();
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
    
    // 应用交替索引
    UpdateAlternationIndexes();
}

void ItemsControl::OnItemTemplateChanged() {
    if (HasItemsSource()) {
        ApplyItemsSource(GetItemsSource());
    }
}

void ItemsControl::UpdateAlternationIndexes() {
    if (!itemsPanel_) {
        return;
    }
    
    int alternationCount = GetAlternationCount();
    if (alternationCount <= 0) {
        return; // 不使用交替索引
    }
    
    static const auto& property = DependencyProperty::RegisterAttached(
        "AlternationIndex",
        typeid(int),
        typeid(ItemsControl),
        binding::PropertyMetadata(0));
    
    auto children = itemsPanel_->GetChildren();
    for (size_t i = 0; i < children.size(); ++i) {
        int index = static_cast<int>(i % alternationCount);
        children[i]->SetValue(property, index);
    }
}

std::shared_ptr<UIElement> ItemsControl::RealizeItem(const std::any& value) {
    auto templateFunc = GetItemTemplate();
    if (templateFunc) {
        if (auto generated = templateFunc(value)) {
            return generated;
        }
    }

    // 如果没有 ItemTemplate，尝试使用 DisplayMemberPath
    auto displayPath = GetDisplayMemberPath();
    if (!displayPath.empty()) {
        // TODO: 实现属性路径解析，现在简单地将 value 转为字符串
        // 需要反射机制或类型擦除来访问成员
        // 暂时返回 nullptr，等待完整实现
    }

    if (value.type() == typeid(std::shared_ptr<UIElement>)) {
        return std::any_cast<std::shared_ptr<UIElement>>(value);
    }

    // 最后尝试将 value 转为字符串显示
    // 可以创建一个 TextBlock 来显示文本（需要 TextBlock 类）
    
    return nullptr;
}

// OnChanged 回调实现
void ItemsControl::OnItemsSourceChanged(const std::vector<std::any>& oldValue, const std::vector<std::any>& newValue) {
    ApplyItemsSource(newValue);
}

void ItemsControl::OnItemTemplateChanged(const ItemTemplateFunc& oldValue, const ItemTemplateFunc& newValue) {
    OnItemTemplateChanged();
}

void ItemsControl::OnDisplayMemberPathChanged(const std::string& oldValue, const std::string& newValue) {
    // DisplayMemberPath 改变时，重新生成 items
    if (HasItemsSource()) {
        ApplyItemsSource(GetItemsSource());
    }
}

void ItemsControl::OnAlternationCountChanged(int oldValue, int newValue) {
    // AlternationCount 改变时，更新所有容器的 AlternationIndex
    UpdateAlternationIndexes();
}

} // namespace fk::ui
