# StackPanel 文档

## 概述

`StackPanel` 是一维线性布局容器，将子元素按水平或竖直方向排列。相比 Grid 的二维复杂性，StackPanel 提供简洁直观的线性排列方式，是最常用的布局控件之一。

**继承链：** `UIElement` → `FrameworkElement` → `PanelBase` → `StackPanel`

**命名空间：** `fk::ui`

**特性：**
- ✅ 水平/竖直方向排列
- ✅ 自动间距（Spacing）
- ✅ 灵活的子元素尺寸处理
- ✅ 嵌套支持（StackPanel 中包含 StackPanel）

---

## 核心属性

### Orientation
- **类型：** `Orientation` (枚举)
- **默认值：** `Orientation::Vertical`
- **说明：** 子元素的排列方向
- **可选值：**
  - `Orientation::Horizontal` - 水平排列（从左到右）
  - `Orientation::Vertical` - 竖直排列（从上到下）
- **可绑定：** ✅ 是

**用法：**
```cpp
// 水平排列
ui::stackPanel()->Orientation(Orientation::Horizontal);

// 竖直排列（默认）
ui::stackPanel()->Orientation(Orientation::Vertical);
```

### Spacing
- **类型：** `float`
- **默认值：** `0.0f`
- **说明：** 相邻子元素之间的间距（单位：像素）
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::stackPanel()
    ->Orientation(Orientation::Horizontal)
    ->Spacing(10.0f)  // 每两个子元素之间 10 像素
    ->Children({
        ui::button()->Content("A"),
        ui::button()->Content("B"),
        ui::button()->Content("C")
    });
```

---

## 从 Panel 继承的方法

### Ptr Children(const UIElementCollection& children)
- **说明：** 批量设置子元素
- **返回值：** 返回 `this` 便于链式调用

```cpp
auto panel = ui::stackPanel()
    ->Orientation(Orientation::Horizontal)
    ->Children({
        ui::textBlock()->Text("标签"),
        ui::textBox()->Width(200),
        ui::button()->Content("确定")
    });
```

### void AddChild(std::shared_ptr\<UIElement\> child)
- **说明：** 添加单个子元素

```cpp
auto panel = ui::stackPanel();
panel->AddChild(ui::button()->Content("按钮"));
panel->AddChild(ui::textBlock()->Text("文本"));
```

### std::span\<const std::shared_ptr\<UIElement\>\> GetChildren() const
- **说明：** 获取所有子元素

```cpp
auto children = panel->GetChildren();
for (const auto& child : children) {
    // 处理每个子元素
}
```

### Ptr ClearChildren()
- **说明：** 移除所有子元素

```cpp
panel->ClearChildren();
```

---

## 从 FrameworkElement 继承的属性

### Width / Height
- **类型：** `float`
- **默认值：** `NaN` (自动)
- **说明：** 固定尺寸

```cpp
ui::stackPanel()->Width(400.0f)->Height(300.0f);
```

### HorizontalAlignment / VerticalAlignment (继承)
- **类型：** `HorizontalAlignment` / `VerticalAlignment`
- **默认值：** `Stretch`
- **说明：** 在父容器中的对齐方式

```cpp
ui::stackPanel()
    ->HorizontalAlignment(HorizontalAlignment::Center)
    ->VerticalAlignment(VerticalAlignment::Top);
```

### Margin / Padding
- **类型：** `Thickness`
- **默认值：** `{0, 0, 0, 0}`
- **说明：** 外边距 / 内边距

```cpp
ui::stackPanel()
    ->Margin({10, 10, 10, 10})    // 外边距
    ->Padding({5, 5, 5, 5});      // 内边距
```

---

## 布局计算流程

### 测量阶段 (Measure)
1. 根据 Orientation 确定主轴与交叉轴
2. 遍历所有子元素，依次测量其大小
3. 累加子元素在主轴上的尺寸，加上间距
4. 交叉轴取最大值
5. 返回 StackPanel 的总所需尺寸

**例：Horizontal StackPanel**
- 主轴（水平）：所有子元素宽度之和 + 间距
- 交叉轴（竖直）：最高子元素的高度

### 排列阶段 (Arrange)
1. 在主轴方向上依次排列子元素
2. 在交叉轴方向上对齐或拉伸子元素（根据其对齐属性）

---

## 完整使用示例

### 示例 1：简单工具栏

```cpp
auto toolbar = ui::stackPanel()
    ->Orientation(Orientation::Horizontal)
    ->Spacing(5.0f)
    ->Padding({5, 5, 5, 5})
    ->Background(Color::FromRGB(240, 240, 240))
    ->Children({
        ui::button()->Content("新建"),
        ui::button()->Content("打开"),
        ui::button()->Content("保存"),
        ui::button()->Content("关闭")
    });
```

### 示例 2：竖直表单

```cpp
auto form = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(10.0f)
    ->Padding({20, 20, 20, 20})
    ->Width(400.0f)
    ->Children({
        ui::textBlock()->Text("用户信息"),
        
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(10.0f)
            ->Children({
                ui::textBlock()->Text("姓名:")->Width(50),
                ui::textBox()->Width(300)
            }),
        
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(10.0f)
            ->Children({
                ui::textBlock()->Text("邮箱:")->Width(50),
                ui::textBox()->Width(300)
            }),
        
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(10.0f)
            ->HorizontalAlignment(HorizontalAlignment::Center)
            ->Children({
                ui::button()->Content("保存")->Width(80),
                ui::button()->Content("取消")->Width(80)
            })
    });
```

### 示例 3：嵌套 StackPanel

```cpp
auto layout = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(10.0f)
    ->Children({
        // 顶部栏 - 水平
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(5.0f)
            ->Children({
                ui::button()->Content("菜单"),
                ui::textBlock()->Text("应用标题"),
                ui::button()->Content("帮助")
            }),
        
        // 主内容区 - 水平
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(10.0f)
            ->Children({
                // 左侧菜单 - 竖直
                ui::stackPanel()
                    ->Orientation(Orientation::Vertical)
                    ->Width(150)
                    ->Children({
                        ui::button()->Content("项目 1"),
                        ui::button()->Content("项目 2"),
                        ui::button()->Content("项目 3")
                    }),
                
                // 右侧内容
                ui::textBlock()->Text("主要内容")
            }),
        
        // 底部栏 - 水平
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Children({
                ui::textBlock()->Text("版本 1.0")
            })
    });
```

### 示例 4：内容列表

```cpp
auto items = std::vector<std::string>{"项目 A", "项目 B", "项目 C", "项目 D"};

auto listPanel = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(5.0f);

for (const auto& item : items) {
    listPanel->AddChild(
        ui::button()
            ->Content(item)
            ->HorizontalAlignment(HorizontalAlignment::Stretch)
    );
}
```

---

## 从 UIElement 继承的属性

### Visibility (继承)
- **类型：** `Visibility`
- **默认值：** `Visibility::Visible`
- **说明：** 控制元素是否可见
- **可选值：**
  - `Visibility::Visible` - 可见
  - `Visibility::Hidden` - 隐藏但占用空间
  - `Visibility::Collapsed` - 隐藏且不占用空间

```cpp
ui::stackPanel()->Visibility(Visibility::Collapsed);
```

### IsEnabled (继承)
- **类型：** `bool`
- **默认值：** `true`
- **说明：** 是否启用，禁用时子元素通常显示为灰化

```cpp
ui::stackPanel()->IsEnabled(false);  // 禁用所有子元素
```

### Opacity (继承)
- **类型：** `float`
- **默认值：** `1.0f`
- **说明：** 不透明度 (0.0 = 完全透明, 1.0 = 完全不透明)

```cpp
ui::stackPanel()->Opacity(0.8f);
```

---

## 最佳实践

1. **合理选择方向：**
   - 线性数据 → StackPanel（水平或竖直）
   - 矩阵数据 → Grid

2. **使用 Spacing 而不是 Margin：**
   ```cpp
   // ✅ 推荐
   stackPanel->Spacing(10.0f);
   
   // ❌ 不推荐
   for (auto child : children) {
       child->Margin({0, 5, 0, 5});  // 手动设置边距
   }
   ```

3. **嵌套深度有限：**
   - 3-4 层嵌套通常可接受
   - 过深的嵌套会降低布局计算效率

4. **动态添加子元素：**
   ```cpp
   // 保持对 StackPanel 的引用，方便后续操作
   auto panel = ui::stackPanel();
   panel->AddChild(newElement);
   ```

---

## 常见问题

### Q1：如何让 StackPanel 中的某个子元素占满剩余空间？
**A：** 设置子元素的 `HorizontalAlignment` / `VerticalAlignment` 为 `Stretch`，并设置相应的尺寸：
```cpp
ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Children({
        ui::textBlock()->Text("标题"),
        ui::textBlock()
            ->Text("主要内容...")
            ->VerticalAlignment(VerticalAlignment::Stretch)
            ->Height(200)  // 或不设置，让其自动拉伸
    });
```

### Q2：如何实现 StackPanel 中子元素的均匀分布？
**A：** 使用 Star 尺寸（类似 Grid）或在渲染时计算等间距：
```cpp
// 方式 1：在 Grid 中实现更精确的分布
auto grid = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()}
    })
    ->Children({
        ui::button()->Content("A") | cell(0, 0),
        ui::button()->Content("B") | cell(0, 1),
        ui::button()->Content("C") | cell(0, 2)
    });
```

### Q3：StackPanel 与 Grid 的选择标准是什么？
**A：**
| 特性 | StackPanel | Grid |
|-----|-----------|------|
| 排列方向 | 一维（水平/竖直） | 二维（行+列） |
| 复杂度 | 低 | 高 |
| 灵活性 | 低 | 高 |
| 学习曲线 | 平缓 | 陡峭 |

选择：
- 简单列表/工具栏 → StackPanel
- 复杂表单/仪表板 → Grid

### Q4：如何在 StackPanel 中实现子元素居中对齐？
**A：** 设置 StackPanel 的对齐属性：
```cpp
ui::stackPanel()
    ->Orientation(Orientation::Horizontal)
    ->HorizontalAlignment(HorizontalAlignment::Center)
    ->VerticalAlignment(VerticalAlignment::Center)
    ->Children({...});
```

---

## 性能考虑

- **大量子元素：** > 100 个子元素时性能开始下降，建议使用虚拟化列表
- **深度嵌套：** 每增加一层嵌套都会增加测量/排列的开销
- **频繁添加/移除：** 批量操作优于单个操作

---

## 另见

- [Grid.md](Grid.md) - 二维布局容器
- [Panel.md](Panel.md) - 容器基类
- [UIElement.md](UIElement.md) - 基础元素
