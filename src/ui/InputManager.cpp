#include "fk/ui/InputManager.h"
#include "fk/ui/Visual.h"
#include "fk/ui/Transform.h"

namespace fk::ui {

// ========== 构造/析构 ==========

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

// ========== 根节点管理 ==========

void InputManager::SetRoot(Visual* root) {
    root_ = root;
}

// ========== 命中测试 ==========

UIElement* InputManager::HitTest(const Point& screenPoint, Visual* root) {
    Visual* testRoot = root ? root : root_;
    if (!testRoot) {
        return nullptr;
    }
    
    // TODO: 考虑窗口坐标转换
    // 目前假设 screenPoint 已经是相对于 root 的局部坐标
    return HitTestRecursive(testRoot, screenPoint);
}

UIElement* InputManager::HitTestRecursive(Visual* visual, const Point& localPoint) {
    if (!visual) {
        return nullptr;
    }
    
    // 尝试转换为 UIElement
    auto* element = dynamic_cast<UIElement*>(visual);
    if (!element) {
        return nullptr;
    }
    
    // 检查可见性和启用状态
    if (element->GetVisibility() != Visibility::Visible || !element->GetIsEnabled()) {
        return nullptr;
    }
    
    // 检查是否在边界内
    Rect bounds(0, 0, element->GetRenderSize().width, element->GetRenderSize().height);
    if (!bounds.Contains(localPoint)) {
        return nullptr;
    }
    
    // 递归检查子元素（从后向前，因为后面的元素在视觉上更靠前）
    size_t childCount = visual->GetVisualChildrenCount();
    for (int i = static_cast<int>(childCount) - 1; i >= 0; --i) {
        Visual* child = visual->GetVisualChild(i);
        if (!child) continue;
        
        UIElement* childElement = dynamic_cast<UIElement*>(child);
        if (!childElement) continue;
        
        // 获取子元素的布局矩形（在父元素坐标系中的位置）
        Rect childLayoutRect = childElement->GetLayoutRect();
        
        // 转换坐标到子元素局部空间
        // 首先减去子元素的偏移量
        Point childLocalPoint(localPoint.x - childLayoutRect.x, 
                             localPoint.y - childLayoutRect.y);
        
        // 如果子元素有 RenderTransform，应用逆变换
        Transform* transform = childElement->GetRenderTransform();
        if (transform) {
            Matrix3x2 inverseMatrix = transform->GetInverseMatrix();
            childLocalPoint = inverseMatrix.TransformPoint(childLocalPoint);
        }
        
        UIElement* hitChild = HitTestRecursive(child, childLocalPoint);
        if (hitChild) {
            return hitChild;
        }
    }
    
    // 如果没有子元素命中，返回当前元素
    return element;
}

// ========== 指针捕获 ==========

void InputManager::CapturePointer(UIElement* element, int pointerId) {
    if (element) {
        pointerCaptures_[pointerId] = element;
    }
}

void InputManager::ReleasePointerCapture(int pointerId) {
    pointerCaptures_.erase(pointerId);
}

UIElement* InputManager::GetPointerCapture(int pointerId) const {
    auto it = pointerCaptures_.find(pointerId);
    return (it != pointerCaptures_.end()) ? it->second : nullptr;
}

// ========== 指针事件处理 ==========

void InputManager::ProcessPointerEvent(const PlatformPointerEvent& event) {
    // 检查是否有指针捕获
    UIElement* target = GetPointerCapture(event.pointerId);
    
    // 如果没有捕获，执行命中测试
    if (!target) {
        target = HitTest(event.position);
    }
    
    // 记录位置
    lastPointerPosition_ = event.position;
    
    // 根据事件类型分发
    switch (event.type) {
        case PlatformPointerEvent::Type::Move:
            if (target) {
                DispatchPointerMove(target, event);
            }
            UpdateMouseOver(event.position);
            break;
            
        case PlatformPointerEvent::Type::Down:
            if (target) {
                DispatchPointerDown(target, event);
            }
            break;
            
        case PlatformPointerEvent::Type::Up:
            if (target) {
                DispatchPointerUp(target, event);
            }
            break;
            
        case PlatformPointerEvent::Type::Enter:
            if (target) {
                DispatchPointerEnter(target, event);
            }
            break;
            
        case PlatformPointerEvent::Type::Leave:
            if (target) {
                DispatchPointerLeave(target, event);
            }
            break;
            
        case PlatformPointerEvent::Type::Wheel:
            // TODO: 实现滚轮事件
            break;
    }
}

void InputManager::DispatchPointerDown(UIElement* target, const PlatformPointerEvent& event) {
    if (!target) return;
    
    PointerEventArgs args(target, event.position, event.pointerId);
    target->OnPointerPressed(args);
}

void InputManager::DispatchPointerUp(UIElement* target, const PlatformPointerEvent& event) {
    if (!target) return;
    
    PointerEventArgs args(target, event.position, event.pointerId);
    target->OnPointerReleased(args);
}

void InputManager::DispatchPointerMove(UIElement* target, const PlatformPointerEvent& event) {
    if (!target) return;
    
    PointerEventArgs args(target, event.position, event.pointerId);
    target->OnPointerMoved(args);
}

void InputManager::DispatchPointerEnter(UIElement* target, const PlatformPointerEvent& event) {
    if (!target) return;
    
    PointerEventArgs args(target, event.position, event.pointerId);
    target->OnPointerEntered(args);
}

void InputManager::DispatchPointerLeave(UIElement* target, const PlatformPointerEvent& event) {
    if (!target) return;
    
    PointerEventArgs args(target, event.position, event.pointerId);
    target->OnPointerExited(args);
}

// ========== 鼠标悬停追踪 ==========

void InputManager::UpdateMouseOver(const Point& position) {
    UIElement* newMouseOver = HitTest(position);
    
    if (newMouseOver != mouseOverElement_) {
        // 生成 Leave 事件
        if (mouseOverElement_) {
            PlatformPointerEvent leaveEvent;
            leaveEvent.type = PlatformPointerEvent::Type::Leave;
            leaveEvent.position = position;
            DispatchPointerLeave(mouseOverElement_, leaveEvent);
        }
        
        // 生成 Enter 事件
        if (newMouseOver) {
            PlatformPointerEvent enterEvent;
            enterEvent.type = PlatformPointerEvent::Type::Enter;
            enterEvent.position = position;
            DispatchPointerEnter(newMouseOver, enterEvent);
        }
        
        mouseOverElement_ = newMouseOver;
    }
}

// ========== 键盘事件处理 ==========

void InputManager::ProcessKeyboardEvent(const PlatformKeyEvent& event) {
    UIElement* target = GetFocusedElement();
    
    if (!target) {
        return;
    }
    
    switch (event.type) {
        case PlatformKeyEvent::Type::Down:
            DispatchKeyDown(target, event);
            break;
            
        case PlatformKeyEvent::Type::Up:
            DispatchKeyUp(target, event);
            break;
            
        case PlatformKeyEvent::Type::Char:
            // TODO: 字符输入事件
            break;
    }
}

void InputManager::DispatchKeyDown(UIElement* target, const PlatformKeyEvent& event) {
    if (!target) return;
    
    KeyEventArgs args(target, event.key, event.isRepeat);
    target->OnKeyDown(args);
}

void InputManager::DispatchKeyUp(UIElement* target, const PlatformKeyEvent& event) {
    if (!target) return;
    
    KeyEventArgs args(target, event.key, event.isRepeat);
    target->OnKeyUp(args);
}

UIElement* InputManager::GetFocusedElement() const {
    // TODO: 与 FocusManager 集成
    // 目前返回 nullptr
    return nullptr;
}

} // namespace fk::ui
