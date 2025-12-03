#pragma once

#include "fk/ui/base/FrameworkElement.h"
#include "fk/ui/styling/DataTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include <any>

namespace fk::ui {

/**
 * @brief 内容展示器
 * 
 * 职责：
 * - 在 ControlTemplate 中展示 ContentControl 的 Content
 * - 自动应用 ContentTemplate（如果内容不是 UIElement）
 * - 支持模板绑定（TemplateBinding）到 Content 和 ContentTemplate
 * 
 * WPF 对应：ContentPresenter
 * 
 * 使用场景：
 * ```cpp
 * // 在 Button 的 ControlTemplate 中：
 * auto* template = new ControlTemplate();
 * template->SetFactory([]() -> UIElement* {
 *     return (new Border())
 *         ->Background(Color{0.5f, 0.5f, 0.5f, 1.0f})
 *         ->Child(new ContentPresenter());  // 显示 Button 的 Content
 * });
 * ```
 */
template<typename Derived = void>
class ContentPresenter : public FrameworkElement<
    std::conditional_t<std::is_void_v<Derived>, ContentPresenter<>, Derived>
> {
private:
    using Base = FrameworkElement<
        std::conditional_t<std::is_void_v<Derived>, ContentPresenter<>, Derived>
    >;
    
public:
    ContentPresenter() {
        // 订阅 Loaded 事件以应用内容
        this->Loaded += [this]() {
            this->UpdateContent();
        };
    }
    
    virtual ~ContentPresenter() = default;

    // ========== 依赖属性声明 ==========
    
    /**
     * @brief Content 依赖属性
     * 
     * 通常通过 TemplateBinding 绑定到 ContentControl.Content
     */
    static const binding::DependencyProperty& ContentProperty();
    
    /**
     * @brief ContentTemplate 依赖属性
     * 
     * 通常通过 TemplateBinding 绑定到 ContentControl.ContentTemplate
     */
    static const binding::DependencyProperty& ContentTemplateProperty();

    // ========== Content 属性 ==========
    
    std::any GetContent() const {
        return this->GetValue(ContentProperty());
    }
    
    void SetContent(const std::any& value) {
        this->SetValue(ContentProperty(), value);
        UpdateContent();
    }
    
    auto* Content(const std::any& value) {
        SetContent(value);
        return static_cast<std::conditional_t<std::is_void_v<Derived>, ContentPresenter<>*, Derived*>>(this);
    }
    
    std::any Content() const { return GetContent(); }

    // ========== ContentTemplate 属性 ==========
    
    DataTemplate* GetContentTemplate() const {
        return this->template GetValue<DataTemplate*>(ContentTemplateProperty());
    }
    
    void SetContentTemplate(DataTemplate* tmpl) {
        this->SetValue(ContentTemplateProperty(), tmpl);
        UpdateContent();
    }
    
    auto* ContentTemplate(DataTemplate* tmpl) {
        SetContentTemplate(tmpl);
        return static_cast<std::conditional_t<std::is_void_v<Derived>, ContentPresenter<>*, Derived*>>(this);
    }
    
    DataTemplate* ContentTemplate() const { return GetContentTemplate(); }

protected:
    /**
     * @brief 更新显示的内容
     * 
     * 根据 Content 和 ContentTemplate 决定如何显示：
     * 1. 如果 Content 是 UIElement*，直接显示
     * 2. 如果 Content 不是 UIElement 且有 ContentTemplate，应用模板
     * 3. 否则不显示
     */
    virtual void UpdateContent();
    
    /**
     * @brief 获取当前显示的视觉子元素
     */
    UIElement* GetVisualChild() const { return visualChild_; }
    
    /**
     * @brief 设置当前显示的视觉子元素
     */
    void SetVisualChild(UIElement* child);
    
    /**
     * @brief 测量内容元素
     */
    Size MeasureOverride(const Size& availableSize) override;
    
    /**
     * @brief 排列内容元素
     */
    Size ArrangeOverride(const Size& finalSize) override;

private:
    UIElement* visualChild_{nullptr};  ///< 当前显示的视觉子元素
};

// ========== 依赖属性实现 ==========

template<typename Derived>
const binding::DependencyProperty& ContentPresenter<Derived>::ContentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Content",
        typeid(std::any),
        typeid(ContentPresenter<Derived>),
        binding::PropertyMetadata{std::any()}
    );
    return property;
}

template<typename Derived>
const binding::DependencyProperty& ContentPresenter<Derived>::ContentTemplateProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "ContentTemplate",
        typeid(DataTemplate*),
        typeid(ContentPresenter<Derived>),
        binding::PropertyMetadata{std::any(static_cast<DataTemplate*>(nullptr))}
    );
    return property;
}

// ========== UpdateContent 实现 ==========

template<typename Derived>
void ContentPresenter<Derived>::UpdateContent() {
    auto content = GetContent();
    
    // 清除旧的视觉子元素
    if (visualChild_) {
        this->RemoveVisualChild(visualChild_);
        visualChild_ = nullptr;
    }
    
    // 情况 1: Content 是 UIElement*，直接显示
    if (content.type() == typeid(UIElement*)) {
        auto* element = std::any_cast<UIElement*>(content);
        if (element) {
            SetVisualChild(element);
            this->InvalidateMeasure();
            return;
        }
    }
    
    // 情况 2: Content 不是 UIElement 且有 ContentTemplate
    auto* tmpl = GetContentTemplate();
    if (tmpl && tmpl->IsValid() && content.has_value()) {
        // 使用模板实例化视觉树
        UIElement* visualTree = tmpl->Instantiate(content);
        if (visualTree) {
            SetVisualChild(visualTree);
            this->InvalidateMeasure();
            return;
        }
    }
    
    // 情况 3: 无法显示内容
    this->InvalidateMeasure();
}

template<typename Derived>
void ContentPresenter<Derived>::SetVisualChild(UIElement* child) {
    if (visualChild_ == child) {
        return;
    }
    
    // 移除旧子元素
    if (visualChild_) {
        this->RemoveVisualChild(visualChild_);
    }
    
    // 添加新子元素
    visualChild_ = child;
    if (visualChild_) {
        this->AddVisualChild(visualChild_);
        this->TakeOwnership(visualChild_);
    }
}

// ========== MeasureOverride 实现 ==========

template<typename Derived>
Size ContentPresenter<Derived>::MeasureOverride(const Size& availableSize) {
    // ContentPresenter 测量其视觉子元素
    if (visualChild_) {
        visualChild_->Measure(availableSize);
        return visualChild_->GetDesiredSize();
    }
    
    return Size(0, 0);
}

// ========== ArrangeOverride 实现 ==========

template<typename Derived>
Size ContentPresenter<Derived>::ArrangeOverride(const Size& finalSize) {
    // ContentPresenter 排列其视觉子元素
    if (visualChild_) {
        visualChild_->Arrange(Rect(0, 0, finalSize.width, finalSize.height));
        return visualChild_->GetRenderSize();
    }
    
    return Size(0, 0);
}

// 特化默认模板参数
using ContentPresenter_t = ContentPresenter<>;

} // namespace fk::ui
