#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/UIElement.h"

#include <memory>
#include <span>
#include <vector>

namespace fk::ui {

class Panel : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    Panel();
    ~Panel() override;

    // Setter: 添加子元素
    void AddChild(std::shared_ptr<UIElement> child);
    
    // Setter: 移除子元素
    void RemoveChild(UIElement* child);
    
    // Setter: 清空所有子元素
    void ClearChildren();

    // Getter: 获取所有子元素
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> GetChildren() const noexcept;
    
    // Getter: 获取子元素数量
    [[nodiscard]] std::size_t GetChildCount() const noexcept { return children_.size(); }
    
    // Getter: 是否有子元素
    [[nodiscard]] bool HasChildren() const noexcept { return !children_.empty(); }

protected:
    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    virtual Size MeasureOverride(const Size& availableSize) = 0;
    virtual Size ArrangeOverride(const Size& finalSize) = 0;

    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> ChildSpan() const noexcept { return { children_.data(), children_.size() }; }

    static void MeasureChild(UIElement& child, const Size& availableSize);
    static void ArrangeChild(UIElement& child, const Rect& finalRect);

private:
    void AttachChild(UIElement& child);
    void DetachChild(UIElement& child);
    void AttachAllChildren();
    void DetachAllChildren();

    std::vector<std::shared_ptr<UIElement>> children_;
};

} // namespace fk::ui
