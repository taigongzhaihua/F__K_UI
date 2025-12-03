/**
 * @file ScrollViewer.h
 * @brief ScrollViewer 滚动视图控件
 * 
 * 设计理念：
 * 1. 继承自 ContentControl，支持完全模板化
 * 2. 使用 ScrollContentPresenter 展示可滚动内容
 * 3. 使用 ScrollBar 提供用户交互
 * 4. 支持鼠标滚轮滚动
 * 5. 支持 ScrollBarVisibility 控制滚动条显示
 * 
 * WPF 对应：System.Windows.Controls.ScrollViewer
 * 
 * 模板结构：
 * ┌───────────────────┬───┐
 * │                   │ ▲ │
 * │   ScrollContent   │   │
 * │    Presenter      │ V │ <- 垂直 ScrollBar
 * │                   │   │
 * │                   │ ▼ │
 * ├───────────────────┼───┤
 * │ ◄     H      ►    │   │ <- 水平 ScrollBar
 * └───────────────────┴───┘
 */

#pragma once

#include "fk/ui/controls/ContentControl.h"
#include "fk/ui/scrolling/ScrollContentPresenter.h"
#include "fk/ui/scrolling/ScrollBar.h"
#include "fk/ui/styling/Enums.h"
#include "fk/binding/Binding.h"
#include "fk/core/Event.h"

namespace fk::ui {

/**
 * @brief 滚动变化事件参数
 */
struct ScrollChangedEventArgs {
    double horizontalChange;   ///< 水平偏移变化量
    double verticalChange;     ///< 垂直偏移变化量
    double horizontalOffset;   ///< 当前水平偏移
    double verticalOffset;     ///< 当前垂直偏移
    double extentWidth;        ///< 内容宽度
    double extentHeight;       ///< 内容高度
    double viewportWidth;      ///< 视口宽度
    double viewportHeight;     ///< 视口高度
    
    ScrollChangedEventArgs() = default;
    ScrollChangedEventArgs(double hChange, double vChange,
                          double hOffset, double vOffset,
                          double extW, double extH,
                          double vpW, double vpH)
        : horizontalChange(hChange), verticalChange(vChange)
        , horizontalOffset(hOffset), verticalOffset(vOffset)
        , extentWidth(extW), extentHeight(extH)
        , viewportWidth(vpW), viewportHeight(vpH) {}
};

/**
 * @brief 滚动视图控件
 * 
 * ScrollViewer 提供可滚动的内容视图，支持水平和垂直滚动。
 * 
 * 使用示例：
 * @code
 * auto* scrollViewer = new ScrollViewer();
 * scrollViewer->HorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
 * scrollViewer->VerticalScrollBarVisibility(ScrollBarVisibility::Visible);
 * 
 * auto* content = new StackPanel();
 * // ... 添加很多子元素
 * scrollViewer->Content(content);
 * 
 * scrollViewer->ScrollChanged += [](const ScrollChangedEventArgs& e) {
 *     // 响应滚动变化
 * };
 * @endcode
 */
class ScrollViewer : public ContentControl<ScrollViewer> {
public:
    ScrollViewer();
    virtual ~ScrollViewer();
    
    // ========== 模板部件名称 ==========
    
    static constexpr const char* PART_ScrollContentPresenter = "PART_ScrollContentPresenter";
    static constexpr const char* PART_VerticalScrollBar = "PART_VerticalScrollBar";
    static constexpr const char* PART_HorizontalScrollBar = "PART_HorizontalScrollBar";
    
    // ========== 依赖属性 ==========
    
    /// HorizontalScrollBarVisibility 属性
    static const binding::DependencyProperty& HorizontalScrollBarVisibilityProperty();
    
    /// VerticalScrollBarVisibility 属性
    static const binding::DependencyProperty& VerticalScrollBarVisibilityProperty();
    
    /// HorizontalOffset 属性（只读）
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /// VerticalOffset 属性（只读）
    static const binding::DependencyProperty& VerticalOffsetProperty();
    
    /// CanContentScroll 属性（是否启用逻辑滚动）
    static const binding::DependencyProperty& CanContentScrollProperty();
    
    // ========== 属性访问器 ==========
    
    // ----- HorizontalScrollBarVisibility -----
    
    ScrollBarVisibility GetHorizontalScrollBarVisibility() const;
    ScrollViewer* SetHorizontalScrollBarVisibility(ScrollBarVisibility value);
    ScrollViewer* HorizontalScrollBarVisibility(ScrollBarVisibility value) {
        return SetHorizontalScrollBarVisibility(value);
    }
    ScrollViewer* HorizontalScrollBarVisibility(binding::Binding binding) {
        SetBinding(HorizontalScrollBarVisibilityProperty(), std::move(binding));
        return this;
    }
    
    // ----- VerticalScrollBarVisibility -----
    
    ScrollBarVisibility GetVerticalScrollBarVisibility() const;
    ScrollViewer* SetVerticalScrollBarVisibility(ScrollBarVisibility value);
    ScrollViewer* VerticalScrollBarVisibility(ScrollBarVisibility value) {
        return SetVerticalScrollBarVisibility(value);
    }
    ScrollViewer* VerticalScrollBarVisibility(binding::Binding binding) {
        SetBinding(VerticalScrollBarVisibilityProperty(), std::move(binding));
        return this;
    }
    
    // ----- HorizontalOffset (只读) -----
    
    double GetHorizontalOffset() const;
    
    // ----- VerticalOffset (只读) -----
    
    double GetVerticalOffset() const;
    
    // ----- CanContentScroll -----
    
    bool GetCanContentScroll() const;
    ScrollViewer* SetCanContentScroll(bool value);
    ScrollViewer* CanContentScroll(bool value) {
        return SetCanContentScroll(value);
    }
    ScrollViewer* CanContentScroll(binding::Binding binding) {
        SetBinding(CanContentScrollProperty(), std::move(binding));
        return this;
    }
    
    // ========== 只读属性（来自 ScrollContentPresenter）==========
    
    /// 内容总宽度
    double GetExtentWidth() const;
    
    /// 内容总高度
    double GetExtentHeight() const;
    
    /// 视口宽度
    double GetViewportWidth() const;
    
    /// 视口高度
    double GetViewportHeight() const;
    
    /// 可水平滚动的距离
    double GetScrollableWidth() const;
    
    /// 可垂直滚动的距离
    double GetScrollableHeight() const;
    
    // ========== 滚动操作 ==========
    
    /// 向上滚动一行
    void LineUp();
    
    /// 向下滚动一行
    void LineDown();
    
    /// 向左滚动一列
    void LineLeft();
    
    /// 向右滚动一列
    void LineRight();
    
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
    
    /// 滚动到最左边
    void ScrollToLeftEnd();
    
    /// 滚动到最右边
    void ScrollToRightEnd();
    
    /// 滚动到起始位置（左上角）
    void ScrollToHome();
    
    /// 滚动到结束位置（右下角）
    void ScrollToEnd();
    
    /// 滚动到指定水平偏移
    void ScrollToHorizontalOffset(double offset);
    
    /// 滚动到指定垂直偏移
    void ScrollToVerticalOffset(double offset);
    
    /// 使指定矩形区域可见
    Rect MakeVisible(UIElement* visual, const Rect& rectangle);
    
    // ========== 事件 ==========
    
    /// 滚动变化事件
    core::Event<const ScrollChangedEventArgs&> ScrollChanged;
    
    // ========== 模板部件访问 ==========
    
    /// 获取内部 ScrollContentPresenter
    ScrollContentPresenter_t* GetScrollContentPresenter() const { return scrollContentPresenter_; }
    
    /// 获取垂直滚动条
    ScrollBar* GetVerticalScrollBar() const { return verticalScrollBar_; }
    
    /// 获取水平滚动条
    ScrollBar* GetHorizontalScrollBar() const { return horizontalScrollBar_; }

protected:
    // ========== 模板应用 ==========
    
    void OnTemplateApplied() override;
    
    // ========== 内容变化 ==========
    
    void OnContentChanged(const std::any& oldContent, const std::any& newContent) override;
    
    // ========== 输入处理 ==========
    
    /// 处理鼠标滚轮事件（由外部调用）
    void HandleMouseWheel(float delta);
    
    /// 重写鼠标滚轮事件处理
    void OnMouseWheel(PointerEventArgs& e) override;
    
    // ========== 布局 ==========
    
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    // ========== 裁剪 ==========
    
    bool ShouldClipToBounds() const override { return true; }

private:
    // ========== 模板部件引用 ==========
    
    ScrollContentPresenter_t* scrollContentPresenter_{nullptr};
    ScrollBar* verticalScrollBar_{nullptr};
    ScrollBar* horizontalScrollBar_{nullptr};
    
    // ========== 缓存值 ==========
    
    double lastHorizontalOffset_{0};
    double lastVerticalOffset_{0};
    
    // ========== 内部方法 ==========
    
    /// 更新滚动条可见性
    void UpdateScrollBarVisibility();
    
    /// 同步 ScrollContentPresenter 的属性
    void SyncScrollContentPresenter();
    
    /// 同步滚动条的属性
    void SyncScrollBars();
    
    /**
     * @brief 重写 UpdateContentPresenter
     * 
     * ScrollViewer 不使用 ContentControl 默认的 ContentPresenter 查找逻辑，
     * 而是将内容直接传递给专门的 ScrollContentPresenter。
     */
    void UpdateContentPresenter() override;
    
    /// 处理 ScrollContentPresenter 的 ScrollInfoChanged 事件
    void OnScrollInfoChanged();
    
    /// 处理垂直滚动条的值变化
    void OnVerticalScrollBarValueChanged(float oldVal, float newVal);
    
    /// 处理水平滚动条的值变化
    void OnHorizontalScrollBarValueChanged(float oldVal, float newVal);
    
    /// 触发 ScrollChanged 事件
    void RaiseScrollChanged();
    
    /// 根据 ScrollBarVisibility 和内容大小判断滚动条是否应该显示
    bool ShouldShowHorizontalScrollBar() const;
    bool ShouldShowVerticalScrollBar() const;
    
    /// 判断是否可以水平/垂直滚动
    bool CanScrollHorizontally() const;
    bool CanScrollVertically() const;
    
    /// 防止循环更新的标志
    bool isUpdatingFromPresenter_{false};
    bool isUpdatingFromScrollBar_{false};
};

} // namespace fk::ui
