/**
 * @file ScrollViewer.h
 * @brief ScrollViewer 滚动查看器控件
 * 
 * 职责：
 * - 提供可滚动的内容容器
 * - 自动显示/隐藏滚动条
 * - 内容裁剪和偏移
 * - 鼠标滚轮支持
 * 
 * WPF 对应：ScrollViewer
 */

#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/ScrollBar.h"
#include "fk/ui/Enums.h"

namespace fk::ui {

/**
 * @brief 滚动条可见性模式
 */
enum class ScrollBarVisibility {
    Disabled,   ///< 禁用滚动条，内容不可滚动
    Auto,       ///< 需要时自动显示滚动条
    Hidden,     ///< 隐藏滚动条但内容可滚动
    Visible     ///< 始终显示滚动条
};

/**
 * @brief 滚动查看器控件
 * 
 * 提供带滚动条的内容查看区域，支持内容超出时的滚动浏览。
 */
class ScrollViewer : public ContentControl<ScrollViewer> {
public:
    ScrollViewer();
    virtual ~ScrollViewer() = default;
    
    // ========== 依赖属性 ==========
    
    /// HorizontalScrollBarVisibility 属性
    static const binding::DependencyProperty& HorizontalScrollBarVisibilityProperty();
    
    /// VerticalScrollBarVisibility 属性
    static const binding::DependencyProperty& VerticalScrollBarVisibilityProperty();
    
    /// HorizontalOffset 属性
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /// VerticalOffset 属性
    static const binding::DependencyProperty& VerticalOffsetProperty();
    
    /// ViewportWidth 属性（只读）
    static const binding::DependencyProperty& ViewportWidthProperty();
    
    /// ViewportHeight 属性（只读）
    static const binding::DependencyProperty& ViewportHeightProperty();
    
    /// ExtentWidth 属性（内容总宽度，只读）
    static const binding::DependencyProperty& ExtentWidthProperty();
    
    /// ExtentHeight 属性（内容总高度，只读）
    static const binding::DependencyProperty& ExtentHeightProperty();
    
    // ========== 属性访问 ==========
    
    ScrollBarVisibility GetHorizontalScrollBarVisibility() const { return horizontalScrollBarVisibility_; }
    ScrollViewer* SetHorizontalScrollBarVisibility(ScrollBarVisibility value) {
        horizontalScrollBarVisibility_ = value;
        UpdateScrollBarsVisibility();
        return this;
    }
    
    ScrollBarVisibility GetVerticalScrollBarVisibility() const { return verticalScrollBarVisibility_; }
    ScrollViewer* SetVerticalScrollBarVisibility(ScrollBarVisibility value) {
        verticalScrollBarVisibility_ = value;
        UpdateScrollBarsVisibility();
        return this;
    }
    
    float GetHorizontalOffset() const { return horizontalOffset_; }
    void SetHorizontalOffset(float value);
    
    float GetVerticalOffset() const { return verticalOffset_; }
    void SetVerticalOffset(float value);
    
    float GetViewportWidth() const { return viewportWidth_; }
    float GetViewportHeight() const { return viewportHeight_; }
    
    float GetExtentWidth() const { return extentWidth_; }
    float GetExtentHeight() const { return extentHeight_; }
    
    // ========== 滚动方法 ==========
    
    /// 向左滚动一行
    void LineLeft();
    
    /// 向右滚动一行
    void LineRight();
    
    /// 向上滚动一行
    void LineUp();
    
    /// 向下滚动一行
    void LineDown();
    
    /// 向上翻页
    void PageUp();
    
    /// 向下翻页
    void PageDown();
    
    /// 向左翻页
    void PageLeft();
    
    /// 向右翻页
    void PageRight();
    
    /// 滚动到顶部
    void ScrollToTop();
    
    /// 滚动到底部
    void ScrollToBottom();
    
    /// 滚动到最左
    void ScrollToLeftEnd();
    
    /// 滚动到最右
    void ScrollToRightEnd();
    
    /// 滚动到指定位置
    void ScrollToHorizontalOffset(float offset);
    void ScrollToVerticalOffset(float offset);
    
    // ========== 滚动条访问 ==========
    
    ScrollBar* GetHorizontalScrollBar() const { return horizontalScrollBar_; }
    ScrollBar* GetVerticalScrollBar() const { return verticalScrollBar_; }
    
protected:
    // ========== 重写方法 ==========
    
    Size MeasureOverride(Size availableSize);
    Size ArrangeOverride(Size finalSize);
    void OnRender(class RenderContext& context);
    
    // ========== 事件处理 ==========
    
    virtual void OnScrollChanged();
    
private:
    ScrollBarVisibility horizontalScrollBarVisibility_{ScrollBarVisibility::Auto};
    ScrollBarVisibility verticalScrollBarVisibility_{ScrollBarVisibility::Auto};
    
    float horizontalOffset_{0.0f};
    float verticalOffset_{0.0f};
    float viewportWidth_{0.0f};
    float viewportHeight_{0.0f};
    float extentWidth_{0.0f};
    float extentHeight_{0.0f};
    
    ScrollBar* horizontalScrollBar_{nullptr};
    ScrollBar* verticalScrollBar_{nullptr};
    
    /// 更新滚动条可见性
    void UpdateScrollBarsVisibility();
    
    /// 更新滚动条范围
    void UpdateScrollBarsRange();
    
    /// 处理滚动条值变更
    void OnHorizontalScrollBarValueChanged(float oldValue, float newValue);
    void OnVerticalScrollBarValueChanged(float oldValue, float newValue);
    
    /// 计算视口矩形
    Rect CalculateViewportRect() const;
};

} // namespace fk::ui
