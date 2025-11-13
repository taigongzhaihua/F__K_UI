#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/ObservableCollection.h"
#include "fk/ui/ItemContainerGenerator.h"
#include "fk/binding/DependencyProperty.h"
#include <vector>
#include <any>
#include <memory>

namespace fk::ui {

// 前向声明
class DataTemplate;
template<typename Derived> class Panel;

/**
 * @brief 项目控件基类（CRTP 模式）
 * 
 * 职责：
 * - 多项目集合展示
 * - 项目模板化
 * - 项目面板定制
 * 
 * 模板参数：Derived - 派生类类型（CRTP）
 * 继承：Control<Derived>
 */
template<typename Derived>
class ItemsControl : public Control<Derived> {
public:
    // ========== 依赖属性 ==========
    
    /// ItemsSource 属性：项目数据源
    static const binding::DependencyProperty& ItemsSourceProperty();
    
    /// ItemTemplate 属性：项目模板
    static const binding::DependencyProperty& ItemTemplateProperty();
    
    /// ItemsPanel 属性：项目面板
    static const binding::DependencyProperty& ItemsPanelProperty();

public:
    ItemsControl() : generator_(reinterpret_cast<ItemsControl<void>*>(this)) {
        // Configure generator with default factory and preparer
        generator_.SetContainerFactory([this](const std::any& item) -> UIElement* {
            return this->GetContainerForItem(item);
        });
        
        generator_.SetContainerPreparer([this](UIElement* container, const std::any& item) {
            this->PrepareContainerForItem(container, item);
        });
        
        generator_.SetEnableRecycling(true);  // Enable recycling by default
    }
    
    virtual ~ItemsControl() = default;

    // ========== 项目源 ==========
    
    std::any GetItemsSource() const { return this->template GetValue<std::any>(ItemsSourceProperty()); }
    void SetItemsSource(const std::any& items) { 
        this->SetValue(ItemsSourceProperty(), items);
        OnItemsSourceChanged();
    }
    
    Derived* ItemsSource(const std::any& items) {
        SetItemsSource(items);
        return static_cast<Derived*>(this);
    }
    std::any ItemsSource() const { return GetItemsSource(); }

    // ========== 项目模板 ==========
    
    DataTemplate* GetItemTemplate() const { return this->template GetValue<DataTemplate*>(ItemTemplateProperty()); }
    void SetItemTemplate(DataTemplate* tmpl) {
        this->SetValue(ItemTemplateProperty(), tmpl);
        OnItemTemplateChanged();
    }
    
    Derived* ItemTemplate(DataTemplate* tmpl) {
        SetItemTemplate(tmpl);
        return static_cast<Derived*>(this);
    }
    DataTemplate* ItemTemplate() const { return GetItemTemplate(); }

    // ========== 项目面板 ==========
    
    UIElement* GetItemsPanel() const { return this->template GetValue<UIElement*>(ItemsPanelProperty()); }
    void SetItemsPanel(UIElement* panel) {
        this->SetValue(ItemsPanelProperty(), panel);
        this->InvalidateMeasure();
    }
    
    Derived* ItemsPanel(UIElement* panel) {
        SetItemsPanel(panel);
        return static_cast<Derived*>(this);
    }
    UIElement* ItemsPanel() const { return GetItemsPanel(); }

    // ========== 直接项集合 ==========
    
    /**
     * @brief 获取直接项集合（当没有 ItemsSource 时使用）
     */
    ObservableCollection& GetItems() {
        if (!items_) {
            items_ = std::make_unique<ObservableCollection>();
            // 监听集合变更
            items_->CollectionChanged() += [this](const CollectionChangedEventArgs& args) {
                OnItemsChanged(args);
            };
        }
        return *items_;
    }

    // ========== ItemContainerGenerator ==========
    
    /**
     * @brief 获取容器生成器
     */
    ItemContainerGenerator& GetItemContainerGenerator() {
        return generator_;
    }
    
    const ItemContainerGenerator& GetItemContainerGenerator() const {
        return generator_;
    }

    // ========== 容器生成 ==========
    
    /**
     * @brief 判断项是否已经是容器
     */
    virtual bool IsItemItsOwnContainer(const std::any& item) const {
        // 检查是否是 UIElement*
        try {
            auto* element = std::any_cast<UIElement*>(item);
            return element != nullptr;
        } catch (...) {
            return false;
        }
    }
    
    /**
     * @brief 为数据项创建容器
     */
    virtual UIElement* GetContainerForItem(const std::any& item) {
        // 默认实现：如果项本身是 UIElement，直接使用
        if (IsItemItsOwnContainer(item)) {
            return std::any_cast<UIElement*>(item);
        }
        
        // 否则创建一个 ContentControl 作为容器
        // TODO: 使用 ItemTemplate 创建内容
        return nullptr;
    }
    
    /**
     * @brief 准备容器（设置数据上下文等）
     */
    virtual void PrepareContainerForItem(UIElement* container, const std::any& item) {
        if (!container) return;
        
        // 设置数据上下文
        container->SetDataContext(item);
        
        // TODO: 应用 ItemTemplate
    }

protected:
    /**
     * @brief 项目源变更钩子
     */
    virtual void OnItemsSourceChanged() {
        // 当设置了 ItemsSource，清空 Items 集合
        if (items_) {
            items_->Clear();
        }
        
        // 重新生成项目容器
        GenerateContainers();
    }
    
    /**
     * @brief Items 集合变更钩子
     */
    virtual void OnItemsChanged(const CollectionChangedEventArgs& args) {
        // Items 集合变更时，更新显示
        try {
            switch (args.action) {
                case CollectionChangeAction::Add:
                    // 为新项生成容器
                    for (const auto& item : args.newItems) {
                        AddItemContainer(item);
                    }
                    break;
                    
                case CollectionChangeAction::Remove:
                    // 移除旧项的容器
                    for (const auto& item : args.oldItems) {
                        RemoveItemContainer(item);
                    }
                    break;
                    
                case CollectionChangeAction::Reset:
                    // 清空并重建
                    RegenerateItemContainers();
                    break;
                    
                default:
                    break;
            }
        } catch (const std::bad_any_cast&) {
            // 忽略集合操作中的any_cast错误
            // 这可能发生在清空或重建容器时
        } catch (...) {
            // 忽略其他错误
        }
        
        this->InvalidateMeasure();
    }
    
    /**
     * @brief 项目模板变更钩子
     */
    virtual void OnItemTemplateChanged() {
        // 重新应用模板到所有项目
        RegenerateItemContainers();
    }
    
    /**
     * @brief 生成项目容器
     */
    virtual void GenerateContainers() {
        // 清除现有容器
        ClearItemContainers();
        
        // 从 ItemsSource 或 Items 生成容器
        std::any itemsSource = GetItemsSource();
        
        if (itemsSource.has_value()) {
            // TODO: 遍历 ItemsSource 生成容器
            // 需要实现 IEnumerable 接口支持
        } else if (items_) {
            // 从 Items 集合生成 - 使用 ItemContainerGenerator
            std::vector<std::any> itemVector;
            for (const auto& item : items_->GetItems()) {
                itemVector.push_back(item);
            }
            
            auto containers = generator_.GenerateContainers(itemVector);
            // TODO: 添加到面板中
        }
    }
    
    /**
     * @brief 重新生成项目容器
     */
    virtual void RegenerateItemContainers() {
        GenerateContainers();
        this->InvalidateMeasure();
    }
    
    /**
     * @brief 为单个项添加容器
     */
    virtual void AddItemContainer(const std::any& item) {
        bool isNew = false;
        UIElement* container = generator_.GenerateContainer(item, isNew);
        if (container) {
            // TODO: 添加到面板中
            this->InvalidateMeasure();
        }
    }
    
    /**
     * @brief 移除单个项的容器
     */
    virtual void RemoveItemContainer(const std::any& item) {
        auto* container = generator_.ContainerFromItem(item);
        if (container) {
            generator_.RecycleContainer(container);
            // TODO: 从面板中移除
            this->InvalidateMeasure();
        }
    }
    
    /**
     * @brief 清除所有项容器
     */
    virtual void ClearItemContainers() {
        generator_.RemoveAll();
        this->InvalidateMeasure();
    }

private:
    std::unique_ptr<ObservableCollection> items_;           // 直接项集合
    ItemContainerGenerator generator_;                      // 容器生成器
};

} // namespace fk::ui
