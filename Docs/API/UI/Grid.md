# Grid API 文档

## 概述

`Grid` 是一个强大的布局容器，支持行列定义、跨度、自适应尺寸（Pixel、Auto、Star）。类似 WPF/XAML 的 Grid，提供灵活的二维布局能力。

**继承链：** `UIElement` → `FrameworkElement` → `PanelBase` → `Grid`

**命名空间：** `fk::ui`

---

## 核心概念

### GridLength 与尺寸单位

```cpp
struct GridLength {
    float value;
    GridUnitType unit;  // Auto, Pixel, Star
};
```

- **Pixel(float)：** 固定像素数，如 `GridLength::Pixel(100)` → 100 像素
- **Auto()：** 自适应，自动根据内容调整大小
- **Star(float weight = 1.0)：** 相对宽度，等比分配剩余空间
  - 如果有 3 列 `Star(1), Star(2), Star(1)`，第二列占剩余空间的 50%

**便捷创建：**
```cpp
GridLength::Pixel(100)    // 100 像素
GridLength::Auto()        // 自适应
GridLength::Star()        // 权重 1
GridLength::Star(2.0f)    // 权重 2
```

### RowDefinition 与 ColumnDefinition

```cpp
struct RowDefinition {
    GridLength height;
    float minHeight;  // 默认 0
    float maxHeight;  // 默认 ∞
};

struct ColumnDefinition {
    GridLength width;
    float minWidth;   // 默认 0
    float maxWidth;   // 默认 ∞
};
```

---

## 核心属性

### RowDefinitions
- **类型：** `RowDefinitionCollection` (std::vector\<RowDefinition\>)
- **默认值：** 空（自动根据子元素 Row 属性推导行数）
- **说明：** 定义所有行的高度规则

**用法：**
```cpp
grid->RowDefinitions({
    RowDefinition{GridLength::Auto()},        // 行 0：自适应
    RowDefinition{GridLength::Pixel(50.0f)},  // 行 1：50 像素
    RowDefinition{GridLength::Star(1.0f)}     // 行 2：占剩余空间
});
```

### ColumnDefinitions
- **类型：** `ColumnDefinitionCollection` (std::vector\<ColumnDefinition\>)
- **默认值：** 空（自动根据子元素 Column 属性推导列数）
- **说明：** 定义所有列的宽度规则

**用法：**
```cpp
grid->ColumnDefinitions({
    ColumnDefinition{GridLength::Pixel(100.0f)},
    ColumnDefinition{GridLength::Star(1.0f)},
    ColumnDefinition{GridLength::Star(2.0f)}
});
```

---

## 附加属性（Attached Properties）

这些属性设置在 Grid 的子元素上，告诉 Grid 该元素的位置和大小。

### Row
- **类型：** `int`
- **默认值：** `0`
- **说明：** 子元素所在的行索引（从 0 开始）

**用法：**
```cpp
// 静态方法设置（传统方式）
Grid::SetRow(element.get(), 1);

// 运算符语法设置（推荐）
element | cell(1, 0);
```

### Column
- **类型：** `int`
- **默认值：** `0`
- **说明：** 子元素所在的列索引

### RowSpan
- **类型：** `int`
- **默认值：** `1`
- **说明：** 子元素跨越的行数

**示例：** `RowSpan=2` 表示元素占据 2 行

### ColumnSpan
- **类型：** `int`
- **默认值：** `1`
- **说明：** 子元素跨越的列数

**示例：** `ColumnSpan=3` 表示元素占据 3 列

---

## 设置子元素位置的方法

### 方式 1：运算符语法（推荐）✨

```cpp
grid->Children({
    ui::button()->Content("按钮 A") | cell(0, 0),
    ui::button()->Content("按钮 B") | cell(0, 1, 1, 2),  // 行 0，列 1-2
    ui::textBlock()->Text("标签") | cell(1, 0, 2, 1)     // 行 1-2，列 0
});
```

**语法说明：**
- `cell(row, column)` - 简单定位
- `cell(row, column, rowSpan, columnSpan)` - 完整定位
- `element | cell(...)` - 使用运算符将定位信息应用到元素

### 方式 2：静态方法设置

```cpp
auto btn = ui::button()->Content("保存");
Grid::SetRow(btn.get(), 1);
Grid::SetColumn(btn.get(), 2);
Grid::SetRowSpan(btn.get(), 1);
Grid::SetColumnSpan(btn.get(), 2);

grid->AddChild(btn);
```

### 方式 3：链式调用

```cpp
grid->Children({
    ui::button()->Content("A"),  // 默认 (0, 0)
    ui::button()->Content("B")   // 需要用运算符指定位置
});
```

---

## 静态查询方法

### static int GetRow(const UIElement* element)
- **返回值：** 元素的行索引，默认 0

### static int GetColumn(const UIElement* element)
- **返回值：** 元素的列索引，默认 0

### static int GetRowSpan(const UIElement* element)
- **返回值：** 元素的行跨度，默认 1

### static int GetColumnSpan(const UIElement* element)
- **返回值：** 元素的列跨度，默认 1

**用法：**
```cpp
int row = Grid::GetRow(element.get());
int span = Grid::GetRowSpan(element.get());
```

---

## 便捷方法

### Ptr AddRow(RowDefinition row)
- **说明：** 在末尾添加一行定义
- **返回值：** 返回 `this` 便于链式调用

**用法：**
```cpp
grid->AddRow(RowDefinition{GridLength::Auto()})
    ->AddRow(RowDefinition{GridLength::Star()});
```

### Ptr AddColumn(ColumnDefinition column)
- **说明：** 在末尾添加一列定义

### Ptr ClearRows()
- **说明：** 清空所有行定义

### Ptr ClearColumns()
- **说明：** 清空所有列定义

---

## 从 Panel 继承的方法

### Ptr Children(const UIElementCollection& children)
```cpp
grid->Children({elem1, elem2, elem3});
```

### void AddChild(std::shared_ptr\<UIElement\> child)
```cpp
grid->AddChild(element);
```

### std::span\<const std::shared_ptr\<UIElement\>\> GetChildren() const
```cpp
auto children = grid->GetChildren();
for (const auto& child : children) {
    // 处理每个子元素
}
```

---

## 完整使用示例

### 示例 1：简单表单布局

```cpp
auto grid = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Auto()},   // 标签列：自适应
        ColumnDefinition{GridLength::Star()}    // 输入列：占剩余
    })
    ->RowDefinitions({
        RowDefinition{GridLength::Auto()},
        RowDefinition{GridLength::Auto()},
        RowDefinition{GridLength::Auto()}
    })
    ->Children({
        ui::textBlock()->Text("姓名:")       | cell(0, 0),
        ui::textBox()->Width(200)            | cell(0, 1),
        
        ui::textBlock()->Text("邮箱:")       | cell(1, 0),
        ui::textBox()->Width(200)            | cell(1, 1),
        
        ui::button()->Content("保存")        | cell(2, 1),
    });
```

### 示例 2：复杂跨度布局

```cpp
auto grid = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Pixel(100.0f)},
        ColumnDefinition{GridLength::Star(1.0f)},
        ColumnDefinition{GridLength::Star(1.0f)}
    })
    ->RowDefinitions({
        RowDefinition{GridLength::Pixel(50.0f)},
        RowDefinition{GridLength::Star(1.0f)},
        RowDefinition{GridLength::Pixel(40.0f)}
    })
    ->Children({
        // 标题跨全列
        ui::textBlock()
            ->Text("仪表板")
            ->FontSize(20.0f) | cell(0, 0, 1, 3),
        
        // 左侧菜单 (跨 2 行)
        ui::stackPanel() | cell(1, 0, 2, 1),
        
        // 中间内容
        ui::textBlock()->Text("内容区") | cell(1, 1, 1, 2),
        
        // 底部状态栏
        ui::textBlock()->Text("就绪") | cell(2, 1, 1, 2)
    });
```

### 示例 3：响应式栅栏系统

```cpp
auto grid = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()}
    })
    ->Children({
        ui::button()->Content("A") | cell(0, 0),
        ui::button()->Content("B") | cell(0, 1),
        ui::button()->Content("C") | cell(0, 2),
        ui::button()->Content("D") | cell(0, 3)
    });
// 四个按钮平均分配宽度
```

---

## 布局计算流程

### 测量阶段 (Measure)
1. 遍历所有子元素，测量其所需大小
2. 统计每行/列的最小所需高度/宽度
3. 将 Pixel 类型的高度/宽度固定
4. Auto 类型基于内容计算
5. 返回 Grid 的总所需大小（所有行高 + 所有列宽）

### 排列阶段 (Arrange)
1. 将可用空间分配给各行/列
   - Pixel 固定分配
   - Auto 按内容分配
   - Star 按权重比例分配剩余空间
2. 根据子元素的 Row/Column/RowSpan/ColumnSpan 确定其最终矩形区域
3. 调用 `ArrangeChild()` 排列每个子元素

---

## 与数据绑定

### RowDefinitions 绑定

```cpp
grid->RowDefinitions(bind("GridRows"));  // 绑定到 ViewModel 属性
```

### 动态行/列

```cpp
// 在 ViewModel 中修改 GridRows 时，Grid 自动重新布局
viewModel->SetGridRows(newRowDefinitions);
```

---

## 常见问题

### Q1：子元素超出定义的行列如何处理？
**A：** Grid 会自动创建隐式行/列容纳超出的元素。例如，如果只定义 2 行但有元素指定 Row=5，Grid 会自动创建行 2、3、4、5。

### Q2：Star 的权重如何计算？
**A：** 
```
某列宽度 = 剩余空间 × (该列权重 / 所有 Star 列的权重之和)
```

例如：
- Grid 宽 600，Pixel 列占 100，剩余 500
- 3 列 Star(1), Star(2), Star(1)，总权重 4
- 各列宽：125, 250, 125

### Q3：如何左对齐 Star 列的内容？
**A：** 使用 `HorizontalAlignment` 属性：
```cpp
element->HorizontalAlignment(HorizontalAlignment::Left) | cell(0, 1)
```

### Q4：子元素大小超过单元格时？
**A：** 默认会溢出。可设置：
```cpp
element->Width(GridLength::Star())  // 占满单元格
element->HorizontalAlignment(HorizontalAlignment::Stretch)
```

---

## 性能考虑

- **大量子元素：** Grid 的布局计算复杂度为 O(n²)（n 为子元素数），建议 < 1000 个子元素
- **频繁修改行列定义：** 每次修改触发重新测量/排列，考虑批量更新
- **复杂 Star 比例：** 如果权重计算复杂，预先计算可避免每帧重复计算

---

## 另见

- [UIElement.md](UIElement.md) - 基础元素
- [StackPanel.md](StackPanel.md) - 一维线性布局
- [Panel.md](Panel.md) - 容器基类

