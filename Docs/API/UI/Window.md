# Window

## 概览

**目的**：顶级窗口，应用程序的主容器

**命名空间**：`fk::ui`

**继承**：`ContentControl` → `Control` → `FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/Window.h`

## 描述

`Window` 表示应用程序的顶级窗口。它提供标题栏、大小、位置等窗口特定功能。

## 公共接口

### 窗口属性

#### Title
```cpp
static const DependencyProperty& TitleProperty();
Window* Title(const std::string& title);
```

设置窗口标题。

**示例**：
```cpp
window->Title("我的应用程序");
```

#### Width / Height
```cpp
Window* Width(double width);
Window* Height(double height);
```

设置窗口大小。

**示例**：
```cpp
window->Width(1024)->Height(768);
```

### 窗口状态

#### WindowState
```cpp
static const DependencyProperty& WindowStateProperty();
Window* WindowState(WindowState state);
```

设置窗口状态：
- `Normal` - 正常窗口
- `Minimized` - 最小化
- `Maximized` - 最大化
- `Fullscreen` - 全屏

**示例**：
```cpp
window->WindowState(WindowState::Maximized);
```

### 窗口样式

#### WindowStyle
```cpp
static const DependencyProperty& WindowStyleProperty();
```

设置窗口样式（有边框/无边框等）。

### 内容设置

#### SetContent
```cpp
template<typename T>
std::shared_ptr<T> SetContent();

void SetContent(std::shared_ptr<UIElement> content);
```

设置窗口的主内容。

**示例**：
```cpp
auto mainPanel = window->SetContent<StackPanel>();
mainPanel->AddChild<Button>()->Content("按钮");
```

### 显示和关闭

#### Show / Hide / Close
```cpp
void Show();
void Hide();
void Close();
```

显示、隐藏或关闭窗口。

**示例**：
```cpp
window->Show();
// ...
window->Close();
```

### 窗口事件

#### Closing / Closed
```cpp
core::Event<CancelEventArgs&> Closing;
core::Event<> Closed;
```

窗口关闭前和关闭后的事件。

**示例**：
```cpp
window->Closing += [](CancelEventArgs& e) {
    if (!ConfirmExit()) {
        e.Cancel = true;  // 取消关闭
    }
};

window->Closed += []() {
    std::cout << "窗口已关闭" << std::endl;
};
```

## 使用示例

### 基本窗口
```cpp
auto app = app::Application::Create();
auto window = app->CreateWindow();

window->Title("我的应用")
      ->Width(800)
      ->Height(600);

return app->Run();
```

### 带内容的窗口
```cpp
auto window = app->CreateWindow();
window->Title("示例应用")->Width(1024)->Height(768);

// 设置主内容
auto mainGrid = window->SetContent<Grid>();
mainGrid->AddRowDefinition()->Height(GridLength::Auto());
mainGrid->AddRowDefinition()->Height(GridLength(1, GridUnitType::Star));

// 标题栏
auto header = mainGrid->AddChild<Border>();
header->Row(0)
      ->Background(Colors::DarkBlue)
      ->Padding(Thickness(10));
      
auto title = header->SetChild<TextBlock>();
title->Text("应用程序标题")
     ->Foreground(Colors::White)
     ->FontSize(18);

// 主内容区
auto content = mainGrid->AddChild<Border>();
content->Row(1)->Padding(Thickness(10));
```

### 确认关闭
```cpp
window->Closing += [](CancelEventArgs& e) {
    // 显示确认对话框
    if (MessageBox::Show("确定要退出吗？", "确认", MessageBoxButtons::YesNo) 
        == MessageBoxResult::No) {
        e.Cancel = true;
    }
};
```

### 最大化窗口
```cpp
window->Title("全屏应用")
      ->WindowState(WindowState::Maximized);
```

### 模态对话框
```cpp
auto dialog = app->CreateWindow();
dialog->Title("对话框")
      ->Width(400)
      ->Height(300);

auto stack = dialog->SetContent<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(10)
     ->Margin(Thickness(20));

auto message = stack->AddChild<TextBlock>();
message->Text("这是一个对话框");

auto okButton = stack->AddChild<Button>();
okButton->Content("确定")->Width(100);
okButton->Click += [dialog]() {
    dialog->Close();
};

dialog->Show();
```

## 相关类

- [ContentControl](ContentControl.md) - 基类
- [Application](../App/Application.md) - 创建窗口

## 另请参阅

- [设计文档](../../Design/UI/Window.md)
