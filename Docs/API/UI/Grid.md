# Grid

## 概览

**目的**：基于行列的灵活布局容器

**命名空间**：`fk::ui`

**继承**：`Panel` → `FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/Grid.h`

## 描述

`Grid` 提供基于行和列的灵活布局系统。子元素可以跨越多行/列，支持固定大小、比例大小（Star）和自动大小。

## 公共接口

### 行和列定义

#### AddRowDefinition / AddColumnDefinition
```cpp
RowDefinition* AddRowDefinition();
ColumnDefinition* AddColumnDefinition();
```

添加行和列定义。

**示例**：
```cpp
auto grid = std::make_shared<Grid>();

// 添加2行3列
grid->AddRowDefinition()->Height(GridLength(100));  // 固定100像素
grid->AddRowDefinition()->Height(GridLength(1, GridUnitType::Star));  // 比例1*

grid->AddColumnDefinition()->Width(GridLength::Auto());  // 自动大小
grid->AddColumnDefinition()->Width(GridLength(2, GridUnitType::Star));  // 比例2*
grid->AddColumnDefinition()->Width(GridLength(200));  // 固定200像素
```

### 子元素定位

#### 附加属性
```cpp
static const DependencyProperty& RowProperty();
static const DependencyProperty& ColumnProperty();
static const DependencyProperty& RowSpanProperty();
static const DependencyProperty& ColumnSpanProperty();
```

设置子元素在网格中的位置和跨越。

**示例**：
```cpp
auto button = grid->AddChild<Button>();
button->Content("按钮")
      ->Row(0)          // 第0行
      ->Column(1)       // 第1列
      ->RowSpan(2)      // 跨2行
      ->ColumnSpan(1);  // 跨1列
```

## GridLength 类型

- **Fixed（固定）**：`GridLength(200)` - 固定像素
- **Star（比例）**：`GridLength(1, GridUnitType::Star)` - 按比例分配
- **Auto（自动）**：`GridLength::Auto()` - 根据内容自动大小

## 使用示例

### 基本网格布局
```cpp
auto grid = std::make_shared<Grid>();

// 定义3行2列
grid->AddRowDefinition()->Height(GridLength::Auto());
grid->AddRowDefinition()->Height(GridLength(1, GridUnitType::Star));
grid->AddRowDefinition()->Height(GridLength(50));

grid->AddColumnDefinition()->Width(GridLength(1, GridUnitType::Star));
grid->AddColumnDefinition()->Width(GridLength(1, GridUnitType::Star));

// 添加子元素
auto header = grid->AddChild<TextBlock>();
header->Text("标题")->Row(0)->Column(0)->ColumnSpan(2);

auto content1 = grid->AddChild<Border>();
content1->Row(1)->Column(0);

auto content2 = grid->AddChild<Border>();
content2->Row(1)->Column(1);

auto footer = grid->AddChild<Button>();
footer->Content("确定")->Row(2)->Column(0)->ColumnSpan(2);
```

### 复杂布局
```cpp
// 创建一个典型的应用程序布局
auto grid = std::make_shared<Grid>();

// 行：标题栏 | 内容区 | 状态栏
grid->AddRowDefinition()->Height(GridLength(40));
grid->AddRowDefinition()->Height(GridLength(1, GridUnitType::Star));
grid->AddRowDefinition()->Height(GridLength(25));

// 列：侧边栏 | 主内容
grid->AddColumnDefinition()->Width(GridLength(200));
grid->AddColumnDefinition()->Width(GridLength(1, GridUnitType::Star));

// 标题栏（跨所有列）
auto header = grid->AddChild<Border>();
header->Row(0)->ColumnSpan(2)->Background(Colors::DarkGray);

// 侧边栏
auto sidebar = grid->AddChild<StackPanel>();
sidebar->Row(1)->Column(0);

// 主内容区
auto mainContent = grid->AddChild<Border>();
mainContent->Row(1)->Column(1);

// 状态栏（跨所有列）
auto statusBar = grid->AddChild<TextBlock>();
statusBar->Row(2)->ColumnSpan(2);
```

## 相关类

- [Panel](Panel.md) - 基类
- [StackPanel](StackPanel.md) - 另一种布局容器

## 另请参阅

- [设计文档](../../Design/UI/Grid.md)
