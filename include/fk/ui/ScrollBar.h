/**
 * @file ScrollBar.h
 * @brief ScrollBar 滚动条控件
 * 
 * 职责：
 * - 提供滚动条UI控件
 * - 支持水平和垂直方向
 * - 值范围控制（Minimum/Maximum）
 * - 视口大小控制（ViewportSize）
 * - 滚动增量控制
 * 
 * WPF 对应：ScrollBar
 */

#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/Enums.h"
#include "fk/core/Event.h"

namespace fk::render {
    class RenderContext;
}

namespace fk::ui {

/**
 * @brief 滚动条控件
 * 
 * 提供可视化的滚动条，用于在有限空间内浏览大量内容。
 */
class ScrollBar : public Control<ScrollBar> {
public:
    ScrollBar();
    virtual ~ScrollBar() = default;
    
    // ========== 依赖属性 ==========
    
    /// Orientation 属性
    static const binding::DependencyProperty& OrientationProperty();
    
    /// Minimum 属性
    static const binding::DependencyProperty& MinimumProperty();
    
    /// Maximum 属性
    static const binding::DependencyProperty& MaximumProperty();
    
    /// Value 属性
    static const binding::DependencyProperty& ValueProperty();
    
    /// ViewportSize 属性（可见区域大小）
    static const binding::DependencyProperty& ViewportSizeProperty();
    
    /// SmallChange 属性（小增量）
    static const binding::DependencyProperty& SmallChangeProperty();
    
    /// LargeChange 属性（大增量）
    static const binding::DependencyProperty& LargeChangeProperty();
    
    // ========== 属性访问 ==========
    
    Orientation GetOrientation() const { return orientation_; }
    ScrollBar* SetOrientation(Orientation value) {
        orientation_ = value;
        InvalidateVisual();
        return this;
    }
    
    float GetMinimum() const { return minimum_; }
    ScrollBar* SetMinimum(float value) {
        minimum_ = value;
        CoerceValue();
        return this;
    }
    
    float GetMaximum() const { return maximum_; }
    ScrollBar* SetMaximum(float value) {
        maximum_ = value;
        CoerceValue();
        return this;
    }
    
    float GetValue() const { return value_; }
    ScrollBar* SetValue(float value) {
        float oldValue = value_;
        value_ = std::max(minimum_, std::min(maximum_, value));
        if (value_ != oldValue) {
            OnValueChanged(oldValue, value_);
        }
        return this;
    }
    
    float GetViewportSize() const { return viewportSize_; }
    ScrollBar* SetViewportSize(float value) {
        viewportSize_ = value;
        InvalidateVisual();
        return this;
    }
    
    float GetSmallChange() const { return smallChange_; }
    ScrollBar* SetSmallChange(float value) {
        smallChange_ = value;
        return this;
    }
    
    float GetLargeChange() const { return largeChange_; }
    ScrollBar* SetLargeChange(float value) {
        largeChange_ = value;
        return this;
    }
    
    // ========== 事件 ==========
    
    /// 值改变事件
    core::Event<float, float> ValueChanged;
    
    // ========== 滚动操作 ==========
    
    /// 向上/左滚动（小增量）
    void LineUp();
    
    /// 向下/右滚动（小增量）
    void LineDown();
    
    /// 向上/左滚动（大增量）
    void PageUp();
    
    /// 向下/右滚动（大增量）
    void PageDown();
    
    /// 滚动到开始
    void ScrollToStart();
    
    /// 滚动到结束
    void ScrollToEnd();
    
protected:
    // ========== 重写方法 ==========
    
    Size MeasureOverride(Size availableSize);
    Size ArrangeOverride(Size finalSize);
    void OnRender(render::RenderContext& context);
    
    // ========== 事件处理 ==========
    
    virtual void OnValueChanged(float oldValue, float newValue);
    
private:
    Orientation orientation_{Orientation::Vertical};
    float minimum_{0.0f};
    float maximum_{100.0f};
    float value_{0.0f};
    float viewportSize_{10.0f};
    float smallChange_{1.0f};
    float largeChange_{10.0f};
    
    /// 强制值在有效范围内
    void CoerceValue();
    
    /// 计算滑块位置
    Rect CalculateThumbRect(Size containerSize) const;
};

} // namespace fk::ui
