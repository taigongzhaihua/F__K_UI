# FrameworkElement

## 概览

**目的**：布局感知的UI元素，提供对齐、边距和数据上下文

**命名空间**：`fk::ui`

**继承**：`UIElement` → `Visual` → `DependencyObject`

**头文件**：`fk/ui/FrameworkElement.h`

**模板参数**：CRTP 模式，`FrameworkElement<Derived>`

## 描述

`FrameworkElement` 是一个模板基类，为UI元素提供布局系统支持、对齐、边距、数据上下文等功能。使用 CRTP 模式实现类型安全的方法链。

## 公共接口

### 布局属性

#### Width / Height / MinWidth / MaxWidth / MinHeight / MaxHeight
```cpp
static const DependencyProperty& WidthProperty();
static const DependencyProperty& HeightProperty();
static const DependencyProperty& MinWidthProperty();
static const DependencyProperty& MaxWidthProperty();
static const DependencyProperty& MinHeightProperty();
static const DependencyProperty& MaxHeightProperty();
```

控制元素的大小约束。

**示例**：
```cpp
element->Width(200)->Height(100)
       ->MinWidth(100)->MaxWidth(400);
```

### 对齐和边距

#### HorizontalAlignment / VerticalAlignment
```cpp
static const DependencyProperty& HorizontalAlignmentProperty();
static const DependencyProperty& VerticalAlignmentProperty();

Derived* HorizontalAlignment(HorizontalAlignment alignment);
Derived* VerticalAlignment(VerticalAlignment alignment);
```

设置元素在父容器中的对齐方式。

**示例**：
```cpp
element->HorizontalAlignment(HorizontalAlignment::Center)
       ->VerticalAlignment(VerticalAlignment::Top);
```

#### Margin / Padding
```cpp
static const DependencyProperty& MarginProperty();
static const DependencyProperty& PaddingProperty();

Derived* Margin(const Thickness& margin);
Derived* Padding(const Thickness& padding);
```

设置外边距和内边距。

**示例**：
```cpp
element->Margin(Thickness(10))
       ->Padding(Thickness(5));
```

### 数据上下文

#### DataContext
```cpp
static const DependencyProperty& DataContextProperty();
```

数据绑定的上下文对象，沿逻辑树继承。

**示例**：
```cpp
// 设置数据上下文
element->SetValue(FrameworkElement::DataContextProperty(), viewModel);

// 子元素自动继承
child->SetValue(TextBlock::TextProperty(), Binding("PropertyName"));
```

### 布局重写

#### MeasureOverride / ArrangeOverride
```cpp
virtual Size MeasureOverride(const Size& availableSize);
virtual Size ArrangeOverride(const Size& finalSize);
```

派生类重写这些方法以实现自定义布局逻辑。

## 使用示例

### 完整布局配置
```cpp
auto element = std::make_shared<MyElement>();
element->Width(300)
       ->Height(200)
       ->MinWidth(100)
       ->HorizontalAlignment(HorizontalAlignment::Center)
       ->VerticalAlignment(VerticalAlignment::Center)
       ->Margin(Thickness(10))
       ->Padding(Thickness(5));
```

### 数据绑定
```cpp
class ViewModel : public ObservableObject {
    PROPERTY(std::string, Title, "标题")
};

auto viewModel = std::make_shared<ViewModel>();
element->SetValue(FrameworkElement::DataContextProperty(), viewModel);
```

## 相关类

- [UIElement](UIElement.md) - 基类
- [Control](Control.md) - 派生类
- [Panel](Panel.md) - 派生类

## 另请参阅

- [设计文档](../../Design/UI/FrameworkElement.md)
