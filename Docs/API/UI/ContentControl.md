# ContentControl

## 概览

**目的**：可以承载单个内容的控件基类

**命名空间**：`fk::ui`

**继承**：`Control` → `FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/ContentControl.h`

**模板参数**：CRTP 模式，`ContentControl<Derived>`

## 描述

`ContentControl` 是可以包含单个内容对象的控件基类。内容可以是字符串、UIElement或任何对象。

## 公共接口

### 内容属性

#### Content
```cpp
static const DependencyProperty& ContentProperty();

Derived* Content(const std::string& text);
Derived* Content(std::shared_ptr<UIElement> element);
```

设置控件的内容。

**示例**：
```cpp
// 文本内容
control->Content("Hello");

// UI元素内容
auto stack = std::make_shared<StackPanel>();
control->Content(stack);
```

#### GetContent
```cpp
std::shared_ptr<UIElement> GetContent() const;
```

获取当前内容元素。

### 内容模板

#### ContentTemplate
```cpp
static const DependencyProperty& ContentTemplateProperty();
```

设置用于呈现内容的数据模板。

## 使用示例

### 文本内容
```cpp
auto button = std::make_shared<Button>();
button->Content("点击我");
```

### 复杂内容
```cpp
auto button = std::make_shared<Button>();

// 创建复杂的内容布局
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Horizontal)->Spacing(5);

auto icon = stack->AddChild<Image>();
icon->Source("icon.png")->Width(16)->Height(16);

auto label = stack->AddChild<TextBlock>();
label->Text("保存");

button->Content(stack);
```

### 动态内容
```cpp
auto contentControl = std::make_shared<ContentControl<ContentControl>>();

// 设置初始内容
contentControl->Content("加载中...");

// 稍后更改内容
std::thread([contentControl, dispatcher]() {
    // 执行工作...
    
    dispatcher->InvokeAsync([contentControl]() {
        auto result = std::make_shared<TextBlock>();
        result->Text("完成！");
        contentControl->Content(result);
    });
}).detach();
```

### 数据绑定内容
```cpp
class ViewModel : public ObservableObject {
    PROPERTY(std::string, DisplayText, "")
};

auto viewModel = std::make_shared<ViewModel>();
contentControl->SetValue(FrameworkElement::DataContextProperty(), viewModel);
contentControl->SetValue(ContentControl::ContentProperty(), 
                        Binding("DisplayText"));

// 更新ViewModel会自动更新内容
viewModel->SetDisplayText("新内容");
```

## 相关类

- [Control](Control.md) - 基类
- [Button](Button.md) - 派生类
- [Window](Window.md) - 派生类

## 另请参阅

- [设计文档](../../Design/UI/ContentControl.md)
