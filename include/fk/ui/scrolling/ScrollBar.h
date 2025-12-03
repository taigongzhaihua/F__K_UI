/**
 * @file ScrollBar.h
 * @brief ScrollBar 滚动条控件
 * 
 * 设计理念：
 * 1. 继承自 Control，支持完全模板化
 * 2. 内部使用 Track 管理滑块和翻页区域
 * 3. 提供 LineUp/LineDown/PageUp/PageDown 等滚动操作
 * 4. 支持水平和垂直两种方向
 * 
 * WPF 对应：System.Windows.Controls.Primitives.ScrollBar
 * 
 * 使用场景：
 * - ScrollViewer 内部的滚动条
 * - 独立的滚动条控件
 */

#pragma once

#include "fk/ui/controls/Control.h"
#include "fk/ui/scrolling/Track.h"
#include "fk/ui/styling/Enums.h"
#include "fk/binding/Binding.h"
#include "fk/core/Event.h"

namespace fk::ui {

// 前向声明
class RepeatButton;

/**
 * @brief 滚动命令类型
 */
enum class ScrollEventType {
    LineUp,         ///< 向上/左滚动一行
    LineDown,       ///< 向下/右滚动一行
    PageUp,         ///< 向上/左滚动一页
    PageDown,       ///< 向下/右滚动一页
    ThumbTrack,     ///< 拖动滑块中
    ThumbPosition,  ///< 拖动滑块结束
    First,          ///< 滚动到起始位置
    Last,           ///< 滚动到结束位置
    EndScroll       ///< 滚动结束
};

/**
 * @brief 滚动事件参数
 */
struct ScrollEventArgs {
    ScrollEventType scrollEventType;
    float newValue;
    
    ScrollEventArgs(ScrollEventType type, float value)
        : scrollEventType(type), newValue(value) {}
};

/**
 * @brief 滚动条控件
 * 
 * ScrollBar 提供用户通过拖动滑块或点击按钮来滚动内容的功能。
 * 
 * 模板结构（垂直方向）：
 * ┌─────────────┐
 * │  ▲ LineUp   │  <- RepeatButton
 * ├─────────────┤
 * │   Track     │  <- 包含 Thumb 和翻页区域
 * │   ┌─────┐   │
 * │   │Thumb│   │
 * │   └─────┘   │
 * ├─────────────┤
 * │  ▼ LineDown │  <- RepeatButton
 * └─────────────┘
 * 
 * 使用示例：
 * @code
 * auto* scrollBar = new ScrollBar();
 * scrollBar->SetOrientation(Orientation::Vertical);
 * scrollBar->SetMinimum(0);
 * scrollBar->SetMaximum(1000);
 * scrollBar->SetValue(0);
 * scrollBar->SetViewportSize(100);
 * 
 * scrollBar->ValueChanged += [](float oldVal, float newVal) {
 *     // 更新内容位置
 * };
 * @endcode
 */
class ScrollBar : public Control<ScrollBar> {
public:
    ScrollBar();
    virtual ~ScrollBar();
    
    // ========== 模板部件名称 ==========
    
    static constexpr const char* PART_Track = "PART_Track";
    static constexpr const char* PART_LineUpButton = "PART_LineUpButton";
    static constexpr const char* PART_LineDownButton = "PART_LineDownButton";
    
    // ========== 依赖属性 ==========
    
    /// Orientation 属性（滚动条方向）
    static const binding::DependencyProperty& OrientationProperty();
    
    /// Minimum 属性（最小值）
    static const binding::DependencyProperty& MinimumProperty();
    
    /// Maximum 属性（最大值）
    static const binding::DependencyProperty& MaximumProperty();
    
    /// Value 属性（当前值）
    static const binding::DependencyProperty& ValueProperty();
    
    /// ViewportSize 属性（视口大小）
    static const binding::DependencyProperty& ViewportSizeProperty();
    
    /// SmallChange 属性（小步长，用于 LineUp/LineDown）
    static const binding::DependencyProperty& SmallChangeProperty();
    
    /// LargeChange 属性（大步长，用于 PageUp/PageDown）
    static const binding::DependencyProperty& LargeChangeProperty();
    
    // ========== 属性访问器 ==========
    
    /// 获取滚动条方向
    ui::Orientation GetOrientation() const;
    
    /// 设置滚动条方向
    ScrollBar* SetOrientation(ui::Orientation value);
    
    /// 链式调用
    ScrollBar* Orientation(ui::Orientation value) { return SetOrientation(value); }
    
    /// 绑定支持
    ScrollBar* Orientation(binding::Binding binding) {
        SetBinding(OrientationProperty(), std::move(binding));
        return this;
    }
    
    /// 获取最小值
    float GetMinimum() const;
    
    /// 设置最小值
    ScrollBar* SetMinimum(float value);
    
    /// 链式调用
    ScrollBar* Minimum(float value) { return SetMinimum(value); }
    
    /// 绑定支持
    ScrollBar* Minimum(binding::Binding binding) {
        SetBinding(MinimumProperty(), std::move(binding));
        return this;
    }
    
    /// 获取最大值
    float GetMaximum() const;
    
    /// 设置最大值
    ScrollBar* SetMaximum(float value);
    
    /// 链式调用
    ScrollBar* Maximum(float value) { return SetMaximum(value); }
    
    /// 绑定支持
    ScrollBar* Maximum(binding::Binding binding) {
        SetBinding(MaximumProperty(), std::move(binding));
        return this;
    }
    
    /// 获取当前值
    float GetValue() const;
    
    /// 设置当前值
    ScrollBar* SetValue(float value);
    
    /// 链式调用
    ScrollBar* Value(float value) { return SetValue(value); }
    
    /// 绑定支持
    ScrollBar* Value(binding::Binding binding) {
        SetBinding(ValueProperty(), std::move(binding));
        return this;
    }
    
    /// 获取视口大小
    float GetViewportSize() const;
    
    /// 设置视口大小
    ScrollBar* SetViewportSize(float value);
    
    /// 链式调用
    ScrollBar* ViewportSize(float value) { return SetViewportSize(value); }
    
    /// 绑定支持
    ScrollBar* ViewportSize(binding::Binding binding) {
        SetBinding(ViewportSizeProperty(), std::move(binding));
        return this;
    }
    
    /// 获取小步长
    float GetSmallChange() const;
    
    /// 设置小步长
    ScrollBar* SetSmallChange(float value);
    
    /// 链式调用
    ScrollBar* SmallChange(float value) { return SetSmallChange(value); }
    
    /// 绑定支持
    ScrollBar* SmallChange(binding::Binding binding) {
        SetBinding(SmallChangeProperty(), std::move(binding));
        return this;
    }
    
    /// 获取大步长
    float GetLargeChange() const;
    
    /// 设置大步长
    ScrollBar* SetLargeChange(float value);
    
    /// 链式调用
    ScrollBar* LargeChange(float value) { return SetLargeChange(value); }
    
    /// 绑定支持
    ScrollBar* LargeChange(binding::Binding binding) {
        SetBinding(LargeChangeProperty(), std::move(binding));
        return this;
    }
    
    // ========== 滚动操作 ==========
    
    /// 向上/左滚动一行
    void LineUp();
    
    /// 向下/右滚动一行
    void LineDown();
    
    /// 向上/左滚动一页
    void PageUp();
    
    /// 向下/右滚动一页
    void PageDown();
    
    /// 滚动到指定值
    void ScrollToValue(float value);
    
    /// 滚动到起始位置
    void ScrollToMinimum();
    
    /// 滚动到结束位置
    void ScrollToMaximum();
    
    // ========== 事件 ==========
    
    /// Value 变化事件（参数：oldValue, newValue）
    core::Event<float, float> ValueChanged;
    
    /// 滚动事件（参数：ScrollEventArgs）
    core::Event<const ScrollEventArgs&> Scroll;
    
    // ========== 模板部件访问 ==========
    
    /// 获取内部 Track
    Track* GetTrack() const { return track_; }
    
protected:
    // ========== 模板应用 ==========
    
    void OnTemplateApplied() override;
    
    // ========== 视觉状态 ==========
    
    virtual void UpdateVisualState(bool useTransitions);
    
private:
    // ========== 模板部件引用 ==========
    
    Track* track_{nullptr};
    RepeatButton* lineUpButton_{nullptr};
    RepeatButton* lineDownButton_{nullptr};
    
    // ========== 事件处理 ==========
    
    void OnTrackValueChanged(float oldVal, float newVal);
    void OnLineUpClick();
    void OnLineDownClick();
    
    // ========== 辅助方法 ==========
    
    /// 触发滚动事件
    void RaiseScrollEvent(ScrollEventType type, float newValue);
    
    /// 更新内部 Track 的属性
    void SyncTrackProperties();
};

} // namespace fk::ui
