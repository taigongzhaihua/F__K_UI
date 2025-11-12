# UIElement

## 概览

**目的**：可交互UI元素的基类，添加输入、焦点和布局支持

**命名空间**：`fk::ui`

**继承**：`Visual` → `DependencyObject`

**头文件**：`fk/ui/UIElement.h`

## 描述

`UIElement` 扩展 `Visual` 类，添加输入事件处理、焦点管理和布局系统支持。所有可交互的UI元素都派生自此类。

## 公共接口

### 布局属性

#### Width / Height
```cpp
static const DependencyProperty& WidthProperty();
static const DependencyProperty& HeightProperty();
```

设置元素的宽度和高度。

**示例**：
```cpp
element->SetValue(UIElement::WidthProperty(), 200.0);
element->SetValue(UIElement::HeightProperty(), 100.0);
```

### 输入事件

#### 鼠标事件
```cpp
core::Event<const MouseEventArgs&> MouseEnter;
core::Event<const MouseEventArgs&> MouseLeave;
core::Event<const MouseEventArgs&> MouseMove;
core::Event<const MouseButtonEventArgs&> MouseDown;
core::Event<const MouseButtonEventArgs&> MouseUp;
```

处理鼠标交互。

**示例**：
```cpp
element->MouseDown += [](const MouseButtonEventArgs& e) {
    std::cout << "鼠标点击位置: " << e.Position.X << ", " << e.Position.Y << std::endl;
};
```

#### 键盘事件
```cpp
core::Event<const KeyEventArgs&> KeyDown;
core::Event<const KeyEventArgs&> KeyUp;
```

处理键盘输入。

### 焦点管理

#### Focus / IsFocused
```cpp
bool Focus();
bool IsFocused() const;
static const DependencyProperty& IsFocusableProperty();
```

管理键盘焦点。

**示例**：
```cpp
element->SetValue(UIElement::IsFocusableProperty(), true);
if (element->Focus()) {
    std::cout << "焦点已设置" << std::endl;
}
```

### 布局方法

#### Measure / Arrange
```cpp
void Measure(const Size& availableSize);
void Arrange(const Rect& finalRect);
```

参与布局系统的测量和排列。

#### InvalidateMeasure / InvalidateArrange
```cpp
void InvalidateMeasure();
void InvalidateArrange();
```

标记布局为脏，触发重新布局。

### 启用状态

#### IsEnabled
```cpp
static const DependencyProperty& IsEnabledProperty();
```

控制元素是否响应输入。

**示例**：
```cpp
element->SetValue(UIElement::IsEnabledProperty(), false);  // 禁用
```

## 使用示例

### 处理输入事件
```cpp
auto element = std::make_shared<UIElement>();

element->MouseDown += [](const MouseButtonEventArgs& e) {
    if (e.Button == MouseButton::Left) {
        std::cout << "左键点击" << std::endl;
    }
};

element->KeyDown += [](const KeyEventArgs& e) {
    if (e.Key == Key::Enter) {
        std::cout << "Enter键按下" << std::endl;
    }
};
```

### 设置大小和启用状态
```cpp
element->SetValue(UIElement::WidthProperty(), 300.0);
element->SetValue(UIElement::HeightProperty(), 200.0);
element->SetValue(UIElement::IsEnabledProperty(), true);
```

## 相关类

- [Visual](Visual.md) - 基类
- [FrameworkElement](FrameworkElement.md) - 派生类
- [InputManager](InputManager.md) - 输入管理
- [FocusManager](FocusManager.md) - 焦点管理

## 另请参阅

- [设计文档](../../Design/UI/UIElement.md)
