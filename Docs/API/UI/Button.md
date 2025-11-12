# Button

## 概览

**目的**：响应指针点击的交互式按钮控件

**命名空间**：`fk::ui`

**继承**：`ContentControl<Button>` → `Control` → `FrameworkElement` → `UIElement` → `Visual` → `DependencyObject`

**头文件**：`fk/ui/Button.h`

## 描述

`Button` 是一个可点击的控件，可以包含任何内容（文本、图像或复杂布局）。它在悬停和按下时提供视觉反馈，并在激活时触发 `Click` 事件。

按钮使用 CRTP 模式（`ContentControl<Button>`）实现编译时多态和方法链。

## 公共接口

### 构造函数

```cpp
Button();
virtual ~Button() = default;
```

创建新的按钮实例。

**示例**：
```cpp
auto button = std::make_shared<Button>();
```

### 事件

#### Click
```cpp
core::Event<> Click;
```

按钮被点击时触发（指针在按钮上按下并释放）。

**用法**：
```cpp
button->Click += []() {
    std::cout << "按钮被点击！" << std::endl;
};

// 或使用断开连接
auto connection = button->Click += []() {
    std::cout << "点击" << std::endl;
};
// 稍后...
connection.Disconnect();
```

### 继承的方法（来自 ContentControl）

#### Content
```cpp
Button* Content(const std::string& text);
Button* Content(std::shared_ptr<UIElement> element);
```

设置按钮的内容。

**参数**：
- `text`：要显示的文本字符串
- `element`：要作为内容的 UI 元素

**返回**：`this` 用于方法链

**示例**：
```cpp
// 文本内容
button->Content("点击我");

// 复杂内容
auto stack = std::make_shared<StackPanel>();
stack->AddChild<TextBlock>()->Text("图标");
stack->AddChild<TextBlock>()->Text("标签");
button->Content(stack);
```

### 继承的属性（来自 FrameworkElement）

#### Width / Height
```cpp
Button* Width(double width);
Button* Height(double height);
```

设置按钮的大小。

**示例**：
```cpp
button->Width(120)->Height(40);
```

#### Margin / Padding
```cpp
Button* Margin(const Thickness& margin);
Button* Padding(const Thickness& padding);
```

设置外边距和内边距。

**示例**：
```cpp
button->Margin(Thickness(10))
      ->Padding(Thickness(5));
```

#### HorizontalAlignment / VerticalAlignment
```cpp
Button* HorizontalAlignment(HorizontalAlignment alignment);
Button* VerticalAlignment(VerticalAlignment alignment);
```

设置对齐方式。

**示例**：
```cpp
button->HorizontalAlignment(HorizontalAlignment::Center)
      ->VerticalAlignment(VerticalAlignment::Center);
```

### 继承的属性（来自 Control）

#### Background / Foreground
```cpp
Button* Background(const Color& color);
Button* Foreground(const Color& color);
```

设置背景色和前景色。

**示例**：
```cpp
button->Background(Colors::Blue)
      ->Foreground(Colors::White);
```

#### FontSize / FontFamily
```cpp
Button* FontSize(double size);
Button* FontFamily(const std::string& family);
```

设置字体属性。

**示例**：
```cpp
button->FontSize(16)
      ->FontFamily("Arial");
```

## 使用示例

### 基本按钮

```cpp
#include "fk/ui/Button.h"

auto button = std::make_shared<Button>();
button->Content("点击我")
      ->Width(100)
      ->Height(40);

button->Click += []() {
    std::cout << "按钮被点击！" << std::endl;
};
```

### 样式化按钮

```cpp
auto button = std::make_shared<Button>();
button->Content("提交")
      ->Width(120)
      ->Height(45)
      ->Background(Colors::Blue)
      ->Foreground(Colors::White)
      ->FontSize(16)
      ->Margin(Thickness(10));
```

### 带图标的按钮

```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Horizontal)
     ->Spacing(5);

auto icon = stack->AddChild<Image>();
icon->Source("icons/save.png")
    ->Width(16)
    ->Height(16);

auto label = stack->AddChild<TextBlock>();
label->Text("保存");

auto button = std::make_shared<Button>();
button->Content(stack)
      ->Width(100)
      ->Height(40);

button->Click += []() {
    SaveDocument();
};
```

### 禁用按钮

```cpp
button->IsEnabled(false);

// 稍后重新启用
button->IsEnabled(true);
```

### 工具提示

```cpp
button->ToolTip("点击保存您的工作");
```

### 使用数据绑定

```cpp
class ViewModel : public ObservableObject {
    PROPERTY(std::string, ButtonText, "点击我")
    PROPERTY(bool, IsButtonEnabled, true)
};

auto viewModel = std::make_shared<ViewModel>();

button->SetValue(Button::ContentProperty(), 
                 Binding("ButtonText").Source(viewModel));
button->SetValue(Button::IsEnabledProperty(),
                 Binding("IsButtonEnabled").Source(viewModel));
```

## 视觉状态

按钮有几种视觉状态：

1. **Normal** - 默认外观
2. **MouseOver** - 鼠标悬停在按钮上
3. **Pressed** - 按钮被按下
4. **Disabled** - 按钮被禁用

这些状态当前通过内部渲染逻辑处理。未来的实现将使用 VisualStateManager。

## 事件顺序

典型的点击操作事件顺序：

1. `MouseEnter` - 鼠标进入按钮
2. `MouseMove` - 鼠标在按钮上移动
3. `MouseDown` - 鼠标按钮按下
4. `MouseUp` - 鼠标按钮释放
5. `Click` - 点击事件触发
6. `MouseLeave` - 鼠标离开按钮

## 键盘支持

按钮支持键盘激活：

- **Space** - 聚焦时激活按钮
- **Enter** - 聚焦时激活按钮（如果按钮是默认按钮）

```cpp
button->Focus();
// 用户按 Space 或 Enter 触发 Click 事件
```

## 性能考虑

- 按钮是轻量级控件（~300 字节）
- 事件处理是 O(1)
- 渲染是高效的，具有基本的视觉状态
- 避免在点击处理程序中进行繁重操作

## 常见模式

### 命令模式

```cpp
class Command {
public:
    virtual void Execute() = 0;
    virtual bool CanExecute() = 0;
};

auto button = std::make_shared<Button>();
auto command = std::make_shared<SaveCommand>();

button->Click += [command]() {
    if (command->CanExecute()) {
        command->Execute();
    }
};
```

### 切换按钮

```cpp
bool isToggled = false;
button->Click += [&isToggled, button]() {
    isToggled = !isToggled;
    button->Background(isToggled ? Colors::Blue : Colors::Gray);
};
```

### 带确认的按钮

```cpp
button->Click += []() {
    if (ShowConfirmDialog("确定要继续吗？")) {
        PerformAction();
    }
};
```

## 相关类

- [ContentControl](ContentControl.md) - 基类
- [Control](Control.md) - 控件基类
- [TextBlock](TextBlock.md) - 用于文本内容
- [Image](Image.md) - 用于图标
- [StackPanel](StackPanel.md) - 用于复杂布局

## 另请参阅

- [设计文档](../../Design/UI/Button.md)
- [API 索引](../README.md)
- [入门指南](../../GettingStarted.md)
