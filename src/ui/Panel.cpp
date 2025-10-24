#include "fk/ui/Panel.h"

#include <algorithm>

namespace fk::ui {

Panel::Panel() = default;

Panel::~Panel() {
    ClearChildren();
}

void Panel::AddChild(std::shared_ptr<UIElement> child) {
    VerifyAccess();
    if (!child) {
        return;
    }

    auto* const rawChild = child.get();

    const auto alreadyPresent = std::any_of(children_.begin(), children_.end(), [rawChild](const auto& existing) {
        return existing.get() == rawChild;
    });
    if (alreadyPresent) {
        return;
    }

    if (auto* existingParent = dynamic_cast<DependencyObject*>(rawChild->GetLogicalParent()); existingParent && existingParent != this) {
        existingParent->RemoveLogicalChild(rawChild);
    }

    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(rawChild);
    } else {
        BindingDependencyObject::AddLogicalChild(rawChild);
    }

    children_.push_back(std::move(child));
    InvalidateMeasure();
    InvalidateArrange();
}

bool Panel::RemoveChild(UIElement* child) {
    VerifyAccess();
    if (!child) {
        return false;
    }

    const auto it = std::find_if(children_.begin(), children_.end(), [child](const auto& existing) {
        return existing.get() == child;
    });
    if (it == children_.end()) {
        return false;
    }

    DetachChild(*child);
    children_.erase(it);

    InvalidateMeasure();
    InvalidateArrange();
    return true;
}

void Panel::ClearChildren() {
    VerifyAccess();
    DetachAllChildren();
    children_.clear();
    InvalidateMeasure();
    InvalidateArrange();
}

std::span<const std::shared_ptr<UIElement>> Panel::Children() const noexcept {
    return { children_.data(), children_.size() };
}

void Panel::OnAttachedToLogicalTree() {
    FrameworkElement::OnAttachedToLogicalTree();
    AttachAllChildren();
}

void Panel::OnDetachedFromLogicalTree() {
    DetachAllChildren();
    FrameworkElement::OnDetachedFromLogicalTree();
}

void Panel::MeasureChild(UIElement& child, const Size& availableSize) {
    child.Measure(availableSize);
}

void Panel::ArrangeChild(UIElement& child, const Rect& finalRect) {
    child.Arrange(finalRect);
}

void Panel::AttachChild(UIElement& child) {
    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(&child);
    } else {
        BindingDependencyObject::AddLogicalChild(&child);
    }
}

void Panel::DetachChild(UIElement& child) {
    if (child.GetLogicalParent() != this) {
        return;
    }
    RemoveLogicalChild(&child);
}

void Panel::AttachAllChildren() {
    for (const auto& child : children_) {
        if (!child) {
            continue;
        }
        AttachChild(*child);
    }
}

void Panel::DetachAllChildren() {
    for (const auto& child : children_) {
        if (!child) {
            continue;
        }
        if (child->GetLogicalParent() == this) {
            RemoveLogicalChild(child.get());
        }
    }
}

} // namespace fk::ui
