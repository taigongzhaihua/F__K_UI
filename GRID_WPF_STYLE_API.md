# Grid WPF/WinUI 风格 API 参考

## 🎨 新的声明式 API

### 基本语法

```cpp
auto grid = new Grid();
grid->Rows("Auto, *, Auto")
    ->Columns("*, *")
    ->Children({
        new Button() | cell(0, 0),
        new Button() | cell(0, 1),
        new Button() | cell(1, 0)
    });
```

---

## 📐 cell() 函数

### 基本用法

```cpp
cell(row, col)                    // 指定行列
cell(row, col, rowSpan, colSpan)  // 指定行列和跨度
```

### 链式设置跨度

```cpp
cell(0, 0).RowSpan(2)           // 跨 2 行
cell(0, 0).ColumnSpan(2)        // 跨 2 列
cell(0, 0).RowSpan(2).ColumnSpan(2)  // 跨 2x2
```

---

## 🔧 管道操作符 `|`

### 语法

```cpp
element | cell(row, col)
```

### 工作原理

`|` 操作符将 `GridCellAttacher` 应用到 `UIElement`，自动设置：
- `Grid.Row`
- `Grid.Column`
- `Grid.RowSpan`
- `Grid.ColumnSpan`

### 示例

```cpp
// 单个元素
auto button = new Button()->Content("Click") | cell(0, 0);
grid->AddChild(button);

// 批量添加（推荐）
grid->Children({
    new Button()->Content("A") | cell(0, 0),
    new Button()->Content("B") | cell(0, 1),
    new Button()->Content("C") | cell(1, 0)
});
```

---

## 📊 与 WPF XAML 对比

### WPF XAML

```xml
<Grid>
  <Grid.RowDefinitions>
    <RowDefinition Height="Auto"/>
    <RowDefinition Height="*"/>
  </Grid.RowDefinitions>
  <Grid.ColumnDefinitions>
    <ColumnDefinition Width="200"/>
    <ColumnDefinition Width="*"/>
  </Grid.ColumnDefinitions>
  
  <Button Grid.Row="0" Grid.Column="0">按钮 A</Button>
  <Button Grid.Row="0" Grid.Column="1" Grid.RowSpan="2">按钮 B</Button>
  <Button Grid.Row="1" Grid.Column="0">按钮 C</Button>
</Grid>
```

### F__K_UI C++（新 API）

```cpp
auto grid = new Grid();
grid->Rows("Auto, *")
    ->Columns("200, *")
    ->Children({
        new Button()->Content("按钮 A") | cell(0, 0),
        new Button()->Content("按钮 B") | cell(0, 1).RowSpan(2),
        new Button()->Content("按钮 C") | cell(1, 0)
    });
```

**相似性**:
- `Grid.Row="0"` → `cell(0, 0)`
- `Grid.RowSpan="2"` → `.RowSpan(2)`
- 链式调用 → 结构清晰

---

## 🚀 完整示例

### 示例 1: 简单 3x3 网格

```cpp
auto grid = new Grid();
grid->Rows("*, *, *")
    ->Columns("*, *, *")
    ->Children({
        new Button()->Content("1") | cell(0, 0),
        new Button()->Content("2") | cell(0, 1),
        new Button()->Content("3") | cell(0, 2),
        new Button()->Content("4") | cell(1, 0),
        new Button()->Content("5") | cell(1, 1),
        new Button()->Content("6") | cell(1, 2),
        new Button()->Content("7") | cell(2, 0),
        new Button()->Content("8") | cell(2, 1),
        new Button()->Content("9") | cell(2, 2)
    });
```

---

### 示例 2: 跨行列布局

```cpp
auto grid = new Grid();
grid->Rows("100, 100, 100")
    ->Columns("100, 100, 100")
    ->Children({
        new Button()->Content("跨 2 列") | cell(0, 0).ColumnSpan(2),
        new Button()->Content("跨 2 行") | cell(1, 2).RowSpan(2),
        new Button()->Content("跨 2x2") | cell(1, 0).RowSpan(2).ColumnSpan(2),
        new Button()->Content("普通") | cell(0, 2)
    });
```

---

### 示例 3: IDE 布局

```cpp
auto mainGrid = new Grid();
mainGrid->Rows("60, *, 30")      // 工具栏、内容、状态栏
        ->Columns("200, *, 200")  // 左边栏、编辑器、右边栏
        ->Children({
            // 工具栏（跨 3 列）
            new Border()->Background(0xFF2B2B2B) 
                | cell(0, 0).ColumnSpan(3),
            
            // 左侧边栏
            new StackPanel() | cell(1, 0),
            
            // 编辑器
            new Border()->Background(0xFF252526) | cell(1, 1),
            
            // 右侧边栏
            new StackPanel() | cell(1, 2),
            
            // 状态栏（跨 3 列）
            new Border()->Background(0xFF007ACC) 
                | cell(2, 0).ColumnSpan(3)
        });
```

---

### 示例 4: 表单布局

```cpp
auto formGrid = new Grid();
formGrid->Rows("Auto, Auto, Auto, Auto, *")
        ->Columns("Auto, *")
        ->Children({
            new TextBlock()->Text("用户名:") | cell(0, 0),
            new TextBox() | cell(0, 1),
            
            new TextBlock()->Text("密码:") | cell(1, 0),
            new TextBox() | cell(1, 1),
            
            new TextBlock()->Text("邮箱:") | cell(2, 0),
            new TextBox() | cell(2, 1),
            
            new TextBlock()->Text("电话:") | cell(3, 0),
            new TextBox() | cell(3, 1),
            
            // 按钮区域（跨 2 列）
            (new StackPanel()
                ->Orientation(Orientation::Horizontal)
                ->HorizontalAlignment(HorizontalAlignment::Right)
                ->Children({
                    new Button()->Content("取消"),
                    new Button()->Content("提交")
                })
            ) | cell(4, 0).ColumnSpan(2)
        });
```

---

## 🔄 API 风格对比

### 方式 1: 传统 API

```cpp
auto button = new Button();
Grid::SetRow(button, 0);
Grid::SetColumn(button, 1);
Grid::SetRowSpan(button, 2);
grid->AddChild(button);
```

**优点**: 显式、易于理解  
**缺点**: 冗长、分散

---

### 方式 2: 流式语法（之前实现）

```cpp
auto button = grid->AddChild(new Button())
    ->Row(0)
    ->Column(1)
    ->RowSpan(2);
```

**优点**: 链式调用、紧凑  
**缺点**: 需要先 AddChild

---

### 方式 3: WPF 风格（新 API，推荐）

```cpp
grid->Children({
    new Button() | cell(0, 1).RowSpan(2)
});
```

**优点**: 
- ✅ 声明式、可读性最强
- ✅ 与 WPF/WinUI XAML 风格一致
- ✅ 批量添加简洁
- ✅ 类似标记语言的结构感

**推荐场景**: 
- 复杂布局（多个子元素）
- 需要清晰表达布局结构
- 团队熟悉 WPF/WinUI

---

## 🎯 最佳实践

### 1. 使用 Children() 批量添加

```cpp
// ✅ 推荐
grid->Children({
    button1 | cell(0, 0),
    button2 | cell(0, 1),
    button3 | cell(1, 0)
});

// ❌ 不推荐（太冗长）
grid->AddChild(button1 | cell(0, 0));
grid->AddChild(button2 | cell(0, 1));
grid->AddChild(button3 | cell(1, 0));
```

---

### 2. 链式设置跨度

```cpp
// ✅ 推荐
button | cell(0, 0).RowSpan(2).ColumnSpan(2)

// ✅ 也可以（如果跨度固定）
button | cell(0, 0, 2, 2)
```

---

### 3. 嵌套复杂元素

```cpp
grid->Children({
    // 使用括号包裹复杂表达式
    (new StackPanel()
        ->Orientation(Orientation::Vertical)
        ->Children({
            new Button()->Content("A"),
            new Button()->Content("B")
        })
    ) | cell(0, 0)
});
```

---

### 4. 保持对齐和缩进

```cpp
// ✅ 好的风格
grid->Children({
    new Button()->Content("A")     | cell(0, 0),
    new Button()->Content("B")     | cell(0, 1),
    new Button()->Content("C")     | cell(1, 0).RowSpan(2),
    new Button()->Content("Long")  | cell(1, 1)
});

// ❌ 难以阅读
grid->Children({
new Button()->Content("A") | cell(0, 0),
new Button()->Content("B") | cell(0, 1),
new Button()->Content("C") | cell(1, 0).RowSpan(2)
});
```

---

## 🔧 技术细节

### GridCellAttacher 结构

```cpp
struct GridCellAttacher {
    int row{0};
    int col{0};
    int rowSpan{1};
    int colSpan{1};
    
    GridCellAttacher& RowSpan(int span);
    GridCellAttacher& ColumnSpan(int span);
    UIElement* ApplyTo(UIElement* element) const;
};
```

### cell() 辅助函数

```cpp
inline GridCellAttacher cell(int row, int col);
inline GridCellAttacher cell(int row, int col, int rowSpan, int colSpan);
```

### 管道操作符

```cpp
inline UIElement* operator|(UIElement* element, const GridCellAttacher& attacher) {
    return attacher.ApplyTo(element);
}
```

---

## 🆚 旧 API 兼容性

所有旧 API 依然可用，新旧 API 可以混合使用：

```cpp
auto grid = new Grid();

// 旧 API
auto btn1 = new Button();
Grid::SetRow(btn1, 0);
grid->AddChild(btn1);

// 流式 API（之前实现）
auto btn2 = grid->AddChild(new Button())->Row(0)->Column(1);

// 新 WPF 风格 API
grid->Children({
    new Button() | cell(1, 0),
    new Button() | cell(1, 1)
});

// 所有方式都可以正常工作！
```

---

## ✨ 优势总结

### 与 XAML 的对应关系

| WPF XAML | F__K_UI C++ |
|----------|-------------|
| `Grid.Row="0"` | `cell(0, ...)` |
| `Grid.Column="1"` | `cell(..., 1)` |
| `Grid.RowSpan="2"` | `.RowSpan(2)` |
| `Grid.ColumnSpan="2"` | `.ColumnSpan(2)` |
| `<Grid.Children>` | `->Children({...})` |

### 代码对比

**XAML (15 行)**:
```xml
<Grid>
  <Grid.RowDefinitions>
    <RowDefinition Height="*"/>
    <RowDefinition Height="*"/>
  </Grid.RowDefinitions>
  <Grid.ColumnDefinitions>
    <ColumnDefinition Width="*"/>
    <ColumnDefinition Width="*"/>
  </Grid.ColumnDefinitions>
  <Button Grid.Row="0" Grid.Column="0">A</Button>
  <Button Grid.Row="0" Grid.Column="1">B</Button>
  <Button Grid.Row="1" Grid.Column="0">C</Button>
  <Button Grid.Row="1" Grid.Column="1">D</Button>
</Grid>
```

**C++ 新 API (8 行)**:
```cpp
auto grid = new Grid();
grid->Rows("*, *")->Columns("*, *")->Children({
    new Button()->Content("A") | cell(0, 0),
    new Button()->Content("B") | cell(0, 1),
    new Button()->Content("C") | cell(1, 0),
    new Button()->Content("D") | cell(1, 1)
});
```

**减少代码量**: ~47%  
**可读性**: ⭐⭐⭐⭐⭐

---

## 📚 相关文档

- [Grid 完整实现文档](./GRID_COMPLETE_IMPLEMENTATION.md)
- [Grid 快速参考](./GRID_QUICK_REFERENCE.md)
- [完整示例程序](./example_grid_wpf_style.cpp)

---

**更新日期**: 2025年11月19日  
**版本**: v2.1 - WPF Style API  
**状态**: ✅ 生产就绪
