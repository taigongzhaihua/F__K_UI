#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/DataTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include <memory>

namespace fk::ui {

/**
 * @brief 内容控件基类（CRTP 模式）
 * 
 * 职责：
 * - 单一内容展示
 * - 内容模板化
 * 
 * 模板参数：Derived - 派生类类型（CRTP）
 * 继承：Control<Derived>
 */
template<typename Derived>
class ContentControl : public Control<Derived> {
public:
    ContentControl() = default;
    virtual ~ContentControl() = default;

    // ========== 依赖属性声明 ==========
    
    /**
     * @brief 内容依赖属性
     */
    static const binding::DependencyProperty& ContentProperty();
    
    /**
     * @brief 内容模板依赖属性
     */
    static const binding::DependencyProperty& ContentTemplateProperty();

    // ========== 内容 ==========
    
    std::any GetContent() const {
        return this->template GetValue<std::any>(ContentProperty());
    }
    void SetContent(const std::any& value) {
        auto oldValue = GetContent();  // 保存旧值
        this->SetValue(ContentProperty(), value);
        
        // 如果内容是 UIElement*，获取其所有权
        if (value.type() == typeid(UIElement*)) {
            auto* element = std::any_cast<UIElement*>(value);
            if (element) {
                this->TakeOwnership(element);
            }
        }
        
        OnContentChanged(oldValue, value);
        this->InvalidateMeasure();
    }
    
    Derived* Content(const std::any& value) {
        SetContent(value);
        return static_cast<Derived*>(this);
    }
    std::any Content() const { return GetContent(); }

    // ========== 内容模板 ==========
    
    DataTemplate* GetContentTemplate() const {
        return this->template GetValue<DataTemplate*>(ContentTemplateProperty());
    }
    void SetContentTemplate(DataTemplate* tmpl) {
        this->SetValue(ContentTemplateProperty(), tmpl);
        OnContentTemplateChanged();
        this->InvalidateMeasure();
    }
    
    Derived* ContentTemplate(DataTemplate* tmpl) {
        SetContentTemplate(tmpl);
        return static_cast<Derived*>(this);
    }
    DataTemplate* ContentTemplate() const { return GetContentTemplate(); }

protected:
    /**
     * @brief 内容变更钩子
     * @param oldContent 旧内容
     * @param newContent 新内容
     */
    virtual void OnContentChanged(const std::any& oldContent, const std::any& newContent) {
        // 内容变化时，尝试应用 DataTemplate
        ApplyContentTemplate();
    }
    
    /**
     * @brief 内容模板变更钩子
     */
    virtual void OnContentTemplateChanged() {
        // 模板变化时，尝试应用 DataTemplate
        ApplyContentTemplate();
    }
    
    /**
     * @brief 应用 ContentTemplate 到 Content
     * 
     * 如果 Content 不是 UIElement 且存在 ContentTemplate，
     * 使用 DataTemplate 实例化视觉树并显示
     */
    virtual void ApplyContentTemplate();
};

// ApplyContentTemplate 实现
template<typename Derived>
void ContentControl<Derived>::ApplyContentTemplate() {
    auto content = GetContent();
    auto* tmpl = GetContentTemplate();
    
    // 如果内容已经是 UIElement，不需要模板化
    if (content.type() == typeid(UIElement*)) {
        return;
    }
    
    // 如果有模板且内容有效，应用模板
    if (tmpl && tmpl->IsValid() && content.has_value()) {
        // 实例化模板，传入数据上下文
        UIElement* visualTree = tmpl->Instantiate(content);
        
        if (visualTree) {
            // 将实例化的视觉树设置为内容
            // 注意：这会递归调用 OnContentChanged，但由于内容已是 UIElement，会返回
            SetContent(visualTree);
        }
    }
}

} // namespace fk::ui
