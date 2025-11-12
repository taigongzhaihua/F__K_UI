# Visual

## 概览

**目的**：视觉树系统的基类，所有可见UI元素的根基类

**命名空间**：`fk::ui`

**继承**：`DependencyObject`

**头文件**：`fk/ui/Visual.h`

## 描述

`Visual` 是所有可视化元素的基类，提供视觉树管理、渲染支持和基本的可视化功能。它管理父子关系、不透明度、可见性和变换。

## 公共接口

### 视觉树管理

#### AddChild / RemoveChild
```cpp
void AddChild(Visual* child);
void RemoveChild(Visual* child);
void ClearChildren();
```

管理视觉树的子元素。

**示例**：
```cpp
auto parent = std::make_shared<Visual>();
auto child = std::make_shared<Visual>();
parent->AddChild(child.get());
```

#### GetParent / GetChildren
```cpp
Visual* GetParent() const;
const std::vector<Visual*>& GetChildren() const;
```

获取父元素和子元素列表。

### 可见性和不透明度

#### Opacity / IsVisible
```cpp
static const DependencyProperty& OpacityProperty();
static const DependencyProperty& VisibilityProperty();
```

控制元素的不透明度和可见性。

**示例**：
```cpp
visual->SetValue(Visual::OpacityProperty(), 0.5);  // 半透明
visual->SetValue(Visual::VisibilityProperty(), Visibility::Collapsed);
```

### 变换

#### RenderTransform
```cpp
static const DependencyProperty& RenderTransformProperty();
void SetRenderTransform(Transform* transform);
```

设置渲染变换（旋转、缩放、平移等）。

**示例**：
```cpp
auto transform = std::make_shared<RotateTransform>(45.0f);
visual->SetRenderTransform(transform.get());
```

### 渲染

#### OnRender
```cpp
virtual void OnRender(const RenderContext& context);
```

重写此方法以提供自定义渲染逻辑。

## 使用示例

### 基本视觉树
```cpp
auto root = std::make_shared<Visual>();
auto child1 = std::make_shared<Visual>();
auto child2 = std::make_shared<Visual>();

root->AddChild(child1.get());
root->AddChild(child2.get());
```

### 设置不透明度
```cpp
visual->SetValue(Visual::OpacityProperty(), 0.8);
```

### 应用变换
```cpp
auto scaleTransform = std::make_shared<ScaleTransform>(2.0f, 2.0f);
visual->SetRenderTransform(scaleTransform.get());
```

## 相关类

- [UIElement](UIElement.md) - 添加输入支持的派生类
- [DependencyObject](../Binding/DependencyObject.md) - 基类
- [Transform](Transform.md) - 变换系统

## 另请参阅

- [设计文档](../../Design/UI/Visual.md)
- [架构概览](../../Architecture.md)
