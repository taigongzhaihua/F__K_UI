#pragma once

#include "fk/ui/Control.h"
#include "fk/binding/DependencyProperty.h"
#include <memory>

namespace fk::ui {

// 前向声明
class DataTemplate;

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
        this->SetValue(ContentProperty(), value);
        
        // 如果内容是 UIElement*，获取其所有权
        if (value.type() == typeid(UIElement*)) {
            auto* element = std::any_cast<UIElement*>(value);
            if (element) {
                this->TakeOwnership(element);
            }
        }
        
        OnContentChanged();
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
     */
    virtual void OnContentChanged() {}
    
    /**
     * @brief 内容模板变更钩子
     */
    virtual void OnContentTemplateChanged() {}
};

} // namespace fk::ui
