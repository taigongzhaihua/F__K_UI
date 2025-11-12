# FocusManager

## 概览

**目的**：键盘焦点管理

**命名空间**：`fk::ui`

**头文件**：`fk/ui/FocusManager.h`

## 描述

`FocusManager` 管理键盘焦点和焦点导航。

## 公共接口

### 焦点控制

#### SetFocus
```cpp
bool SetFocus(UIElement* element);
```

设置焦点到指定元素。

#### GetFocusedElement
```cpp
UIElement* GetFocusedElement() const;
```

获取当前焦点元素。

### 焦点导航

#### MoveFocus
```cpp
bool MoveFocus(FocusNavigationDirection direction);
```

按方向移动焦点（Next、Previous、Up、Down等）。

## 使用示例

### 设置焦点
```cpp
auto focusManager = FocusManager::Instance();
focusManager->SetFocus(textBox.get());
```

### 焦点导航
```cpp
// Tab键导航
focusManager->MoveFocus(FocusNavigationDirection::Next);
```

## 相关类

- [UIElement](UIElement.md)
- [InputManager](InputManager.md)
