# Grid API 完整参考手册

**版本**: 1.0.0  
**最后更新**: 2025-11-19

---

## 📑 目录

1. [快速开始](#快速开始)
2. [核心类型](#核心类型)
3. [Grid 类](#grid-类)
4. [附加属性](#附加属性)
5. [GridCellAttacher](#gridcellattacher)
6. [API 风格对比](#api-风格对比)
7. [完整示例](#完整示例)
8. [最佳实践](#最佳实践)

---

## 快速开始

### 最简单的例子（WPF 风格）

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

---

## 核心类型

### RowDefinition

行定义结构，描述网格行的尺寸行为。

#### 构造函数

```cpp
RowDefinition();                          // Auto 模式
RowDefinition(SizeType type, float value); // 指定类型和值
```

#### 静态工厂方法

```cpp
static RowDefinition Auto();              // 根据内容自动调整
static RowDefinition Pixel(float pixels); // 固定像素高度
static RowDefinition Star(float stars);   // 比例分配（默认 1.0）
```

#### 成员变量

```cpp
SizeType type;        // Auto, Pixel, Star
float value;          // 尺寸值（Star: 权重, Pixel: 像素）
float actualHeight;   // 测量后的实际高度（只读）
float minHeight;      // 最小高度约束（默认 0）
float maxHeight;      // 最大高度约束（默认无穷大）
```

#### 流式 API

```cpp
RowDefinition& MinHeight(float min);  // 设置最小高度
RowDefinition& MaxHeight(float max);  // 设置最大高度
```

#### 示例

```cpp
// Auto 行
auto row1 = RowDefinition::Auto();

// 固定 100px
auto row2 = RowDefinition::Pixel(100);

// 占 2 倍空间，限制 50-200px
auto row3 = RowDefinition::Star(2.0f)
    .MinHeight(50)
    .MaxHeight(200);
```

---

### ColumnDefinition

列定义结构，描述网格列的尺寸行为。

#### 构造函数

```cpp
ColumnDefinition();                          // Auto 模式
ColumnDefinition(SizeType type, float value); // 指定类型和值
```

#### 静态工厂方法

```cpp
static ColumnDefinition Auto();              // 根据内容自动调整
static ColumnDefinition Pixel(float pixels); // 固定像素宽度
static ColumnDefinition Star(float stars);   // 比例分配（默认 1.0）
```

#### 成员变量

```cpp
SizeType type;        // Auto, Pixel, Star
float value;          // 尺寸值（Star: 权重, Pixel: 像素）
float actualWidth;    // 测量后的实际宽度（只读）
float minWidth;       // 最小宽度约束（默认 0）
float maxWidth;       // 最大宽度约束（默认无穷大）
```

#### 流式 API

```cpp
ColumnDefinition& MinWidth(float min);  // 设置最小宽度
ColumnDefinition& MaxWidth(float max);  // 设置最大宽度
```

#### 示例

```cpp
// Auto 列
auto col1 = ColumnDefinition::Auto();

// 固定 200px
auto col2 = ColumnDefinition::Pixel(200);

// 占 3 倍空间，最少 100px
auto col3 = ColumnDefinition::Star(3.0f)
    .MinWidth(100);
```

---

## Grid 类

```cpp
class Grid : public Panel<Grid>
```

网格布局面板，按行列组织子元素。

### 构造函数

```cpp
Grid();  // 默认构造
```

### 行列定义方法

#### 添加单个定义

```cpp
Grid* AddRowDefinition(const RowDefinition& row);
Grid* AddColumnDefinition(const ColumnDefinition& col);
```

**参数**:
- `row`: 行定义
- `col`: 列定义

**返回**: `this` 指针（支持链式调用）

**示例**:
```cpp
grid->AddRowDefinition(RowDefinition::Star())
    ->AddRowDefinition(RowDefinition::Pixel(100))
    ->AddColumnDefinition(ColumnDefinition::Auto());
```

---

#### 批量添加定义

```cpp
Grid* RowDefinitions(std::initializer_list<RowDefinition> rows);
Grid* ColumnDefinitions(std::initializer_list<ColumnDefinition> cols);
```

**参数**:
- `rows`: 行定义列表
- `cols`: 列定义列表

**返回**: `this` 指针（支持链式调用）

**示例**:
```cpp
grid->RowDefinitions({
    RowDefinition::Auto(),
    RowDefinition::Star(1),
    RowDefinition::Star(2),
    RowDefinition::Pixel(50)
});

grid->ColumnDefinitions({
    ColumnDefinition::Pixel(200),
    ColumnDefinition::Star()
});
```

---

#### 字符串解析（推荐）

```cpp
Grid* Rows(const std::string& spec);
Grid* Columns(const std::string& spec);
```

**参数**:
- `spec`: 尺寸规格字符串

**字符串格式**:
- `"Auto"` - 自动尺寸
- `"100"` - 100 像素
- `"*"` - 1 星（比例分配）
- `"2*"` - 2 星（占 2 倍空间）
- 用逗号分隔多个定义

**返回**: `this` 指针（支持链式调用）

**示例**:
```cpp
// 顶栏 60px, 内容区自适应, 底栏 30px
grid->Rows("60, *, 30");

// 左边栏 200px, 主区自适应, 右边栏 200px
grid->Columns("200, *, 200");

// 混合使用
grid->Rows("Auto, *, 2*, 100");
//         自动  1倍 2倍  固定
```

---

#### 查询行列定义

```cpp
const std::vector<RowDefinition>& GetRowDefinitions() const;
const std::vector<ColumnDefinition>& GetColumnDefinitions() const;
```

**返回**: 当前行列定义的引用

**示例**:
```cpp
auto& rows = grid->GetRowDefinitions();
std::cout << "行数: " << rows.size() << std::endl;
for (size_t i = 0; i < rows.size(); ++i) {
    std::cout << "行 " << i << " 高度: " 
              << rows[i].actualHeight << std::endl;
}
```

---

### 附加属性 DependencyProperty

Grid 通过附加属性系统定义子元素位置。

#### 属性定义

```cpp
static const binding::DependencyProperty& RowProperty();
static const binding::DependencyProperty& ColumnProperty();
static const binding::DependencyProperty& RowSpanProperty();
static const binding::DependencyProperty& ColumnSpanProperty();
```

**返回**: 对应的依赖属性引用

---

## 附加属性

Grid 提供静态方法来设置子元素的位置和跨度。

### Grid.Row

指定元素所在的行索引（从 0 开始）。

```cpp
static void SetRow(UIElement* element, int row);
static int GetRow(UIElement* element);
```

**参数**:
- `element`: 目标元素
- `row`: 行索引

**默认值**: 0

**示例**:
```cpp
Grid::SetRow(button, 2);  // 放在第 3 行
int row = Grid::GetRow(button);
```

---

### Grid.Column

指定元素所在的列索引（从 0 开始）。

```cpp
static void SetColumn(UIElement* element, int col);
static int GetColumn(UIElement* element);
```

**参数**:
- `element`: 目标元素
- `col`: 列索引

**默认值**: 0

**示例**:
```cpp
Grid::SetColumn(button, 3);  // 放在第 4 列
int col = Grid::GetColumn(button);
```

---

### Grid.RowSpan

指定元素跨越的行数。

```cpp
static void SetRowSpan(UIElement* element, int span);
static int GetRowSpan(UIElement* element);
```

**参数**:
- `element`: 目标元素
- `span`: 跨越行数

**默认值**: 1

**示例**:
```cpp
Grid::SetRowSpan(button, 2);  // 跨越 2 行
int span = Grid::GetRowSpan(button);
```

---

### Grid.ColumnSpan

指定元素跨越的列数。

```cpp
static void SetColumnSpan(UIElement* element, int span);
static int GetColumnSpan(UIElement* element);
```

**参数**:
- `element`: 目标元素
- `span`: 跨越列数

**默认值**: 1

**示例**:
```cpp
Grid::SetColumnSpan(button, 3);  // 跨越 3 列
int span = Grid::GetColumnSpan(button);
```

---

## GridCellAttacher

WPF 风格的声明式 API 辅助结构。

### 构造函数

```cpp
GridCellAttacher(int row, int col);
GridCellAttacher(int row, int col, int rowSpan, int colSpan);
```

**参数**:
- `row`: 行索引
- `col`: 列索引
- `rowSpan`: 跨行数（可选）
- `colSpan`: 跨列数（可选）

---

### 成员方法

#### RowSpan

```cpp
GridCellAttacher& RowSpan(int span);
```

设置跨行数。

**参数**: `span` - 跨越行数

**返回**: `*this`（支持链式调用）

---

#### ColumnSpan

```cpp
GridCellAttacher& ColumnSpan(int span);
```

设置跨列数。

**参数**: `span` - 跨越列数

**返回**: `*this`（支持链式调用）

---

#### ApplyTo

```cpp
UIElement* ApplyTo(UIElement* element) const;
```

将位置信息应用到元素。

**参数**: `element` - 目标元素

**返回**: 元素指针（支持链式调用）

---

### 辅助函数

#### cell()

```cpp
GridCellAttacher cell(int row, int col);
GridCellAttacher cell(int row, int col, int rowSpan, int colSpan);
```

创建 `GridCellAttacher` 实例的便捷函数。

**示例**:
```cpp
cell(0, 0)                    // 位置 (0, 0)
cell(1, 2)                    // 位置 (1, 2)
cell(0, 0, 2, 2)              // 位置 (0, 0), 跨 2x2
cell(0, 0).RowSpan(3)         // 位置 (0, 0), 跨 3 行
cell(0, 0).ColumnSpan(2)      // 位置 (0, 0), 跨 2 列
```

---

### 管道操作符

```cpp
UIElement* operator|(UIElement* element, const GridCellAttacher& attacher);
```

WPF 风格的声明式语法。

**示例**:
```cpp
grid->Children({
    new Button()->Content("A") | cell(0, 0),
    new Button()->Content("B") | cell(0, 1).RowSpan(2),
    new TextBlock()->Text("C") | cell(1, 0)
});
```

---

## API 风格对比

Grid 支持三种 API 风格，可根据场景选择。

### 风格 1: WPF 声明式（推荐）✨

**优点**: 最简洁、最接近 WPF、结构最清晰

```cpp
grid->Rows("60, *, 30")
    ->Columns("200, *, 200")
    ->Children({
        toolbar      | cell(0, 0).ColumnSpan(3),
        leftSidebar  | cell(1, 0),
        editor       | cell(1, 1),
        rightSidebar | cell(1, 2),
        statusBar    | cell(2, 0).ColumnSpan(3)
    });
```

**对应 XAML**:
```xml
<Grid>
  <Grid.RowDefinitions>
    <RowDefinition Height="60"/>
    <RowDefinition Height="*"/>
    <RowDefinition Height="30"/>
  </Grid.RowDefinitions>
  <Grid.ColumnDefinitions>
    <ColumnDefinition Width="200"/>
    <ColumnDefinition Width="*"/>
    <ColumnDefinition Width="200"/>
  </Grid.ColumnDefinitions>
  
  <ToolBar Grid.Row="0" Grid.ColumnSpan="3"/>
  <Sidebar Grid.Row="1" Grid.Column="0"/>
  <Editor Grid.Row="1" Grid.Column="1"/>
  <Sidebar Grid.Row="1" Grid.Column="2"/>
  <StatusBar Grid.Row="2" Grid.ColumnSpan="3"/>
</Grid>
```

---

### 风格 2: 流式 API

**优点**: 可保留元素引用、适合动态添加

```cpp
grid->Rows("*, *")->Columns("*, *");

auto button1 = new Button()->Content("A");
grid->AddChild(button1)->Row(0)->Column(0);

auto button2 = new Button()->Content("B");
grid->AddChild(button2)->Row(0)->Column(1);

// 可以后续访问 button1, button2
```

---

### 风格 3: 传统 API

**优点**: 最大灵活性、显式控制

```cpp
grid->AddRowDefinition(RowDefinition::Star());
grid->AddRowDefinition(RowDefinition::Star());
grid->AddColumnDefinition(RowDefinition::Star());
grid->AddColumnDefinition(RowDefinition::Star());

auto button = new Button()->Content("A");
Grid::SetRow(button, 0);
Grid::SetColumn(button, 0);
grid->AddChild(button);
```

---

### 风格选择指南

| 场景 | 推荐风格 |
|------|----------|
| 复杂静态布局 | **WPF 声明式** ✨ |
| 需要保留元素引用 | 流式 API |
| 动态添加/删除 | 流式 API 或传统 API |
| 学习理解机制 | 传统 API |
| 从 WPF 迁移 | **WPF 声明式** ✨ |
| 追求代码简洁 | **WPF 声明式** ✨ |

---

## 完整示例

### 示例 1: 简单网格

```cpp
#include "fk/ui/Grid.h"
#include "fk/ui/Button.h"

void SimpleGrid() {
    auto grid = new Grid();
    
    // 3x3 网格
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
}
```

---

### 示例 2: 跨行列

```cpp
void SpanningExample() {
    auto grid = new Grid();
    
    grid->Rows("100, 100, 100")
        ->Columns("100, 100, 100")
        ->Children({
            // 左上角，跨 2 列
            new Button()->Content("Wide")
                | cell(0, 0).ColumnSpan(2),
            
            // 右上角
            new Button()->Content("A") | cell(0, 2),
            
            // 中间，跨 2 行
            new Button()->Content("Tall")
                | cell(1, 0).RowSpan(2),
            
            // 中间右侧，跨 2x2
            new Button()->Content("Big")
                | cell(1, 1).RowSpan(2).ColumnSpan(2)
        });
}
```

---

### 示例 3: 应用程序布局

```cpp
void AppLayout() {
    auto grid = new Grid();
    
    // 经典应用布局: 工具栏 + 内容 + 状态栏
    grid->Rows("60, *, 30")
        ->Columns("200, *, 200")
        ->Children({
            // 工具栏（跨全部列）
            new Border()->Background(0xFF2B2B2B)
                | cell(0, 0).ColumnSpan(3),
            
            // 左边栏
            new Border()->Background(0xFF1E1E1E)
                | cell(1, 0),
            
            // 主编辑区
            new Border()->Background(0xFF252526)
                | cell(1, 1),
            
            // 右边栏
            new Border()->Background(0xFF1E1E1E)
                | cell(1, 2),
            
            // 状态栏（跨全部列）
            new Border()->Background(0xFF007ACC)
                | cell(2, 0).ColumnSpan(3)
        });
}
```

---

### 示例 4: 表单布局

```cpp
void FormLayout() {
    auto grid = new Grid();
    
    // 左列: 标签（Auto）
    // 右列: 输入框（*）
    grid->Rows("Auto, Auto, Auto, *")
        ->Columns("Auto, *")
        ->Children({
            // 第 1 行
            new TextBlock()->Text("用户名:") | cell(0, 0),
            new TextBox() | cell(0, 1),
            
            // 第 2 行
            new TextBlock()->Text("密码:") | cell(1, 0),
            new TextBox() | cell(1, 1),
            
            // 第 3 行
            new TextBlock()->Text("邮箱:") | cell(2, 0),
            new TextBox() | cell(2, 1),
            
            // 第 4 行（按钮面板跨 2 列）
            CreateButtonPanel() | cell(3, 0).ColumnSpan(2)
        });
}
```

---

### 示例 5: 约束使用

```cpp
void ConstrainedGrid() {
    auto grid = new Grid();
    
    // 使用流式 API 设置约束
    grid->RowDefinitions({
        RowDefinition::Auto(),
        RowDefinition::Star(1)
            .MinHeight(200)
            .MaxHeight(600),
        RowDefinition::Pixel(50)
    });
    
    grid->ColumnDefinitions({
        ColumnDefinition::Star(1)
            .MinWidth(150)
            .MaxWidth(300),
        ColumnDefinition::Star(2)
    });
    
    grid->Children({
        new TextBlock()->Text("Header") | cell(0, 0).ColumnSpan(2),
        new Border() | cell(1, 0),
        new Border() | cell(1, 1),
        new TextBlock()->Text("Footer") | cell(2, 0).ColumnSpan(2)
    });
}
```

---

### 示例 6: 混合 API 风格

```cpp
void MixedApiStyle() {
    auto grid = new Grid();
    
    // WPF 风格定义行列
    grid->Rows("*, *")->Columns("*, *");
    
    // 传统风格添加第一个元素
    auto button1 = new Button()->Content("Traditional");
    Grid::SetRow(button1, 0);
    Grid::SetColumn(button1, 0);
    grid->AddChild(button1);
    
    // 流式风格添加第二个元素
    auto button2 = grid->AddChild(new Button()->Content("Fluent"));
    button2->Row(0)->Column(1);
    
    // WPF 风格批量添加
    grid->Children({
        new Button()->Content("WPF Style 1") | cell(1, 0),
        new Button()->Content("WPF Style 2") | cell(1, 1)
    });
}
```

---

## 最佳实践

### ✅ 推荐做法

#### 1. 使用字符串解析定义行列

```cpp
// ✅ 简洁清晰
grid->Rows("Auto, *, 2*, 100");
grid->Columns("200, *, Auto");
```

```cpp
// ❌ 冗长
grid->AddRowDefinition(RowDefinition::Auto());
grid->AddRowDefinition(RowDefinition::Star(1));
grid->AddRowDefinition(RowDefinition::Star(2));
grid->AddRowDefinition(RowDefinition::Pixel(100));
```

---

#### 2. WPF 风格用于复杂布局

```cpp
// ✅ 结构清晰
grid->Rows("60, *, 30")
    ->Columns("200, *, 200")
    ->Children({
        toolbar  | cell(0, 0).ColumnSpan(3),
        sidebar1 | cell(1, 0),
        content  | cell(1, 1),
        sidebar2 | cell(1, 2),
        status   | cell(2, 0).ColumnSpan(3)
    });
```

---

#### 3. 使用约束限制尺寸

```cpp
// ✅ 防止过小或过大
auto row = RowDefinition::Star(1)
    .MinHeight(100)   // 最少 100px
    .MaxHeight(500);  // 最多 500px

auto col = ColumnDefinition::Star(1)
    .MinWidth(150)
    .MaxWidth(300);
```

---

#### 4. 合理使用 Auto 和 Star

```cpp
// ✅ 标签用 Auto，输入框用 Star
grid->Columns("Auto, *");

// ✅ 固定头尾，内容自适应
grid->Rows("60, *, 30");
```

---

#### 5. 跨行列时使用链式调用

```cpp
// ✅ 清晰表达意图
new Button() | cell(0, 0).RowSpan(2).ColumnSpan(3)

// ❌ 分散设置
auto button = new Button();
Grid::SetRow(button, 0);
Grid::SetColumn(button, 0);
Grid::SetRowSpan(button, 2);
Grid::SetColumnSpan(button, 3);
```

---

### ❌ 避免的错误

#### 1. 行列不足

```cpp
// ❌ 只有 2 行 2 列，但访问 (5, 5)
grid->Rows("*, *")->Columns("*, *");
grid->Children({
    new Button() | cell(5, 5)  // 超出范围！
});
```

```cpp
// ✅ 确保行列足够
grid->Rows("*, *, *, *, *, *")
    ->Columns("*, *, *, *, *, *");
grid->Children({
    new Button() | cell(5, 5)
});
```

---

#### 2. 括号不匹配

```cpp
// ❌ 复杂元素缺少外层括号
grid->Children({
    new StackPanel()->Children({...})
    | cell(0, 0)  // 语法错误！
});
```

```cpp
// ✅ 添加括号
grid->Children({
    (new StackPanel()->Children({...}))
    | cell(0, 0)
});
```

---

#### 3. 忘记链式返回

```cpp
// ❌ Rows() 返回 this, 不要丢弃
auto grid = new Grid();
grid->Rows("*, *");  // 返回 grid
grid->Columns("*, *");  // 分开调用
```

```cpp
// ✅ 链式调用
auto grid = new Grid();
grid->Rows("*, *")
    ->Columns("*, *");
```

---

#### 4. 过度使用 Pixel

```cpp
// ❌ 所有尺寸都固定，无法自适应
grid->Rows("100, 200, 300");
grid->Columns("150, 250, 350");
```

```cpp
// ✅ 混合使用，保持灵活
grid->Rows("Auto, *, 50");
grid->Columns("200, *, 200");
```

---

## 性能考虑

### 测量缓存

Grid 自动缓存测量结果，当 `availableSize` 相同时避免重复计算。

```cpp
// 第一次测量：完整计算
grid->Measure(Size(800, 600));

// 第二次测量：如果尺寸相同，使用缓存
grid->Measure(Size(800, 600));  // 快速返回
```

---

### 约束优化

- **使用约束**: 防止 Star 分配极端值
- **避免过多 Auto**: Auto 需要测量所有子元素
- **合理使用 Pixel**: Pixel 计算最快

```cpp
// ✅ 性能较好
grid->Rows("100, *, *");  // 1 个 Pixel, 2 个 Star

// ⚠️ 性能一般
grid->Rows("Auto, Auto, Auto");  // 3 个 Auto 都需要测量
```

---

## 常见问题

### Q: Star 如何分配空间？

**A**: 按权重比例分配剩余空间。

```cpp
grid->Rows("*, 2*, 3*");  // 比例 1:2:3
// 如果剩余 600px:
// Row 0: 100px (1/6 * 600)
// Row 1: 200px (2/6 * 600)
// Row 2: 300px (3/6 * 600)
```

---

### Q: Auto 如何计算尺寸？

**A**: 测量该行/列中所有子元素的期望尺寸，取最大值。

```cpp
grid->Rows("Auto, *");
// Row 0 的高度 = max(子元素的 DesiredSize.Height)
```

---

### Q: 约束何时生效？

**A**: 在分配 Star 空间时应用约束。

```cpp
auto row = RowDefinition::Star(1)
    .MinHeight(100)
    .MaxHeight(200);

// 如果计算出 250px，会被限制为 200px
// 如果计算出 50px，会被提升为 100px
```

---

### Q: 可以动态修改行列吗？

**A**: 可以，重新设置后调用 `InvalidateMeasure()` 触发重新布局。

```cpp
grid->Rows("*, *");  // 初始 2 行
// ... 使用一段时间

grid->RowDefinitions({  // 改为 3 行
    RowDefinition::Star(),
    RowDefinition::Star(),
    RowDefinition::Star()
});
grid->InvalidateMeasure();  // 触发重新布局
```

---

### Q: 三种 API 可以混用吗？

**A**: 可以，它们操作的是同一个底层系统。

```cpp
// 混合使用没问题
grid->Rows("*, *");  // WPF 风格

auto btn1 = new Button();
Grid::SetRow(btn1, 0);  // 传统风格
grid->AddChild(btn1);

auto btn2 = grid->AddChild(new Button());
btn2->Column(1);  // 流式风格

grid->Children({
    new Button() | cell(1, 0)  // WPF 风格
});
```

---

## 相关文档

- [Grid 完整实现文档](./GRID_COMPLETE_IMPLEMENTATION.md)
- [Grid 快速参考](./GRID_QUICK_REFERENCE.md)
- [Grid WPF 风格 API](./GRID_WPF_STYLE_API.md)
- [Grid 快速开始](./GRID_QUICK_START.md)
- [示例程序](./example_grid_wpf_style.cpp)
- [测试程序](./test_grid_complete.cpp)

---

## 版本历史

### 1.0.0 (2025-11-19)

- ✅ 完整的 Grid 实现
- ✅ Auto/Pixel/Star 三种尺寸模式
- ✅ Min/Max 约束支持
- ✅ 字符串解析 API
- ✅ WPF 风格管道操作符
- ✅ 性能缓存优化
- ✅ DependencyProperty 附加属性
- ✅ 完整测试覆盖

---

**F__K_UI Framework** - 让 C++ UI 开发更优雅 ✨
