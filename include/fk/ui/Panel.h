#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/UIElement.h"
#include "fk/binding/Binding.h"

#include <memory>
#include <span>
#include <vector>

namespace fk::ui {

// 用于 Children 依赖属性的集合类型
using UIElementCollection = std::vector<std::shared_ptr<UIElement>>;

// Panel 基类（非模板）
class PanelBase : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    PanelBase();
    ~PanelBase() override;

    // 依赖属性
    static const binding::DependencyProperty& ChildrenProperty();

    // Setter: 添加子元素
    void AddChild(std::shared_ptr<UIElement> child);
    
    // Setter: 移除子元素
    void RemoveChild(UIElement* child);
    
    // Setter: 清空所有子元素
    void ClearChildren();

    // Getter: 获取所有子元素（返回内部集合的视图）
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> GetChildren() const noexcept;
    
    // Getter: 获取子元素数量
    [[nodiscard]] std::size_t GetChildCount() const noexcept;
    
    // Getter: 是否有子元素
    [[nodiscard]] bool HasChildren() const noexcept;

    // Visual interface - 返回所有子元素作为可视子元素
    std::vector<Visual*> GetVisualChildren() const override;

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    virtual Size MeasureOverride(const Size& availableSize) = 0;
    virtual Size ArrangeOverride(const Size& finalSize) = 0;

    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> ChildSpan() const noexcept { return { children_.data(), children_.size() }; }

    static void MeasureChild(UIElement& child, const Size& availableSize);
    static void ArrangeChild(UIElement& child, const Rect& finalRect);
    
    // 子类可以重写以响应子元素集合变化
    virtual void OnChildrenChanged(const UIElementCollection& oldChildren, const UIElementCollection& newChildren);
    
    // 重写鼠标事件处理,实现事件路由
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;
    bool OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) override;
    
    // 命中测试重写
    UIElement* HitTestChildren(double x, double y) override;
    
    // 提供给模板类使用的内部方法
    void SetChildrenInternal(const UIElementCollection& children);
    void SetChildrenInternal(UIElementCollection&& children);

private:
    static binding::PropertyMetadata BuildChildrenMetadata();
    
    static void ChildrenPropertyChanged(binding::DependencyObject& sender, 
        const binding::DependencyProperty& property,
        const std::any& oldValue, 
        const std::any& newValue);
    
    static bool ValidateChildren(const std::any& value);
    
    static UIElementCollection ToCollection(const std::any& value);

    void AttachChild(UIElement& child);
    void DetachChild(UIElement& child);
    void AttachAllChildren();
    void DetachAllChildren();

    std::vector<std::shared_ptr<UIElement>> children_;
    UIElement* lastHoveredChild_ = nullptr;  // 跟踪上一次悬停的子元素
};

// Panel 模板类（支持 CRTP 链式调用）
template <typename Derived>
class Panel : public PanelBase {
public:
    using BaseType = PanelBase;
    using Ptr = std::shared_ptr<Derived>;

    using PanelBase::PanelBase;

    // Fluent API: 获取 Children 集合
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Children() const noexcept {
        return GetChildren();
    }
    
    // Fluent API: 设置 Children 集合（拷贝语义，返回派生类指针）
    Ptr Children(const UIElementCollection& children) {
        SetChildrenInternal(children);
        return Self();
    }
    
    // Fluent API: 设置 Children 集合（移动语义，返回派生类指针）
    Ptr Children(UIElementCollection&& children) {
        SetChildrenInternal(std::move(children));
        return Self();
    }
    
    // 🎯 绑定支持：Children 属性
    Ptr Children(binding::Binding binding) {
        SetBinding(PanelBase::ChildrenProperty(), std::move(binding));
        return Self();
    }

protected:
    Ptr Self() {
        // 通过 View 的 enable_shared_from_this 获取 shared_ptr
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};

} // namespace fk::ui
