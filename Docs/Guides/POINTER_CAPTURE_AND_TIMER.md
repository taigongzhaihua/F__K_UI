# 指针捕获和定时器使用指南

本指南介绍如何使用FK_UI框架中新增的指针捕获API和RepeatButton控件。

## 指针捕获API

指针捕获允许UI元素在指针（鼠标）移出其边界后继续接收指针事件。这对于实现拖动、滑动等交互非常有用。

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

### API说明

#### `bool CapturePointer(int pointerId = 0)`
捕获指定的指针。

- **参数**：
  - `pointerId`: 指针ID，默认为0（主指针/鼠标）
- **返回值**：成功返回true，失败返回false
- **说明**：捕获后，所有指针事件都会发送到此元素，即使指针移出元素边界

#### `void ReleasePointerCapture(int pointerId = 0)`
释放指针捕获。

- **参数**：
  - `pointerId`: 指针ID，默认为0
- **说明**：释放之前通过CapturePointer捕获的指针

#### `bool HasPointerCapture(int pointerId = 0) const`
检查当前元素是否捕获了指定指针。

- **参数**：
  - `pointerId`: 指针ID，默认为0
- **返回值**：如果此元素捕获了指针返回true，否则返回false

## RepeatButton控件

RepeatButton是一个特殊的按钮，在按下时会持续触发Click事件。常用于滚动条按钮、数字调整器等需要连续操作的场景。

### 基本用法

```cpp
#include <fk/ui/RepeatButton.h>
#include <fk/ui/StackPanel.h>
#include <iostream>

// 创建一个重复按钮
auto repeatBtn = std::make_shared<fk::ui::RepeatButton>();
repeatBtn->SetContent("增加")
        ->SetWidth(100)
        ->SetHeight(30)
        ->Delay(250)      // 首次重复前延迟250ms
        ->Interval(50);   // 重复间隔50ms

// 订阅Click事件
repeatBtn->Click.Connect([&counter]() {
    counter++;
    std::cout << "当前值: " << counter << std::endl;
});
```

### 配置属性

#### Delay（延迟）
按下按钮后，首次重复触发Click事件之前的延迟时间。

- **类型**：int（毫秒）
- **默认值**：250ms
- **设置方法**：`SetDelay(int)` 或 `Delay(int)`

```cpp
repeatBtn->Delay(500);  // 设置500ms延迟
```

#### Interval（间隔）
重复触发Click事件的时间间隔。

- **类型**：int（毫秒）
- **默认值**：33ms（约30Hz）
- **设置方法**：`SetInterval(int)` 或 `Interval(int)`

```cpp
repeatBtn->Interval(100);  // 设置100ms间隔
```

### 使用场景

#### 1. 滚动条按钮

```cpp
auto scrollUpButton = std::make_shared<fk::ui::RepeatButton>();
scrollUpButton->SetContent("▲")
             ->Delay(250)
             ->Interval(33);  // ~30Hz

scrollUpButton->Click.Connect([scrollViewer]() {
    scrollViewer->LineUp();
});
```

#### 2. 数字调整器

```cpp
auto incrementButton = std::make_shared<fk::ui::RepeatButton>();
incrementButton->SetContent("+")
              ->Delay(300)
              ->Interval(100);

incrementButton->Click.Connect([&value]() {
    value++;
    UpdateDisplay(value);
});
```

### 工作原理

1. **首次点击**：按下按钮时立即触发一次Click事件
2. **延迟期**：等待`Delay`时间
3. **重复触发**：以`Interval`间隔持续触发Click事件
4. **停止**：释放按钮时停止触发

### 注意事项

- RepeatButton使用指针捕获确保即使鼠标移出按钮也能正确工作
- 按钮在禁用状态（IsEnabled=false）时不会触发重复
- 定时器在UI线程上运行，确保线程安全
- 推荐的Interval范围：30-100ms，太短可能导致性能问题

## 实现细节

### 指针捕获实现
- 指针捕获通过`InputManager`实现
- UIElement通过向上遍历视觉树找到Window，然后访问其InputManager
- 支持多点触控（通过pointerId区分不同的指针）

### RepeatButton实现
- 继承自ButtonBase，具有所有标准按钮功能
- 使用`core::Timer`实现定时触发
- 使用全局单例Dispatcher管理所有定时器
- 使用`isFirstTick_`标志管理从延迟到重复间隔的转换

## 示例代码

完整示例请参考：
- 指针捕获：`examples/interaction/pointer_capture_demo.cpp`（待创建）
- RepeatButton：`examples/controls/repeat_button_demo.cpp`（待创建）

## 相关文档

- [ScrollViewer使用指南](./SCROLLVIEWER_GUIDE.md)
- [输入系统概述](../Design/UI/InputSystem.md)
- [定时器系统](../Design/Core/Timer.md)
