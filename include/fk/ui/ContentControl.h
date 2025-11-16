#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/DataTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include <memory>
#include <string>

namespace fk::ui {

// 前向声明
class TextBlock;
template<typename> class ContentPresenter;

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
        
        // 如果有模板，不直接管理内容元素，而是通过 ContentPresenter
        if (this->GetTemplate()) {
            // 有模板：内容将通过模板中的 ContentPresenter 显示
            // 如果模板已应用，立即更新 ContentPresenter
            if (this->GetTemplateRoot()) {
                UpdateContentPresenter();
            }
            // 如果模板还没应用，OnTemplateApplied 会在模板应用时调用 UpdateContentPresenter
        } else {
            // 回退机制：没有模板时直接显示内容
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
     * @brief 获取逻辑子元素（覆写 UIElement）
     * 
     * ContentControl 的逻辑子元素是其内容元素或模板根
     */
    std::vector<UIElement*> GetLogicalChildren() const override {
        std::vector<UIElement*> children;
        
        // 如果有模板根，返回模板根
        if (this->GetTemplateRoot()) {
            children.push_back(this->GetTemplateRoot());
        }
        // 否则返回直接内容元素
        else if (contentElement_) {
            children.push_back(contentElement_);
        }
        
        return children;
    }
    
    /**
     * @brief 测量内容元素
     */
    Size MeasureCore(const Size& availableSize) override {
        // 如果有模板根，测量模板根
        if (this->GetTemplateRoot()) {
            this->GetTemplateRoot()->Measure(availableSize);
            return this->GetTemplateRoot()->GetDesiredSize();
        }
        
        // 回退机制：直接测量内容元素
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
        // 如果有模板根，排列模板根
        if (this->GetTemplateRoot()) {
            // 模板根应该在 (0, 0) 开始，占据整个 finalRect 的尺寸
            // 这是因为模板根是相对于 ContentControl 的坐标系
            Rect templateRect(0, 0, finalRect.width, finalRect.height);
            this->GetTemplateRoot()->Arrange(templateRect);
            // 设置 ContentControl 自身的渲染尺寸为分配给它的尺寸
            this->SetRenderSize(Size(finalRect.width, finalRect.height));
            return;
        }
        
        // 回退机制：直接排列内容元素
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
            // 设置渲染尺寸
            this->SetRenderSize(Size(finalRect.width, finalRect.height));
        } else {
            // 没有内容，仍然需要设置渲染尺寸
            this->SetRenderSize(Size(finalRect.width, finalRect.height));
        }
    }
    
    /**
     * @brief 模板应用后的钩子
     */
    void OnTemplateApplied() override {
        // 模板应用后，查找并更新 ContentPresenter
        UpdateContentPresenter();
    }
    
    /**
     * @brief 更新模板中的 ContentPresenter
     */
    void UpdateContentPresenter();
    
    /**
     * @brief 在视觉树中查找 ContentPresenter
     */
    template<typename T = void>
    ContentPresenter<T>* FindContentPresenter(UIElement* root);

    /**
     * @brief 从字符串创建 TextBlock
     */
    UIElement* CreateTextBlockFromString(const std::string& text);
    
    /**
     * @brief 渲染内容控件
     * 
     * ContentControl 需要在 OnRender 中确保模板根元素被正确渲染。
     * 根据用户需求，应该在此方法中：
     * 1. 确保模板已实例化（如果有模板）
     * 2. 显式调用模板根元素或内容元素的 OnRender，传递 RenderContext
     */
    void OnRender(render::RenderContext& context) override;

private:
    // 当前内容元素（回退机制使用）
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
