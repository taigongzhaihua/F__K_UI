#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/binding/PropertyStore.h"

#include <any>
#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace fk::ui {

// 前置声明
class PanelBase;
class UIElement;

class ItemsControl : public Control<ItemsControl> {
public:
    using Base = Control<ItemsControl>;
    using ItemTemplateFunc = std::function<std::shared_ptr<UIElement>(const std::any&)>;

    ItemsControl();
    ~ItemsControl() override;

    // 依赖属性声明
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(ItemsSource, std::vector<std::any>);
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(ItemTemplate, ItemTemplateFunc);
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(DisplayMemberPath, std::string);
    FK_DEPENDENCY_PROPERTY_DECLARE(AlternationCount, int);

public:

    void SetItemsPanel(std::shared_ptr<PanelBase> panel);
    [[nodiscard]] std::shared_ptr<PanelBase> GetItemsPanel() const noexcept { return itemsPanel_; }

    // 便利方法：访问内部 Panel 的子元素
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Children() const noexcept;
    [[nodiscard]] std::size_t GetChildCount() const noexcept;
    [[nodiscard]] bool HasChildren() const noexcept;

    void AddItem(std::shared_ptr<UIElement> item);
    bool RemoveItem(UIElement* item);
    void ClearItems();

    // 便利方法：批量操作
    void AppendItems(std::initializer_list<std::shared_ptr<UIElement>> items);
    void InsertItem(std::size_t index, std::shared_ptr<UIElement> item);
    [[nodiscard]] UIElement* GetItemAt(std::size_t index) const;
    [[nodiscard]] std::size_t GetItemCount() const noexcept;

    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Items() const noexcept;

    // 便利方法
    void ClearItemsSource();
    [[nodiscard]] bool HasItemsSource() const noexcept;
    void ClearItemTemplate();
    void ClearDisplayMemberPath();

    // SetDisplayMemberPath 便利重载（接受 string_view）
    void SetDisplayMemberPath(std::string_view path) {
        SetDisplayMemberPath(std::string(path));
    }
    
    // 获取元素的交替索引（附加属性）
    static int GetAlternationIndex(DependencyObject* element);

    // 🎯 绑定支持：链式 API
    // ItemsSource
    [[nodiscard]] const std::vector<std::any>& ItemsSource() const {
        return GetItemsSource();
    }
    
    Base::Ptr ItemsSource(std::vector<std::any> source) {
        SetItemsSource(std::move(source));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Base::Ptr ItemsSource(binding::Binding binding) {
        SetBinding(ItemsSourceProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // DisplayMemberPath
    [[nodiscard]] std::string DisplayMemberPath() const {
        return GetDisplayMemberPath();
    }
    
    Base::Ptr DisplayMemberPath(std::string_view path) {
        SetDisplayMemberPath(path);
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Base::Ptr DisplayMemberPath(binding::Binding binding) {
        SetBinding(DisplayMemberPathProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // AlternationCount
    [[nodiscard]] int AlternationCount() const {
        return GetAlternationCount();
    }
    
    Base::Ptr AlternationCount(int count) {
        SetAlternationCount(count);
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Base::Ptr AlternationCount(binding::Binding binding) {
        SetBinding(AlternationCountProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // ItemTemplate
    [[nodiscard]] ItemTemplateFunc ItemTemplate() const {
        return GetItemTemplate();
    }
    
    Base::Ptr ItemTemplate(ItemTemplateFunc templateFunc) {
        SetItemTemplate(std::move(templateFunc));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Base::Ptr ItemTemplate(binding::Binding binding) {
        SetBinding(ItemTemplateProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

private:
    void EnsurePanelContent();
    void AttachItem(const std::shared_ptr<UIElement>& item);
    void DetachItem(UIElement* item);
    void RebuildItems();
    void ClearItemsSourceInternal();
    void ApplyItemsSource(const std::vector<std::any>& source);
    void OnItemTemplateChanged();
    void UpdateAlternationIndexes();
    std::shared_ptr<UIElement> RealizeItem(const std::any& value);

    std::shared_ptr<PanelBase> itemsPanel_;
    std::vector<std::shared_ptr<UIElement>> items_;
};

} // namespace fk::ui
