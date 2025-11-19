# Grid API 快速开始

## 🚀 三种 API 风格（按推荐顺序）

---

## 1️⃣ WPF 风格 API（推荐）✨

### 最简单的例子

```cpp
#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"

auto grid = new Grid();
grid->Rows("*, *")
    ->Columns("*, *")
    ->Children({
        new Button()->Content("A") | cell(0, 0),
        new Button()->Content("B") | cell(0, 1),
        new Button()->Content("C") | cell(1, 0),
        new Button()->Content("D") | cell(1, 1)
    });
```

### 为什么推荐？

✅ **最接近 WPF/WinUI XAML**  
✅ **代码最简洁**（减少 50%）  
✅ **结构最清晰**（声明式）  
✅ **最易维护**（布局一目了然）

### 对比 XAML

**WPF XAML**:
```xml
<Grid>
  <Button Grid.Row="0" Grid.Column="0">A</Button>
  <Button Grid.Row="0" Grid.Column="1">B</Button>
</Grid>
```

**F__K_UI C++**:
```cpp
grid->Children({
    new Button()->Content("A") | cell(0, 0),
    new Button()->Content("B") | cell(0, 1)
});
```

几乎一模一样！🎯

---

## 2️⃣ 流式 API

### 例子

```cpp
auto grid = new Grid();
grid->Rows("*, *")->Columns("*, *");

auto buttonA = grid->AddChild(new Button()->Content("A"))
    ->Row(0)->Column(0);

auto buttonB = grid->AddChild(new Button()->Content("B"))
    ->Row(0)->Column(1);
```

### 适用场景

- 需要保留子元素指针
- 动态添加子元素
- 需要后续修改元素

---

## 3️⃣ 传统 API

### 例子

```cpp
auto grid = new Grid();
grid->AddRowDefinition(RowDefinition::Star());
grid->AddRowDefinition(RowDefinition::Star());
grid->AddColumnDefinition(ColumnDefinition::Star());
grid->AddColumnDefinition(ColumnDefinition::Star());

auto buttonA = new Button()->Content("A");
Grid::SetRow(buttonA, 0);
Grid::SetColumn(buttonA, 0);
grid->AddChild(buttonA);

auto buttonB = new Button()->Content("B");
Grid::SetRow(buttonB, 0);
Grid::SetColumn(buttonB, 1);
grid->AddChild(buttonB);
```

### 适用场景

- 最大灵活性
- 显式控制
- 学习理解机制

---

## 📚 常用场景速查

### 场景 1: 简单网格

```cpp
grid->Rows("*, *, *")
    ->Columns("*, *, *")
    ->Children({
        new Button() | cell(0, 0),
        new Button() | cell(0, 1),
        new Button() | cell(0, 2),
        new Button() | cell(1, 0),
        new Button() | cell(1, 1),
        new Button() | cell(1, 2),
        new Button() | cell(2, 0),
        new Button() | cell(2, 1),
        new Button() | cell(2, 2)
    });
```

### 场景 2: 跨行列

```cpp
grid->Rows("100, 100, 100")
    ->Columns("100, 100, 100")
    ->Children({
        new Button() | cell(0, 0).ColumnSpan(2),  // 跨 2 列
        new Button() | cell(1, 2).RowSpan(2),     // 跨 2 行
        new Button() | cell(0, 2)
    });
```

### 场景 3: 工具栏 + 内容 + 状态栏

```cpp
grid->Rows("60, *, 30")   // 顶栏、内容、底栏
    ->Columns("*")
    ->Children({
        toolbar   | cell(0, 0),
        content   | cell(1, 0),
        statusBar | cell(2, 0)
    });
```

### 场景 4: 三列布局（IDE 风格）

```cpp
grid->Rows("60, *, 30")          // 工具栏、内容、状态栏
    ->Columns("200, *, 200")     // 左边栏、主区、右边栏
    ->Children({
        toolbar      | cell(0, 0).ColumnSpan(3),
        leftSidebar  | cell(1, 0),
        editor       | cell(1, 1),
        rightSidebar | cell(1, 2),
        statusBar    | cell(2, 0).ColumnSpan(3)
    });
```

### 场景 5: 表单

```cpp
grid->Rows("Auto, Auto, Auto, *")
    ->Columns("Auto, *")
    ->Children({
        new TextBlock()->Text("用户名:") | cell(0, 0),
        new TextBox() | cell(0, 1),
        
        new TextBlock()->Text("密码:") | cell(1, 0),
        new TextBox() | cell(1, 1),
        
        new TextBlock()->Text("邮箱:") | cell(2, 0),
        new TextBox() | cell(2, 1),
        
        buttonPanel | cell(3, 0).ColumnSpan(2)
    });
```

---

## 🎨 行列定义速查

### 字符串解析（推荐）

```cpp
grid->Rows("Auto, *, 2*, 100");
grid->Columns("200, *, Auto");
```

### 手动添加

```cpp
grid->RowDefinitions({
    RowDefinition::Auto(),
    RowDefinition::Star(1),
    RowDefinition::Star(2),
    RowDefinition::Pixel(100)
});
```

### 带约束

```cpp
auto row = RowDefinition::Star(1)
    .MinHeight(100)
    .MaxHeight(500);
grid->AddRowDefinition(row);
```

---

## 🔧 cell() 函数速查

### 基本用法

```cpp
cell(row, col)                     // 指定位置
cell(row, col, rowSpan, colSpan)   // 指定位置和跨度
```

### 链式设置

```cpp
cell(0, 0).RowSpan(2)              // 跨 2 行
cell(0, 0).ColumnSpan(3)           // 跨 3 列
cell(0, 0).RowSpan(2).ColumnSpan(2) // 跨 2x2
```

---

## 💡 最佳实践

### ✅ 推荐

```cpp
// 使用 WPF 风格 + 字符串解析
grid->Rows("Auto, *, Auto")
    ->Columns("200, *")
    ->Children({
        header  | cell(0, 0).ColumnSpan(2),
        sidebar | cell(1, 0),
        content | cell(1, 1),
        footer  | cell(2, 0).ColumnSpan(2)
    });
```

### ❌ 不推荐

```cpp
// 混乱的缩进和格式
grid->Rows("Auto, *, Auto")->Columns("200, *")->Children({
new Button() | cell(0, 0),
new Button() | cell(0, 1)
});
```

---

## 🚨 常见错误

### 错误 1: 忘记定义行列

```cpp
// ❌ 错误
auto grid = new Grid();
grid->Children({
    new Button() | cell(5, 5)  // 行列不存在！
});
```

```cpp
// ✅ 正确
auto grid = new Grid();
grid->Rows("*, *, *, *, *, *")  // 至少 6 行
    ->Columns("*, *, *, *, *, *")  // 至少 6 列
    ->Children({
        new Button() | cell(5, 5)
    });
```

### 错误 2: 括号不匹配

```cpp
// ❌ 错误（缺少右括号）
grid->Children({
    (new StackPanel()
        ->Children({...})
    | cell(0, 0)
});
```

```cpp
// ✅ 正确
grid->Children({
    (new StackPanel()
        ->Children({...})
    ) | cell(0, 0)  // 注意右括号
});
```

---

## 📖 完整示例

```cpp
#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"

int main() {
    // 创建主窗口 Grid
    auto mainGrid = new Grid();
    
    // 定义行列（工具栏、内容区、状态栏 × 左边栏、主区、右边栏）
    mainGrid->Rows("60, *, 30")
            ->Columns("200, *, 200");
    
    // 创建各区域
    auto toolbar = new Border()->Background(0xFF2B2B2B);
    auto leftSidebar = new Border()->Background(0xFF1E1E1E);
    auto editor = new Border()->Background(0xFF252526);
    auto rightSidebar = new Border()->Background(0xFF1E1E1E);
    auto statusBar = new Border()->Background(0xFF007ACC);
    
    // 使用 WPF 风格 API 组装
    mainGrid->Children({
        toolbar      | cell(0, 0).ColumnSpan(3),
        leftSidebar  | cell(1, 0),
        editor       | cell(1, 1),
        rightSidebar | cell(1, 2),
        statusBar    | cell(2, 0).ColumnSpan(3)
    });
    
    // 测量和排列
    mainGrid->Measure(Size(1024, 768));
    mainGrid->Arrange(Rect(0, 0, 1024, 768));
    
    return 0;
}
```

---

## 🎯 选择哪种 API？

| 场景 | 推荐 API |
|------|----------|
| 复杂布局（多个子元素） | **WPF 风格** ✨ |
| 需要保留元素引用 | 流式 API |
| 动态添加/删除元素 | 传统 API 或流式 API |
| 学习理解机制 | 传统 API |
| 团队熟悉 WPF/WinUI | **WPF 风格** ✨ |
| 追求代码简洁 | **WPF 风格** ✨ |

**默认推荐**: **WPF 风格 API** 🏆

---

## 📚 更多资源

- [完整 WPF API 文档](./GRID_WPF_STYLE_API.md)
- [Grid 实现文档](./GRID_COMPLETE_IMPLEMENTATION.md)
- [Grid 快速参考](./GRID_QUICK_REFERENCE.md)
- [示例程序](./example_grid_wpf_style.cpp)

---

**开始你的第一个 Grid 布局吧！** 🚀
