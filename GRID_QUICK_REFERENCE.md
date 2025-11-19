# Grid 快速参考指南

## 🚀 快速开始

### 基本用法

```cpp
#include "fk/ui/Grid.h"

// 创建 Grid
auto grid = new Grid();

// 方法 1：逐个添加行列
grid->AddRowDefinition(RowDefinition::Auto());
grid->AddRowDefinition(RowDefinition::Star(2));
grid->AddColumnDefinition(ColumnDefinition::Pixel(200));

// 方法 2：批量添加
grid->RowDefinitions({
    RowDefinition::Auto(),
    RowDefinition::Star(),
    RowDefinition::Pixel(100)
});

// 方法 3：字符串解析（最简洁）
grid->Rows("Auto, *, 100");
grid->Columns("200, 2*, *");
```

---

## 📐 尺寸类型

### Auto - 自适应内容

```cpp
RowDefinition::Auto()        // 根据子元素高度自动调整
ColumnDefinition::Auto()     // 根据子元素宽度自动调整
```

**用途**：标题栏、工具栏、状态栏等内容驱动的区域

### Pixel - 固定像素

```cpp
RowDefinition::Pixel(100)    // 固定 100 像素高
ColumnDefinition::Pixel(200) // 固定 200 像素宽
```

**用途**：边栏、分隔符等固定尺寸区域

### Star - 比例分配

```cpp
RowDefinition::Star()        // 1* (默认)
RowDefinition::Star(2)       // 2* (两倍权重)
ColumnDefinition::Star(3)    // 3* (三倍权重)
```

**用途**：主内容区域、响应式布局

**分配示例**：
```
总空间 600px，定义 "*, 2*, *"
结果：150px, 300px, 150px (比例 1:2:1)
```

---

## 🎯 附加属性

### 方法 1：静态方法

```cpp
auto button = new Button();
Grid::SetRow(button, 1);
Grid::SetColumn(button, 2);
Grid::SetRowSpan(button, 2);
Grid::SetColumnSpan(button, 1);
grid->AddChild(button);
```

### 方法 2：流式语法（推荐）

```cpp
auto button = grid->AddChild<Button>()
    ->Row(1)
    ->Column(2)
    ->RowSpan(2)
    ->ColumnSpan(1);
```

---

## 📏 约束条件

### 设置 Min/Max

```cpp
// 行约束
auto row = RowDefinition::Star(1)
    .MinHeight(100)
    .MaxHeight(500);

// 列约束
auto col = ColumnDefinition::Star(2)
    .MinWidth(200)
    .MaxWidth(800);

grid->AddRowDefinition(row);
grid->AddColumnDefinition(col);
```

### 约束行为

- **MinHeight/MinWidth**：行列最小尺寸（Star 分配时保证）
- **MaxHeight/MaxWidth**：行列最大尺寸（超过则裁剪）
- **Star 分配优先级**：先满足 Min，再按比例分配，最后应用 Max

---

## 🎨 对齐和边距

### 对齐方式

```cpp
auto button = grid->AddChild<Button>()
    ->Row(0)->Column(0)
    ->HorizontalAlignment(HorizontalAlignment::Center)   // 水平居中
    ->VerticalAlignment(VerticalAlignment::Bottom);      // 垂直底部
```

**选项**：
- `HorizontalAlignment`: `Left`, `Center`, `Right`, `Stretch`
- `VerticalAlignment`: `Top`, `Center`, `Bottom`, `Stretch`

### 边距

```cpp
auto button = grid->AddChild<Button>()
    ->Margin(Thickness(10, 20, 10, 20))  // 左、上、右、下
    ->Row(0)->Column(0);
```

---

## 📋 常用布局模式

### 1. 经典三列布局

```cpp
auto grid = new Grid();
grid->Rows("Auto, *, Auto")      // 顶栏、内容、底栏
    ->Columns("200, *, 200");    // 左边栏、主区、右边栏

// 顶栏（跨三列）
auto header = grid->AddChild<Border>()
    ->Row(0)->Column(0)->ColumnSpan(3)
    ->Height(60);

// 左边栏
auto leftSidebar = grid->AddChild<StackPanel>()
    ->Row(1)->Column(0);

// 主内容区
auto content = grid->AddChild<ScrollViewer>()
    ->Row(1)->Column(1);

// 右边栏
auto rightSidebar = grid->AddChild<StackPanel>()
    ->Row(1)->Column(2);

// 底栏（跨三列）
auto footer = grid->AddChild<Border>()
    ->Row(2)->Column(0)->ColumnSpan(3)
    ->Height(30);
```

### 2. 表单布局

```cpp
auto grid = new Grid();
grid->Rows("Auto, Auto, Auto, Auto, *")  // 多行标签+输入
    ->Columns("Auto, *");                // 标签列、输入列

// 用户名
auto label1 = grid->AddChild<TextBlock>()
    ->Row(0)->Column(0)
    ->Text("用户名:")
    ->Margin(Thickness(0, 0, 10, 5));

auto input1 = grid->AddChild<TextBox>()
    ->Row(0)->Column(1)
    ->Margin(Thickness(0, 0, 0, 5));

// 密码
auto label2 = grid->AddChild<TextBlock>()
    ->Row(1)->Column(0)
    ->Text("密码:")
    ->Margin(Thickness(0, 0, 10, 5));

auto input2 = grid->AddChild<TextBox>()
    ->Row(1)->Column(1)
    ->Margin(Thickness(0, 0, 0, 5));

// 按钮区域（跨两列，右对齐）
auto buttonPanel = grid->AddChild<StackPanel>()
    ->Row(4)->Column(0)->ColumnSpan(2)
    ->Orientation(Orientation::Horizontal)
    ->HorizontalAlignment(HorizontalAlignment::Right);
```

### 3. 分割视图

```cpp
auto grid = new Grid();
grid->Rows("*")
    ->Columns("*, 5, *");  // 左区域、分隔符、右区域

// 左侧内容
auto leftPane = grid->AddChild<Border>()
    ->Row(0)->Column(0);

// 分隔符
auto splitter = grid->AddChild<Border>()
    ->Row(0)->Column(1)
    ->Background(0xFFCCCCCC);

// 右侧内容
auto rightPane = grid->AddChild<Border>()
    ->Row(0)->Column(2);
```

### 4. 九宫格布局

```cpp
auto grid = new Grid();
grid->Rows("*, *, *")
    ->Columns("*, *, *");

for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
        auto button = grid->AddChild<Button>()
            ->Row(row)
            ->Column(col)
            ->Content(std::to_string(row * 3 + col + 1));
    }
}
```

### 5. 响应式工具栏

```cpp
auto grid = new Grid();
grid->Rows("Auto")
    ->Columns("Auto, *, Auto");  // 左工具、中间空白、右工具

// 左侧工具
auto leftTools = grid->AddChild<StackPanel>()
    ->Row(0)->Column(0)
    ->Orientation(Orientation::Horizontal);

// 右侧工具
auto rightTools = grid->AddChild<StackPanel>()
    ->Row(0)->Column(2)
    ->Orientation(Orientation::Horizontal);
```

---

## 🔧 高级技巧

### 1. 动态添加行列

```cpp
// 运行时添加新行
for (int i = 0; i < count; i++) {
    grid->AddRowDefinition(RowDefinition::Auto());
    
    auto label = grid->AddChild<TextBlock>()
        ->Row(i)
        ->Text("Item " + std::to_string(i));
}
```

### 2. 嵌套 Grid

```cpp
auto mainGrid = new Grid();
mainGrid->Rows("*, *")->Columns("*, *");

// 在单元格中嵌套另一个 Grid
auto nestedGrid = mainGrid->AddChild<Grid>()
    ->Row(0)->Column(0)
    ->Rows("Auto, *")
    ->Columns("100, *");
```

### 3. 条件布局

```cpp
if (isCompactMode) {
    grid->Rows("Auto, *")        // 垂直堆叠
        ->Columns("*");
} else {
    grid->Rows("*")              // 水平布局
        ->Columns("200, *");
}
```

### 4. 约束优先级

```cpp
// 场景：窗口很小时，确保关键内容可见
auto grid = new Grid();
grid->Rows("Auto, *, Auto")
    ->Columns("*");

// 顶部工具栏：最小高度 40，确保可见
auto toolbar = grid->AddChild<Border>()
    ->Row(0)
    ->Height(60)
    ->MinHeight(40);  // 窗口很小时至少 40

// 内容区：自适应剩余空间
auto content = grid->AddChild<ScrollViewer>()
    ->Row(1);

// 状态栏：固定高度
auto statusBar = grid->AddChild<Border>()
    ->Row(2)
    ->Height(24);
```

---

## ⚠️ 常见错误

### ❌ 忘记设置行列定义

```cpp
auto grid = new Grid();
auto button = grid->AddChild<Button>();  // 错误：没有定义行列
```

**✅ 正确做法**：
```cpp
auto grid = new Grid();
grid->Rows("*")->Columns("*");  // 或者不设置（会自动创建 1* 行列）
auto button = grid->AddChild<Button>();
```

### ❌ 索引越界

```cpp
grid->Rows("*, *");  // 只有 2 行
button->Row(5);      // 错误：索引 5 不存在
```

**✅ 正确做法**：使用索引保护（已内置）或检查行列数

### ❌ Star 空间不足

```cpp
grid->Rows("*, *");  // 每行需要至少一些空间
grid->Measure(Size(0, 0));  // 错误：没有可用空间
```

**✅ 正确做法**：确保容器有足够空间，或使用 Auto/Pixel

---

## 🎯 性能最佳实践

### 1. 缓存友好的布局

```cpp
// ✅ 好：复用相同尺寸测量
grid->Measure(Size(800, 600));
grid->Measure(Size(800, 600));  // 使用缓存，快 80%

// ❌ 差：频繁改变尺寸
for (int i = 0; i < 100; i++) {
    grid->Measure(Size(800 + i, 600));  // 每次都重新计算
}
```

### 2. 避免过多 Auto 行列

```cpp
// ❌ 差：大量 Auto 行（需要测量所有子元素）
for (int i = 0; i < 1000; i++) {
    grid->AddRowDefinition(RowDefinition::Auto());
}

// ✅ 好：使用 Star 或固定高度
grid->AddRowDefinition(RowDefinition::Pixel(40));
```

### 3. 合理使用 Visibility.Collapsed

```cpp
// Collapsed 元素不参与布局计算
button->SetVisibility(Visibility::Collapsed);  // 性能提升
```

---

## 📊 字符串解析语法

| 语法 | 解析结果 | 说明 |
|------|----------|------|
| `"Auto"` | `RowDefinition::Auto()` | 自适应内容 |
| `"*"` | `RowDefinition::Star(1)` | 1 倍 Star |
| `"2*"` | `RowDefinition::Star(2)` | 2 倍 Star |
| `"100"` | `RowDefinition::Pixel(100)` | 100 像素 |
| `"Auto, *, 100"` | 三行：Auto, 1*, 100px | 逗号分隔 |
| `" Auto , * , 100 "` | 同上 | 自动去除空格 |

---

## 🐛 故障排查

### 问题：子元素不显示

**可能原因**：
1. 子元素 `Visibility` 为 `Collapsed`
2. 子元素尺寸为 0
3. Grid 行列定义错误
4. 未调用 `Measure` 和 `Arrange`

**解决方法**：
```cpp
// 检查可见性
if (child->GetVisibility() == Visibility::Collapsed) { ... }

// 检查尺寸
auto size = child->GetDesiredSize();
std::cout << "Size: " << size.width << " x " << size.height << "\n";

// 确保布局过程完整
grid->Measure(availableSize);
grid->Arrange(Rect(0, 0, finalSize.width, finalSize.height));
```

### 问题：Star 行列尺寸为 0

**可能原因**：
1. 没有剩余空间分配给 Star
2. 其他行列占用了全部空间

**解决方法**：
```cpp
// 检查总空间
auto desired = grid->GetDesiredSize();
std::cout << "Total: " << desired.width << " x " << desired.height << "\n";

// 检查各行列尺寸
for (const auto& row : grid->GetRowDefinitions()) {
    std::cout << "Row: " << row.actualHeight << "\n";
}
```

### 问题：约束不生效

**可能原因**：
1. 约束设置在错误的对象上
2. 父容器尺寸小于约束最小值

**解决方法**：
```cpp
// 确保约束设置正确
auto row = RowDefinition::Star(1)
    .MinHeight(100)
    .MaxHeight(500);

// 检查实际尺寸是否在范围内
std::cout << "Actual: " << row.actualHeight 
          << " (min=" << row.minHeight 
          << ", max=" << row.maxHeight << ")\n";
```

---

## 📚 相关文档

- [完整实现总结](./GRID_COMPLETE_IMPLEMENTATION.md)
- [StackPanel 参考](./STACKPANEL_GUIDE.md)
- [布局系统概述](./LAYOUT_SYSTEM_OVERVIEW.md)

---

**最后更新**：2025年11月19日  
**版本**：v2.0
