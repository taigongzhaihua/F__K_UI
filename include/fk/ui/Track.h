/**
 * @file Track.h
 * @brief Track 滚动条轨道 - Phase 3 Step 3
 * 
 * 设计理念：
 * 1. 管理三个子元素的布局（DecreaseButton、Thumb、IncreaseButton）
 * 2. 根据 Value/Minimum/Maximum/ViewportSize 计算 Thumb 的位置和大小
 * 3. 支持水平和垂直方向
 * 4. Track 本身不直接处理交互，由子元素触发事件
 * 
 * WPF 对应：System.Windows.Controls.Primitives.Track
 * 
 * 布局逻辑（垂直方向）：
 * ┌─────────────┐
 * │  Decrease   │ <- DecreaseRepeatButton (固定大小或 0)
 * ├─────────────┤
 * │             │ <- 轨道空间（可点击）
 * │    Thumb    │ <- Thumb（大小和位置由 Value/ViewportSize 决定）
 * │             │ <- 轨道空间（可点击）
 * ├─────────────┤
 * │  Increase   │ <- IncreaseRepeatButton (固定大小或 0)
 * └─────────────┘
 */

#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/Enums.h"

namespace fk::ui {

class RepeatButton;
class Thumb;

/**
 * @brief 滚动条轨道布局控件
 * 
 * Track 负责管理滚动条的三个主要部分的布局：
 * - DecreaseRepeatButton: 向上/左滚动按钮
 * - Thumb: 可拖动的滑块
 * - IncreaseRepeatButton: 向下/右滚动按钮
 * 
 * Track 根据 Value、Minimum、Maximum 和 ViewportSize 自动计算
 * Thumb 的位置和大小。
 * 
 * 使用示例：
 * @code
 * auto* track = new Track();
 * track->SetOrientation(Orientation::Vertical);
 * track->SetMinimum(0);
 * track->SetMaximum(100);
 * track->SetValue(25);
 * track->SetViewportSize(10);
 * 
 * track->SetDecreaseRepeatButton(decreaseButton);
 * track->SetThumb(thumb);
 * track->SetIncreaseRepeatButton(increaseButton);
 * @endcode
 */
class Track : public FrameworkElement<Track> {
public:
    Track();
    virtual ~Track();
    
    // ========== 子元素管理 ==========
    
    /// 获取向上/左按钮
    RepeatButton* GetDecreaseRepeatButton() const { return decreaseButton_; }
    
    /// 设置向上/左按钮
    void SetDecreaseRepeatButton(RepeatButton* button);
    
    /// 获取滑块
    Thumb* GetThumb() const { return thumb_; }
    
    /// 设置滑块
    void SetThumb(Thumb* thumb);
    
    /// 获取向下/右按钮
    RepeatButton* GetIncreaseRepeatButton() const { return increaseButton_; }
    
    /// 设置向下/右按钮
    void SetIncreaseRepeatButton(RepeatButton* button);
    
    // ========== 滚动条属性 ==========
    
    /// 获取方向
    Orientation GetOrientation() const { return orientation_; }
    
    /// 设置方向
    Track* SetOrientation(Orientation value) {
        orientation_ = value;
        InvalidateArrange();
        return this;
    }
    
    /// 获取最小值
    float GetMinimum() const { return minimum_; }
    
    /// 设置最小值
    Track* SetMinimum(float value) {
        minimum_ = value;
        InvalidateArrange();
        return this;
    }
    
    /// 获取最大值
    float GetMaximum() const { return maximum_; }
    
    /// 设置最大值
    Track* SetMaximum(float value) {
        maximum_ = value;
        InvalidateArrange();
        return this;
    }
    
    /// 获取当前值
    float GetValue() const { return value_; }
    
    /// 设置当前值
    Track* SetValue(float value) {
        value_ = value;
        InvalidateArrange();
        return this;
    }
    
    /// 获取视口大小
    float GetViewportSize() const { return viewportSize_; }
    
    /// 设置视口大小
    Track* SetViewportSize(float value) {
        viewportSize_ = value;
        InvalidateArrange();
        return this;
    }
    
    // ========== 计算方法 ==========
    
    /// 根据 Thumb 的偏移量计算新的 Value
    float ValueFromDistance(float horizontal, float vertical);
    
    /// 根据点击位置计算新的 Value（用于点击轨道跳转）
    float ValueFromPoint(float x, float y);
    
protected:
    // ========== 布局重写 ==========
    
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    /// 获取可视子元素数量
    int GetVisualChildrenCount() const;
    
    /// 获取指定索引的可视子元素
    UIElement* GetVisualChild(int index) const;
    
private:
    // ========== 子元素 ==========
    
    RepeatButton* decreaseButton_{nullptr};  ///< 向上/左按钮
    Thumb* thumb_{nullptr};                   ///< 滑块
    RepeatButton* increaseButton_{nullptr};  ///< 向下/右按钮
    
    // ========== 滚动条属性 ==========
    
    Orientation orientation_{Orientation::Vertical};
    float minimum_{0.0f};
    float maximum_{100.0f};
    float value_{0.0f};
    float viewportSize_{10.0f};
    
    // ========== 辅助方法 ==========
    
    /// 计算 Thumb 的大小
    float CalculateThumbSize(float trackLength) const;
    
    /// 计算 Thumb 的位置
    float CalculateThumbPosition(float trackLength, float thumbSize) const;
    
    /// 获取轨道长度（不包括按钮）
    float GetTrackLength(const Size& arrangeSize) const;
};

} // namespace fk::ui
