#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/DataTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include <memory>
#include <string>

namespace fk::ui {

// 前向声明
class TextBlock;

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
    ContentControl() : contentElement_(nullptr) {}
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
        return this->GetValue(ContentProperty());
    }
    void SetContent(const std::any& value) {
        auto oldValue = GetContent();  // 保存旧值
        this->SetValue(ContentProperty(), value);
        
        // 移除旧的内容元素
        if (contentElement_) {
            this->RemoveVisualChild(contentElement_);
            contentElement_ = nullptr;
        }
        
        // 处理新内容
        if (value.has_value()) {
            if (value.type() == typeid(UIElement*)) {
                // 内容是 UIElement*
                auto* element = std::any_cast<UIElement*>(value);
                if (element) {
                    contentElement_ = element;
                    this->AddVisualChild(contentElement_);
                    this->TakeOwnership(element);
                }
            } else if (value.type() == typeid(const char*)) {
                // 内容是 C 字符串
                const char* str = std::any_cast<const char*>(value);
                contentElement_ = CreateTextBlockFromString(str);
                this->AddVisualChild(contentElement_);
                this->TakeOwnership(contentElement_);
            } else if (value.type() == typeid(std::string)) {
                // 内容是 std::string
                std::string str = std::any_cast<std::string>(value);
                contentElement_ = CreateTextBlockFromString(str);
                this->AddVisualChild(contentElement_);
                this->TakeOwnership(contentElement_);
            }
        }
        
        OnContentChanged(oldValue, value);
        this->InvalidateMeasure();
    }
    
    Derived* Content(const std::any& value) {
        SetContent(value);
        return static_cast<Derived*>(this);
    }
    
    // 流畅 API：直接接受 UIElement* 指针，无需手动包装成 std::any
    Derived* Content(UIElement* element) {
        SetContent(std::any(element));
        return static_cast<Derived*>(this);
    }
    
    std::any Content() const { return GetContent(); }

    // ========== 内容模板 ==========
    
    DataTemplate* GetContentTemplate() const {
        const auto& value = this->GetValue(ContentTemplateProperty());
        if (!value.has_value()) {
            return nullptr;
        }
        try {
            return std::any_cast<DataTemplate*>(value);
        } catch (const std::bad_any_cast&) {
            return nullptr;
        }
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
    
    /**
     * @brief 测量内容元素
     */
    Size MeasureCore(const Size& availableSize) override {
        if (contentElement_) {
            // 减去 Padding
            auto padding = this->GetPadding();
            auto borderThickness = this->GetBorderThickness();
            
            float paddingWidth = padding.left + padding.right + borderThickness.left + borderThickness.right;
            float paddingHeight = padding.top + padding.bottom + borderThickness.top + borderThickness.bottom;
            
            Size contentAvailableSize(
                std::max(0.0f, availableSize.width - paddingWidth),
                std::max(0.0f, availableSize.height - paddingHeight)
            );
            
            contentElement_->Measure(contentAvailableSize);
            Size desiredSize = contentElement_->GetDesiredSize();
            
            return Size(
                desiredSize.width + paddingWidth,
                desiredSize.height + paddingHeight
            );
        }
        
        return Size(0, 0);
    }
    
    /**
     * @brief 排列内容元素
     */
    void ArrangeCore(const Rect& finalRect) override {
        if (contentElement_) {
            auto padding = this->GetPadding();
            auto borderThickness = this->GetBorderThickness();
            
            float left = padding.left + borderThickness.left;
            float top = padding.top + borderThickness.top;
            float right = padding.right + borderThickness.right;
            float bottom = padding.bottom + borderThickness.bottom;
            
            Rect contentRect(
                left,
                top,
                std::max(0.0f, finalRect.width - left - right),
                std::max(0.0f, finalRect.height - top - bottom)
            );
            
            contentElement_->Arrange(contentRect);
        }
    }
    
    /**
     * @brief 从字符串创建 TextBlock
     */
    UIElement* CreateTextBlockFromString(const std::string& text);

private:
    // 当前内容元素
    UIElement* contentElement_;
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
