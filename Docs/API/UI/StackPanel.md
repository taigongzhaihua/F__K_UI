# StackPanel

## 概览

**目的**：水平或垂直排列子元素的简单布局容器

**命名空间**：`fk::ui`

**继承**：`Panel` → `FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/StackPanel.h`

## 描述

`StackPanel` 按照垂直或水平方向依次排列子元素。这是最简单的布局容器之一。

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
StackPanel* Spacing(double spacing);
```

设置子元素之间的间距（像素）。

**示例**：
```cpp
stackPanel->Spacing(10);  // 子元素之间10像素间距
```

### 子元素管理

继承自 `Panel` 的子元素管理方法：

```cpp
template<typename T>
std::shared_ptr<T> AddChild();

void AddChild(std::shared_ptr<UIElement> child);
```

## 使用示例

### 垂直堆栈
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(5);

stack->AddChild<Button>()->Content("按钮1");
stack->AddChild<Button>()->Content("按钮2");
stack->AddChild<TextBlock>()->Text("文本");
```

### 水平堆栈
```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Horizontal)
     ->Spacing(10);

stack->AddChild<Image>()->Source("icon.png")->Width(32)->Height(32);
stack->AddChild<TextBlock>()->Text("图标和文本");
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

### 工具栏
```cpp
auto toolbar = std::make_shared<StackPanel>();
toolbar->Orientation(Orientation::Horizontal)
       ->Spacing(2)
       ->Background(Colors::LightGray)
       ->Padding(Thickness(5));

toolbar->AddChild<Button>()->Content("新建");
toolbar->AddChild<Button>()->Content("打开");
toolbar->AddChild<Button>()->Content("保存");
```

## 相关类

- [Panel](Panel.md) - 基类
- [Grid](Grid.md) - 更灵活的布局

## 另请参阅

- [设计文档](../../Design/UI/StackPanel.md)
