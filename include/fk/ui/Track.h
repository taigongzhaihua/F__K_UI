/**
 * @file Track.h
 * @brief Track 轨道布局组件
 * 
 * 设计理念：
 * 1. 继承自 FrameworkElement（纯布局组件，不需要模板）
 * 2. 管理 DecreaseRepeatButton、Thumb、IncreaseRepeatButton 三个子元素
 * 3. 根据 Value/Minimum/Maximum/ViewportSize 计算 Thumb 大小和位置
 * 4. 响应 Thumb 拖动，反向计算新的 Value
 * 
 * WPF 对应：System.Windows.Controls.Primitives.Track
 * 
 * 使用场景：
 * - ScrollBar 内部的轨道组件
 * - Slider 内部的轨道组件
 */

#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/Enums.h"
#include "fk/ui/Thumb.h"
#include "fk/ui/RepeatButton.h"
#include "fk/binding/Binding.h"
#include "fk/core/Event.h"
#include <cmath>
#include <algorithm>

namespace fk::ui {

/**
 * @brief 轨道布局组件
 * 
 * Track 是 ScrollBar/Slider 内部的核心布局组件，负责：
 * - 管理三个子元素的布局（DecreaseButton、Thumb、IncreaseButton）
 * - 根据 Value 计算 Thumb 位置
 * - 响应拖动事件，反向计算 Value
 * 
 * 布局结构（水平方向）：
 * ┌─────────────────────────────────────────────────────────┐
 * │ [DecreaseButton] │ ████ Thumb ████ │ [IncreaseButton]   │
 * │<-- thumbOffset -->                                      │
 * └─────────────────────────────────────────────────────────┘
 * 
 * 使用示例：
 * @code
 * auto* track = new Track();
 * track->SetOrientation(Orientation::Vertical);
 * track->SetMinimum(0);
 * track->SetMaximum(100);
 * track->SetValue(25);
 * track->SetViewportSize(20);
 * 
 * track->SetThumb(new Thumb());
 * track->SetDecreaseRepeatButton(new RepeatButton());
 * track->SetIncreaseRepeatButton(new RepeatButton());
 * 
 * track->ValueChanged += [](float oldVal, float newVal) {
 *     std::cout << "Value: " << newVal << std::endl;
 * };
 * @endcode
 */
class Track : public FrameworkElement<Track> {
public:
    Track();
    virtual ~Track();
    
    // ========== 依赖属性 ==========
    
    /// Orientation 属性（布局方向）
    static const binding::DependencyProperty& OrientationProperty();
    
    /// Minimum 属性（最小值）
    static const binding::DependencyProperty& MinimumProperty();
    
    /// Maximum 属性（最大值）
    static const binding::DependencyProperty& MaximumProperty();
    
    /// Value 属性（当前值）
    static const binding::DependencyProperty& ValueProperty();
    
    /// ViewportSize 属性（视口大小，用于计算 Thumb 大小）
    static const binding::DependencyProperty& ViewportSizeProperty();
    
    /// IsDirectionReversed 属性（是否反转方向）
    static const binding::DependencyProperty& IsDirectionReversedProperty();
    
    // ========== 属性访问器 ==========
    
    /// 获取布局方向
    ui::Orientation GetOrientation() const;
    
    /// 设置布局方向
    Track* SetOrientation(ui::Orientation value);
    
    /// 链式调用
    Track* Orientation(ui::Orientation value) { return SetOrientation(value); }
    
    /// 绑定支持
    Track* Orientation(binding::Binding binding) {
        SetBinding(OrientationProperty(), std::move(binding));
        return this;
    }
    
    /// 获取最小值
    float GetMinimum() const;
    
    /// 设置最小值
    Track* SetMinimum(float value);
    
    /// 链式调用
    Track* Minimum(float value) { return SetMinimum(value); }
    
    /// 绑定支持
    Track* Minimum(binding::Binding binding) {
        SetBinding(MinimumProperty(), std::move(binding));
        return this;
    }
    
    /// 获取最大值
    float GetMaximum() const;
    
    /// 设置最大值
    Track* SetMaximum(float value);
    
    /// 链式调用
    Track* Maximum(float value) { return SetMaximum(value); }
    
    /// 绑定支持
    Track* Maximum(binding::Binding binding) {
        SetBinding(MaximumProperty(), std::move(binding));
        return this;
    }
    
    /// 获取当前值
    float GetValue() const;
    
    /// 设置当前值
    Track* SetValue(float value);
    
    /// 链式调用
    Track* Value(float value) { return SetValue(value); }
    
    /// 绑定支持
    Track* Value(binding::Binding binding) {
        SetBinding(ValueProperty(), std::move(binding));
        return this;
    }
    
    /// 获取视口大小
    float GetViewportSize() const;
    
    /// 设置视口大小
    Track* SetViewportSize(float value);
    
    /// 链式调用
    Track* ViewportSize(float value) { return SetViewportSize(value); }
    
    /// 绑定支持
    Track* ViewportSize(binding::Binding binding) {
        SetBinding(ViewportSizeProperty(), std::move(binding));
        return this;
    }
    
    /// 获取是否反转方向
    bool GetIsDirectionReversed() const;
    
    /// 设置是否反转方向
    Track* SetIsDirectionReversed(bool value);
    
    /// 链式调用
    Track* IsDirectionReversed(bool value) { return SetIsDirectionReversed(value); }
    
    /// 绑定支持
    Track* IsDirectionReversed(binding::Binding binding) {
        SetBinding(IsDirectionReversedProperty(), std::move(binding));
        return this;
    }
    
    // ========== 组件设置 ==========
    
    /// 设置减少按钮（PageUp 区域）
    Track* SetDecreaseRepeatButton(RepeatButton* button);
    
    /// 获取减少按钮
    RepeatButton* GetDecreaseRepeatButton() const { return decreaseRepeatButton_; }
    
    /// 设置增加按钮（PageDown 区域）
    Track* SetIncreaseRepeatButton(RepeatButton* button);
    
    /// 获取增加按钮
    RepeatButton* GetIncreaseRepeatButton() const { return increaseRepeatButton_; }
    
    /// 设置 Thumb
    Track* SetThumb(Thumb* thumb);
    
    /// 获取 Thumb
    Thumb* GetThumb() const { return thumb_; }
    
    // ========== 工具方法 ==========
    
    /**
     * @brief 从拖动距离计算值变化量
     * @param horizontal 水平拖动距离（像素）
     * @param vertical 垂直拖动距离（像素）
     * @return 对应的值变化量
     */
    float ValueFromDistance(float horizontal, float vertical);
    
    // ========== 事件 ==========
    
    /// Value 变化事件（参数：oldValue, newValue）
    core::Event<float, float> ValueChanged;
    
protected:
    // ========== 布局重写 ==========
    
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
private:
    // ========== 组件引用 ==========
    
    RepeatButton* decreaseRepeatButton_{nullptr};  ///< 减少按钮（PageUp 区域）
    RepeatButton* increaseRepeatButton_{nullptr};  ///< 增加按钮（PageDown 区域）
    Thumb* thumb_{nullptr};                        ///< 可拖动的滑块
    
    // ========== 布局计算 ==========
    
    /**
     * @brief 计算 Thumb 大小
     * @param trackLength 轨道总长度
     * @return Thumb 在主轴方向的大小
     */
    float CalculateThumbSize(float trackLength);
    
    /**
     * @brief 计算 Thumb 偏移位置
     * @param trackLength 轨道总长度
     * @param thumbSize Thumb 大小
     * @return Thumb 相对于轨道起点的偏移
     */
    float CalculateThumbOffset(float trackLength, float thumbSize);
    
    // ========== 内部常量 ==========
    
    static constexpr float MinThumbSize = 16.0f;   ///< Thumb 最小尺寸
    static constexpr float DefaultThumbSize = 20.0f; ///< 默认 Thumb 尺寸（无 ViewportSize 时）
    
    // ========== 事件处理 ==========
    
    /// Thumb 拖动增量处理
    void OnThumbDragDelta(const DragDeltaEventArgs& e);
    
    /// 减少按钮点击处理
    void OnDecreaseButtonClick();
    
    /// 增加按钮点击处理
    void OnIncreaseButtonClick();
    
    // ========== 辅助方法 ==========
    
    /// 绑定组件事件
    void BindComponentEvents();
    
    /// 解绑组件事件
    void UnbindComponentEvents();
    
    /// 计算大页面变化量
    float GetLargeChange() const;
};

} // namespace fk::ui
