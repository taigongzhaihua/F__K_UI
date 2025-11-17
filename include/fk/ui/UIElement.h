#pragma once

#include "fk/ui/Visual.h"
#include "fk/ui/Primitives.h"
#include "fk/ui/Thickness.h"
#include "fk/core/Event.h"
#include "fk/binding/DependencyProperty.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <vector>

namespace fk::render {
class RenderContext;
}

namespace fk::ui {

// 前向声明
class RoutedEvent;
struct RoutedEventArgs;
struct PointerEventArgs;
struct KeyEventArgs;
class Transform;
class NameScope;

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
    
    /**
     * @brief 裁剪区域依赖属性
     * 值类型：Rect（矩形裁剪区域）
     */
    static const binding::DependencyProperty& ClipProperty();
    
    /**
     * @brief 渲染变换依赖属性
     */
    static const binding::DependencyProperty& RenderTransformProperty();

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
    
    /**
     * @brief 获取布局矩形（元素在父容器中的位置和尺寸）
     */
    Rect GetLayoutRect() const { return layoutRect_; }

    /**
     * @brief 获取元素外边距（默认没有边距）
     */
    virtual Thickness GetMargin() const;
    
    // ========== 模板支持 ==========
    
    /**
     * @brief 获取模板化父元素
     * 当元素是从 ControlTemplate 实例化时，这指向应用模板的控件
     */
    UIElement* GetTemplatedParent() const { return templatedParent_; }
    
    /**
     * @brief 设置模板化父元素（内部使用）
     */
    void SetTemplatedParent(UIElement* parent) { templatedParent_ = parent; }
    
    /**
     * @brief 获取元素名称
     * 
     * 直接使用DependencyObject的elementName_，同时支持FindName和ElementName绑定
     */
    const std::string& GetName() const { return GetElementName(); }
    
    /**
     * @brief 设置元素名称（用于 FindName 和 ElementName 绑定）
     * 
     * 统一使用DependencyObject的elementName_，避免数据冗余
     * 同时自动更新最近的 NameScope（如果存在）
     */
    void SetName(const std::string& name);
    
    /**
     * @brief 在当前元素的逻辑树中查找指定名称的元素
     * 
     * @param name 要查找的元素名称
     * @return 找到的元素指针，未找到返回 nullptr
     * 
     * 此方法会递归搜索当前元素及其所有逻辑子元素（通过GetLogicalChildren()获取）。
     * 类似于WPF中的FrameworkElement.FindName()方法。
     * 
     * 示例：
     * @code
     * auto* panel = new StackPanel();
     * auto* button = new Button();
     * button->Name("myButton");
     * panel->AddChild(button);
     * 
     * // 稍后查找
     * UIElement* found = panel->FindName("myButton");
     * if (found) {
     *     auto* btn = dynamic_cast<Button*>(found);
     *     // 使用按钮...
     * }
     * @endcode
     */
    UIElement* FindName(const std::string& name);
    
    /**
     * @brief 创建并关联 NameScope 到当前元素
     * 
     * NameScope 提供 O(1) 的名称查找性能，适用于大型UI或频繁查找的场景。
     * 
     * 注意：
     * - Window 会自动创建 NameScope
     * - 对于普通容器，可以选择性地创建 NameScope 以优化性能
     * - 创建后，所有子元素的名称会自动注册到此 NameScope
     * 
     * 示例：
     * @code
     * auto* panel = new StackPanel();
     * panel->CreateNameScope();  // 为大型面板创建 NameScope
     * 
     * // 添加子元素时会自动注册到 NameScope
     * auto* button = new Button();
     * button->Name("myButton");
     * panel->AddChild(button);
     * 
     * // O(1) 查找
     * auto* found = panel->FindName("myButton");
     * @endcode
     */
    void CreateNameScope();
    
    /**
     * @brief 获取当前元素关联的 NameScope
     * 
     * @return NameScope 指针，如果未创建返回 nullptr
     */
    NameScope* GetNameScope() const;
    
    /**
     * @brief 查找最近的 NameScope（向上遍历逻辑树）
     * 
     * @return 找到的 NameScope 指针，未找到返回 nullptr
     * 
     * 此方法会向上遍历逻辑树，查找第一个拥有 NameScope 的祖先元素
     */
    NameScope* FindNearestNameScope() const;
    
    /**
     * @brief 使用快速查找（优先使用 NameScope，回退到递归查找）
     * 
     * @param name 要查找的元素名称
     * @return 找到的元素指针，未找到返回 nullptr
     * 
     * 查找策略：
     * 1. 首先查找最近的 NameScope（O(1)）
     * 2. 如果未找到或没有 NameScope，回退到递归查找（O(n)）
     * 
     * 此方法是 FindName() 的优化版本，当存在 NameScope 时提供更好的性能
     */
    UIElement* FindNameFast(const std::string& name);

    // ========== 事件处理 ==========
    
    void SetVisibility(Visibility value);
    Visibility GetVisibility() const;

    // ========== 交互状态 ==========
    
    void SetIsEnabled(bool value);
    bool GetIsEnabled() const;
    
    // ========== 不透明度 ==========
    
    void SetOpacity(float value);
    float GetOpacity() const;
    
    // ========== 裁剪 ==========
    
    void SetClip(const Rect& value);
    Rect GetClip() const;
    bool HasClip() const;
    
    // ========== 变换 ==========
    
    void SetRenderTransform(Transform* value);
    Transform* GetRenderTransform() const;

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
    
    // ========== 逻辑树遍历 ==========
    
    /**
     * @brief 获取逻辑子元素（虚方法，容器类覆写）
     * @return 子元素列表（不拥有所有权）
     * 
     * 容器类（如 Panel）应覆写此方法返回其子元素集合。
     * 用于模板系统的递归遍历、FindName 等功能。
     */
    virtual std::vector<UIElement*> GetLogicalChildren() const;
    
    // ========== 对象克隆 ==========
    
    /**
     * @brief 深拷贝当前元素（虚方法，各派生类实现）
     * @return 新创建的克隆对象（调用者拥有所有权）
     * 
     * 用于模板系统克隆视觉树。
     * 派生类应覆写此方法实现完整的深拷贝，包括：
     * - 依赖属性值
     * - 子元素（递归克隆）
     * - 元素名称
     * 
     * 注意：返回的指针需要调用者管理内存
     */
    virtual UIElement* Clone() const;
    
    // ========== 渲染 ==========
    
    /**
     * @brief 收集绘制命令（重写 Visual 的方法）
     * 
     * UIElement 在此处理布局偏移，将元素的布局位置转换为渲染坐标
     */
    void CollectDrawCommands(render::RenderContext& context) override;

protected:
    /**
     * @brief 派生类用于绘制自身内容
     */
    virtual void OnRender(render::RenderContext& context);
    
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
    
    /**
     * @brief 设置渲染尺寸（由 ArrangeCore 使用）
     */
    void SetRenderSize(const Size& size) { renderSize_ = size; }

private:
    Size desiredSize_;
    Size renderSize_;
    Rect layoutRect_;  // 布局矩形（父容器坐标系）
    bool measureDirty_{true};
    bool arrangeDirty_{true};
    
    // 注意：元素名称现在统一使用继承自DependencyObject的elementName_
    // 这样FindName和ElementName绑定都使用同一个存储，避免冗余
    
    // 模板化父元素（从 ControlTemplate 实例化时设置）
    UIElement* templatedParent_{nullptr};
    
    // 事件处理器映射
    std::unordered_map<RoutedEvent*, std::vector<EventHandler>> eventHandlers_;
    
    // 拥有的子对象（自动管理内存）
    std::vector<std::unique_ptr<UIElement>> ownedChildren_;
    
    // NameScope 实例（性能优化：提供 O(1) 名称查找）
    std::unique_ptr<NameScope> nameScope_;
    
    // 辅助方法：递归注册子元素名称到 NameScope
    static void RegisterNamesToScope(UIElement* element, NameScope* scope);
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
 * @brief 鼠标按钮枚举
 */
enum class MouseButton {
    None = 0,
    Left = 1,
    Right = 2,
    Middle = 3
};

/**
 * @brief 修饰键枚举（可按位组合）
 */
enum class ModifierKeys {
    None = 0,
    Ctrl = 1 << 0,   // 0x01
    Shift = 1 << 1,  // 0x02
    Alt = 1 << 2     // 0x04
};

// 修饰键按位操作符
inline ModifierKeys operator|(ModifierKeys a, ModifierKeys b) {
    return static_cast<ModifierKeys>(static_cast<int>(a) | static_cast<int>(b));
}

inline ModifierKeys operator&(ModifierKeys a, ModifierKeys b) {
    return static_cast<ModifierKeys>(static_cast<int>(a) & static_cast<int>(b));
}

inline bool HasModifier(ModifierKeys keys, ModifierKeys modifier) {
    return (keys & modifier) == modifier;
}

/**
 * @brief 指针/鼠标事件参数
 */
struct PointerEventArgs : RoutedEventArgs {
    Point position;
    int pointerId{0};
    MouseButton button{MouseButton::None};
    ModifierKeys modifiers{ModifierKeys::None};
    
    PointerEventArgs() = default;
    PointerEventArgs(UIElement* src, const Point& pos, int id = 0)
        : RoutedEventArgs(src), position(pos), pointerId(id) {}
    
    // 便捷方法
    bool IsLeftButton() const { return button == MouseButton::Left; }
    bool IsRightButton() const { return button == MouseButton::Right; }
    bool IsMiddleButton() const { return button == MouseButton::Middle; }
    bool HasCtrl() const { return HasModifier(modifiers, ModifierKeys::Ctrl); }
    bool HasShift() const { return HasModifier(modifiers, ModifierKeys::Shift); }
    bool HasAlt() const { return HasModifier(modifiers, ModifierKeys::Alt); }
};

/**
 * @brief 键盘事件参数
 */
struct KeyEventArgs : RoutedEventArgs {
    int key{0};
    bool isRepeat{false};
    ModifierKeys modifiers{ModifierKeys::None};
    
    KeyEventArgs() = default;
    KeyEventArgs(UIElement* src, int k, bool repeat = false)
        : RoutedEventArgs(src), key(k), isRepeat(repeat) {}
    
    // 便捷方法
    bool HasCtrl() const { return HasModifier(modifiers, ModifierKeys::Ctrl); }
    bool HasShift() const { return HasModifier(modifiers, ModifierKeys::Shift); }
    bool HasAlt() const { return HasModifier(modifiers, ModifierKeys::Alt); }
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
