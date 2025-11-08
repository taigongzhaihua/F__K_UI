# UI DependencyObject

## 概述

**UI DependencyObject** 是 UI 框架中的核心基类，继承自 binding 模块的 `DependencyObject` 和 `DispatcherObject`。它提供了以下功能：

- **逻辑树管理** - UI 元素的父子关系维护
- **调度程序绑定** - UI 线程上下文管理
- **依赖属性** - 数据绑定、样式、继承的支持
- **数据上下文** - MVVM 数据绑定的基础

UI DependencyObject 是所有 UI 元素（Button、TextBox、Panel 等）的基础。

## 继承关系

```
DependencyObject (binding)
    ↑
    │
    ├── DispatcherObject (UI 线程)
    │
UI DependencyObject
    ↑
    ├── FrameworkElement
        ├── Control
            ├── Button
            ├── TextBox
            ├── ...
        ├── Panel
            ├── Grid
            ├── StackPanel
            ├── ...
        ├── Window
        ├── ScrollViewer
        └── ...
```

## 核心 API

### 构造函数

```cpp
// 默认构造 - 创建默认调度程序
DependencyObject();

// 指定调度程序 - 在特定 UI 线程上创建
explicit DependencyObject(std::shared_ptr<core::Dispatcher> dispatcher);
```

### 逻辑树管理

```cpp
// 附加到逻辑树 - 绑定到 UI 线程
void AttachToLogicalTree(std::shared_ptr<core::Dispatcher> dispatcher = {});

// 附加到逻辑父元素
void AttachToLogicalParent(DependencyObject* parent);

// 从逻辑树分离
void DetachFromLogicalTree();

// 检查是否已附加到逻辑树
bool IsAttachedToLogicalTree() const noexcept;
```

### 逻辑子元素管理

```cpp
// 添加逻辑子元素
void AddLogicalChild(DependencyObject* child);

// 移除逻辑子元素
void RemoveLogicalChild(DependencyObject* child);

// 获取逻辑子元素（继承自 binding::DependencyObject）
const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;

// 获取逻辑父元素（继承自 binding::DependencyObject）
DependencyObject* GetLogicalParent() const noexcept;
```

### 依赖属性操作

```cpp
// 获取属性值（继承自 binding::DependencyObject）
const std::any& GetValue(const DependencyProperty& property) const;
template<typename T>
T GetValue(const DependencyProperty& property) const;

// 设置属性值
void SetValue(const DependencyProperty& property, std::any value);
template<typename T>
void SetValue(const DependencyProperty& property, T&& value);

// 清除属性值
void ClearValue(const DependencyProperty& property);

// 获取值来源
ValueSource GetValueSource(const DependencyProperty& property) const;
```

### 数据绑定

```cpp
// 设置绑定
void SetBinding(const DependencyProperty& property, Binding binding);

// 清除绑定
void ClearBinding(const DependencyProperty& property);

// 获取绑定表达式
std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
```

### 数据上下文

```cpp
// 设置数据上下文（通常是 ViewModel）
void SetDataContext(std::any value);
template<typename T>
void SetDataContext(T&& value);

// 清除数据上下文
void ClearDataContext();

// 获取数据上下文
const std::any& GetDataContext() const noexcept;

// 检查是否有数据上下文
bool HasDataContext() const noexcept;

// 设置数据上下文继承来源
void SetDataContextParent(DependencyObject* parent);
```

### 元素查找

```cpp
// 设置元素名称（用于查找）
void SetElementName(std::string name);

// 获取元素名称
const std::string& GetElementName() const noexcept;

// 按名称查找子元素
DependencyObject* FindElementByName(std::string_view name);
const DependencyObject* FindElementByName(std::string_view name) const;
```

### 事件

```cpp
// 元素已附加到逻辑树
core::Event<DependencyObject&> AttachedToLogicalTree;

// 元素已从逻辑树分离
core::Event<DependencyObject&> DetachedFromLogicalTree;

// 属性值已更改（继承自 binding::DependencyObject）
PropertyChangedEvent PropertyChanged;

// 绑定已更改（继承自 binding::DependencyObject）
BindingChangedEvent BindingChanged;

// 数据上下文已更改（继承自 binding::DependencyObject）
DataContextChangedEvent DataContextChanged;
```

### 调度程序

```cpp
// 设置调度程序（UI 线程）
void SetDispatcher(std::shared_ptr<core::Dispatcher> dispatcher);

// 获取调度程序
std::shared_ptr<core::Dispatcher> GetDispatcher() const;
core::Dispatcher* Dispatcher() const noexcept;

// 检查当前线程是否具有访问权限
bool HasThreadAccess() const;

// 验证访问权限（失败时抛出异常）
void VerifyAccess() const;
```

## 常见模式

### 创建和初始化

```cpp
// 创建 UI 元素（例如 Button）
auto button = std::make_shared<Button>();

// 设置数据上下文（通常在窗口级别）
auto viewModel = std::make_shared<MyViewModel>();
button->SetDataContext(viewModel);

// 使用数据绑定
Binding binding;
binding.SetSource(viewModel);
binding.SetPath("ButtonText");
button->SetBinding(Button::ContentProperty, binding);
```

### 逻辑树组织

```cpp
// 创建面板和子元素
auto panel = std::make_shared<StackPanel>();
auto button = std::make_shared<Button>();
auto textBox = std::make_shared<TextBox>();

// 建立逻辑树关系
panel->AddLogicalChild(button.get());
panel->AddLogicalChild(textBox.get());

// 附加到主 UI 树
button->AttachToLogicalParent(panel.get());
textBox->AttachToLogicalParent(panel.get());
```

### 属性值优先级

```cpp
// 属性值按以下优先级应用：
// 1. Local 值（最高优先级）
element->SetValue(MyProperty, localValue);

// 2. Binding 值
Binding binding;
element->SetBinding(MyProperty, binding);

// 3. Style 值（如果使用样式系统）
// 4. Inherited 值（从父元素继承）
// 5. Default 值（最低优先级）

// 获取当前值来源
ValueSource source = element->GetValueSource(MyProperty);
switch (source) {
    case ValueSource::Local:
        // 本地设置的值
        break;
    case ValueSource::Binding:
        // 绑定的值
        break;
    // ...
}
```

### 元素查找

```cpp
// 设置元素名称
textBox->SetElementName("searchInput");
button->SetElementName("searchButton");

// 在树中查找元素
auto found = root->FindElementByName("searchInput");
if (found) {
    auto textBox = dynamic_cast<TextBox*>(found);
    std::string searchText = textBox->GetValue<std::string>(TextBox::TextProperty);
}
```

### 线程安全访问

```cpp
// 在创建 UI 元素时指定调度程序
auto dispatcher = std::make_shared<core::Dispatcher>();
auto element = std::make_shared<Button>(dispatcher);

// 检查当前线程是否可以访问该元素
if (element->HasThreadAccess()) {
    // 直接修改属性
    element->SetValue(Button::ContentProperty, "Click me");
} else {
    // 在 UI 线程上执行操作
    dispatcher->Post([element]() {
        element->SetValue(Button::ContentProperty, "Click me");
    });
}

// 验证访问权限（失败时抛出异常）
try {
    element->VerifyAccess();
    element->SetValue(Button::ContentProperty, "Click me");
} catch (const std::runtime_error& e) {
    std::cerr << "跨线程访问: " << e.what() << std::endl;
}
```

### MVVM 数据绑定模式

```cpp
// ViewModel
class MyViewModel : public ObservableObject {
public:
    std::string GetButtonText() const { return buttonText_; }
    void SetButtonText(const std::string& text) {
        if (buttonText_ != text) {
            buttonText_ = text;
            RaisePropertyChanged("ButtonText");
        }
    }

private:
    std::string buttonText_;
};

// View
auto viewModel = std::make_shared<MyViewModel>();
auto window = std::make_shared<Window>();

// 设置整个窗口的数据上下文
window->SetDataContext(viewModel);

// 子元素继承数据上下文
auto button = std::make_shared<Button>();
window->AddLogicalChild(button.get());
button->AttachToLogicalParent(window.get());

// 创建绑定
Binding binding;
binding.SetPath("ButtonText");
button->SetBinding(Button::ContentProperty, binding);

// 当 ViewModel 属性更改时，UI 自动更新
viewModel->SetButtonText("New Text");
```

### 监听树的变化

```cpp
class MyPanel : public Panel {
public:
    MyPanel() {
        // 订阅逻辑树事件
        AttachedToLogicalTree += [this](DependencyObject& element) {
            std::cout << "元素已附加到树" << std::endl;
        };

        DetachedFromLogicalTree += [this](DependencyObject& element) {
            std::cout << "元素已从树中移除" << std::endl;
        };
    }

protected:
    void OnAttachedToLogicalTree() override {
        Panel::OnAttachedToLogicalTree();
        std::cout << "面板已附加到树" << std::endl;
    }

    void OnDetachedFromLogicalTree() override {
        Panel::OnDetachedFromLogicalTree();
        std::cout << "面板已从树中移除" << std::endl;
    }
};
```

## 最佳实践

### 1. 数据上下文继承

```cpp
// ✅ 好做法：在根元素设置数据上下文，子元素继承
auto root = std::make_shared<Window>();
auto viewModel = std::make_shared<MyViewModel>();
root->SetDataContext(viewModel);

// 子元素会自动继承数据上下文
auto panel = std::make_shared<StackPanel>();
root->AddLogicalChild(panel.get());
// panel->GetDataContext() 会返回 viewModel
```

### 2. 属性值优先级

```cpp
// ✅ 在绑定失败时提供本地默认值
element->SetValue(MyProperty, defaultValue);  // 本地值
Binding binding;
element->SetBinding(MyProperty, binding);     // 绑定值
// 如果绑定失败，会使用本地值

// ❌ 避免：过度使用本地值，可能覆盖绑定值
element->SetValue(MyProperty, newValue);
element->SetBinding(MyProperty, binding);
// 绑定会被本地值覆盖
```

### 3. 线程安全

```cpp
// ✅ 始终在正确的 UI 线程上修改属性
if (!element->HasThreadAccess()) {
    element->GetDispatcher()->Post([element]() {
        element->SetValue(MyProperty, value);
    });
} else {
    element->SetValue(MyProperty, value);
}

// ❌ 避免：从其他线程直接访问 UI 元素
std::thread([element]() {
    element->SetValue(MyProperty, value);  // 可能崩溃
}).detach();
```

### 4. 元素查找

```cpp
// ✅ 使用类型安全的查找
auto found = root->FindElementByName("myButton");
if (auto button = dynamic_cast<Button*>(found)) {
    // 安全地使用 button
}

// ✅ 缓存常用元素
class MyWindow : public Window {
    std::shared_ptr<Button> button_;

    void Initialize() {
        button_ = std::dynamic_pointer_cast<Button>(
            FindElementByName("myButton"));
    }
};
```

### 5. 清理资源

```cpp
// ✅ 从树中移除时清理
void RemoveElement(DependencyObject* element) {
    element->DetachFromLogicalTree();
    element->ClearBinding(SomeProperty);
    element->ClearDataContext();
}

// ❌ 避免：遗留绑定和事件监听器
void BadRemoveElement(DependencyObject* element) {
    // 绑定和事件仍然活跃，可能导致内存泄漏
}
```

## 常见问题

### Q: UI DependencyObject 与 binding::DependencyObject 有什么区别？

**A:** 
- **binding::DependencyObject** - 提供依赖属性、绑定、数据上下文功能（与线程无关）
- **UI DependencyObject** - 继承 binding::DependencyObject，增加了逻辑树管理和调度程序功能
- UI DependencyObject 通常用于 UI 框架，binding::DependencyObject 可用于其他需要数据绑定的应用

### Q: 如何处理跨线程访问？

**A:** 
```cpp
// 检查访问权限
if (!element->HasThreadAccess()) {
    element->GetDispatcher()->Post([element, value]() {
        element->SetValue(Property, value);
    });
}
```

### Q: 数据上下文如何继承？

**A:** 数据上下文从父元素自动继承，除非子元素明确设置了自己的数据上下文：
```cpp
parent->SetDataContext(viewModel1);
child->SetDataContext(viewModel2);  // 覆盖继承的值

// 获取数据上下文时会返回 viewModel2
```

### Q: 如何监听属性变化？

**A:**
```cpp
// 方式1：订阅属性变化事件
element->PropertyChanged += [](const DependencyProperty& prop, 
                               const std::any& oldValue, 
                               const std::any& newValue,
                               ValueSource oldSource,
                               ValueSource newSource) {
    std::cout << "属性已更改" << std::endl;
};

// 方式2：使用数据绑定监听 ViewModel 属性
```

### Q: 逻辑树和可视树有什么区别？

**A:**
- **逻辑树** - 由 DependencyObject 维护，表示 UI 元素的逻辑层次结构
- **可视树** - 由渲染系统维护，表示元素如何呈现到屏幕

## 相关文档

- [binding::DependencyObject](../Binding/DependencyObject.md) - 基类，提供依赖属性支持
- [DispatcherObject](DispatcherObject.md) - 调度程序基类，提供线程安全
- [FrameworkElement](FrameworkElement.md) - UI 元素的抽象基类
- [Binding](../Binding/Binding.md) - 数据绑定系统
- [ObservableObject](../Binding/ObservableObject.md) - MVVM ViewModel 基类
- [DependencyProperty](../Binding/DependencyProperty.md) - 依赖属性系统
