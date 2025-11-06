#include "fk/ui/Panel.h"
#include "fk/ui/Button.h"

#include <algorithm>
#include <iostream>

namespace fk::ui {

using binding::DependencyProperty;

PanelBase::PanelBase() = default;

PanelBase::~PanelBase() {
    ClearChildren();
}

const DependencyProperty& PanelBase::ChildrenProperty() {
    static const auto& property = DependencyProperty::Register(
        "Children",
        typeid(UIElementCollection),
        typeid(PanelBase),
        BuildChildrenMetadata());
    return property;
}

void PanelBase::SetChildrenInternal(const UIElementCollection& children) {
    VerifyAccess();
    
    // 清空现有子元素
    DetachAllChildren();
    children_.clear();
    
    // 添加新的子元素
    for (const auto& child : children) {
        if (!child) {
            continue;
        }
        
        auto* const rawChild = child.get();
        
        // 从旧父元素移除
        if (auto* existingParent = dynamic_cast<DependencyObject*>(rawChild->GetLogicalParent()); 
            existingParent && existingParent != this) {
            existingParent->RemoveLogicalChild(rawChild);
        }
        
        // 添加到逻辑树
        if (IsAttachedToLogicalTree()) {
            AddLogicalChild(rawChild);
        } else {
            BindingDependencyObject::AddLogicalChild(rawChild);
        }
        
        children_.push_back(child);
    }
    
    // 触发依赖属性变更通知
    SetValue(ChildrenProperty(), children_);
    
    InvalidateMeasure();
    InvalidateArrange();
}

void PanelBase::SetChildrenInternal(UIElementCollection&& children) {
    VerifyAccess();
    
    // 清空现有子元素
    DetachAllChildren();
    children_.clear();
    
    // 移动新的子元素
    for (auto& child : children) {
        if (!child) {
            continue;
        }
        
        auto* const rawChild = child.get();
        
        // 从旧父元素移除
        if (auto* existingParent = dynamic_cast<DependencyObject*>(rawChild->GetLogicalParent()); 
            existingParent && existingParent != this) {
            existingParent->RemoveLogicalChild(rawChild);
        }
        
        // 添加到逻辑树
        if (IsAttachedToLogicalTree()) {
            AddLogicalChild(rawChild);
        } else {
            BindingDependencyObject::AddLogicalChild(rawChild);
        }
        
        children_.push_back(std::move(child));
    }
    
    // 触发依赖属性变更通知
    SetValue(ChildrenProperty(), children_);
    
    InvalidateMeasure();
    InvalidateArrange();
}

void PanelBase::AddChild(std::shared_ptr<UIElement> child) {
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
    
    // 触发依赖属性变更通知
    SetValue(ChildrenProperty(), children_);
    
    InvalidateMeasure();
    InvalidateArrange();
}

void PanelBase::RemoveChild(UIElement* child) {
    VerifyAccess();
    if (!child) {
        return;
    }

    const auto it = std::find_if(children_.begin(), children_.end(), [child](const auto& existing) {
        return existing.get() == child;
    });
    if (it == children_.end()) {
        return;
    }

    DetachChild(*child);
    children_.erase(it);

    // 触发依赖属性变更通知
    SetValue(ChildrenProperty(), children_);

    InvalidateMeasure();
    InvalidateArrange();
}

void PanelBase::ClearChildren() {
    VerifyAccess();
    DetachAllChildren();
    children_.clear();
    
    // 触发依赖属性变更通知
    SetValue(ChildrenProperty(), children_);
    
    InvalidateMeasure();
    InvalidateArrange();
}

std::span<const std::shared_ptr<UIElement>> PanelBase::GetChildren() const noexcept {
    return { children_.data(), children_.size() };
}

std::vector<Visual*> PanelBase::GetVisualChildren() const {
    std::vector<Visual*> result;
    result.reserve(children_.size());
    for (const auto& child : children_) {
        if (child) {
            result.push_back(child.get());
        }
    }
    return result;
}

std::size_t PanelBase::GetChildCount() const noexcept {
    return children_.size();
}

bool PanelBase::HasChildren() const noexcept {
    return !children_.empty();
}

void PanelBase::OnAttachedToLogicalTree() {
    FrameworkElement::OnAttachedToLogicalTree();
    AttachAllChildren();
}

void PanelBase::OnDetachedFromLogicalTree() {
    DetachAllChildren();
    FrameworkElement::OnDetachedFromLogicalTree();
}

void PanelBase::MeasureChild(UIElement& child, const Size& availableSize) {
    child.Measure(availableSize);
}

void PanelBase::ArrangeChild(UIElement& child, const Rect& finalRect) {
    child.Arrange(finalRect);
}

void PanelBase::OnChildrenChanged(const UIElementCollection&, const UIElementCollection&) {
    // 子类可以重写以响应集合变化
}

binding::PropertyMetadata PanelBase::BuildChildrenMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = UIElementCollection{};
    metadata.propertyChangedCallback = &PanelBase::ChildrenPropertyChanged;
    metadata.validateCallback = &PanelBase::ValidateChildren;
    return metadata;
}

void PanelBase::ChildrenPropertyChanged(binding::DependencyObject& sender, 
    const DependencyProperty&,
    const std::any& oldValue, 
    const std::any& newValue) {
    auto* panel = dynamic_cast<PanelBase*>(&sender);
    if (!panel) {
        return;
    }

    const auto oldCollection = ToCollection(oldValue);
    const auto newCollection = ToCollection(newValue);

    panel->OnChildrenChanged(oldCollection, newCollection);
}

bool PanelBase::ValidateChildren(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    return value.type() == typeid(UIElementCollection);
}

UIElementCollection PanelBase::ToCollection(const std::any& value) {
    if (!value.has_value()) {
        return {};
    }
    if (value.type() != typeid(UIElementCollection)) {
        return {};
    }
    return std::any_cast<UIElementCollection>(value);
}

void PanelBase::AttachChild(UIElement& child) {
    if (IsAttachedToLogicalTree()) {
        AddLogicalChild(&child);
    } else {
        BindingDependencyObject::AddLogicalChild(&child);
    }
}

void PanelBase::DetachChild(UIElement& child) {
    if (child.GetLogicalParent() != this) {
        return;
    }
    RemoveLogicalChild(&child);
}

void PanelBase::AttachAllChildren() {
    for (const auto& child : children_) {
        if (!child) {
            continue;
        }
        AttachChild(*child);
    }
}

void PanelBase::DetachAllChildren() {
    for (const auto& child : children_) {
        if (!child) {
            continue;
        }
        if (child->GetLogicalParent() == this) {
            RemoveLogicalChild(child.get());
        }
    }
}

// ============================================================================
// 鼠标事件路由
// ============================================================================

bool PanelBase::OnMouseButtonDown(int button, double x, double y) {
    // 将事件路由到子元素 (从后往前,因为后面的元素在上层)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        const auto& child = *it;
        if (!child) continue;
        
        // 获取子元素的边界
        auto bounds = child->GetRenderBounds();
        
        // 检查点击是否在子元素内
        if (x >= bounds.x && x < bounds.x + bounds.width &&
            y >= bounds.y && y < bounds.y + bounds.height) {
            
            // 转换坐标到子元素空间
            double childX = x - bounds.x;
            double childY = y - bounds.y;
            
            // 传递事件到子元素，如果子元素处理了，返回 true
            if (child->OnMouseButtonDown(button, childX, childY)) {
                return true; // 事件已被子元素处理
            }
        }
    }
    
    // 如果没有子元素处理,调用基类默认处理
    return UIElement::OnMouseButtonDown(button, x, y);
}

bool PanelBase::OnMouseButtonUp(int button, double x, double y) {
    // MouseUp 事件需要特殊处理：即使鼠标不在子元素上，
    // 也要传递给所有子元素（因为可能有元素捕获了鼠标）
    bool handled = false;
    
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        const auto& child = *it;
        if (!child) continue;
        
        auto bounds = child->GetRenderBounds();
        double childX = x - bounds.x;
        double childY = y - bounds.y;
        
        // 总是尝试传递 MouseUp（子元素会自己判断是否处理）
        if (child->OnMouseButtonUp(button, childX, childY)) {
            handled = true;
            // 不要 break，继续传递给其他元素
        }
    }
    
    if (handled) {
        return true;
    }
    
    return UIElement::OnMouseButtonUp(button, x, y);
}

bool PanelBase::OnMouseMove(double x, double y) {
    // 查找鼠标当前悬停的子元素
    UIElement* hoveredChild = nullptr;
    bool handled = false;
    
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        const auto& child = *it;
        if (!child) continue;
        
        auto bounds = child->GetRenderBounds();
        
        if (x >= bounds.x && x < bounds.x + bounds.width &&
            y >= bounds.y && y < bounds.y + bounds.height) {
            
            hoveredChild = child.get();
            
            double childX = x - bounds.x;
            double childY = y - bounds.y;
            
            handled = child->OnMouseMove(childX, childY);
            break;
        }
    }
    
    // 处理鼠标离开之前悬停的元素
    if (lastHoveredChild_ && lastHoveredChild_ != hoveredChild) {
        // 触发 MouseLeave
        if (auto* button = dynamic_cast<ui::detail::ButtonBase*>(lastHoveredChild_)) {
            button->HandleMouseLeave();
        }
    }
    
    lastHoveredChild_ = hoveredChild;
    
    if (!hoveredChild) {
        return UIElement::OnMouseMove(x, y);
    }
    
    return handled;
}

bool PanelBase::OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) {
    // 找到鼠标位置下的最上层子元素
    UIElement* hitChild = HitTestChildren(mouseX, mouseY);
    
    if (hitChild) {
        // 传递给命中的子元素
        return hitChild->OnMouseWheel(xoffset, yoffset, mouseX, mouseY);
    }
    
    return false;
}

UIElement* PanelBase::HitTestChildren(double x, double y) {
    // 从后向前遍历(后面的元素在上层)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        const auto& child = *it;
        if (!child || child->GetVisibility() != Visibility::Visible) {
            continue;
        }
        
        // 先检查子元素本身
        if (child->HitTest(x, y)) {
            // 递归检查子元素的子元素
            UIElement* hitInChild = child->HitTestChildren(x, y);
            return hitInChild ? hitInChild : child.get();
        }
    }
    return nullptr;
}

} // namespace fk::ui
