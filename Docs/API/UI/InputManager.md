# InputManager

## 概览

**目的**：输入事件路由和处理

**命名空间**：`fk::ui`

**头文件**：`fk/ui/InputManager.h`

## 描述

`InputManager` 管理鼠标和键盘输入的路由和分发。

## 公共接口

### 处理输入

#### ProcessInput
```cpp
void ProcessInput(Visual* root, const InputEvent& event);
```

处理输入事件。

### 命中测试

#### HitTest
```cpp
Visual* HitTest(Visual* root, const Point& point);
```

查找指定点处的视觉元素。

## 使用示例

### 处理鼠标事件
```cpp
auto inputManager = std::make_shared<InputManager>();

MouseButtonEventArgs mouseEvent(MouseButton::Left, Point(100, 100));
inputManager->ProcessInput(rootVisual, mouseEvent);
```

## 相关类

- [UIElement](UIElement.md)
- [FocusManager](FocusManager.md)
