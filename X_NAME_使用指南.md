# F__K_UI x:Name 功能使用指南

## 简介

F__K_UI 框架现在支持类似 WPF 中 `x:Name` 的元素命名和查找机制。通过这个功能，你可以：

- 为 UI 元素设置名称
- 通过名称快速查找元素
- 无需手动维护元素引用

## 快速开始

### 第一步：为元素命名

使用 `Name()` 方法为元素设置名称：

```cpp
auto* button = new ui::Button();
button->Name("submitButton");  // 设置名称
button->Content("提交");
```

### 第二步：查找元素

使用 `FindName()` 方法查找命名的元素：

```cpp
// 从窗口查找
UIElement* found = window->FindName("submitButton");

// 转换为具体类型
if (found) {
    auto* button = dynamic_cast<ui::Button*>(found);
    if (button) {
        // 使用按钮
        button->Content("新文本");
    }
}
```

## 详细说明

### 1. 命名元素

#### 基本用法

```cpp
// 方式1：先创建，后命名
auto* textBlock = new ui::TextBlock();
textBlock->SetName("myText");

// 方式2：使用流式 API（推荐）
auto* textBlock = new ui::TextBlock();
textBlock->Name("myText");
```

#### 链式调用

`Name()` 方法返回元素指针，可以与其他方法链式调用：

```cpp
auto* button = new ui::Button();
button->Name("submitButton")    // 设置名称
      ->Content("提交")         // 设置内容
      ->Width(200)              // 设置宽度
      ->Height(50)              // 设置高度
      ->Margin(10);             // 设置边距
```

#### 获取元素名称

```cpp
std::string name = element->GetName();
std::cout << "元素名称: " << name << std::endl;
```

### 2. 查找元素

#### 从窗口查找

`Window` 类提供了 `FindName()` 方法，会在整个窗口内容中搜索：

```cpp
auto window = app->CreateWindow();
// ... 构建 UI ...

// 查找元素
UIElement* found = window->FindName("myButton");
if (found) {
    std::cout << "找到元素！" << std::endl;
}
```

#### 从任意元素查找

任何 `UIElement` 都可以调用 `FindName()`，会在其子树中搜索：

```cpp
auto* panel = new ui::StackPanel();
// ... 添加子元素 ...

// 从面板查找
UIElement* found = panel->FindName("childElement");
```

#### 查找不存在的元素

如果元素不存在，`FindName()` 返回 `nullptr`：

```cpp
UIElement* found = window->FindName("nonExistent");
if (found == nullptr) {
    std::cout << "元素不存在" << std::endl;
}
```

#### 类型转换

`FindName()` 返回 `UIElement*`，需要转换为具体类型：

```cpp
UIElement* found = window->FindName("myButton");
if (found) {
    // 安全的类型转换
    auto* button = dynamic_cast<ui::Button*>(found);
    if (button) {
        // 现在可以使用 Button 特有的方法
        button->Content("新内容");
    } else {
        std::cout << "元素类型不是 Button" << std::endl;
    }
}
```

### 3. 完整示例

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/StackPanel.h"

int main() {
    auto app = std::make_shared<Application>();
    auto window = app->CreateWindow();
    
    // 创建面板
    auto* panel = new ui::StackPanel();
    panel->Name("mainPanel");
    
    // 创建标题
    auto* title = new ui::TextBlock();
    title->Name("titleText");
    title->Text("欢迎使用 F__K_UI");
    title->FontSize(24.0f);
    
    // 创建按钮
    auto* button = new ui::Button();
    button->Name("clickButton");
    button->Content("点击我");
    
    // 创建状态文本
    auto* status = new ui::TextBlock();
    status->Name("statusText");
    status->Text("就绪");
    
    // 构建 UI
    panel->AddChild(title);
    panel->AddChild(button);
    panel->AddChild(status);
    window->Content(panel);
    
    // 稍后使用 FindName 查找元素
    auto* foundButton = window->FindName("clickButton");
    if (foundButton) {
        auto* btn = dynamic_cast<ui::Button*>(foundButton);
        // 配置按钮...
    }
    
    auto* foundStatus = window->FindName("statusText");
    if (foundStatus) {
        auto* text = dynamic_cast<ui::TextBlock*>(foundStatus);
        text->Text("初始化完成");
    }
    
    return app->Run();
}
```

## 实用技巧

### 1. 命名约定

建议使用有意义的、描述性的名称：

```cpp
// 好的命名
button->Name("submitButton");
button->Name("cancelButton");
text->Name("userNameLabel");

// 避免使用
button->Name("btn1");
button->Name("b");
```

### 2. 缓存查找结果

如果需要频繁访问某个元素，建议缓存查找结果：

```cpp
// 好的做法：查找一次，多次使用
auto* statusLabel = window->FindName("statusLabel");
auto* label = dynamic_cast<ui::TextBlock*>(statusLabel);

for (int i = 0; i < 100; i++) {
    label->Text("进度: " + std::to_string(i) + "%");
}

// 避免：重复查找
for (int i = 0; i < 100; i++) {
    auto* found = window->FindName("statusLabel");  // 每次都查找，低效
    auto* label = dynamic_cast<ui::TextBlock*>(found);
    label->Text("进度: " + std::to_string(i) + "%");
}
```

### 3. 错误处理

始终检查 `FindName()` 和类型转换的结果：

```cpp
auto* found = window->FindName("myElement");
if (!found) {
    std::cerr << "错误：找不到元素 'myElement'" << std::endl;
    return;
}

auto* button = dynamic_cast<ui::Button*>(found);
if (!button) {
    std::cerr << "错误：元素类型不匹配" << std::endl;
    return;
}

// 现在可以安全使用
button->Content("新内容");
```

### 4. 嵌套查找

对于嵌套的 UI 结构，可以从不同层级开始查找：

```cpp
// 创建嵌套结构
auto* outerPanel = new ui::StackPanel();
outerPanel->Name("outer");

auto* innerPanel = new ui::StackPanel();
innerPanel->Name("inner");

auto* button = new ui::Button();
button->Name("targetButton");

innerPanel->AddChild(button);
outerPanel->AddChild(innerPanel);
window->Content(outerPanel);

// 可以从任何父元素查找
auto* found1 = window->FindName("targetButton");      // 从窗口
auto* found2 = outerPanel->FindName("targetButton");  // 从外层面板
auto* found3 = innerPanel->FindName("targetButton");  // 从内层面板

// 所有方法都会找到同一个按钮
assert(found1 == found2 && found2 == found3);
```

### 5. 在事件处理中使用

```cpp
auto* button = new ui::Button();
button->Name("actionButton");
button->Content("执行操作");

// 添加点击事件
button->AddHandler(ui::Button::ClickEvent(), 
    [window](ui::UIElement* sender, ui::RoutedEventArgs& e) {
        // 在事件处理器中查找其他元素
        auto* found = window->FindName("statusLabel");
        if (found) {
            auto* label = dynamic_cast<ui::TextBlock*>(found);
            label->Text("操作已执行！");
        }
    }
);
```

## 常见问题

### Q1: 如果有多个同名元素会怎样？

A: `FindName()` 使用深度优先搜索，会返回**第一个找到的元素**。

**行为说明**：
- 使用深度优先搜索（DFS）遍历逻辑树
- 遇到第一个匹配的元素就立即返回，不继续搜索
- 从不同的父元素开始查找，可能返回不同的结果

**示例**：
```cpp
// 创建两个同名按钮
auto* button1 = new Button();
button1->Name("myButton");
panel->AddChild(button1);

auto* button2 = new Button();
button2->Name("myButton");  // 重复名称！
panel->AddChild(button2);

// 查找会返回第一个
auto* found = panel->FindName("myButton");
// found == button1 (第一个添加的)
```

**建议**：
- ✅ 在同一逻辑树中使用唯一的名称
- ✅ 使用有意义的前缀区分不同区域的元素
- ✅ 在设计阶段就规划好命名规范
- ❌ 避免依赖查找顺序的不确定行为

详细说明请参考：`DUPLICATE_NAMES_行为说明.md`

### Q2: FindName 的性能如何？

A: `FindName()` 使用递归深度优先搜索，时间复杂度为 O(n)，其中 n 是树中元素的数量。对于大型 UI 树，建议缓存查找结果。

### Q3: 可以在模板中使用吗？

A: 是的，`FindName()` 可以在控件模板中使用，会正确遍历模板实例化的元素。

### Q4: 如何知道查找失败的原因？

A: 如果 `FindName()` 返回 `nullptr`，可能的原因有：
- 元素名称拼写错误
- 元素还未添加到逻辑树
- 从错误的父元素开始查找

可以添加调试输出来诊断：

```cpp
auto* found = window->FindName("myElement");
if (!found) {
    std::cout << "未找到元素 'myElement'" << std::endl;
    std::cout << "请检查：" << std::endl;
    std::cout << "  1. 元素名称是否正确" << std::endl;
    std::cout << "  2. 元素是否已添加到树中" << std::endl;
    std::cout << "  3. 查找起点是否正确" << std::endl;
}
```

## 与 WPF 的对比

| 功能 | WPF (C#) | F__K_UI (C++) |
|------|----------|---------------|
| 设置名称 | `x:Name="myButton"` | `Name("myButton")` |
| 查找元素 | `FindName("myButton")` | `FindName("myButton")` |
| 类型转换 | `as Button` | `dynamic_cast<Button*>` |
| 链式调用 | 不支持 | 支持 |
| 返回类型 | `object` | `UIElement*` |

## 示例程序

完整的示例程序在 `examples/findname_demo.cpp` 中，包含：

1. 基本命名和查找
2. 嵌套元素查找
3. 从不同层级查找
4. 错误处理
5. 流式 API 使用

运行示例：

```bash
cd build
./findname_demo
```

## 更多资源

- **详细文档**：`Docs/X_NAME_FEATURE.md`
- **实现总结**：`X_NAME_IMPLEMENTATION_SUMMARY.md`
- **示例代码**：`examples/findname_demo.cpp`

## 总结

F__K_UI 的 x:Name 机制提供了：

✅ **简洁的 API**：流式接口，链式调用  
✅ **灵活的查找**：从任意父元素开始  
✅ **类型安全**：使用 dynamic_cast 转换  
✅ **WPF 兼容**：类似的使用体验  
✅ **文档完善**：详细的使用说明

现在你可以像在 WPF 中一样，轻松地命名和查找 UI 元素了！

---

**最后更新**：2025年11月16日  
**版本**：1.0
