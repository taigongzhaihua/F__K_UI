/**
 * @file ScrollViewer.h
 * @brief ScrollViewer 滚动查看器控件 - 新设计（WPF 风格）
 * 
 * 设计理念：
 * 1. 采用 WPF ScrollViewer 的架构设计
 * 2. 使用 ScrollContentPresenter 作为内容呈现器
 * 3. 支持模板化的滚动条（可通过样式/模板自定义）
 * 4. 清晰的职责分离：
 *    - ScrollViewer: 顶层容器，管理滚动策略和状态
 *    - ScrollContentPresenter: 负责内容的裁剪和偏移
 *    - ScrollBar: 独立的滚动条控件（可模板化）
 * 
 * WPF 对应：ScrollViewer
 * 
 * TODO 实现计划：
 * - Phase 1: 基础架构和接口定义 ✓
 * - Phase 2: ScrollContentPresenter 实现
 * - Phase 3: 滚动条集成和事件处理
 * - Phase 4: 模板和样式支持
 * - Phase 5: 鼠标滚轮和触摸支持
 */

#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/ui/Enums.h"

namespace fk::ui {

// 前向声明
class ScrollBar;  // Phase 3
class ScrollContentPresenter;  // Phase 2 ✓

/**
 * @brief 滚动条可见性模式（与 WPF 一致）
 */
enum class ScrollBarVisibility {
    Disabled,   ///< 禁用滚动，内容不可滚动
    Auto,       ///< 需要时自动显示滚动条
    Hidden,     ///< 隐藏滚动条但内容可滚动（programmatic scrolling）
    Visible     ///< 始终显示滚动条
};

/**
 * @brief 滚动查看器控件（新设计 - WPF 风格）
 * 
 * 架构特点：
 * 1. 使用 ScrollContentPresenter 管理内容的实际滚动
 * 2. ScrollViewer 本身只是容器和策略管理器
 * 3. 支持通过 ControlTemplate 完全自定义外观
 * 4. 滚动条作为模板的一部分，可以替换或重新定位
 * 
 * 使用示例：
 * @code
 * auto scrollViewer = new ScrollViewer();
 * scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
 * scrollViewer->SetContent(myLargeContent);
 * @endcode
 */
class ScrollViewer : public ContentControl<ScrollViewer> {
public:
    ScrollViewer();
    virtual ~ScrollViewer() = default;
    
    // ========== 依赖属性（TODO: 完整实现）==========
    
    /// HorizontalScrollBarVisibility 属性
    static const binding::DependencyProperty& HorizontalScrollBarVisibilityProperty();
    
    /// VerticalScrollBarVisibility 属性
    static const binding::DependencyProperty& VerticalScrollBarVisibilityProperty();
    
    /// HorizontalOffset 属性（只读）
    static const binding::DependencyProperty& HorizontalOffsetProperty();
    
    /// VerticalOffset 属性（只读）
    static const binding::DependencyProperty& VerticalOffsetProperty();
    
    /// ViewportWidth 属性（只读）
    static const binding::DependencyProperty& ViewportWidthProperty();
    
    /// ViewportHeight 属性（只读）
    static const binding::DependencyProperty& ViewportHeightProperty();
    
    /// ExtentWidth 属性（内容总宽度，只读）
    static const binding::DependencyProperty& ExtentWidthProperty();
    
    /// ExtentHeight 属性（内容总高度，只读）
    static const binding::DependencyProperty& ExtentHeightProperty();
    
    /// ScrollableWidth 属性（可滚动宽度，只读）
    static const binding::DependencyProperty& ScrollableWidthProperty();
    
    /// ScrollableHeight 属性（可滚动高度，只读）
    static const binding::DependencyProperty& ScrollableHeightProperty();
    
    /// CanContentScroll 属性（是否支持逻辑滚动）
    static const binding::DependencyProperty& CanContentScrollProperty();
    
    /// IsDeferredScrollingEnabled 属性（是否启用延迟滚动）
    static const binding::DependencyProperty& IsDeferredScrollingEnabledProperty();
    
    // ========== 附加属性（WPF 风格）==========
    
    /// 获取元素的 HorizontalScrollBarVisibility 附加属性
    static ScrollBarVisibility GetHorizontalScrollBarVisibility(const UIElement* element);
    
    /// 设置元素的 HorizontalScrollBarVisibility 附加属性
    static void SetHorizontalScrollBarVisibility(UIElement* element, ScrollBarVisibility value);
    
    /// 获取元素的 VerticalScrollBarVisibility 附加属性
    static ScrollBarVisibility GetVerticalScrollBarVisibility(const UIElement* element);
    
    /// 设置元素的 VerticalScrollBarVisibility 附加属性
    static void SetVerticalScrollBarVisibility(UIElement* element, ScrollBarVisibility value);
    
    // ========== 属性访问器 ==========
    
    ScrollBarVisibility GetHorizontalScrollBarVisibility() const;
    ScrollViewer* SetHorizontalScrollBarVisibility(ScrollBarVisibility value);
    
    ScrollBarVisibility GetVerticalScrollBarVisibility() const;
    ScrollViewer* SetVerticalScrollBarVisibility(ScrollBarVisibility value);
    
    float GetHorizontalOffset() const;
    float GetVerticalOffset() const;
    
    float GetViewportWidth() const;
    float GetViewportHeight() const;
    
    float GetExtentWidth() const;
    float GetExtentHeight() const;
    
    float GetScrollableWidth() const;
    float GetScrollableHeight() const;
    
    bool GetCanContentScroll() const;
    ScrollViewer* SetCanContentScroll(bool value);
    
    // ========== 滚动方法（WPF 兼容）==========
    
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
    
    /// 滚动到指定水平位置
    void ScrollToHorizontalOffset(float offset);
    
    /// 滚动到指定垂直位置
    void ScrollToVerticalOffset(float offset);
    
    /// 滚动使指定元素可见
    void ScrollToElement(UIElement* element);
    
    // ========== ScrollContentPresenter 访问 ==========
    
    /// 获取 ScrollContentPresenter（Phase 2 新增）
    ScrollContentPresenter* GetScrollContentPresenter() const { return scrollContentPresenter_; }
    
    // ========== 事件 ==========
    
    // TODO: 添加 ScrollChanged 路由事件
    
protected:
    // ========== 布局重写 ==========
    
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    
    // ========== 内部回调 ==========
    
    /// 当 ScrollContentPresenter 的滚动信息变更时调用
    void OnScrollContentPresenterChanged();
    
    // ========== 模板部分（TODO）==========
    
    // 这些将在完整实现时处理模板化滚动条
    // virtual void OnApplyTemplate() override;
    
private:
    // ========== 内部状态 ==========
    
    ScrollBarVisibility horizontalScrollBarVisibility_{ScrollBarVisibility::Auto};
    ScrollBarVisibility verticalScrollBarVisibility_{ScrollBarVisibility::Auto};
    
    // 这些值由 ScrollContentPresenter 计算和更新
    float horizontalOffset_{0.0f};
    float verticalOffset_{0.0f};
    float viewportWidth_{0.0f};
    float viewportHeight_{0.0f};
    float extentWidth_{0.0f};
    float extentHeight_{0.0f};
    
    bool canContentScroll_{false};
    
    // ScrollContentPresenter 实例（Phase 2 新增）
    ScrollContentPresenter* scrollContentPresenter_{nullptr};
    
    // ========== 辅助方法 ==========
    
    /// 更新滚动条的范围和可见性
    void UpdateScrollBars();
};

} // namespace fk::ui
