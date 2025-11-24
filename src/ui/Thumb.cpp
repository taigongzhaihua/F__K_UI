/**
 * @file Thumb.cpp
 * @brief Thumb 可拖动滑块实现 - Phase 3 Step 2
 */

#include "fk/ui/Thumb.h"

namespace fk::ui {

// ========== 构造函数 ==========

Thumb::Thumb() {
    isDragging_ = false;
    dragStartX_ = 0.0f;
    dragStartY_ = 0.0f;
    lastX_ = 0.0f;
    lastY_ = 0.0f;
}

// ========== 公共方法 ==========

void Thumb::CancelDrag() {
    if (isDragging_) {
        isDragging_ = false;
        // 不触发 DragCompleted，因为是取消
        // TODO: ReleasePointerCapture() when available
    }
}

// ========== 事件重写 ==========

void Thumb::OnPointerPressed(PointerEventArgs& e) {
    Control<Thumb>::OnPointerPressed(e);
    
    // TODO: CapturePointer() when available
    // 开始拖动
    StartDrag(e.position.x, e.position.y);
    e.handled = true;
}

void Thumb::OnPointerMoved(PointerEventArgs& e) {
    Control<Thumb>::OnPointerMoved(e);
    
    if (isDragging_) {
        // 更新拖动
        UpdateDrag(e.position.x, e.position.y);
        e.handled = true;
    }
}

void Thumb::OnPointerReleased(PointerEventArgs& e) {
    Control<Thumb>::OnPointerReleased(e);
    
    if (isDragging_) {
        // 结束拖动
        EndDrag();
        // TODO: ReleasePointerCapture() when available
        e.handled = true;
    }
}

void Thumb::OnPointerExited(PointerEventArgs& e) {
    Control<Thumb>::OnPointerExited(e);
    
    // 鼠标离开时结束拖动
    if (isDragging_) {
        EndDrag();
    }
}

// ========== 内部方法 ==========

void Thumb::StartDrag(float x, float y) {
    isDragging_ = true;
    dragStartX_ = x;
    dragStartY_ = y;
    lastX_ = x;
    lastY_ = y;
    
    // 触发 DragStarted 事件
    DragStarted();
}

void Thumb::UpdateDrag(float x, float y) {
    if (!isDragging_) {
        return;
    }
    
    // 计算相对于上次位置的偏移
    float deltaX = x - lastX_;
    float deltaY = y - lastY_;
    
    // 更新上次位置
    lastX_ = x;
    lastY_ = y;
    
    // 触发 DragDelta 事件
    DragDeltaEventArgs args;
    args.horizontalChange = deltaX;
    args.verticalChange = deltaY;
    DragDelta(args);
}

void Thumb::EndDrag() {
    if (!isDragging_) {
        return;
    }
    
    isDragging_ = false;
    
    // 触发 DragCompleted 事件
    DragCompleted();
}

} // namespace fk::ui
