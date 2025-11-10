#pragma once

#include "fk/ui/Control.h"
#include "fk/binding/DependencyProperty.h"
#include <vector>
#include <any>

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
    ItemsControl() = default;
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

protected:
    /**
     * @brief 项目源变更钩子
     */
    virtual void OnItemsSourceChanged() {
        // 重新生成项目容器
        GenerateContainers();
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
        // TODO: 为每个数据项创建 UI 容器
    }
    
    /**
     * @brief 重新生成项目容器
     */
    virtual void RegenerateItemContainers() {
        // TODO: 清除并重建所有容器
    }
};

} // namespace fk::ui
