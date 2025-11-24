/**
 * @file Track.cpp
 * @brief Track 滚动条轨道实现 - Phase 3 Step 3
 */

#include "fk/ui/Track.h"
#include "fk/ui/RepeatButton.h"
#include "fk/ui/Thumb.h"
#include <algorithm>

namespace fk::ui {

// 最小 Thumb 大小（像素）
constexpr float MIN_THUMB_SIZE = 10.0f;

// ========== 构造函数 ==========

Track::Track() {
    orientation_ = Orientation::Vertical;
    minimum_ = 0.0f;
    maximum_ = 100.0f;
    value_ = 0.0f;
    viewportSize_ = 10.0f;
}

Track::~Track() {
    // 子元素由父类管理，这里不需要删除
}

// ========== 子元素管理 ==========

void Track::SetDecreaseRepeatButton(RepeatButton* button) {
    if (decreaseButton_ != button) {
        // TODO: 从可视树中移除旧按钮
        decreaseButton_ = button;
        // TODO: 添加到可视树
        InvalidateMeasure();
    }
}

void Track::SetThumb(Thumb* thumb) {
    if (thumb_ != thumb) {
        // TODO: 从可视树中移除旧 thumb
        thumb_ = thumb;
        // TODO: 添加到可视树
        InvalidateMeasure();
    }
}

void Track::SetIncreaseRepeatButton(RepeatButton* button) {
    if (increaseButton_ != button) {
        // TODO: 从可视树中移除旧按钮
        increaseButton_ = button;
        // TODO: 添加到可视树
        InvalidateMeasure();
    }
}

// ========== 布局重写 ==========

Size Track::MeasureOverride(const Size& availableSize) {
    Size desiredSize(0, 0);
    
    // 测量所有子元素
    if (decreaseButton_) {
        decreaseButton_->Measure(availableSize);
        Size buttonSize = decreaseButton_->GetDesiredSize();
        if (orientation_ == Orientation::Vertical) {
            desiredSize.width = std::max(desiredSize.width, buttonSize.width);
            desiredSize.height += buttonSize.height;
        } else {
            desiredSize.width += buttonSize.width;
            desiredSize.height = std::max(desiredSize.height, buttonSize.height);
        }
    }
    
    if (thumb_) {
        thumb_->Measure(availableSize);
        Size thumbSize = thumb_->GetDesiredSize();
        if (orientation_ == Orientation::Vertical) {
            desiredSize.width = std::max(desiredSize.width, thumbSize.width);
            desiredSize.height += thumbSize.height;
        } else {
            desiredSize.width += thumbSize.width;
            desiredSize.height = std::max(desiredSize.height, thumbSize.height);
        }
    }
    
    if (increaseButton_) {
        increaseButton_->Measure(availableSize);
        Size buttonSize = increaseButton_->GetDesiredSize();
        if (orientation_ == Orientation::Vertical) {
            desiredSize.width = std::max(desiredSize.width, buttonSize.width);
            desiredSize.height += buttonSize.height;
        } else {
            desiredSize.width += buttonSize.width;
            desiredSize.height = std::max(desiredSize.height, buttonSize.height);
        }
    }
    
    return desiredSize;
}

Size Track::ArrangeOverride(const Size& finalSize) {
    float trackLength = GetTrackLength(finalSize);
    float thumbSize = CalculateThumbSize(trackLength);
    float thumbPosition = CalculateThumbPosition(trackLength, thumbSize);
    
    if (orientation_ == Orientation::Vertical) {
        float currentY = 0.0f;
        
        // 排列向上按钮
        if (decreaseButton_) {
            Size buttonSize = decreaseButton_->GetDesiredSize();
            decreaseButton_->Arrange(Rect(0, currentY, finalSize.width, buttonSize.height));
            currentY += buttonSize.height;
        }
        
        // 排列 Thumb
        if (thumb_) {
            thumb_->Arrange(Rect(0, currentY + thumbPosition, finalSize.width, thumbSize));
        }
        
        // 排列向下按钮
        if (increaseButton_) {
            Size buttonSize = increaseButton_->GetDesiredSize();
            float buttonY = finalSize.height - buttonSize.height;
            increaseButton_->Arrange(Rect(0, buttonY, finalSize.width, buttonSize.height));
        }
    } else {
        // 水平方向
        float currentX = 0.0f;
        
        // 排列向左按钮
        if (decreaseButton_) {
            Size buttonSize = decreaseButton_->GetDesiredSize();
            decreaseButton_->Arrange(Rect(currentX, 0, buttonSize.width, finalSize.height));
            currentX += buttonSize.width;
        }
        
        // 排列 Thumb
        if (thumb_) {
            thumb_->Arrange(Rect(currentX + thumbPosition, 0, thumbSize, finalSize.height));
        }
        
        // 排列向右按钮
        if (increaseButton_) {
            Size buttonSize = increaseButton_->GetDesiredSize();
            float buttonX = finalSize.width - buttonSize.width;
            increaseButton_->Arrange(Rect(buttonX, 0, buttonSize.width, finalSize.height));
        }
    }
    
    return finalSize;
}

int Track::GetVisualChildrenCount() const {
    int count = 0;
    if (decreaseButton_) count++;
    if (thumb_) count++;
    if (increaseButton_) count++;
    return count;
}

UIElement* Track::GetVisualChild(int index) const {
    int currentIndex = 0;
    
    if (decreaseButton_) {
        if (index == currentIndex) return decreaseButton_;
        currentIndex++;
    }
    
    if (thumb_) {
        if (index == currentIndex) return thumb_;
        currentIndex++;
    }
    
    if (increaseButton_) {
        if (index == currentIndex) return increaseButton_;
        currentIndex++;
    }
    
    return nullptr;
}

// ========== 计算方法 ==========

float Track::ValueFromDistance(float horizontal, float vertical) {
    // 使用渲染大小
    Size renderSize = GetRenderSize();
    float trackLength = GetTrackLength(renderSize);
    float thumbSize = CalculateThumbSize(trackLength);
    
    if (trackLength <= thumbSize) {
        return value_;  // 没有空间移动
    }
    
    float delta = (orientation_ == Orientation::Vertical) ? vertical : horizontal;
    float range = maximum_ - minimum_;
    float availableLength = trackLength - thumbSize;
    
    // delta / availableLength = valueChange / range
    float valueChange = (delta / availableLength) * range;
    
    return std::max(minimum_, std::min(maximum_, value_ + valueChange));
}

float Track::ValueFromPoint(float x, float y) {
    // 使用渲染大小
    Size renderSize = GetRenderSize();
    float trackLength = GetTrackLength(renderSize);
    float thumbSize = CalculateThumbSize(trackLength);
    
    if (trackLength <= thumbSize) {
        return value_;
    }
    
    float clickPosition = (orientation_ == Orientation::Vertical) ? y : x;
    
    // 减去按钮占用的空间
    if (decreaseButton_) {
        Size buttonSize = decreaseButton_->GetDesiredSize();
        float buttonLength = (orientation_ == Orientation::Vertical) 
            ? buttonSize.height 
            : buttonSize.width;
        clickPosition -= buttonLength;
    }
    
    if (clickPosition < 0) {
        return minimum_;
    }
    
    float range = maximum_ - minimum_;
    float availableLength = trackLength - thumbSize;
    
    if (availableLength <= 0) {
        return minimum_;
    }
    
    // clickPosition / availableLength = (value - minimum) / range
    float ratio = clickPosition / availableLength;
    return minimum_ + ratio * range;
}

// ========== 辅助方法 ==========

float Track::CalculateThumbSize(float trackLength) const {
    if (trackLength <= 0) {
        return MIN_THUMB_SIZE;
    }
    
    float range = maximum_ - minimum_ + viewportSize_;
    if (range <= 0) {
        return trackLength;
    }
    
    float ratio = viewportSize_ / range;
    float thumbSize = trackLength * ratio;
    
    return std::max(MIN_THUMB_SIZE, thumbSize);
}

float Track::CalculateThumbPosition(float trackLength, float thumbSize) const {
    float range = maximum_ - minimum_;
    if (range <= 0) {
        return 0.0f;
    }
    
    float availableLength = trackLength - thumbSize;
    if (availableLength <= 0) {
        return 0.0f;
    }
    
    float ratio = (value_ - minimum_) / range;
    return ratio * availableLength;
}

float Track::GetTrackLength(const Size& arrangeSize) const {
    float totalLength = (orientation_ == Orientation::Vertical) 
        ? arrangeSize.height 
        : arrangeSize.width;
    
    // 减去按钮占用的空间
    if (decreaseButton_) {
        float buttonSize = (orientation_ == Orientation::Vertical)
            ? decreaseButton_->GetDesiredSize().height
            : decreaseButton_->GetDesiredSize().width;
        totalLength -= buttonSize;
    }
    
    if (increaseButton_) {
        float buttonSize = (orientation_ == Orientation::Vertical)
            ? increaseButton_->GetDesiredSize().height
            : increaseButton_->GetDesiredSize().width;
        totalLength -= buttonSize;
    }
    
    return std::max(0.0f, totalLength);
}

} // namespace fk::ui
