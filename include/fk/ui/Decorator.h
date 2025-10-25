#pragma once

#include "fk/ui/FrameworkElement.h"

#include <memory>

namespace fk::ui {

class Decorator : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    Decorator();
    ~Decorator() override;

    void SetChild(std::shared_ptr<UIElement> child);
    [[nodiscard]] std::shared_ptr<UIElement> GetChild() const noexcept { return child_; }
    [[nodiscard]] bool HasChild() const noexcept { return child_ != nullptr; }
    void ClearChild();

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

    void OnAttachedToLogicalTree() override;
    void OnDetachedFromLogicalTree() override;

    virtual void OnChildChanged(UIElement* oldChild, UIElement* newChild);

private:
    void AttachChild();
    void DetachChild();

    std::shared_ptr<UIElement> child_;
};

} // namespace fk::ui
