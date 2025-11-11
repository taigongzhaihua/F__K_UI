#pragma once

#include "fk/ui/UIElement.h"
#include "fk/core/Event.h"
#include <memory>

namespace fk::ui {

/**
 * @brief 焦点导航方向
 */
enum class FocusNavigationDirection {
    Next,       // 下一个（Tab）
    Previous,   // 上一个（Shift+Tab）
    Up,         // 向上（Arrow Up）
    Down,       // 向下（Arrow Down）
    Left,       // 向左（Arrow Left）
    Right       // 向右（Arrow Right）
};

/**
 * @brief 焦点变更事件参数
 */
struct FocusChangedEventArgs {
    UIElement* oldFocus{nullptr};   // 失去焦点的元素
    UIElement* newFocus{nullptr};   // 获得焦点的元素
    
    FocusChangedEventArgs() = default;
    FocusChangedEventArgs(UIElement* oldElem, UIElement* newElem)
        : oldFocus(oldElem), newFocus(newElem) {}
};

/**
 * @brief 焦点管理器
 * 
 * 职责：
 * - 管理当前焦点元素
 * - 处理焦点导航（Tab/Arrow键）
 * - 触发焦点变更事件
 * 
 * WPF 对应：FocusManager
 */
class FocusManager {
public:
    FocusManager();
    ~FocusManager();

    // ========== 焦点管理 ==========
    
    /**
     * @brief 设置焦点到指定元素
     * @param element 目标元素（nullptr 表示清除焦点）
     * @return 是否成功设置焦点
     */
    bool SetFocusedElement(UIElement* element);
    
    /**
     * @brief 获取当前焦点元素
     * @return 当前焦点元素（nullptr 表示无焦点）
     */
    UIElement* GetFocusedElement() const { return focusedElement_; }
    
    /**
     * @brief 清除当前焦点
     */
    void ClearFocus();

    // ========== 焦点导航 ==========
    
    /**
     * @brief 按方向移动焦点
     * @param direction 导航方向
     * @return 是否成功移动焦点
     */
    bool MoveFocus(FocusNavigationDirection direction);
    
    /**
     * @brief 查找下一个可聚焦元素
     * @param current 当前元素
     * @param direction 导航方向
     * @return 下一个可聚焦元素（nullptr 表示未找到）
     */
    UIElement* FindNextFocusable(UIElement* current, FocusNavigationDirection direction);

    // ========== 根节点管理 ==========
    
    /**
     * @brief 设置根视觉节点（用于焦点导航）
     */
    void SetRoot(Visual* root);
    
    /**
     * @brief 获取根视觉节点
     */
    Visual* GetRoot() const { return scopeRoot_; }

    // ========== 事件 ==========
    
    /**
     * @brief 焦点变更事件
     */
    core::Event<FocusChangedEventArgs>& FocusChanged() { return focusChangedEvent_; }

    // ========== 焦点能力检查 ==========
    
    /**
     * @brief 检查元素是否可聚焦
     * @param element 要检查的元素
     * @return 元素是否可以接收焦点
     */
    static bool IsFocusable(UIElement* element);

private:
    /**
     * @brief 收集所有可聚焦元素
     * @param root 根节点
     * @param focusables 输出的可聚焦元素列表
     */
    void CollectFocusableElements(Visual* root, std::vector<UIElement*>& focusables);
    
    /**
     * @brief 递归收集可聚焦元素
     */
    void CollectFocusableElementsRecursive(Visual* visual, std::vector<UIElement*>& focusables);
    
    /**
     * @brief 查找 Tab 顺序中的下一个元素
     */
    UIElement* FindNextInTabOrder(UIElement* current, bool reverse);
    
    /**
     * @brief 查找方向键导航的下一个元素
     */
    UIElement* FindNextInDirection(UIElement* current, FocusNavigationDirection direction);
    
    /**
     * @brief 通知焦点变更
     */
    void NotifyFocusChanged(UIElement* oldFocus, UIElement* newFocus);
    
    /**
     * @brief 获取元素的屏幕边界
     */
    Rect GetElementBounds(UIElement* element) const;
    
    /**
     * @brief 检查候选元素是否在指定方向上
     */
    bool IsInDirection(const Point& from, const Point& to, FocusNavigationDirection direction) const;
    
    /**
     * @brief 计算方向得分（偏离度）
     */
    float GetDirectionScore(const Point& from, const Point& to, FocusNavigationDirection direction) const;

private:
    Visual* scopeRoot_{nullptr};                            // 焦点范围根节点
    UIElement* focusedElement_{nullptr};                    // 当前焦点元素
    core::Event<FocusChangedEventArgs> focusChangedEvent_;  // 焦点变更事件
};

} // namespace fk::ui
