# Panel

## 概览

**目的**：多子元素容器的基类

**命名空间**：`fk::ui`

**继承**：`FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/Panel.h`

**模板参数**：CRTP 模式，`Panel<Derived>`

## 描述

`Panel` 是所有布局容器的基类，可以包含多个子元素。派生类重写布局方法来实现特定的布局逻辑。

## 公共接口

### 子元素管理

#### AddChild / RemoveChild
```cpp
template<typename T>
std::shared_ptr<T> AddChild();

void AddChild(std::shared_ptr<UIElement> child);
void RemoveChild(std::shared_ptr<UIElement> child);
void ClearChildren();
```

管理面板的子元素。

**示例**：
```cpp
auto panel = std::make_shared<StackPanel>();

// 添加子元素
auto button = panel->AddChild<Button>();
button->Content("按钮");

// 移除子元素
panel->RemoveChild(button);
```

#### GetChildren
```cpp
const std::vector<std::shared_ptr<UIElement>>& GetChildren() const;
```

获取所有子元素。

### 背景

#### Background
```cpp
static const DependencyProperty& BackgroundProperty();
Derived* Background(const Color& color);
```

设置面板背景色。

## 布局重写

派生类应重写以下方法：

```cpp
virtual Size MeasureOverride(const Size& availableSize);
virtual Size ArrangeOverride(const Size& finalSize);
```

## 使用示例

### 基本用法
```cpp
auto panel = std::make_shared<MyPanel>();
panel->AddChild<Button>()->Content("按钮1");
panel->AddChild<Button>()->Content("按钮2");
panel->AddChild<TextBlock>()->Text("文本");
```

## 相关类

- [StackPanel](StackPanel.md) - 堆栈布局
- [Grid](Grid.md) - 网格布局
- [FrameworkElement](FrameworkElement.md) - 基类

## 另请参阅

- [设计文档](../../Design/UI/Panel.md)
