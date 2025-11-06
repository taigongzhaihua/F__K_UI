#include "fk/ui/ScrollViewer.h"
#include "fk/binding/DependencyProperty.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace fk::ui::detail {

// ============================================================================
// ScrollViewerBase 实现
// ============================================================================

ScrollViewerBase::ScrollViewerBase() {
    // ScrollViewer 必须裁切越界内容
    SetClipToBounds(true);
    
    // 创建 ScrollBar 实例
    horizontalScrollBar_ = std::make_shared<ScrollBarView>();
    horizontalScrollBar_->SetOrientation(ui::Orientation::Horizontal);
    // 不设置宽度/高度,让 ScrollViewer 在 ArrangeOverride 中控制大小
    
    verticalScrollBar_ = std::make_shared<ScrollBarView>();
    verticalScrollBar_->SetOrientation(ui::Orientation::Vertical);
    // 不设置宽度/高度,让 ScrollViewer 在 ArrangeOverride 中控制大小
    
    // 订阅 ScrollBar 值变更事�?
    horizontalScrollBar_->ValueChanged += [this](double value) {
        SetHorizontalOffset(value);
    };
    
    verticalScrollBar_->ValueChanged += [this](double value) {
        SetVerticalOffset(value);
    };
}

ScrollViewerBase::~ScrollViewerBase() = default;

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, HorizontalOffset, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, VerticalOffset, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, HorizontalScrollBarVisibility, ScrollBarVisibility, ScrollBarVisibility::Auto)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, VerticalScrollBarVisibility, ScrollBarVisibility, ScrollBarVisibility::Auto)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, ViewportWidth, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, ViewportHeight, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, ExtentWidth, double, 0.0)
FK_DEPENDENCY_PROPERTY_DEFINE(ScrollViewerBase, ExtentHeight, double, 0.0)

// ============================================================================
// 滚动方法
// ============================================================================

void ScrollViewerBase::ScrollToTop() {
    SetVerticalOffset(0.0);
}

void ScrollViewerBase::ScrollToBottom() {
    double maxOffset = std::max(0.0, GetExtentHeight() - GetViewportHeight());
    SetVerticalOffset(maxOffset);
}

void ScrollViewerBase::ScrollToLeft() {
    SetHorizontalOffset(0.0);
}

void ScrollViewerBase::ScrollToRight() {
    double maxOffset = std::max(0.0, GetExtentWidth() - GetViewportWidth());
    SetHorizontalOffset(maxOffset);
}

void ScrollViewerBase::ScrollToVerticalOffset(double offset) {
    SetVerticalOffset(offset);
}

void ScrollViewerBase::ScrollToHorizontalOffset(double offset) {
    SetHorizontalOffset(offset);
}

void ScrollViewerBase::LineUp() {
    double newOffset = std::max(0.0, GetVerticalOffset() - lineSize_);
    SetVerticalOffset(newOffset);
}

void ScrollViewerBase::LineDown() {
    double maxOffset = std::max(0.0, GetExtentHeight() - GetViewportHeight());
    double newOffset = std::min(maxOffset, GetVerticalOffset() + lineSize_);
    SetVerticalOffset(newOffset);
}

void ScrollViewerBase::LineLeft() {
    double newOffset = std::max(0.0, GetHorizontalOffset() - lineSize_);
    SetHorizontalOffset(newOffset);
}

void ScrollViewerBase::LineRight() {
    double maxOffset = std::max(0.0, GetExtentWidth() - GetViewportWidth());
    double newOffset = std::min(maxOffset, GetHorizontalOffset() + lineSize_);
    SetHorizontalOffset(newOffset);
}

void ScrollViewerBase::PageUp() {
    double newOffset = std::max(0.0, GetVerticalOffset() - GetViewportHeight());
    SetVerticalOffset(newOffset);
}

void ScrollViewerBase::PageDown() {
    double maxOffset = std::max(0.0, GetExtentHeight() - GetViewportHeight());
    double newOffset = std::min(maxOffset, GetVerticalOffset() + GetViewportHeight());
    SetVerticalOffset(newOffset);
}

void ScrollViewerBase::PageLeft() {
    double newOffset = std::max(0.0, GetHorizontalOffset() - GetViewportWidth());
    SetHorizontalOffset(newOffset);
}

void ScrollViewerBase::PageRight() {
    double maxOffset = std::max(0.0, GetExtentWidth() - GetViewportWidth());
    double newOffset = std::min(maxOffset, GetHorizontalOffset() + GetViewportWidth());
    SetHorizontalOffset(newOffset);
}

// ============================================================================
// 布局
// ============================================================================

Size ScrollViewerBase::MeasureOverride(const Size& availableSize) {
    auto content = GetContent();
    if (!content) {
        return Size(0, 0);
    }

    // 测量内容时给予无限空�?以获得实际内容大�?
    Size infiniteSize(
        std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity()
    );
    
    content->Measure(infiniteSize);
    Size contentDesiredSize = content->DesiredSize();
    
    // 更新 Extent (内容实际大小)
    SetExtentWidth(contentDesiredSize.width);
    SetExtentHeight(contentDesiredSize.height);
    
    // 计算 Viewport (可见区域大小)
    float viewportWidth = availableSize.width;
    float viewportHeight = availableSize.height;
    
    // 检查是否需要显示滚动条
    bool needHScroll = contentDesiredSize.width > viewportWidth;
    bool needVScroll = contentDesiredSize.height > viewportHeight;
    
    auto hVisibility = GetHorizontalScrollBarVisibility();
    auto vVisibility = GetVerticalScrollBarVisibility();
    
    // 根据可见性策略决定是否显示滚动条
    bool showHScroll = (hVisibility == ScrollBarVisibility::Visible) ||
                       (hVisibility == ScrollBarVisibility::Auto && needHScroll);
    bool showVScroll = (vVisibility == ScrollBarVisibility::Visible) ||
                       (vVisibility == ScrollBarVisibility::Auto && needVScroll);
    
    // 获取滚动条厚度
    float vScrollThickness = verticalScrollBar_ ? verticalScrollBar_->GetThickness() : 6.0f;
    float hScrollThickness = horizontalScrollBar_ ? horizontalScrollBar_->GetThickness() : 6.0f;
    
    // 滚动条会占用空间
    if (showVScroll) {
        viewportWidth -= vScrollThickness;
    }
    if (showHScroll) {
        viewportHeight -= hScrollThickness;
    }
    
    // 更新 Viewport
    SetViewportWidth(std::max(0.0, static_cast<double>(viewportWidth)));
    SetViewportHeight(std::max(0.0, static_cast<double>(viewportHeight)));
    
    // 🔥 重要: 必须测量 ScrollBar,否则它的 DesiredSize 不会更新!
    if (showVScroll && verticalScrollBar_) {
        Size vScrollSize(vScrollThickness, viewportHeight + (showHScroll ? hScrollThickness : 0.0f));
        verticalScrollBar_->Measure(vScrollSize);
    }
    
    if (showHScroll && horizontalScrollBar_) {
        Size hScrollSize(viewportWidth + (showVScroll ? vScrollThickness : 0.0f), hScrollThickness);
        horizontalScrollBar_->Measure(hScrollSize);
    }
    
    // 更新滚动�?
    UpdateScrollBars();
    
    return Size(
        std::min(availableSize.width, contentDesiredSize.width),
        std::min(availableSize.height, contentDesiredSize.height)
    );
}

Size ScrollViewerBase::ArrangeOverride(const Size& finalSize) {
    auto content = GetContent();
    if (!content) {
        return finalSize;
    }

    double viewportWidth = GetViewportWidth();
    double viewportHeight = GetViewportHeight();
    double extentWidth = GetExtentWidth();
    double extentHeight = GetExtentHeight();
    
    // 内容区域 (考虑滚动偏移)
    double hOffset = -GetHorizontalOffset();
    double vOffset = -GetVerticalOffset();
    
    // 排列内容 (使用实际内容大小,不是 viewport 大小)
    Rect contentRect(
        static_cast<float>(hOffset),
        static_cast<float>(vOffset),
        static_cast<float>(extentWidth),
        static_cast<float>(extentHeight)
    );
    content->Arrange(contentRect);
    
    // 排列滚动�?
    auto hVisibility = GetHorizontalScrollBarVisibility();
    auto vVisibility = GetVerticalScrollBarVisibility();
    
    bool needHScroll = extentWidth > viewportWidth;
    bool needVScroll = extentHeight > viewportHeight;
    
    bool showHScroll = (hVisibility == ScrollBarVisibility::Visible) ||
                       (hVisibility == ScrollBarVisibility::Auto && needHScroll);
    bool showVScroll = (vVisibility == ScrollBarVisibility::Visible) ||
                       (vVisibility == ScrollBarVisibility::Auto && needVScroll);
    
    // 获取滚动条厚度
    float vScrollThickness = verticalScrollBar_ ? verticalScrollBar_->GetThickness() : 8.0f;
    float hScrollThickness = horizontalScrollBar_ ? horizontalScrollBar_->GetThickness() : 8.0f;
    
    if (showHScroll && horizontalScrollBar_) {
        float scrollBarY = finalSize.height - hScrollThickness;
        float scrollBarWidth = finalSize.width - (showVScroll ? vScrollThickness : 0.0f);
        Rect hScrollRect(0, scrollBarY, scrollBarWidth, hScrollThickness);
        horizontalScrollBar_->Arrange(hScrollRect);
    }
    
    if (showVScroll && verticalScrollBar_) {
        float scrollBarX = finalSize.width - vScrollThickness;
        float scrollBarHeight = finalSize.height - (showHScroll ? hScrollThickness : 0.0f);
        Rect vScrollRect(scrollBarX, 0, vScrollThickness, scrollBarHeight);
        verticalScrollBar_->Arrange(vScrollRect);
    }
    
    return finalSize;
}

// ============================================================================
// 可视子元素
// ============================================================================

std::vector<Visual*> ScrollViewerBase::GetVisualChildren() const {
    std::vector<Visual*> children;
    
    // 添加内容
    auto content = GetContent();
    if (content) {
        children.push_back(content.get());
    }
    
    // 添加滚动条
    if (horizontalScrollBar_) {
        children.push_back(horizontalScrollBar_.get());
    }
    if (verticalScrollBar_) {
        children.push_back(verticalScrollBar_.get());
    }
    
    return children;
}

UIElement* ScrollViewerBase::HitTestChildren(double x, double y) {
    // 优先检查滚动条 (它们在最上层)
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        if (verticalScrollBar_->HitTest(x, y)) {
            return verticalScrollBar_.get();
        }
    }
    
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        if (horizontalScrollBar_->HitTest(x, y)) {
            return horizontalScrollBar_.get();
        }
    }
    
    // 然后检查内容
    auto content = GetContent();
    if (content && content->GetVisibility() == Visibility::Visible) {
        if (content->HitTest(x, y)) {
            UIElement* hitInContent = content->HitTestChildren(x, y);
            return hitInContent ? hitInContent : content.get();
        }
    }
    
    return nullptr;
}

// ============================================================================
// 鼠标事件处理
// ============================================================================

bool ScrollViewerBase::OnMouseButtonDown(int button, double x, double y) {
    // 优先检查滚动条（从上往下）
    if (verticalScrollBar_ && verticalScrollBar_->GetVisibility() == Visibility::Visible) {
        if (verticalScrollBar_->HitTest(x, y)) {
            auto bounds = verticalScrollBar_->GetRenderBounds();
            double localX = x - bounds.x;
            double localY = y - bounds.y;
            if (static_cast<UIElement*>(verticalScrollBar_.get())->OnMouseButtonDown(button, localX, localY)) {
                return true;
            }
        }
    }
    
    if (horizontalScrollBar_ && horizontalScrollBar_->GetVisibility() == Visibility::Visible) {
        if (horizontalScrollBar_->HitTest(x, y)) {
            auto bounds = horizontalScrollBar_->GetRenderBounds();
            double localX = x - bounds.x;
            double localY = y - bounds.y;
            if (static_cast<UIElement*>(horizontalScrollBar_.get())->OnMouseButtonDown(button, localX, localY)) {
                return true;
            }
        }
    }
    
    // 然后检查内容
    auto content = GetContent();
    if (content && content->HitTest(x, y)) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseButtonDown(button, localX, localY);
    }
    
    return false;
}

bool ScrollViewerBase::OnMouseButtonUp(int button, double x, double y) {
    // MouseUp 需要传递给所有可能正在交互的元素（特别是正在拖动的 ScrollBar）
    // 注意：即使鼠标不在 ScrollBar 上，也要通知它（因为可能正在拖动）
    bool handled = false;
    
    if (verticalScrollBar_) {
        auto bounds = verticalScrollBar_->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        if (static_cast<UIElement*>(verticalScrollBar_.get())->OnMouseButtonUp(button, localX, localY)) {
            handled = true;
        }
    }
    
    if (horizontalScrollBar_) {
        auto bounds = horizontalScrollBar_->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        if (static_cast<UIElement*>(horizontalScrollBar_.get())->OnMouseButtonUp(button, localX, localY)) {
            handled = true;
        }
    }
    
    if (handled) {
        return true;
    }
    
    // 传递给内容
    auto content = GetContent();
    if (content) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseButtonUp(button, localX, localY);
    }
    
    return false;
}

bool ScrollViewerBase::OnMouseMove(double x, double y) {
    // MouseMove 需要传递给所有可能正在拖动的元素
    bool handled = false;
    
    if (verticalScrollBar_) {
        auto bounds = verticalScrollBar_->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        if (static_cast<UIElement*>(verticalScrollBar_.get())->OnMouseMove(localX, localY)) {
            handled = true;
        }
    }
    
    if (horizontalScrollBar_) {
        auto bounds = horizontalScrollBar_->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        if (static_cast<UIElement*>(horizontalScrollBar_.get())->OnMouseMove(localX, localY)) {
            handled = true;
        }
    }
    
    if (handled) {
        return true;
    }
    
    // 传递给内容
    auto content = GetContent();
    if (content) {
        auto bounds = content->GetRenderBounds();
        double localX = x - bounds.x;
        double localY = y - bounds.y;
        return content->OnMouseMove(localX, localY);
    }
    
    return false;
}

// ============================================================================
// 鼠标滚轮事件处理
// ============================================================================

bool ScrollViewerBase::OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY) {
    // 先尝试传递给内容（例如嵌套的 ScrollViewer）
    auto content = GetContent();
    if (content && content->HitTest(mouseX, mouseY)) {
        if (content->OnMouseWheel(xoffset, yoffset, mouseX, mouseY)) {
            return true; // 内容处理了
        }
    }
    
    // 内容没有处理，由 ScrollViewer 自己处理
    bool handled = false;
    
    // 垂直滚动
    // yoffset > 0: 鼠标向上滚 → 内容向下移(看到上面的内容) → offset 减小
    // yoffset < 0: 鼠标向下滚 → 内容向上移(看到下面的内容) → offset 增大
    if (yoffset != 0) {
        double maxOffset = std::max(0.0, GetExtentHeight() - GetViewportHeight());
        if (maxOffset > 0) {
            if (yoffset > 0) {
                LineUp();    // 减小 offset,内容向下移
            } else {
                LineDown();  // 增大 offset,内容向上移
            }
            handled = true;
        }
    }
    
    // 水平滚动
    if (xoffset != 0) {
        double maxOffset = std::max(0.0, GetExtentWidth() - GetViewportWidth());
        if (maxOffset > 0) {
            if (xoffset > 0) {
                LineRight();
            } else {
                LineLeft();
            }
            handled = true;
        }
    }
    
    return handled;
}

// ============================================================================
// ScrollBar 更新
// ============================================================================

void ScrollViewerBase::UpdateScrollBars() {
    UpdateHorizontalScrollBar();
    UpdateVerticalScrollBar();
}

void ScrollViewerBase::UpdateHorizontalScrollBar() {
    if (!horizontalScrollBar_) return;
    
    double viewportWidth = GetViewportWidth();
    double extentWidth = GetExtentWidth();
    double offset = GetHorizontalOffset();
    
    double maximum = std::max(0.0, extentWidth - viewportWidth);
    
    horizontalScrollBar_->SetMinimum(0.0);
    horizontalScrollBar_->SetMaximum(maximum);
    horizontalScrollBar_->SetViewportSize(viewportWidth);
    
    // 只有值真的改变时才更新,避免循环触发 ValueChanged 事件
    if (std::abs(horizontalScrollBar_->GetValue() - offset) > 0.001) {
        horizontalScrollBar_->SetValue(offset);
    }
}

void ScrollViewerBase::UpdateVerticalScrollBar() {
    if (!verticalScrollBar_) return;
    
    double viewportHeight = GetViewportHeight();
    double extentHeight = GetExtentHeight();
    double offset = GetVerticalOffset();
    
    double maximum = std::max(0.0, extentHeight - viewportHeight);
    
    verticalScrollBar_->SetMinimum(0.0);
    verticalScrollBar_->SetMaximum(maximum);
    verticalScrollBar_->SetViewportSize(viewportHeight);
    
    // 只有值真的改变时才更新,避免循环触发 ValueChanged 事件
    if (std::abs(verticalScrollBar_->GetValue() - offset) > 0.001) {
        verticalScrollBar_->SetValue(offset);
    }
}

// ============================================================================
// 属性变更回�?
// ============================================================================

void ScrollViewerBase::OnHorizontalOffsetChanged(double oldValue, double newValue) {
    // 限制偏移在有效范围内
    double maxOffset = std::max(0.0, GetExtentWidth() - GetViewportWidth());
    double coercedValue = std::clamp(newValue, 0.0, maxOffset);
    
    if (coercedValue != newValue) {
        SetHorizontalOffset(coercedValue);
        return;
    }
    
    // 触发事件
    ScrollChanged(newValue, GetVerticalOffset());
    
    // 🔥 同步滚动条的值
    UpdateHorizontalScrollBar();
    
    // 需要重新排列内容(改变偏移)并重绘
    InvalidateArrange();
    InvalidateVisual();
}

void ScrollViewerBase::OnVerticalOffsetChanged(double oldValue, double newValue) {
    // 限制偏移在有效范围内
    double maxOffset = std::max(0.0, GetExtentHeight() - GetViewportHeight());
    double coercedValue = std::clamp(newValue, 0.0, maxOffset);
    
    if (coercedValue != newValue) {
        SetVerticalOffset(coercedValue);
        return;
    }
    
    // 触发事件
    ScrollChanged(GetHorizontalOffset(), newValue);
    
    // 🔥 同步滚动条的值
    UpdateVerticalScrollBar();
    
    // 需要重新排列内容(改变偏移)并重绘
    InvalidateArrange();
    InvalidateVisual();
}

void ScrollViewerBase::OnHorizontalScrollBarVisibilityChanged(ScrollBarVisibility oldValue, ScrollBarVisibility newValue) {
    InvalidateMeasure();
}

void ScrollViewerBase::OnVerticalScrollBarVisibilityChanged(ScrollBarVisibility oldValue, ScrollBarVisibility newValue) {
    InvalidateMeasure();
}

// ============================================================================
// 元数据构�?
// ============================================================================

binding::PropertyMetadata ScrollViewerBase::BuildHorizontalOffsetMetadata() {
    binding::PropertyMetadata metadata(0.0);
    metadata.propertyChangedCallback = HorizontalOffsetPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildVerticalOffsetMetadata() {
    binding::PropertyMetadata metadata(0.0);
    metadata.propertyChangedCallback = VerticalOffsetPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildHorizontalScrollBarVisibilityMetadata() {
    binding::PropertyMetadata metadata(ScrollBarVisibility::Auto);
    metadata.propertyChangedCallback = HorizontalScrollBarVisibilityPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildVerticalScrollBarVisibilityMetadata() {
    binding::PropertyMetadata metadata(ScrollBarVisibility::Auto);
    metadata.propertyChangedCallback = VerticalScrollBarVisibilityPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildViewportWidthMetadata() {
    binding::PropertyMetadata metadata(0.0);
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildViewportHeightMetadata() {
    binding::PropertyMetadata metadata(0.0);
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildExtentWidthMetadata() {
    binding::PropertyMetadata metadata(0.0);
    return metadata;
}

binding::PropertyMetadata ScrollViewerBase::BuildExtentHeightMetadata() {
    binding::PropertyMetadata metadata(0.0);
    return metadata;
}

} // namespace fk::ui::detail
