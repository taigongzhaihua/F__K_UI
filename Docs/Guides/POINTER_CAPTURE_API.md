# 指针捕获API使用指南

本指南介绍如何使用FK_UI框架中新增的指针捕获API。

## 概述

指针捕获允许UI元素在指针（鼠标）移出其边界后继续接收指针事件。这对于实现拖动、滑动等交互非常有用。

## API说明

### `bool CapturePointer(int pointerId = 0)`
捕获指定的指针。

- **参数**：
  - `pointerId`: 指针ID，默认为0（主指针/鼠标）
- **返回值**：成功返回true，失败返回false
- **说明**：捕获后，所有指针事件都会发送到此元素，即使指针移出元素边界

### `void ReleasePointerCapture(int pointerId = 0)`
释放指针捕获。

- **参数**：
  - `pointerId`: 指针ID，默认为0
- **说明**：释放之前通过CapturePointer捕获的指针

### `bool HasPointerCapture(int pointerId = 0) const`
检查当前元素是否捕获了指定指针。

- **参数**：
  - `pointerId`: 指针ID，默认为0
- **返回值**：如果此元素捕获了指针返回true，否则返回false

## 使用示例

### 基本用法

```cpp
#include <fk/ui/Button.h>

class DraggableButton : public fk::ui::Button {
protected:
    void OnPointerPressed(fk::ui::PointerEventArgs& e) override {
        // 调用基类处理
        Button::OnPointerPressed(e);
        
        // 捕获指针
        if (CapturePointer(e.pointerId)) {
            isDragging_ = true;
            startPos_ = e.position;
        }
    }
    
    void OnPointerMoved(fk::ui::PointerEventArgs& e) override {
        Button::OnPointerMoved(e);
        
        // 只有在捕获了指针时才处理拖动
        if (isDragging_ && HasPointerCapture(e.pointerId)) {
            // 计算偏移并更新位置
            float dx = e.position.x - startPos_.x;
            float dy = e.position.y - startPos_.y;
            // 更新元素位置...
        }
    }
    
    void OnPointerReleased(fk::ui::PointerEventArgs& e) override {
        Button::OnPointerReleased(e);
        
        // 释放指针捕获
        if (isDragging_) {
            ReleasePointerCapture(e.pointerId);
            isDragging_ = false;
        }
    }

private:
    bool isDragging_{false};
    fk::ui::Point startPos_;
};
```

### 滚动条滑块示例

```cpp
void ScrollBarThumb::OnPointerPressed(PointerEventArgs& e) override {
    if (e.button == MouseButton::Left) {
        // 捕获指针以便在拖动时即使鼠标移出也能继续接收事件
        CapturePointer(e.pointerId);
        isDragging_ = true;
        dragStartPos_ = e.position;
    }
}

void ScrollBarThumb::OnPointerMoved(PointerEventArgs& e) override {
    if (isDragging_ && HasPointerCapture(e.pointerId)) {
        // 计算拖动距离并更新滚动条值
        float delta = e.position.y - dragStartPos_.y;
        UpdateScrollPosition(delta);
    }
}

void ScrollBarThumb::OnPointerReleased(PointerEventArgs& e) override {
    if (isDragging_) {
        ReleasePointerCapture(e.pointerId);
        isDragging_ = false;
    }
}
```

## 实现细节

- 指针捕获通过`InputManager`实现
- UIElement通过向上遍历视觉树找到Window，然后访问其InputManager
- 支持多点触控（通过pointerId区分不同的指针）
- Window类暴露了`GetInputManager()`方法供UIElement访问

## 使用场景

1. **拖动操作**：拖动窗口、控件重新排列
2. **滑块控制**：滚动条滑块、音量滑块
3. **绘图应用**：鼠标绘制时不会因为移出画布而中断
4. **调整大小**：窗口或面板的大小调整手柄

## 注意事项

- 始终在释放指针时调用`ReleasePointerCapture`，避免指针一直被捕获
- 使用`HasPointerCapture`检查是否成功捕获，以处理捕获失败的情况
- 支持多点触控，确保使用正确的`pointerId`

## 相关文档

- [ScrollViewer使用指南](./SCROLLVIEWER_GUIDE.md)
- [输入系统概述](../Design/UI/InputSystem.md)
