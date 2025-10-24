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

    void AddChild(std::shared_ptr<UIElement> child);
    bool RemoveChild(UIElement* child);
    void ClearChildren();

    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> Children() const noexcept;
    [[nodiscard]] std::size_t ChildCount() const noexcept { return children_.size(); }

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
