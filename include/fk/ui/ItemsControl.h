#pragma once

#include "fk/ui/ContentControl.h"
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

    static const binding::DependencyProperty& ItemsSourceProperty();
    static const binding::DependencyProperty& ItemTemplateProperty();
    static const binding::DependencyProperty& DisplayMemberPathProperty();
    static const binding::DependencyProperty& AlternationCountProperty();

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

    void SetItemsSource(std::vector<std::any> source);
    [[nodiscard]] const std::vector<std::any>& GetItemsSource() const;
    void ClearItemsSource();
    [[nodiscard]] bool HasItemsSource() const noexcept;

    void SetItemTemplate(ItemTemplateFunc templateFunc);
    [[nodiscard]] ItemTemplateFunc GetItemTemplate() const;
    void ClearItemTemplate();

    // DisplayMemberPath: 简化简单数据绑定场景
    void SetDisplayMemberPath(std::string_view path);
    [[nodiscard]] std::string GetDisplayMemberPath() const;
    void ClearDisplayMemberPath();

    // AlternationCount: 支持奇偶行交替样式
    void SetAlternationCount(int count);
    [[nodiscard]] int GetAlternationCount() const;
    
    // 获取元素的交替索引（附加属性）
    static int GetAlternationIndex(DependencyObject* element);

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

    static binding::PropertyMetadata BuildItemsSourceMetadata();
    static binding::PropertyMetadata BuildItemTemplateMetadata();
    static binding::PropertyMetadata BuildDisplayMemberPathMetadata();
    static binding::PropertyMetadata BuildAlternationCountMetadata();
    
    static void ItemsSourcePropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void ItemTemplatePropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void DisplayMemberPathPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void AlternationCountPropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);

    static bool ValidateItemsSource(const std::any& value);
    static bool ValidateItemTemplate(const std::any& value);
    static bool ValidateDisplayMemberPath(const std::any& value);
    static bool ValidateAlternationCount(const std::any& value);

    std::shared_ptr<PanelBase> itemsPanel_;
    std::vector<std::shared_ptr<UIElement>> items_;
};

} // namespace fk::ui
