# DependencyObject - 依赖对象基类

支持依赖属性、数据绑定、逻辑树的基类。

## 概述

`DependencyObject` 是框架属性系统的基础，提供：

- **依赖属性系统**：GetValue/SetValue
- **数据绑定支持**：SetBinding/GetBinding
- **DataContext** 继承机制
- **逻辑树**：父子关系管理
- **元素命名**：ElementName 支持
- **属性变化通知**：PropertyChanged 事件

## 类层次结构

```
DependencyObject    ← 你在这里
  └─ DispatcherObject（UI 线程）
      └─ UIElement
          └─ ...
```

## 基本用法

### 访问依赖属性

```cpp
#include <fk/binding/DependencyObject.h>

auto element = std::make_shared<Button>();

// 设置值
element->SetValue(Button::ContentProperty(), std::string("Click me"));

// 获取值
auto content = element->GetValue<std::string>(Button::ContentProperty());
```

### 数据绑定

```cpp
auto textBlock = std::make_shared<TextBlock>();

// 设置绑定
textBlock->SetBinding(
    TextBlock::TextProperty(),
    bind("UserName").Source(viewModel)
);

// 获取绑定表达式
auto binding = textBlock->GetBinding(TextBlock::TextProperty());

// 清除绑定
textBlock->ClearBinding(TextBlock::TextProperty());
```

## 核心属性

### DataContext - 数据上下文

```cpp
void SetDataContext(std::any value);
const std::any& GetDataContext() const noexcept;
void ClearDataContext();
bool HasDataContext() const noexcept;
```

数据上下文，自动继承父元素的 DataContext。

**示例：**

```cpp
// 设置 ViewModel
auto vm = std::make_shared<MyViewModel>();
window->SetDataContext(vm);

// 子元素自动继承
auto button = button();
window->AddChild(button);
// button 自动获得 vm 作为 DataContext

// 绑定时无需指定 Source
button->SetBinding(
    Button::ContentProperty(),
    bind("ButtonText")  // 自动从 DataContext 查找
);
```

### ElementName - 元素名称

```cpp
void SetElementName(std::string name);
const std::string& GetElementName() const noexcept;
```

设置元素名称，用于 ElementName 绑定。

**示例：**

```cpp
auto slider = std::make_shared<Slider>();
slider->SetElementName("VolumeSlider");

// 其他元素可通过名称引用
auto label = textBlock();
label->SetBinding(
    TextBlock::TextProperty(),
    bind("Value").ElementName("VolumeSlider")
);
```

## 主要方法

### GetValue / SetValue - 访问依赖属性

```cpp
const std::any& GetValue(const DependencyProperty& property) const;

template<typename T>
T GetValue(const DependencyProperty& property) const;

void SetValue(const DependencyProperty& property, std::any value);

template<typename T>
void SetValue(const DependencyProperty& property, T&& value);
```

获取或设置依赖属性的值。

**示例：**

```cpp
// 设置
button->SetValue(Button::WidthProperty(), 100.0);
button->SetValue(Button::ContentProperty(), std::string("OK"));

// 获取
double width = button->GetValue<double>(Button::WidthProperty());
auto content = button->GetValue<std::string>(Button::ContentProperty());
```

### ClearValue - 清除属性值

```cpp
void ClearValue(const DependencyProperty& property);
```

清除本地值，恢复到默认值或继承值。

**示例：**

```cpp
button->SetValue(Button::WidthProperty(), 100.0);
button->ClearValue(Button::WidthProperty());  // 恢复默认值
```

### GetValueSource - 获取值来源

```cpp
ValueSource GetValueSource(const DependencyProperty& property) const;
```

```cpp
enum class ValueSource {
    Unset,          // 未设置
    Default,        // 默认值
    Inherited,      // 继承值
    Local,          // 本地值
    Binding,        // 绑定值
    Animation       // 动画值（未来）
};
```

**示例：**

```cpp
auto source = button->GetValueSource(Button::WidthProperty());
if (source == ValueSource::Binding) {
    std::cout << "Width is bound" << std::endl;
}
```

### SetBinding / GetBinding / ClearBinding

```cpp
void SetBinding(const DependencyProperty& property, Binding binding);
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
void ClearBinding(const DependencyProperty& property);
```

管理属性绑定。

**示例：**

```cpp
// 设置绑定
text->SetBinding(
    TextBlock::TextProperty(),
    bind("Label").Source(vm)
);

// 获取绑定表达式
if (auto expr = text->GetBinding(TextBlock::TextProperty())) {
    expr->UpdateTarget();  // 手动更新
}

// 清除绑定
text->ClearBinding(TextBlock::TextProperty());
```

### 逻辑树管理

```cpp
void SetLogicalParent(DependencyObject* parent);
DependencyObject* GetLogicalParent() const noexcept;

void AddLogicalChild(DependencyObject* child);
void RemoveLogicalChild(DependencyObject* child);
const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;
```

管理逻辑树结构。

**示例：**

```cpp
auto parent = stackPanel();
auto child = button();

// 添加到逻辑树
parent->AddLogicalChild(child.get());
child->SetLogicalParent(parent.get());

// 访问
auto p = child->GetLogicalParent();
auto children = parent->GetLogicalChildren();
```

### FindElementByName - 按名称查找元素

```cpp
DependencyObject* FindElementByName(std::string_view name);
const DependencyObject* FindElementByName(std::string_view name) const;
```

在逻辑树中查找指定名称的元素。

**示例：**

```cpp
slider->SetElementName("MySlider");
parent->AddLogicalChild(slider.get());

// 查找
auto found = parent->FindElementByName("MySlider");
if (found) {
    std::cout << "Found slider!" << std::endl;
}
```

## 事件

### PropertyChanged - 属性变化事件

```cpp
core::Event<
    const DependencyProperty&,  // property
    const std::any&,            // oldValue
    const std::any&,            // newValue
    ValueSource,                // oldSource
    ValueSource                 // newSource
> PropertyChanged;
```

任何依赖属性变化时触发。

**示例：**

```cpp
button->PropertyChanged.Subscribe([](
    const DependencyProperty& prop,
    const std::any& oldVal,
    const std::any& newVal,
    ValueSource oldSrc,
    ValueSource newSrc
) {
    std::cout << "Property changed: " << prop.Name() << std::endl;
});

button->SetValue(Button::WidthProperty(), 100.0);
// 触发 PropertyChanged
```

### BindingChanged - 绑定变化事件

```cpp
core::Event<
    const DependencyProperty&,
    const std::shared_ptr<BindingExpression>&,  // oldBinding
    const std::shared_ptr<BindingExpression>&   // newBinding
> BindingChanged;
```

绑定设置或清除时触发。

**示例：**

```cpp
element->BindingChanged.Subscribe([](
    const DependencyProperty& prop,
    const auto& oldBinding,
    const auto& newBinding
) {
    if (newBinding) {
        std::cout << "Binding set on " << prop.Name() << std::endl;
    } else {
        std::cout << "Binding cleared on " << prop.Name() << std::endl;
    }
});
```

### DataContextChanged - 数据上下文变化事件

```cpp
core::Event<const std::any&, const std::any&> DataContextChanged;
```

DataContext 变化时触发，参数为 `(oldContext, newContext)`。

**示例：**

```cpp
button->DataContextChanged.Subscribe([](const std::any& old, const std::any& newCtx) {
    std::cout << "DataContext changed" << std::endl;
});

button->SetDataContext(viewModel);
// 触发 DataContextChanged
```

## 完整示例

### 示例 1：依赖属性基本操作

```cpp
auto button = std::make_shared<Button>();

// 设置值
button->SetValue(Button::WidthProperty(), 100.0);
button->SetValue(Button::HeightProperty(), 50.0);
button->SetValue(Button::ContentProperty(), std::string("Click Me"));

// 获取值
double w = button->GetValue<double>(Button::WidthProperty());
std::cout << "Width: " << w << std::endl;

// 检查值来源
auto source = button->GetValueSource(Button::WidthProperty());
if (source == ValueSource::Local) {
    std::cout << "Width is locally set" << std::endl;
}

// 清除值
button->ClearValue(Button::WidthProperty());
```

### 示例 2：DataContext 继承

```cpp
class ViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Title);
    FK_PROPERTY(int, Count);
};

auto vm = std::make_shared<ViewModel>();
vm->SetTitle("My App");
vm->SetCount(42);

auto window = std::make_shared<Window>();
window->SetDataContext(vm);

// 子元素自动继承 DataContext
auto titleText = textBlock();
titleText->SetBinding(
    TextBlock::TextProperty(),
    bind("Title")  // 从 DataContext 获取
);

auto countText = textBlock();
countText->SetBinding(
    TextBlock::TextProperty(),
    bind("Count")
);

window->AddChild(titleText);
window->AddChild(countText);

// titleText 和 countText 自动获得 vm 作为 DataContext
```

### 示例 3：逻辑树遍历

```cpp
auto root = stackPanel();
root->SetElementName("Root");

auto child1 = button();
child1->SetElementName("Button1");

auto child2 = textBlock();
child2->SetElementName("Text1");

root->AddLogicalChild(child1.get());
root->AddLogicalChild(child2.get());
child1->SetLogicalParent(root.get());
child2->SetLogicalParent(root.get());

// 遍历子元素
for (auto* child : root->GetLogicalChildren()) {
    std::cout << "Child: " << child->GetElementName() << std::endl;
}

// 按名称查找
if (auto found = root->FindElementByName("Button1")) {
    std::cout << "Found Button1" << std::endl;
}
```

### 示例 4：监听属性变化

```cpp
auto slider = std::make_shared<Slider>();

slider->PropertyChanged.Subscribe([](
    const DependencyProperty& prop,
    const std::any& oldVal,
    const std::any& newVal,
    ValueSource oldSrc,
    ValueSource newSrc
) {
    if (prop == Slider::ValueProperty()) {
        double oldV = std::any_cast<double>(oldVal);
        double newV = std::any_cast<double>(newVal);
        std::cout << "Value changed from " << oldV << " to " << newV << std::endl;
    }
});

slider->SetValue(Slider::ValueProperty(), 50.0);
// 输出：Value changed from 0 to 50
```

### 示例 5：绑定管理

```cpp
auto text = textBlock();

// 设置绑定
text->SetBinding(
    TextBlock::TextProperty(),
    bind("Label").Source(vm)
);

// 检查是否有绑定
if (auto binding = text->GetBinding(TextBlock::TextProperty())) {
    std::cout << "Text is bound" << std::endl;
    
    // 手动更新
    binding->UpdateTarget();
}

// 检查值来源
auto source = text->GetValueSource(TextBlock::TextProperty());
if (source == ValueSource::Binding) {
    std::cout << "Value comes from binding" << std::endl;
}

// 清除绑定
text->ClearBinding(TextBlock::TextProperty());
```

### 示例 6：自定义 DependencyObject

```cpp
class CustomControl : public Control<CustomControl> {
public:
    // 自动支持所有 DependencyObject 功能
    
    void DoSomething() {
        // 访问依赖属性
        auto width = GetValue<double>(WidthProperty());
        
        // 检查绑定
        if (auto binding = GetBinding(ContentProperty())) {
            binding->UpdateTarget();
        }
        
        // 访问 DataContext
        if (HasDataContext()) {
            auto ctx = GetDataContext();
            // 使用 DataContext
        }
    }
};
```

## 最佳实践

### ✅ 推荐做法

**1. 优先使用类型安全的泛型方法**
```cpp
// ✅ 类型安全
double w = element->GetValue<double>(WidthProperty());

// ❌ 需要手动转换
auto value = element->GetValue(WidthProperty());
double w = std::any_cast<double>(value);
```

**2. 利用 DataContext 继承**
```cpp
// ✅ 设置根 DataContext，子元素自动继承
root->SetDataContext(vm);

// ❌ 为每个子元素单独设置
child1->SetDataContext(vm);
child2->SetDataContext(vm);
```

**3. 使用 ElementName 而非指针**
```cpp
// ✅ 松耦合
slider->SetElementName("Volume");
label->SetBinding(TextProperty(), bind("Value").ElementName("Volume"));

// ❌ 紧耦合
label->SetBinding(TextProperty(), bind("Value").Source(slider));
```

**4. 清理时解除绑定**
```cpp
~MyControl() {
    ClearBinding(SomeProperty());
}
```

### ❌ 避免的做法

**1. 混用 SetValue 和绑定（绑定会被覆盖）**
```cpp
// ❌ 绑定后设置值会清除绑定
element->SetBinding(Property(), binding);
element->SetValue(Property(), value);  // 绑定失效！
```

**2. 忘记设置逻辑树关系**
```cpp
// ❌ 子元素无法继承 DataContext
parent->AddChild(child);  // 仅视觉树
// 应该同时设置逻辑树：
parent->AddLogicalChild(child.get());
child->SetLogicalParent(parent.get());
```

**3. 在属性回调中修改同一属性（递归）**
```cpp
// ❌ 递归调用
element->PropertyChanged.Subscribe([element](auto& prop, ...) {
    if (prop == SomeProperty()) {
        element->SetValue(SomeProperty(), newValue);  // 递归！
    }
});
```

## 常见问题

**Q: DependencyObject 和普通对象有什么区别？**

A: `DependencyObject` 支持依赖属性、数据绑定、属性继承等高级特性，而普通对象只有普通成员变量。

**Q: 何时使用 SetValue vs SetBinding？**

A: `SetValue` 设置静态值，`SetBinding` 设置动态绑定。绑定会自动响应数据源变化。

**Q: DataContext 如何继承？**

A: 子元素如果没有显式设置 DataContext，会自动使用父元素的 DataContext。

**Q: 如何检查属性是否被绑定？**

A: 使用 `GetValueSource` 或 `GetBinding`：
```cpp
if (GetValueSource(Property()) == ValueSource::Binding) { /*...*/ }
// 或
if (GetBinding(Property()) != nullptr) { /*...*/ }
```

**Q: ClearValue 和 SetValue(nullptr) 有什么区别？**

A: `ClearValue` 移除本地值，恢复默认/继承值；`SetValue(nullptr)` 是设置值为 `nullptr`。

## 性能考虑

- **属性访问**：O(log n)（内部使用 map）
- **绑定开销**：每个绑定 ~256 字节
- **事件订阅**：每个订阅 ~64 字节

**优化建议：**
- 避免过多属性变化通知
- 只在需要时订阅事件
- 合理使用 OneTime 绑定模式

## 相关类型

- **DependencyProperty** - 依赖属性定义
- **Binding** - 数据绑定
- **BindingExpression** - 绑定表达式
- **ObservableObject** - ViewModel 基类

## 总结

`DependencyObject` 是属性系统基础，提供：

✅ 依赖属性 GetValue/SetValue  
✅ 数据绑定支持  
✅ DataContext 继承  
✅ 逻辑树管理  
✅ 属性变化通知  
✅ ElementName 支持

所有 UI 元素都派生自 DependencyObject。
