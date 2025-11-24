/**
 * @file ScrollContentPresenter.cpp
 * @brief ScrollContentPresenter 滚动内容呈现器实现 - Phase 2
 */

#include "fk/ui/ScrollContentPresenter.h"
#include "fk/ui/ScrollViewer.h"
#include <algorithm>
#include <limits>

namespace fk::ui {

// ========== 构造函数 ==========

ScrollContentPresenter::ScrollContentPresenter() {
    // 默认启用双向滚动
    canHorizontallyScroll_ = true;
    canVerticallyScroll_ = true;
}

// ========== 滚动偏移控制 ==========

void ScrollContentPresenter::SetHorizontalOffset(float offset) {
    // 限制偏移在有效范围内
    float maxOffset = std::max(0.0f, extentWidth_ - viewportWidth_);
    float newOffset = std::max(0.0f, std::min(maxOffset, offset));
    
    if (horizontalOffset_ != newOffset) {
        horizontalOffset_ = newOffset;
        InvalidateArrange();  // 需要重新排列内容
        NotifyScrollChanged();
    }
}

void ScrollContentPresenter::SetVerticalOffset(float offset) {
    // 限制偏移在有效范围内
    float maxOffset = std::max(0.0f, extentHeight_ - viewportHeight_);
    float newOffset = std::max(0.0f, std::min(maxOffset, offset));
    
    if (verticalOffset_ != newOffset) {
        verticalOffset_ = newOffset;
        InvalidateArrange();  // 需要重新排列内容
        NotifyScrollChanged();
    }
}

// ========== 布局重写 ==========

Size ScrollContentPresenter::MeasureOverride(const Size& availableSize) {
    // 检查内容是否实现 IScrollInfo
    UpdateScrollInfo();
    
    // 获取内容元素
    UIElement* content = GetVisualChild();
    
    if (!content) {
        // 没有内容，尺寸为 0
        extentWidth_ = 0.0f;
        extentHeight_ = 0.0f;
        viewportWidth_ = availableSize.width;
        viewportHeight_ = availableSize.height;
        return Size(0, 0);
    }
    
    // 如果内容实现了 IScrollInfo 且启用逻辑滚动，使用逻辑滚动模式
    if (scrollInfo_) {
        // 配置 IScrollInfo
        scrollInfo_->SetCanHorizontallyScroll(canHorizontallyScroll_);
        scrollInfo_->SetCanVerticallyScroll(canVerticallyScroll_);
        
        // 测量内容
        content->Measure(availableSize);
        
        // 从 IScrollInfo 获取尺寸信息
        extentWidth_ = scrollInfo_->GetExtentWidth();
        extentHeight_ = scrollInfo_->GetExtentHeight();
        viewportWidth_ = scrollInfo_->GetViewportWidth();
        viewportHeight_ = scrollInfo_->GetViewportHeight();
        
        // 同步偏移
        horizontalOffset_ = scrollInfo_->GetHorizontalOffset();
        verticalOffset_ = scrollInfo_->GetVerticalOffset();
    } else {
        // 物理滚动模式：给内容无限大小以获取其实际尺寸
        Size measureSize(
            canHorizontallyScroll_ ? std::numeric_limits<float>::infinity() : availableSize.width,
            canVerticallyScroll_ ? std::numeric_limits<float>::infinity() : availableSize.height
        );
        
        content->Measure(measureSize);
        
        // 记录内容的期望大小
        Size desiredSize = content->GetDesiredSize();
        extentWidth_ = desiredSize.width;
        extentHeight_ = desiredSize.height;
        
        // 视口大小就是可用大小
        viewportWidth_ = availableSize.width;
        viewportHeight_ = availableSize.height;
    }
    
    // 强制偏移在有效范围内
    CoerceOffsets();
    
    // 通知滚动信息变更
    NotifyScrollChanged();
    
    // 返回视口大小作为期望大小
    return Size(
        std::min(availableSize.width, extentWidth_),
        std::min(availableSize.height, extentHeight_)
    );
}

Size ScrollContentPresenter::ArrangeOverride(const Size& finalSize) {
    UIElement* content = GetVisualChild();
    
    if (!content) {
        return finalSize;
    }
    
    // 更新视口大小
    viewportWidth_ = finalSize.width;
    viewportHeight_ = finalSize.height;
    
    if (scrollInfo_) {
        // 逻辑滚动：直接排列在视口大小
        content->Arrange(Rect(0, 0, finalSize.width, finalSize.height));
        
        // 同步偏移（由 IScrollInfo 管理）
        horizontalOffset_ = scrollInfo_->GetHorizontalOffset();
        verticalOffset_ = scrollInfo_->GetVerticalOffset();
    } else {
        // 物理滚动：排列在完整大小，应用负偏移
        // 内容位置 = -offset，这样内容的开头部分会被移出视口
        content->Arrange(Rect(
            -horizontalOffset_,
            -verticalOffset_,
            std::max(extentWidth_, viewportWidth_),
            std::max(extentHeight_, viewportHeight_)
        ));
    }
    
    // 强制偏移在有效范围内（可能因视口大小变化）
    CoerceOffsets();
    
    return finalSize;
}

// ========== 辅助方法 ==========

void ScrollContentPresenter::CoerceOffsets() {
    bool changed = false;
    
    // 限制水平偏移
    float maxHOffset = std::max(0.0f, extentWidth_ - viewportWidth_);
    float newHOffset = std::max(0.0f, std::min(maxHOffset, horizontalOffset_));
    if (horizontalOffset_ != newHOffset) {
        horizontalOffset_ = newHOffset;
        changed = true;
    }
    
    // 限制垂直偏移
    float maxVOffset = std::max(0.0f, extentHeight_ - viewportHeight_);
    float newVOffset = std::max(0.0f, std::min(maxVOffset, verticalOffset_));
    if (verticalOffset_ != newVOffset) {
        verticalOffset_ = newVOffset;
        changed = true;
    }
    
    // 如果偏移被强制修改，需要通知
    if (changed && scrollInfo_) {
        scrollInfo_->SetHorizontalOffset(horizontalOffset_);
        scrollInfo_->SetVerticalOffset(verticalOffset_);
    }
}

void ScrollContentPresenter::NotifyScrollChanged() {
    // 触发本地事件
    ScrollChanged();
    
    // 通知关联的 ScrollViewer
    if (scrollOwner_) {
        // ScrollViewer 会更新其属性和滚动条
        // 这个调用将在下一步实现
        // scrollOwner_->OnScrollContentPresenterChanged();
    }
}

void ScrollContentPresenter::UpdateScrollInfo() {
    UIElement* content = GetVisualChild();
    
    // 尝试将内容转换为 IScrollInfo
    // 注意：这需要内容实现 IScrollInfo 接口
    // 典型的实现者包括 VirtualizingPanel 等
    scrollInfo_ = dynamic_cast<IScrollInfo*>(content);
}

} // namespace fk::ui
