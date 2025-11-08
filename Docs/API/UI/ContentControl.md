# ContentControl - 内容控件基类

单一内容容器控件的基类，可以容纳一个子元素。

## 概述

`ContentControl` 是所有单内容控件的基类，提供：

- 单个子元素（Content）管理
- 内容变化事件通知
- 继承自 `Control`，支持样式、模板等特性

典型派生类：Button、ScrollViewer 等。

## 类层次结构

```
DependencyObject
  └─ DispatcherObject
      └─ UIElement
          └─ FrameworkElement
              └─ Control
                  └─ ContentControl    ← 你在这里
                      ├─ Button
                      ├─ ScrollViewer
                      └─ 其他单内容控件
```

## 基本用法

### 创建 ContentControl

```cpp
#include <fk/ui/ContentControl.h>

// 工厂函数
auto control = contentControl();

// 或直接创建
auto control = std::make_shared<ContentControl>();
```

### 设置内容

```cpp
auto btn = button();
auto text = textBlock().Text("Click me");

// 设置内容
btn->SetContent(text);

// 链式 API
auto btn2 = button()
    .Content(textBlock().Text("Submit"));
```

## 核心属性

### Content - 内容元素

```cpp
std::shared_ptr<UIElement> GetContent() const;
void SetContent(std::shared_ptr<UIElement> content);
```

获取或设置控件的内容元素。

**示例：**

```cpp
auto control = contentControl();
auto text = textBlock().Text("Hello");

control->SetContent(text);

// 获取
auto content = control->GetContent();
if (content) {
    std::cout << "Has content" << std::endl;
}
```

## 事件

### ContentChanged - 内容变化事件

```cpp
core::Event<UIElement*, UIElement*> ContentChanged;
```

当内容变化时触发，参数为 `(oldContent, newContent)`。

**示例：**

```cpp
auto control = contentControl();

control->ContentChanged.Subscribe([](UIElement* oldContent, UIElement* newContent) {
    std::cout << "Content changed from " << oldContent 
              << " to " << newContent << std::endl;
});

control->SetContent(textBlock().Text("New content"));
// 触发 ContentChanged 事件
```

## 虚方法

### OnContentChanged

```cpp
virtual void OnContentChanged(UIElement* oldContent, UIElement* newContent);
```

内容变化时调用的虚方法，派生类可重写以自定义行为。

**重写示例：**

```cpp
class MyControl : public ContentControl {
protected:
    void OnContentChanged(UIElement* oldContent, UIElement* newContent) override {
        ContentControl::OnContentChanged(oldContent, newContent);
        
        // 自定义逻辑
        if (newContent) {
            std::cout << "New content set" << std::endl;
        }
    }
};
```

## 完整示例

### 示例 1：简单的内容容器

```cpp
#include <fk/ui/ContentControl.h>
#include <fk/ui/TextBlock.h>
#include <fk/ui/Button.h>

int main() {
    auto control = contentControl();
    
    // 设置文本内容
    control->SetContent(
        textBlock()
            .Text("Hello, World!")
            .FontSize(24)
    );
    
    // 添加到窗口...
}
```

### 示例 2：动态切换内容

```cpp
auto container = contentControl();
auto text1 = textBlock().Text("View 1");
auto text2 = textBlock().Text("View 2");

// 初始内容
container->SetContent(text1);

// 监听内容变化
container->ContentChanged.Subscribe([](UIElement* old, UIElement* newContent) {
    std::cout << "Switched to new view" << std::endl;
});

// 切换内容
std::this_thread::sleep_for(std::chrono::seconds(2));
container->SetContent(text2);  // 触发 ContentChanged
```

### 示例 3：嵌套内容

```cpp
auto outer = contentControl();
auto inner = contentControl();

// 嵌套
inner->SetContent(textBlock().Text("Inner content"));
outer->SetContent(inner);

// 访问嵌套内容
auto outerContent = outer->GetContent();  // inner
if (auto innerControl = std::dynamic_pointer_cast<ContentControl>(outerContent)) {
    auto deepContent = innerControl->GetContent();  // textBlock
}
```

### 示例 4：链式 API

```cpp
auto btn = button()
    .Content(
        textBlock()
            .Text("Click Me")
            .FontSize(16)
    )
    .Width(100)
    .Height(40);
```

### 示例 5：自定义 ContentControl

```cpp
class Card : public ContentControl {
public:
    Card() {
        // 默认样式
        SetBackground(Color::White());
        SetBorderThickness(Thickness(1));
        SetBorderBrush(Color::Gray());
        SetPadding(Thickness(10));
    }
    
protected:
    void OnContentChanged(UIElement* oldContent, UIElement* newContent) override {
        ContentControl::OnContentChanged(oldContent, newContent);
        
        // 为新内容应用样式
        if (auto element = dynamic_cast<FrameworkElement*>(newContent)) {
            element->SetMargin(Thickness(5));
        }
    }
};

// 使用
auto card = std::make_shared<Card>();
card->SetContent(textBlock().Text("Card content"));
```

## 与其他类的关系

### 与 Panel 的区别

| 特性 | ContentControl | Panel |
| --- | --- | --- |
| 子元素数量 | 单个 | 多个 |
| 用途 | 单一内容容器 | 布局容器 |
| 典型派生类 | Button、ScrollViewer | StackPanel、Grid |

### 派生类示例

**Button（按钮）**
```cpp
auto btn = button()
    .Content(textBlock().Text("OK"));
```

**ScrollViewer（滚动查看器）**
```cpp
auto viewer = scrollViewer()
    .Content(
        textBlock().Text("Long content...")
    );
```

## 布局行为

`ContentControl` 的布局：

1. **Measure 阶段**：测量内容元素的期望尺寸
2. **Arrange 阶段**：将内容元素排列在控件区域内

**内容对齐：**
```cpp
auto control = contentControl();
control->SetContent(textBlock().Text("Centered"));
control->SetHorizontalContentAlignment(HorizontalAlignment::Center);
control->SetVerticalContentAlignment(VerticalAlignment::Center);
```

## 最佳实践

### ✅ 推荐做法

**1. 使用工厂函数和链式 API**
```cpp
auto btn = button()
    .Content(textBlock().Text("Submit"))
    .Width(100);
```

**2. 监听内容变化事件**
```cpp
control->ContentChanged.Subscribe([](UIElement* old, UIElement* newContent) {
    // 响应内容变化
});
```

**3. 派生类重写 OnContentChanged**
```cpp
class MyControl : public ContentControl {
protected:
    void OnContentChanged(UIElement* old, UIElement* newContent) override {
        ContentControl::OnContentChanged(old, newContent);
        // 自定义逻辑
    }
};
```

### ❌ 避免的做法

**1. 设置多个子元素（使用 Panel 代替）**
```cpp
// ❌ ContentControl 只能有一个内容
control->SetContent(child1);
control->SetContent(child2);  // child1 被替换

// ✅ 使用 Panel
auto panel = stackPanel();
panel->Children({child1, child2});
```

**2. 忘记检查空指针**
```cpp
// ❌ 可能为 nullptr
auto content = control->GetContent();
content->SetVisible(false);  // 崩溃！

// ✅ 检查
if (auto content = control->GetContent()) {
    content->SetVisible(false);
}
```

## 常见问题

**Q: ContentControl 和 Panel 有什么区别？**

A: `ContentControl` 只能有**一个**子元素，`Panel` 可以有**多个**。Button、ScrollViewer 等使用 ContentControl，StackPanel、Grid 等使用 Panel。

**Q: 如何实现多内容控件？**

A: 将 `Panel` 作为 Content：
```cpp
auto control = contentControl();
auto panel = stackPanel();
panel->Children({child1, child2, child3});
control->SetContent(panel);
```

**Q: Content 可以为 nullptr 吗？**

A: 可以。设置为 `nullptr` 会清空内容。

**Q: 如何实现内容模板？**

A: 派生类可以在 `OnContentChanged` 中应用模板逻辑：
```cpp
void OnContentChanged(UIElement* old, UIElement* newContent) override {
    if (newContent) {
        // 应用模板样式
        ApplyTemplate(newContent);
    }
}
```

**Q: ContentChanged 何时触发？**

A: 调用 `SetContent` 时，如果新内容与旧内容不同（即使都是 `nullptr`），就会触发。

## 性能考虑

- **内存开销**：~16 字节（一个智能指针）
- **内容设置**：O(1) 操作
- **布局开销**：取决于内容元素的复杂度

## 相关类型

- **Control** - 父类，提供样式和模板支持
- **Button** - 派生类，可点击的内容控件
- **ScrollViewer** - 派生类，可滚动的内容控件
- **Panel** - 多子元素容器

## 总结

`ContentControl` 是单内容控件的基类，提供：

✅ 单个子元素管理  
✅ 内容变化事件  
✅ 虚方法重写支持  
✅ 链式 API

适用于 Button、ScrollViewer 等单内容场景。
