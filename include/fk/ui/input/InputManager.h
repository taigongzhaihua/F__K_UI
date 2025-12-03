#pragma once

#include "fk/ui/graphics/Primitives.h"
#include "fk/ui/base/UIElement.h"
#include <unordered_map>
#include <memory>
#include <functional>

namespace fk::ui {

// 前向声明
class Visual;
class FocusManager;

/**
 * @brief 平台指针事件
 */
struct PlatformPointerEvent {
    enum class Type {
        Move,
        Down,
        Up,
        Enter,
        Leave,
        Wheel
    };
    
    Type type{Type::Move};
    Point position;          // 屏幕坐标
    int pointerId{0};        // 指针ID（支持多点触控）
    int button{0};           // 按钮（0=左键, 1=中键, 2=右键）
    int wheelDelta{0};       // 滚轮增量
    bool ctrlKey{false};     // Ctrl 修饰键
    bool shiftKey{false};    // Shift 修饰键
    bool altKey{false};      // Alt 修饰键
};

/**
 * @brief 平台键盘事件
 */
struct PlatformKeyEvent {
    enum class Type {
        Down,
        Up,
        Char
    };
    
    Type type{Type::Down};
    int key{0};              // 键码
    int scanCode{0};         // 扫描码
    char32_t character{0};   // Unicode 字符
    bool isRepeat{false};    // 是否为重复按键
    bool ctrlKey{false};     // Ctrl 修饰键
    bool shiftKey{false};    // Shift 修饰键
    bool altKey{false};      // Alt 修饰键
};

/**
 * @brief 输入管理器
 * 
 * 职责：
 * - 处理平台输入事件
 * - 执行命中测试
 * - 管理指针捕获
 * - 分发事件到目标元素
 * 
 * WPF 对应：InputManager
 */
class InputManager {
public:
    InputManager();
    ~InputManager();

    // ========== 根节点管理 ==========
    
    /**
     * @brief 设置根视觉节点
     */
    void SetRoot(Visual* root);
    
    /**
     * @brief 获取根视觉节点
     */
    Visual* GetRoot() const { return root_; }

    // ========== 指针事件处理 ==========
    
    /**
     * @brief 处理平台指针事件
     * @param event 平台指针事件
     */
    void ProcessPointerEvent(const PlatformPointerEvent& event);
    
    /**
     * @brief 执行命中测试
     * @param screenPoint 屏幕坐标点
     * @param root 根视觉节点（nullptr 则使用当前根）
     * @return 命中的元素（nullptr 表示未命中）
     */
    UIElement* HitTest(const Point& screenPoint, Visual* root = nullptr);

    // ========== 指针捕获 ==========
    
    /**
     * @brief 捕获指针到指定元素
     * @param element 目标元素
     * @param pointerId 指针ID
     */
    void CapturePointer(UIElement* element, int pointerId = 0);
    
    /**
     * @brief 释放指针捕获
     * @param pointerId 指针ID
     */
    void ReleasePointerCapture(int pointerId = 0);
    
    /**
     * @brief 获取当前捕获指针的元素
     * @param pointerId 指针ID
     * @return 捕获的元素（nullptr 表示未捕获）
     */
    UIElement* GetPointerCapture(int pointerId = 0) const;

    // ========== 键盘事件处理 ==========
    
    /**
     * @brief 处理键盘事件
     * @param event 平台键盘事件
     */
    void ProcessKeyboardEvent(const PlatformKeyEvent& event);
    
    /**
     * @brief 设置焦点管理器
     * @param focusManager 焦点管理器指针
     */
    void SetFocusManager(FocusManager* focusManager);
    
    /**
     * @brief 获取当前焦点元素（与 FocusManager 交互）
     */
    UIElement* GetFocusedElement() const;

    // ========== 鼠标悬停追踪 ==========
    
    /**
     * @brief 获取当前鼠标悬停的元素
     */
    UIElement* GetMouseOverElement() const { return mouseOverElement_; }
    
    /**
     * @brief 更新鼠标悬停状态
     */
    void UpdateMouseOver(const Point& position);

private:
    /**
     * @brief 执行递归命中测试
     */
    UIElement* HitTestRecursive(Visual* visual, const Point& localPoint);
    
    /**
     * @brief 分发指针按下事件
     */
    void DispatchPointerDown(UIElement* target, const PlatformPointerEvent& event);
    
    /**
     * @brief 分发指针释放事件
     */
    void DispatchPointerUp(UIElement* target, const PlatformPointerEvent& event);
    
    /**
     * @brief 分发指针移动事件
     */
    void DispatchPointerMove(UIElement* target, const PlatformPointerEvent& event);
    
    /**
     * @brief 分发指针进入/离开事件
     */
    void DispatchPointerEnter(UIElement* target, const PlatformPointerEvent& event);
    void DispatchPointerLeave(UIElement* target, const PlatformPointerEvent& event);
    
    /**
     * @brief 分发鼠标滚轮事件
     */
    void DispatchMouseWheel(UIElement* target, const PlatformPointerEvent& event);

    void BubblePointerEvent(
        UIElement* target,
        const PlatformPointerEvent& event,
        const std::function<void(UIElement*, PointerEventArgs&)>& dispatcher);
    PointerEventArgs CreatePointerArgs(UIElement* source, const PlatformPointerEvent& event) const;
    ModifierKeys BuildModifiers(const PlatformPointerEvent& event) const;
    MouseButton ConvertButton(int button) const;
    UIElement* GetBubbleParent(UIElement* current) const;
    
    /**
     * @brief 分发键盘按下事件
     */
    void DispatchKeyDown(UIElement* target, const PlatformKeyEvent& event);
    
    /**
     * @brief 分发键盘释放事件
     */
    void DispatchKeyUp(UIElement* target, const PlatformKeyEvent& event);

private:
    Visual* root_{nullptr};                                    // 根视觉节点
    FocusManager* focusManager_{nullptr};                      // 焦点管理器
    std::unordered_map<int, UIElement*> pointerCaptures_;     // 指针捕获映射
    UIElement* mouseOverElement_{nullptr};                     // 当前鼠标悬停元素
    Point lastPointerPosition_;                                // 最后指针位置
    std::unordered_map<int, UIElement*> pointerDownTargets_;   // 记录按下的元素
};

} // namespace fk::ui
