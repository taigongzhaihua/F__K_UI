# Panel 文档

## 概述

`Panel` 是所有容器控件的基类，提供子元素管理、布局计算、测量/排列等基础功能。开发者通常不直接使用 Panel，而是使用它的派生类如 Grid、StackPanel、Canvas 等。

**继承链：** `UIElement` → `FrameworkElement` → `PanelBase` → `Panel`

**命名空间：** `fk::ui`

**职责：**
- ✅ 管理子元素集合
- ✅ 协调子元素的测量和排列
- ✅ 支持子元素的添加/移除
- ✅ 为派生类提供扩展点（override points）

---

## 子元素管理

### 添加子元素

#### Children(const UIElementCollection& children)
- **参数：** 子元素集合
- **返回值：** `this` (便于链式调用)
- **说明：** 批量设置子元素（替换现有子元素）

```cpp
auto panel = ui::stackPanel()
    ->Children({
        ui::button()->Content("按钮 1"),
        ui::button()->Content("按钮 2"),
        ui::textBlock()->Text("标签")
    });
```

#### AddChild(std::shared_ptr\<UIElement\> child)
- **参数：** 要添加的子元素
- **说明：** 在末尾添加单个子元素

```cpp
auto panel = ui::stackPanel();
panel->AddChild(ui::button()->Content("新按钮"));
```

#### InsertChild(size_t index, std::shared_ptr\<UIElement\> child)
- **参数：** 
  - `index` - 插入位置
  - `child` - 要插入的子元素
- **说明：** 在指定位置插入子元素

```cpp
panel->InsertChild(0, ui::button()->Content("新按钮"));  // 插入开头
```

### 移除子元素

#### RemoveChild(UIElement* child)
- **参数：** 要移除的子元素
- **返回值：** `true` 移除成功，`false` 未找到
- **说明：** 移除指定子元素

```cpp
auto btn = ui::button();
panel->AddChild(btn);
panel->RemoveChild(btn.get());  // 移除
```

#### RemoveChildAt(size_t index)
- **参数：** 要移除的子元素索引
- **说明：** 按索引移除子元素

```cpp
panel->RemoveChildAt(0);  // 移除第一个子元素
```

#### ClearChildren()
- **说明：** 移除所有子元素
- **返回值：** `this`

```cpp
panel->ClearChildren();
```

### 查询子元素

#### std::span\<const std::shared_ptr\<UIElement\>\> GetChildren() const
- **返回值：** 所有子元素的只读视图
- **说明：** 用于遍历所有子元素

```cpp
auto children = panel->GetChildren();
for (const auto& child : children) {
    // 处理子元素
    if (auto btn = std::dynamic_pointer_cast<Button>(child)) {
        // 这是一个 Button
    }
}
```

#### size_t GetChildrenCount() const
- **返回值：** 子元素个数

```cpp
auto count = panel->GetChildrenCount();
```

#### std::shared_ptr\<UIElement\> GetChildAt(size_t index) const
- **参数：** 子元素索引
- **返回值：** 指定索引的子元素，若索引越界返回 nullptr

```cpp
auto child = panel->GetChildAt(0);
if (child) {
    // 处理子元素
}
```

---

## 布局相关方法

### 通知布局失效

#### InvalidateMeasure()
- **说明：** 标记测量为失效，下一帧将重新测量
- **触发条件：** 
  - 子元素尺寸改变
  - 子元素集合改变
  - 布局相关属性改变

```cpp
panel->AddChild(newElement);
panel->InvalidateMeasure();  // 标记需要重新测量
```

#### InvalidateArrange()
- **说明：** 标记排列为失效，下一帧将重新排列

```cpp
panel->InvalidateArrange();
```

---

## 从 FrameworkElement 继承的属性

### Width / Height / Margin / Padding / MinWidth / MaxWidth 等
详见 [FrameworkElement.md](FrameworkElement.md)

---

## Panel 派生类

### Grid
二维网格布局，支持行列定义。
- **用途：** 复杂表单、仪表板、表格
- **详见：** [Grid.md](Grid.md)

### StackPanel
一维线性布局（水平或竖直）。
- **用途：** 工具栏、菜单、列表
- **详见：** [StackPanel.md](StackPanel.md)

### Canvas
绝对定位容器。
- **用途：** 自由绘图、游戏界面
- **详见：** [Canvas.md](Canvas.md)

### WrapPanel
自动换行布局。
- **用途：** 照片墙、标签云
- **详见：** [WrapPanel.md](WrapPanel.md)

---

## 完整使用示例

### 示例 1：动态添加/移除子元素

```cpp
auto panel = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(5.0f);

// 初始化
std::vector<std::string> items = {"A", "B", "C"};
for (const auto& item : items) {
    panel->AddChild(
        ui::button()->Content(item)
    );
}

// 添加新元素
panel->AddChild(ui::button()->Content("D"));

// 移除第一个元素
panel->RemoveChildAt(0);

// 清空所有元素
panel->ClearChildren();
```

### 示例 2：查询特定类型的子元素

```cpp
auto panel = ui::grid();
// ... 添加多个子元素 ...

int buttonCount = 0;
for (const auto& child : panel->GetChildren()) {
    if (std::dynamic_pointer_cast<Button>(child)) {
        buttonCount++;
    }
}
printf("总按钮数：%d\n", buttonCount);
```

### 示例 3：在特定位置插入元素

```cpp
auto panel = ui::stackPanel();
panel->Children({
    ui::button()->Content("第 1 个"),
    ui::button()->Content("第 3 个")
});

// 在索引 1 处插入新元素
panel->InsertChild(1, ui::button()->Content("第 2 个"));

// 结果顺序：第 1 个、第 2 个、第 3 个
```

### 示例 4：递归遍历所有子元素

```cpp
void PrintElementTree(const Panel* panel, int depth = 0) {
    std::string indent(depth * 2, ' ');
    
    printf("%sPanel: %s\n", indent.c_str(), 
           typeid(*panel).name());
    
    for (const auto& child : panel->GetChildren()) {
        printf("%s- %s\n", indent.c_str(), 
               typeid(*child).name());
        
        if (auto childPanel = std::dynamic_pointer_cast<Panel>(child)) {
            PrintElementTree(childPanel.get(), depth + 1);
        }
    }
}
```

---

## 高级用法

### 自定义 Panel 派生类

当需要特殊布局算法时，可以继承 Panel 并重写测量/排列逻辑：

```cpp
class CustomPanel : public Panel {
protected:
    // 重写测量逻辑
    SizeF MeasureOverride(SizeF availableSize) override {
        // 1. 测量所有子元素
        for (const auto& child : GetChildren()) {
            child->Measure(availableSize);
        }
        // 2. 计算并返回所需尺寸
        return {availableSize.width, availableSize.height};
    }
    
    // 重写排列逻辑
    SizeF ArrangeOverride(SizeF finalSize) override {
        // 1. 为每个子元素分配矩形区域
        PointF position{0, 0};
        for (const auto& child : GetChildren()) {
            RectF childRect{position, child->GetDesiredSize()};
            child->Arrange(childRect);
            position.x += childRect.width;
        }
        return finalSize;
    }
};
```

### 控制子元素可见性

```cpp
void ShowChildrenByType(Panel* panel, 
                        const std::type_info& type) {
    for (const auto& child : panel->GetChildren()) {
        bool show = typeid(*child) == type;
        child->SetVisibility(show ? Visibility::Visible 
                                  : Visibility::Collapsed);
    }
}

// 使用
ShowChildrenByType(mainPanel.get(), typeid(Button));
```

---

## 常见问题

### Q1：Panel 与 Container 的区别？
**A：** 在本框架中，Panel 是容器的基类。Panel 专注于布局和子元素管理，而 Container（如 ContentControl）专注于单个内容。

### Q2：如何在 Panel 中查找特定的子元素？
**A：** 使用 `GetChildren()` 遍历并过滤：
```cpp
auto panel = ui::grid();
// ...
auto buttons = std::vector<std::shared_ptr<Button>>();
for (const auto& child : panel->GetChildren()) {
    if (auto btn = std::dynamic_pointer_cast<Button>(child)) {
        buttons.push_back(btn);
    }
}
```

### Q3：添加子元素后为什么需要 InvalidateMeasure()？
**A：** Panel 使用延迟布局。添加子元素不会立即重新计算布局，而是标记为失效，下一帧才重新计算。手动调用 `InvalidateMeasure()` 可确保立即失效。

### Q4：子元素超出 Panel 容器时会发生什么？
**A：** 默认情况下，子元素会溢出显示。可以：
1. 设置 Panel 的 `ClipToBounds = true`（如果支持）
2. 调整 Panel 的尺寸
3. 修改布局算法

---

## 性能建议

1. **批量操作：** 先清空子元素，再批量添加，比逐个添加效率高
   ```cpp
   // ❌ 低效
   for (auto item : items) {
       panel->AddChild(item);
   }
   
   // ✅ 高效
   panel->Children(items);
   ```

2. **避免频繁重新布局：** 批量修改后再调用一次 `InvalidateMeasure()`
   ```cpp
   for (auto& child : panel->GetChildren()) {
       child->SetWidth(newWidth);  // 多次修改
   }
   panel->InvalidateMeasure();  // 一次失效
   ```

3. **虚拟化大列表：** 如果有上千个子元素，考虑使用虚拟化容器

---

## 另见

- [FrameworkElement.md](FrameworkElement.md) - 布局基础
- [Grid.md](Grid.md) - 二维布局
- [StackPanel.md](StackPanel.md) - 线性布局
- [UIElement.md](UIElement.md) - 基础元素
