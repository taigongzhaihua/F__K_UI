#pragma once

#include "fk/ui/Visual.h"
#include "fk/ui/Primitives.h"
#include "fk/core/Event.h"
#include "fk/binding/DependencyProperty.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>

namespace fk::ui {

// 前向声明
class RoutedEvent;
struct RoutedEventArgs;
struct PointerEventArgs;
struct KeyEventArgs;

/**
 * @brief 可见性枚举
 */
enum class Visibility {
    Visible,    // 可见且参与布局
    Hidden,     // 不可见但参与布局
    Collapsed   // 不可见且不参与布局
};

/**
 * @brief UI 元素基类
 * 
 * 职责：
 * - 输入事件处理
 * - 布局系统（Measure/Arrange）
 * - 路由事件机制
 * 
 * 继承：Visual
 */
class UIElement : public Visual {
public:
    UIElement();
    virtual ~UIElement();

    // ========== 依赖属性声明 ==========
    
    /**
     * @brief 可见性依赖属性
     */
    static const binding::DependencyProperty& VisibilityProperty();
    
    /**
     * @brief 启用状态依赖属性
     */
    static const binding::DependencyProperty& IsEnabledProperty();
    
    /**
     * @brief 不透明度依赖属性（0.0 - 1.0）
     */
    static const binding::DependencyProperty& OpacityProperty();
    
    // TODO: ClipProperty - 裁剪区域依赖属性
    // TODO: RenderTransformProperty - 渲染变换依赖属性

    // ========== 布局 ==========
    
    /**
     * @brief 测量期望尺寸
     */
    void Measure(const Size& availableSize);
    
    /**
     * @brief 确定最终布局
     */
    void Arrange(const Rect& finalRect);
    
    /**
     * @brief 标记需要重新测量
     */
    void InvalidateMeasure();
    
    /**
     * @brief 标记需要重新排列
     */
    void InvalidateArrange();
    
    /**
     * @brief 获取期望尺寸
     */
    Size GetDesiredSize() const { return desiredSize_; }
    
    /**
     * @brief 获取渲染尺寸
     */
    Size GetRenderSize() const { return renderSize_; }

    // ========== 可见性 ==========
    
    void SetVisibility(Visibility value);
    Visibility GetVisibility() const;

    // ========== 交互状态 ==========
    
    void SetIsEnabled(bool value);
    bool GetIsEnabled() const;
    
    // ========== 不透明度 ==========
    
    void SetOpacity(float value);
    float GetOpacity() const;

    // ========== 路由事件 ==========
    
    using EventHandler = std::function<void(UIElement*, RoutedEventArgs&)>;
    
    /**
     * @brief 触发路由事件
     */
    void RaiseEvent(RoutedEventArgs& args);
    
    /**
     * @brief 注册事件处理器
     */
    void AddHandler(RoutedEvent* routedEvent, EventHandler handler);
    
    /**
     * @brief 移除事件处理器
     */
    void RemoveHandler(RoutedEvent* routedEvent, EventHandler handler);

    // ========== 输入事件（虚函数） ==========
    
    virtual void OnPointerPressed(PointerEventArgs& e);
    virtual void OnPointerReleased(PointerEventArgs& e);
    virtual void OnPointerMoved(PointerEventArgs& e);
    virtual void OnPointerEntered(PointerEventArgs& e);
    virtual void OnPointerExited(PointerEventArgs& e);
    virtual void OnKeyDown(KeyEventArgs& e);
    virtual void OnKeyUp(KeyEventArgs& e);

protected:
    /**
     * @brief 核心测量逻辑（派生类覆写）
     */
    virtual Size MeasureCore(const Size& availableSize);
    
    /**
     * @brief 核心排列逻辑（派生类覆写）
     */
    virtual void ArrangeCore(const Rect& finalRect);
    
    /**
     * @brief 获取子对象所有权（用于自动内存管理）
     * 当子对象添加到父对象时，父对象调用此方法获取所有权
     * 父对象析构时会自动释放所有拥有的子对象
     */
    void TakeOwnership(UIElement* child);

private:
    Size desiredSize_;
    Size renderSize_;
    bool measureDirty_{true};
    bool arrangeDirty_{true};
    
    // 事件处理器映射
    std::unordered_map<RoutedEvent*, std::vector<EventHandler>> eventHandlers_;
    
    // 拥有的子对象（自动管理内存）
    std::vector<std::unique_ptr<UIElement>> ownedChildren_;
};

/**
 * @brief 事件参数基类
 */
struct EventArgs {
    virtual ~EventArgs() = default;
};

/**
 * @brief 路由事件参数基类
 */
struct RoutedEventArgs : EventArgs {
    UIElement* source{nullptr};
    bool handled{false};
    
    RoutedEventArgs() = default;
    explicit RoutedEventArgs(UIElement* src) : source(src) {}
};

/**
 * @brief 指针事件参数
 */
struct PointerEventArgs : RoutedEventArgs {
    Point position;
    int pointerId{0};
    // TODO: 扩展鼠标按钮、修饰键等
    
    PointerEventArgs() = default;
    PointerEventArgs(UIElement* src, const Point& pos, int id = 0)
        : RoutedEventArgs(src), position(pos), pointerId(id) {}
};

/**
 * @brief 键盘事件参数
 */
struct KeyEventArgs : RoutedEventArgs {
    int key{0};
    bool isRepeat{false};
    // TODO: 扩展修饰键等
    
    KeyEventArgs() = default;
    KeyEventArgs(UIElement* src, int k, bool repeat = false)
        : RoutedEventArgs(src), key(k), isRepeat(repeat) {}
};

/**
 * @brief 路由事件定义（基于 core::Event）
 */
class RoutedEvent : public core::Event<UIElement*, RoutedEventArgs&> {
public:
    enum class RoutingStrategy {
        Direct,     // 直接路由
        Bubble,     // 冒泡路由
        Tunnel      // 隧道路由
    };
    
    explicit RoutedEvent(const std::string& name, RoutingStrategy strategy = RoutingStrategy::Bubble)
        : name_(name), strategy_(strategy) {}
    
    const std::string& GetName() const { return name_; }
    RoutingStrategy GetStrategy() const { return strategy_; }
    
private:
    std::string name_;
    RoutingStrategy strategy_;
};

} // namespace fk::ui
