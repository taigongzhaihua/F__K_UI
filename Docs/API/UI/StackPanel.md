# StackPanel

## 概览

**目的**：水平或垂直排列子元素的简单布局容器

**命名空间**：`fk::ui`

**继承**：`Panel` → `FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/StackPanel.h`

## 描述

`StackPanel` 按照垂直或水平方向依次排列子元素。这是最简单的布局容器之一。

**核心特性**：
- 在堆叠方向上，子元素按顺序排列，给予无限空间
- 在垂直方向上，子元素根据其 Alignment 属性决定尺寸和位置
- 支持 Margin 折叠（CSS 风格）和 Spacing 最小间距保证
- 自动跳过 `Visibility.Collapsed` 的元素

**更新日志** (2025-11-19):
- ✅ 完整的 Alignment 支持（HorizontalAlignment/VerticalAlignment）
- ✅ Spacing 作为"最小间距保证"参与 Margin 折叠
- ✅ 性能优化：缓存 Alignment 查询结果

## 公共接口

### 方向

#### Orientation
```cpp
static const DependencyProperty& OrientationProperty();
StackPanel* Orientation(Orientation orientation);
```

设置堆栈方向：
- `Vertical` - 垂直堆栈（默认）
- `Horizontal` - 水平堆栈

**示例**：
```cpp
stackPanel->Orientation(Orientation::Horizontal);
```

### 间距

#### Spacing
```cpp
static const DependencyProperty& SpacingProperty();
StackPanel* Spacing(float spacing);
float Spacing() const;
```

设置子元素之间的最小间距（像素）。

**语义**：Spacing 表示"**最小间距保证**"，与子元素的 Margin 一起参与折叠（取最大值）。

**计算公式**：
```
实际间距 = max(前一个元素的尾部Margin, 当前元素的头部Margin, Spacing)
```

**行为示例**：

| 前Margin | 后Margin | Spacing | 实际间距 | 说明 |
|----------|----------|---------|---------|------|
| 10 | 15 | 0 | 15 | 无 Spacing，Margin 折叠 |
| 10 | 15 | 20 | 20 | Spacing 胜出（最小间距保证） |
| 10 | 15 | 5 | 15 | Margin 更大，Spacing 被吸收 |
| 0 | 0 | 20 | 20 | 仅 Spacing 生效 |

**示例**：
```cpp
// 保证所有按钮间距至少 15 像素
stackPanel->Spacing(15);

// 即使某些按钮有更小的 Margin，也能保证最小间距
button1->SetMargin(Thickness(0, 5, 0, 5));   // 间距 = 15
button2->SetMargin(Thickness(0, 10, 0, 10)); // 间距 = 15
button3->SetMargin(Thickness(0, 30, 0, 5));  // 间距 = 30（Margin 更大）
```

**与 WPF 的区别**：
- WPF 没有 Spacing 属性，需通过 Margin 模拟
- 本实现提供专门的 Spacing 属性，更方便统一控制间距

### 子元素管理

继承自 `Panel` 的子元素管理方法：

```cpp
template<typename T>
std::shared_ptr<T> AddChild();

void AddChild(std::shared_ptr<UIElement> child);
```

## 子元素 Alignment 行为

`StackPanel` 完整支持子元素的 `HorizontalAlignment` 和 `VerticalAlignment` 属性。

### 垂直堆叠（默认）

- **堆叠方向**（垂直）：忽略 VerticalAlignment，给予无限高度
- **垂直方向**（水平）：HorizontalAlignment 控制定位和尺寸

| HorizontalAlignment | 行为 |
|---------------------|------|
| Stretch（默认） | 拉伸至 StackPanel 的完整宽度 |
| Left | 靠左对齐，使用元素的 DesiredSize.width |
| Center | 水平居中，使用元素的 DesiredSize.width |
| Right | 靠右对齐，使用元素的 DesiredSize.width |

### 水平堆叠

- **堆叠方向**（水平）：忽略 HorizontalAlignment，给予无限宽度
- **垂直方向**（垂直）：VerticalAlignment 控制定位和尺寸

| VerticalAlignment | 行为 |
|-------------------|------|
| Stretch（默认） | 拉伸至 StackPanel 的完整高度 |
| Top | 顶部对齐，使用元素的 DesiredSize.height |
| Center | 垂直居中，使用元素的 DesiredSize.height |
| Bottom | 底部对齐，使用元素的 DesiredSize.height |

**设计原则**：
- 堆叠方向：元素自然堆叠，无限空间（由内容决定尺寸）
- 垂直方向：元素可以对齐和拉伸（由 Alignment 决定尺寸）

## 使用示例

### 垂直堆栈（基础）
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(5);

// 默认 Stretch，按钮会拉伸至全宽
stack->AddChild<Button>()->Content("按钮1");
stack->AddChild<Button>()->Content("按钮2");
stack->AddChild<TextBlock>()->Text("文本");
```

### 水平堆栈（基础）
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Horizontal)
     ->Spacing(10);

stack->AddChild<Image>()->Source("icon.png")->Width(32)->Height(32);
stack->AddChild<TextBlock>()->Text("图标和文本");
```

### 居中对齐的按钮列表
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(15);

// 所有按钮水平居中，使用自然宽度
stack->AddChild<Button>()
    ->Content("确定")
    ->HorizontalAlignment(HorizontalAlignment::Center);

stack->AddChild<Button>()
    ->Content("取消")
    ->HorizontalAlignment(HorizontalAlignment::Center);

stack->AddChild<Button>()
    ->Content("帮助")
    ->HorizontalAlignment(HorizontalAlignment::Center);
```

### 混合对齐的操作栏
```cpp
auto actions = std::make_shared<StackPanel>();
actions->Orientation(Orientation::Vertical)
       ->Spacing(10);

// 主要操作靠左
actions->AddChild<Button>()
    ->Content("新建项目")
    ->HorizontalAlignment(HorizontalAlignment::Left);

// 次要操作靠右
actions->AddChild<Button>()
    ->Content("设置")
    ->HorizontalAlignment(HorizontalAlignment::Right);

// 居中的标题
actions->AddChild<TextBlock>()
    ->Text("操作中心")
    ->HorizontalAlignment(HorizontalAlignment::Center);
```

### 工具栏（垂直对齐）
```cpp
auto toolbar = std::make_shared<StackPanel>();
toolbar->Orientation(Orientation::Horizontal)
       ->Spacing(8)
       ->Height(48)  // 固定高度
       ->Background(Colors::LightGray)
       ->Padding(Thickness(5));

// 小图标按钮垂直居中
toolbar->AddChild<Button>()
    ->Content("🔍")
    ->Width(32)->Height(32)
    ->VerticalAlignment(VerticalAlignment::Center);

// 文本按钮拉伸至全高（默认）
toolbar->AddChild<Button>()->Content("搜索");

// 底部对齐的状态文本
toolbar->AddChild<TextBlock>()
    ->Text("就绪")
    ->VerticalAlignment(VerticalAlignment::Bottom);
```

### Spacing 与 Margin 配合
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Spacing(15);  // 最小间距保证

// 元素1：小 Margin（被 Spacing 覆盖）
stack->AddChild<Button>()
    ->Content("元素1")
    ->Margin(Thickness(0, 5, 0, 5));  // 实际间距 = 15

// 元素2：标准 Margin（与 Spacing 相同）
stack->AddChild<Button>()
    ->Content("元素2")
    ->Margin(Thickness(0, 10, 0, 10));  // 实际间距 = 15

// 元素3：大 Margin（超过 Spacing）
stack->AddChild<Button>()
    ->Content("元素3")
    ->Margin(Thickness(0, 30, 0, 5));  // 实际间距 = 30（Margin 胜出）
```

### 嵌套堆栈
```cpp
// 创建垂直的主堆栈
auto mainStack = std::make_shared<StackPanel>();
mainStack->Orientation(Orientation::Vertical)->Spacing(20);

// 添加水平的按钮行
auto buttonRow = mainStack->AddChild<StackPanel>();
buttonRow->Orientation(Orientation::Horizontal)->Spacing(5);
buttonRow->AddChild<Button>()->Content("确定");
buttonRow->AddChild<Button>()->Content("取消");

// 添加其他内容
mainStack->AddChild<TextBlock>()->Text("更多内容");
```

## WPF/WinUI 兼容性

### 相同特性 ✅
- Orientation 属性（Vertical/Horizontal）
- 完整的 HorizontalAlignment/VerticalAlignment 支持
- Margin 系统
- 自动跳过 Collapsed 元素

### 差异 ⚠️

#### 1. Spacing 属性
- **WPF**: 无原生 Spacing 属性，需通过 Margin 模拟
- **本实现**: 提供专门的 Spacing 属性，更方便统一控制间距

#### 2. Margin 折叠模式
- **WPF**: 相邻元素的 Margin 累加（例如：10 + 15 = 25）
- **本实现**: CSS 风格折叠，取最大值（例如：max(10, 15, Spacing) = 15）

**折叠模式对比**：

| 场景 | 前Margin | 后Margin | Spacing | WPF结果 | 本实现结果 |
|------|----------|----------|---------|---------|-----------|
| 案例1 | 10 | 15 | 0 | 25 | 15 |
| 案例2 | 10 | 15 | 20 | 25 | 20 |
| 案例3 | 30 | 5 | 0 | 35 | 30 |

**迁移建议**：
- 如果需要 WPF 的累加行为，将其中一个元素的 Margin 设为 0
- 利用 Spacing 属性简化统一间距设置
- CSS 折叠模式更符合现代 Web 开发直觉

#### 3. LastChildFill
- **WPF DockPanel**: 有 LastChildFill 属性
- **本实现**: StackPanel 暂不支持此特性（未来可能添加）

### 性能优化 🚀

本实现相对 WPF 的改进：
- **Alignment 查询缓存**：每次布局时缓存子元素的 Alignment 属性，避免重复虚函数调用
- **提前退出**：Collapsed 元素在循环开始时立即跳过
- **单次扫描**：Measure 和 Arrange 各一次遍历，无额外查找

## 相关类

- [Panel](Panel.md) - 基类
- [Grid](Grid.md) - 更灵活的布局
- [DockPanel](DockPanel.md) - 停靠布局（支持 LastChildFill）

## 另请参阅

- [设计文档](../../Design/UI/StackPanel.md)
- [布局系统概述](../../Design/UI/LayoutSystem.md)
- [Alignment 枚举](Alignment.md)
