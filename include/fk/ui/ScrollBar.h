/**
 * @file ScrollBar.h
 * @brief ScrollBar 滚动条控件 - 新设计（WPF 风格）
 * 
 * 设计理念：
 * 1. 采用 WPF ScrollBar 的架构设计
 * 2. 完全可模板化 - 支持通过 ControlTemplate 自定义外观
 * 3. 由三个主要部分组成（通过模板定义）：
 *    - Track: 滚动轨道（包含 Thumb 和两个 RepeatButton）
 *    - Thumb: 可拖动的滑块
 *    - RepeatButton: 两端的递增/递减按钮
 * 4. 支持 RangeBase 模式（继承值范围行为）
 * 
 * WPF 对应：ScrollBar (继承自 RangeBase)
 * 
 * TODO 实现计划：
 * - Phase 1: 基础架构和接口定义 ✓
 * - Phase 2: RangeBase 行为实现（或直接集成）
 * - Phase 3: Track/Thumb/RepeatButton 模板部分
 * - Phase 4: 鼠标交互和拖动支持
 * - Phase 5: 样式和动画
 */

#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Enums.h"
#include "fk/core/Event.h"

namespace fk::ui {

// 前向声明（待实现的组件，Phase 3）
// class Track;
// class Thumb;
// class RepeatButton;

/**
 * @brief 滚动条控件（新设计 - WPF 风格）
 * 
 * 架构特点：
 * 1. 作为 RangeBase 的特化，提供滚动条特有的功能
 * 2. 完全基于模板 - 外观由 ControlTemplate 定义
 * 3. 默认模板包含：
 *    - LineUpButton (RepeatButton): 向上/左按钮
 *    - LineDownButton (RepeatButton): 向下/右按钮
 *    - Track: 包含 Thumb 和可点击区域的轨道
 * 4. ViewportSize 决定 Thumb 的大小（比例）
 * 
 * 使用示例：
 * @code
 * auto scrollBar = new ScrollBar();
 * scrollBar->SetOrientation(Orientation::Vertical);
 * scrollBar->SetMinimum(0);
 * scrollBar->SetMaximum(100);
 * scrollBar->SetValue(50);
 * scrollBar->SetViewportSize(10);
 * @endcode
 */
class ScrollBar : public Control<ScrollBar> {
public:
    ScrollBar();
    virtual ~ScrollBar() = default;
    
    // ========== 依赖属性（TODO: 完整实现）==========
    
    /// Orientation 属性
    static const binding::DependencyProperty& OrientationProperty();
    
    /// Minimum 属性（继承自 RangeBase 概念）
    static const binding::DependencyProperty& MinimumProperty();
    
    /// Maximum 属性（继承自 RangeBase 概念）
    static const binding::DependencyProperty& MaximumProperty();
    
    /// Value 属性（继承自 RangeBase 概念）
    static const binding::DependencyProperty& ValueProperty();
    
    /// ViewportSize 属性（滚动条特有）
    static const binding::DependencyProperty& ViewportSizeProperty();
    
    /// SmallChange 属性（小增量，用于 RepeatButton）
    static const binding::DependencyProperty& SmallChangeProperty();
    
    /// LargeChange 属性（大增量，用于 Track 点击）
    static const binding::DependencyProperty& LargeChangeProperty();
    
    // ========== 属性访问器 ==========
    
    Orientation GetOrientation() const;
    ScrollBar* SetOrientation(Orientation value);
    
    float GetMinimum() const;
    ScrollBar* SetMinimum(float value);
    
    float GetMaximum() const;
    ScrollBar* SetMaximum(float value);
    
    float GetValue() const;
    ScrollBar* SetValue(float value);
    
    float GetViewportSize() const;
    ScrollBar* SetViewportSize(float value);
    
    float GetSmallChange() const;
    ScrollBar* SetSmallChange(float value);
    
    float GetLargeChange() const;
    ScrollBar* SetLargeChange(float value);
    
    // ========== 事件 ==========
    
    /// 值改变事件（TODO: 改为路由事件）
    core::Event<float, float> ValueChanged;
    
    /// Scroll 事件（WPF 特有，提供滚动类型信息）
    // TODO: 添加 Scroll 路由事件
    
    // ========== 命令方法 ==========
    
    /// 向上/左滚动（SmallChange）
    void LineUp();
    
    /// 向下/右滚动（SmallChange）
    void LineDown();
    
    /// 向上/左滚动（LargeChange）
    void PageUp();
    
    /// 向下/右滚动（LargeChange）
    void PageDown();
    
    /// 滚动到开始位置
    void ScrollToStart();
    
    /// 滚动到结束位置
    void ScrollToEnd();
    
protected:
    // ========== 模板部分（TODO）==========
    
    // 在完整实现中，这些方法将处理模板元素的获取和事件绑定
    // virtual void OnApplyTemplate() override;
    
    // ========== 事件处理 ==========
    
    virtual void OnValueChanged(float oldValue, float newValue);
    
    // ========== 内部状态（临时）==========
    
    Orientation orientation_{Orientation::Vertical};
    float minimum_{0.0f};
    float maximum_{100.0f};
    float value_{0.0f};
    float viewportSize_{10.0f};
    float smallChange_{1.0f};
    float largeChange_{10.0f};
    
private:
    /// 强制值在有效范围内
    void CoerceValue();
};

/**
 * @brief Track - 滚动条轨道（WPF 中的 Track 类）
 * 
 * TODO: Phase 3 实现
 * Track 是一个特殊的布局控件，包含三个子元素：
 * - DecreaseRepeatButton: 在 Thumb 之前的区域
 * - Thumb: 可拖动的滑块
 * - IncreaseRepeatButton: 在 Thumb 之后的区域
 * 
 * 实现时将创建单独的头文件：fk/ui/Track.h
 */
// class Track : public UIElement;

/**
 * @brief Thumb - 可拖动的滑块（WPF 中的 Thumb 类）
 * 
 * TODO: Phase 3 实现
 * Thumb 提供拖动功能，触发 DragStarted、DragDelta、DragCompleted 事件
 * 
 * 实现时将创建单独的头文件：fk/ui/Thumb.h
 */
// class Thumb : public Control<Thumb>;

/**
 * @brief RepeatButton - 重复按钮（WPF 中的 RepeatButton 类）
 * 
 * TODO: Phase 3 实现
 * RepeatButton 在按住时重复触发 Click 事件
 * 用于滚动条两端的递增/递减按钮
 * 
 * 实现时将创建单独的头文件：fk/ui/RepeatButton.h
 */
// class RepeatButton : public Control<RepeatButton>;

} // namespace fk::ui
