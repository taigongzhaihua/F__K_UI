# Padding 属性实现完成

## 实现内容

在 `FrameworkElement` 基类中添加了 Padding 属性，使所有派生类（包括 Panel 和 Control）都可以使用 Padding。

## 修改的文件

### 1. `include/fk/ui/FrameworkElement.h`

**添加的内容：**

- **依赖属性声明**：
  ```cpp
  static const binding::DependencyProperty& PaddingProperty();
  ```

- **访问器方法**：
  ```cpp
  void SetPadding(const Thickness& value);
  Thickness GetPadding() const;
  Derived* Padding(const Thickness& value);
  Derived* Padding(float uniform);
  Derived* Padding(float left, float top, float right, float bottom);
  Thickness Padding() const;
  ```

- **MeasureCore 修改**：
  ```cpp
  // 减去 Margin 和 Padding
  float availWidth = availableSize.width 
                   - margin.left - margin.right 
                   - padding.left - padding.right;
  
  // ... MeasureOverride ...
  
  // 加上 Padding（但不加 Margin）
  desiredSize.width += padding.left + padding.right;
  desiredSize.height += padding.top + padding.bottom;
  ```

- **ArrangeCore 修改**：
  ```cpp
  // 减去 Margin 和 Padding
  float arrangeWidth = finalRect.width 
                     - margin.left - margin.right
                     - padding.left - padding.right;
  
  // desiredSize 已经包含 Padding，需要减去
  float desiredWidth = desiredSize.width 
                     - padding.left - padding.right;
  ```

### 2. `src/ui/FrameworkElement.cpp`

**添加的内容：**

```cpp
template<typename Derived>
const binding::DependencyProperty& FrameworkElement<Derived>::PaddingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Padding",
        typeid(Thickness),
        typeid(FrameworkElement<Derived>),
        {Thickness(0)}
    );
    return property;
}
```

## 设计原理

### Margin vs Padding 的处理

```
┌─────────────────────────────────────────┐
│  ← Margin (父容器处理，不含在 desiredSize)│
│ ┌─────────────────────────────────────┐ │
│ │ ← Padding (自己处理，含在 desiredSize)│ │
│ │ ┌─────────────────────────────────┐ │ │
│ │ │                                 │ │ │
│ │ │   Content / Children            │ │ │
│ │ │   (MeasureOverride 处理)        │ │ │
│ │ │                                 │ │ │
│ │ └─────────────────────────────────┘ │ │
│ │           Padding →                 │ │
│ └─────────────────────────────────────┘ │
│             Margin →                    │
└─────────────────────────────────────────┘

desiredSize = Content + Padding (不含 Margin)
```

### 布局流程

**Measure 阶段：**

1. `MeasureCore` 减去 Margin 和 Padding
2. 传给 `MeasureOverride` 处理内容
3. `MeasureCore` 将 Padding 加到 desiredSize
4. **不加 Margin**（由父容器处理）

**Arrange 阶段：**

1. `ArrangeCore` 减去 Margin 和 Padding
2. 应用对齐方式
3. 传给 `ArrangeOverride` 处理内容
4. 设置 renderSize

## 测试结果

### 测试代码

```cpp
auto* stackPanel = new StackPanel();
stackPanel->Padding(15);  // StackPanel 现在有 Padding
stackPanel->Margin(5);

auto* text = new TextBlock();
text->Margin(10);
text->Padding(5);  // TextBlock 现在也有 Padding
```

### 测试输出

```
StackPanel:
  Padding: 15
  Margin: 5

Text2:
  Margin: 10
  Padding: 5
  DesiredSize: (166, 34)  ← 包含 Padding (5+5=10)

✅ desiredSize 包含 Padding: 是
✅ desiredSize 不含 Margin: 是
```

## 使用示例

### StackPanel 使用 Padding

```cpp
auto* panel = new StackPanel();
panel->Padding(20);  // 内边距 20px
panel->Margin(10);   // 外边距 10px

// 子元素会从 Padding 区域内开始布局
panel->Children({child1, child2});
```

### TextBlock 使用 Padding

```cpp
auto* text = new TextBlock();
text->Text("Hello")
    ->FontSize(24)
    ->Padding(10)  // 文字到边缘 10px
    ->Margin(5);   // 元素外边距 5px
```

### Border 使用 Padding

```cpp
auto* border = new Border();
border->Padding(15)        // 边框到内容 15px
      ->Margin(20)         // 边框外边距 20px
      ->BorderThickness(Thickness(2));
```

## 优点

1. **统一性**：所有元素都有 Padding 和 Margin
2. **对称性**：Padding 和 Margin 在同一层级
3. **灵活性**：容器和控件都可以使用
4. **兼容性**：符合 WPF/CSS 的设计模式
5. **简洁性**：子类不需要重复实现

## 影响范围

### 受益的类

- ✅ **Panel** 及其子类（StackPanel, Grid 等）现在都有 Padding
- ✅ **TextBlock** 现在可以有 Padding
- ✅ **Image** 现在可以有 Padding
- ✅ 所有 FrameworkElement 的派生类

### 不受影响的类

- **Control** - 已有 Padding，但基类的实现更统一
- **Border** - 已有 Padding，但基类的实现更统一

## 向后兼容性

- ✅ 完全向后兼容
- ✅ 默认 Padding = 0，不影响现有代码
- ✅ Control 和 Border 的 Padding 继续工作

## 总结

Padding 属性已成功在 FrameworkElement 层实现，所有元素现在都可以使用 Padding 来控制内边距。

**布局公式：**
```
desiredSize = Content + Padding
父容器处理 Margin
```

这完善了布局系统，使得 Margin（外边距）和 Padding（内边距）的职责更加清晰，符合标准 UI 框架的设计模式。
