#include "fk/ui/FocusManager.h"
#include "fk/ui/Visual.h"
#include <algorithm>
#include <limits>
#include <cmath>

namespace fk::ui {

// ========== 构造/析构 ==========

FocusManager::FocusManager() {
}

FocusManager::~FocusManager() {
}

// ========== 根节点管理 ==========

void FocusManager::SetRoot(Visual* root) {
    scopeRoot_ = root;
}

// ========== 焦点管理 ==========

bool FocusManager::SetFocusedElement(UIElement* element) {
    // 如果是同一个元素，不做处理
    if (element == focusedElement_) {
        return true;
    }
    
    // 检查元素是否可聚焦
    if (element && !IsFocusable(element)) {
        return false;
    }
    
    UIElement* oldFocus = focusedElement_;
    UIElement* newFocus = element;
    
    // 更新焦点
    focusedElement_ = element;
    
    // 通知变更
    NotifyFocusChanged(oldFocus, newFocus);
    
    return true;
}

void FocusManager::ClearFocus() {
    SetFocusedElement(nullptr);
}

// ========== 焦点能力检查 ==========

bool FocusManager::IsFocusable(UIElement* element) {
    if (!element) {
        return false;
    }
    
    // 检查可见性
    if (element->GetVisibility() != Visibility::Visible) {
        return false;
    }
    
    // 检查启用状态
    if (!element->GetIsEnabled()) {
        return false;
    }
    
    // TODO: 检查 Focusable 属性
    // 目前假设所有可见且启用的元素都可聚焦
    
    return true;
}

// ========== 焦点导航 ==========

bool FocusManager::MoveFocus(FocusNavigationDirection direction) {
    UIElement* nextFocus = FindNextFocusable(focusedElement_, direction);
    
    if (nextFocus) {
        return SetFocusedElement(nextFocus);
    }
    
    return false;
}

UIElement* FocusManager::FindNextFocusable(UIElement* current, FocusNavigationDirection direction) {
    if (!scopeRoot_) {
        return nullptr;
    }
    
    switch (direction) {
        case FocusNavigationDirection::Next:
            return FindNextInTabOrder(current, false);
            
        case FocusNavigationDirection::Previous:
            return FindNextInTabOrder(current, true);
            
        case FocusNavigationDirection::Up:
        case FocusNavigationDirection::Down:
        case FocusNavigationDirection::Left:
        case FocusNavigationDirection::Right:
            return FindNextInDirection(current, direction);
            
        default:
            return nullptr;
    }
}

UIElement* FocusManager::FindNextInTabOrder(UIElement* current, bool reverse) {
    // 收集所有可聚焦元素
    std::vector<UIElement*> focusables;
    CollectFocusableElements(scopeRoot_, focusables);
    
    if (focusables.empty()) {
        return nullptr;
    }
    
    // 如果没有当前焦点，返回第一个或最后一个
    if (!current) {
        return reverse ? focusables.back() : focusables.front();
    }
    
    // 查找当前元素的索引
    auto it = std::find(focusables.begin(), focusables.end(), current);
    
    if (it == focusables.end()) {
        // 当前元素不在列表中，返回第一个或最后一个
        return reverse ? focusables.back() : focusables.front();
    }
    
    // 移动到下一个或上一个
    if (reverse) {
        if (it == focusables.begin()) {
            // 循环到最后一个
            return focusables.back();
        } else {
            return *(--it);
        }
    } else {
        ++it;
        if (it == focusables.end()) {
            // 循环到第一个
            return focusables.front();
        } else {
            return *it;
        }
    }
}

UIElement* FocusManager::FindNextInDirection(UIElement* current, FocusNavigationDirection direction) {
    if (!current || !scopeRoot_) {
        return nullptr;
    }
    
    // Tab 顺序导航直接使用 FindNextInTabOrder
    if (direction == FocusNavigationDirection::Next) {
        return FindNextInTabOrder(current, false);
    } else if (direction == FocusNavigationDirection::Previous) {
        return FindNextInTabOrder(current, true);
    }
    
    // 收集所有可聚焦元素
    std::vector<UIElement*> focusables;
    CollectFocusableElements(scopeRoot_, focusables);
    
    if (focusables.empty()) {
        return nullptr;
    }
    
    // 获取当前元素的边界
    Rect currentBounds = GetElementBounds(current);
    Point currentCenter(
        currentBounds.x + currentBounds.width / 2.0f,
        currentBounds.y + currentBounds.height / 2.0f
    );
    
    // 查找指定方向上最近的元素
    UIElement* bestCandidate = nullptr;
    float bestScore = std::numeric_limits<float>::max();
    
    for (UIElement* candidate : focusables) {
        if (candidate == current) {
            continue;
        }
        
        Rect candidateBounds = GetElementBounds(candidate);
        Point candidateCenter(
            candidateBounds.x + candidateBounds.width / 2.0f,
            candidateBounds.y + candidateBounds.height / 2.0f
        );
        
        // 检查候选元素是否在正确的方向上
        if (!IsInDirection(currentCenter, candidateCenter, direction)) {
            continue;
        }
        
        // 计算距离得分（欧几里得距离）
        float dx = candidateCenter.x - currentCenter.x;
        float dy = candidateCenter.y - currentCenter.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // 计算方向得分（越接近主方向越好）
        float directionScore = GetDirectionScore(currentCenter, candidateCenter, direction);
        
        // 综合得分：距离 + 方向偏离度
        float score = distance + directionScore * 100.0f;
        
        if (score < bestScore) {
            bestScore = score;
            bestCandidate = candidate;
        }
    }
    
    return bestCandidate;
}

// ========== 可聚焦元素收集 ==========

void FocusManager::CollectFocusableElements(Visual* root, std::vector<UIElement*>& focusables) {
    if (!root) {
        return;
    }
    
    CollectFocusableElementsRecursive(root, focusables);
}

void FocusManager::CollectFocusableElementsRecursive(Visual* visual, std::vector<UIElement*>& focusables) {
    if (!visual) {
        return;
    }
    
    // 尝试转换为 UIElement
    auto* element = dynamic_cast<UIElement*>(visual);
    if (element && IsFocusable(element)) {
        focusables.push_back(element);
    }
    
    // 递归子元素
    size_t childCount = visual->GetVisualChildrenCount();
    for (size_t i = 0; i < childCount; ++i) {
        Visual* child = visual->GetVisualChild(i);
        if (child) {
            CollectFocusableElementsRecursive(child, focusables);
        }
    }
}

// ========== 事件通知 ==========

void FocusManager::NotifyFocusChanged(UIElement* oldFocus, UIElement* newFocus) {
    // 触发事件
    FocusChangedEventArgs args(oldFocus, newFocus);
    focusChangedEvent_(args);
    
    // TODO: 调用元素的焦点事件处理器
    // oldFocus->OnLostFocus()
    // newFocus->OnGotFocus()
}

// ========== 方向导航辅助方法 ==========

Rect FocusManager::GetElementBounds(UIElement* element) const {
    if (!element) {
        return Rect(0, 0, 0, 0);
    }
    
    // 获取元素的渲染尺寸
    Size size = element->GetRenderSize();
    
    // 简化实现：假设位置为 (0, 0)
    // TODO: 实现真正的屏幕坐标转换，需要累积父元素偏移量和变换
    // 这需要：
    // 1. 遍历父元素链
    // 2. 累积每个父元素的 VisualOffset
    // 3. 应用每个父元素的 RenderTransform
    
    return Rect(0, 0, size.width, size.height);
}

bool FocusManager::IsInDirection(const Point& from, const Point& to, FocusNavigationDirection direction) const {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    
    const float threshold = 0.01f; // 避免浮点误差
    
    switch (direction) {
        case FocusNavigationDirection::Up:
            return dy < -threshold;
            
        case FocusNavigationDirection::Down:
            return dy > threshold;
            
        case FocusNavigationDirection::Left:
            return dx < -threshold;
            
        case FocusNavigationDirection::Right:
            return dx > threshold;
            
        default:
            return true;
    }
}

float FocusManager::GetDirectionScore(const Point& from, const Point& to, FocusNavigationDirection direction) const {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    
    // 计算偏离主方向的距离
    switch (direction) {
        case FocusNavigationDirection::Up:
        case FocusNavigationDirection::Down:
            return std::abs(dx); // 垂直导航，横向偏离是惩罚
            
        case FocusNavigationDirection::Left:
        case FocusNavigationDirection::Right:
            return std::abs(dy); // 水平导航，纵向偏离是惩罚
            
        default:
            return 0.0f;
    }
}

} // namespace fk::ui
