#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/Panel.h"
#include "fk/binding/PropertyStore.h"

#include <any>
#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace fk::ui {

class ItemsControl : public Control<ItemsControl> {
public:
    using Base = Control<ItemsControl>;
    using ItemTemplateFunc = std::function<std::shared_ptr<UIElement>(const std::any&)>;

    ItemsControl();
    ~ItemsControl() override;

    static const binding::DependencyProperty& ItemsSourceProperty();
    static const binding::DependencyProperty& ItemTemplateProperty();

    void SetItemsPanel(std::shared_ptr<Panel> panel);
    [[nodiscard]] std::shared_ptr<Panel> GetItemsPanel() const noexcept { return itemsPanel_; }

    void AddItem(std::shared_ptr<UIElement> item);
    bool RemoveItem(UIElement* item);
    void ClearItems();

    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Items() const noexcept;

    void SetItemsSource(std::vector<std::any> source);
    [[nodiscard]] const std::vector<std::any>& GetItemsSource() const;
    void ClearItemsSource();
    [[nodiscard]] bool HasItemsSource() const noexcept;

    void SetItemTemplate(ItemTemplateFunc templateFunc);
    [[nodiscard]] ItemTemplateFunc GetItemTemplate() const;
    void ClearItemTemplate();

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
    std::shared_ptr<UIElement> RealizeItem(const std::any& value);

    static binding::PropertyMetadata BuildItemsSourceMetadata();
    static binding::PropertyMetadata BuildItemTemplateMetadata();
    static void ItemsSourcePropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);
    static void ItemTemplatePropertyChanged(binding::DependencyObject& sender, const binding::DependencyProperty& property,
        const std::any& oldValue, const std::any& newValue);

    static bool ValidateItemsSource(const std::any& value);
    static bool ValidateItemTemplate(const std::any& value);

    std::shared_ptr<Panel> itemsPanel_;
    std::vector<std::shared_ptr<UIElement>> items_;
};

} // namespace fk::ui
