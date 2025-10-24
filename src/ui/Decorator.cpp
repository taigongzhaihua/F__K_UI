#include "fk/ui/Decorator.h"

#include <utility>

namespace fk::ui {

Decorator::Decorator() = default;

Decorator::~Decorator() {
    ClearChild();
}

void Decorator::SetChild(std::shared_ptr<UIElement> child) {
    if (child_ == child) {
        return;
    }

    auto oldChild = std::exchange(child_, std::move(child));
    if (oldChild) {
        DetachChild();
    }
    if (child_) {
        AttachChild();
    }

    OnChildChanged(oldChild.get(), child_ ? child_.get() : nullptr);
    InvalidateMeasure();
    InvalidateArrange();
}

void Decorator::ClearChild() {
    if (!child_) {
        return;
    }
    auto oldChild = std::move(child_);
    DetachChild();
    OnChildChanged(oldChild.get(), nullptr);
    InvalidateMeasure();
    InvalidateArrange();
}

Size Decorator::MeasureOverride(const Size& availableSize) {
    if (!child_) {
        return Size{};
    }
    child_->Measure(availableSize);
    return child_->DesiredSize();
}

Size Decorator::ArrangeOverride(const Size& finalSize) {
    if (child_) {
        child_->Arrange(Rect{ 0.0f, 0.0f, finalSize.width, finalSize.height });
    }
    return finalSize;
}

void Decorator::OnAttachedToLogicalTree() {
    FrameworkElement::OnAttachedToLogicalTree();
    AttachChild();
}

void Decorator::OnDetachedFromLogicalTree() {
    DetachChild();
    FrameworkElement::OnDetachedFromLogicalTree();
}

void Decorator::OnChildChanged(UIElement*, UIElement*) {}

void Decorator::AttachChild() {
    if (!child_) {
        return;
    }
    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(child_.get());
    } else {
        BindingDependencyObject::AddLogicalChild(child_.get());
    }
}

void Decorator::DetachChild() {
    if (!child_ || child_->GetLogicalParent() != this) {
        return;
    }
    RemoveLogicalChild(child_.get());
}

} // namespace fk::ui
