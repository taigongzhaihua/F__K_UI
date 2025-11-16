# F__K_UI 中的 x:Name 机制

## 概述

F__K_UI 框架现在支持类似于 WPF 中 `x:Name` 的元素命名和查找机制。这使得开发者可以轻松地命名 UI 元素并在代码中通过名称查找它们，而无需维护额外的引用。

## 功能特性

### 1. 元素命名

每个 `UIElement` 都可以通过以下方式设置名称：

```cpp
// 方式1：使用 SetName() 方法
auto* button = new Button();
button->SetName("submitButton");

// 方式2：使用流式 API Name() 方法（推荐）
auto* button = new Button();
button->Name("submitButton")->Content("提交");

// 获取元素名称
std::string name = button->GetName();
```

### 2. 元素查找

可以在任何 `UIElement` 上调用 `FindName()` 方法来查找命名的元素：

```cpp
// 从任何元素开始查找
UIElement* found = panel->FindName("submitButton");
if (found) {
    auto* button = dynamic_cast<Button*>(found);
    // 使用找到的按钮...
}
```

### 3. 窗口级别的查找

`Window` 类提供了便捷的 `FindName()` 方法，可以从窗口内容开始查找：

```cpp
auto window = app->CreateWindow();
// ... 构建 UI ...

// 从窗口查找元素
UIElement* found = window->FindName("myElement");
```

## 实现原理

### 逻辑树遍历

`FindName()` 方法通过递归遍历逻辑树来查找元素。逻辑树是通过 `GetLogicalChildren()` 虚方法定义的：

- **UIElement**: 默认返回空列表
- **Panel**: 返回所有子元素
- **ContentControl**: 返回内容元素或模板根
- **Border**: 返回单个子元素

### 查找算法

```cpp
UIElement* UIElement::FindName(const std::string& name) {
    // 1. 检查当前元素的名称
    if (name_ == name) {
        return this;
    }
    
    // 2. 递归搜索所有逻辑子元素
    for (UIElement* child : GetLogicalChildren()) {
        if (child) {
            UIElement* found = child->FindName(name);
            if (found) {
                return found;
            }
        }
    }
    
    // 3. 未找到返回 nullptr
    return nullptr;
}
```

## 使用示例

### 基本示例

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"

int main() {
    auto app = std::make_shared<Application>();
    auto window = app->CreateWindow();
    
    // 创建面板
    auto* panel = new ui::StackPanel();
    panel->Name("mainPanel");
    
    // 创建按钮并命名
    auto* button1 = new ui::Button();
    button1->Name("submitButton")->Content("提交");
    
    auto* button2 = new ui::Button();
    button2->Name("cancelButton")->Content("取消");
    
    panel->AddChild(button1);
    panel->AddChild(button2);
    window->Content(panel);
    
    // 稍后查找按钮
    auto* found = window->FindName("submitButton");
    if (found) {
        auto* btn = dynamic_cast<ui::Button*>(found);
        // 使用按钮...
    }
    
    return app->Run();
}
```

### 嵌套元素查找

```cpp
// 创建复杂的嵌套结构
auto* outerPanel = new ui::StackPanel();
outerPanel->Name("outerPanel");

auto* border = new ui::Border();
border->Name("contentBorder");

auto* innerPanel = new ui::StackPanel();
innerPanel->Name("innerPanel");

auto* button = new ui::Button();
button->Name("nestedButton");

// 构建层次结构
innerPanel->AddChild(button);
border->Child(innerPanel);
outerPanel->AddChild(border);
window->Content(outerPanel);

// 可以从任何父元素查找
auto* found1 = window->FindName("nestedButton");        // 从窗口查找
auto* found2 = outerPanel->FindName("nestedButton");    // 从外层面板查找
auto* found3 = border->FindName("nestedButton");        // 从边框查找
auto* found4 = innerPanel->FindName("nestedButton");    // 从内层面板查找

// 所有方法都能找到同一个按钮
assert(found1 == found2 && found2 == found3 && found3 == found4);
```

### 事件处理中的应用

```cpp
auto* button = new ui::Button();
button->Name("actionButton");
button->AddHandler(Button::ClickEvent(), [](UIElement* sender, RoutedEventArgs& e) {
    // 在事件处理器中查找其他元素
    auto* window = /* 获取窗口引用 */;
    auto* statusText = window->FindName("statusLabel");
    if (statusText) {
        auto* label = dynamic_cast<ui::TextBlock*>(statusText);
        label->Text("按钮已点击！");
    }
});
```

## 与 WPF 的对比

### WPF 中的 x:Name

```xaml
<Window>
    <StackPanel>
        <Button x:Name="submitButton" Content="提交" />
        <TextBlock x:Name="statusText" Text="就绪" />
    </StackPanel>
</Window>
```

```csharp
// C# 代码后台
private void OnClick(object sender, RoutedEventArgs e)
{
    statusText.Text = "按钮已点击";
    // 或使用 FindName
    var btn = this.FindName("submitButton") as Button;
}
```

### F__K_UI 中的等价实现

```cpp
auto window = app->CreateWindow();

auto* panel = new ui::StackPanel();

auto* submitButton = new ui::Button();
submitButton->Name("submitButton")->Content("提交");

auto* statusText = new ui::TextBlock();
statusText->Name("statusText")->Text("就绪");

panel->AddChild(submitButton);
panel->AddChild(statusText);
window->Content(panel);

// 稍后使用
auto* text = window->FindName("statusText");
if (text) {
    auto* label = dynamic_cast<ui::TextBlock*>(text);
    label->Text("按钮已点击");
}
```

## 设计注意事项

### 1. 名称唯一性

- 系统不强制要求名称唯一
- 如果有多个相同名称的元素，`FindName()` 返回第一个找到的
- 建议在同一逻辑树中使用唯一的名称

### 2. 性能考虑

- `FindName()` 使用深度优先搜索
- 对于大型 UI 树，查找可能需要一定时间
- 建议缓存频繁访问的元素引用

```cpp
// 推荐：缓存引用
auto* statusLabel = window->FindName("statusLabel");
// 多次使用 statusLabel...

// 不推荐：重复查找
for (int i = 0; i < 1000; i++) {
    auto* label = window->FindName("statusLabel");  // 每次都查找
}
```

### 3. 内存管理

- 查找返回的是原始指针，不涉及所有权转移
- 不要 `delete` 通过 `FindName()` 找到的元素
- 元素的生命周期由其父容器管理

### 4. 类型安全

- `FindName()` 返回 `UIElement*`，需要手动转换类型
- 使用 `dynamic_cast` 进行安全的类型转换

```cpp
auto* found = window->FindName("myButton");
if (found) {
    auto* button = dynamic_cast<ui::Button*>(found);
    if (button) {
        // 安全地使用按钮
        button->Content("新内容");
    }
}
```

## API 参考

### UIElement 类

```cpp
class UIElement {
public:
    // 获取元素名称
    const std::string& GetName() const;
    
    // 设置元素名称
    void SetName(const std::string& name);
    
    // 流式API：设置元素名称并返回this指针
    UIElement* Name(const std::string& name);
    
    // 在当前元素的逻辑树中查找指定名称的元素
    UIElement* FindName(const std::string& name);
    
    // 获取逻辑子元素（虚方法，派生类覆写）
    virtual std::vector<UIElement*> GetLogicalChildren() const;
};
```

### Window 类

```cpp
class Window : public ContentControl<Window> {
public:
    // 在窗口的内容树中查找指定名称的元素
    UIElement* FindName(const std::string& name);
};
```

## 最佳实践

### 1. 命名约定

```cpp
// 使用有意义的名称
button->Name("submitButton");        // 好
button->Name("btn1");                // 不好

// 使用驼峰命名法
textBox->Name("userNameInput");      // 好
textBox->Name("user_name_input");    // 可以但不一致
```

### 2. 组织结构

```cpp
// 为主要容器命名
auto* mainPanel = new ui::StackPanel();
mainPanel->Name("mainPanel");

auto* headerPanel = new ui::StackPanel();
headerPanel->Name("headerPanel");

auto* contentPanel = new ui::StackPanel();
contentPanel->Name("contentPanel");

auto* footerPanel = new ui::StackPanel();
footerPanel->Name("footerPanel");
```

### 3. 错误处理

```cpp
auto* found = window->FindName("myElement");
if (!found) {
    std::cerr << "警告：未找到名为 'myElement' 的元素" << std::endl;
    return;
}

auto* specificType = dynamic_cast<ui::Button*>(found);
if (!specificType) {
    std::cerr << "错误：元素类型不匹配" << std::endl;
    return;
}

// 安全地使用元素
specificType->Content("新内容");
```

## 扩展性

### 自定义容器

如果创建自定义容器控件，需要覆写 `GetLogicalChildren()` 方法：

```cpp
class MyCustomPanel : public Panel<MyCustomPanel> {
public:
    std::vector<UIElement*> GetLogicalChildren() const override {
        std::vector<UIElement*> children;
        // 返回自定义的子元素集合
        for (auto* child : myCustomChildren_) {
            children.push_back(child);
        }
        return children;
    }
    
private:
    std::vector<UIElement*> myCustomChildren_;
};
```

## 总结

F__K_UI 的 x:Name 机制提供了：

- ✅ 简洁的元素命名 API
- ✅ 灵活的元素查找功能
- ✅ 类似 WPF 的开发体验
- ✅ 递归的逻辑树遍历
- ✅ 从任意父元素开始查找的能力

这个机制使得 UI 元素的管理更加方便，特别是在复杂的 UI 层次结构中。
