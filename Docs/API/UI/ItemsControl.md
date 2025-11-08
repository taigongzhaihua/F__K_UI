# ItemsControl - 列表控件基类

数据驱动的列表控件，支持数据绑定、项模板、交替索引等特性。

## 概述

`ItemsControl` 是所有列表类控件的基类，提供：

- **数据绑定**：ItemsSource 属性绑定数据源
- **项模板**：ItemTemplate 定义每项的视觉表示
- **显示成员**：DisplayMemberPath 指定显示属性
- **交替样式**：AlternationCount 支持隔行变色
- **动态管理**：运行时添加/删除项

## 类层次结构

```
DependencyObject
  └─ DispatcherObject
      └─ UIElement
          └─ FrameworkElement
              └─ Control
                  └─ ItemsControl    ← 你在这里
                      ├─ ListBox（未来）
                      ├─ ComboBox（未来）
                      └─ 其他列表控件
```

## 基本用法

### 简单列表

```cpp
#include <fk/ui/ItemsControl.h>
#include <fk/ui/TextBlock.h>

auto list = std::make_shared<ItemsControl>();

// 添加项
list->AddItem(textBlock().Text("Item 1"));
list->AddItem(textBlock().Text("Item 2"));
list->AddItem(textBlock().Text("Item 3"));
```

### 数据绑定

```cpp
// 数据源
std::vector<std::any> data = {
    std::string("Apple"),
    std::string("Banana"),
    std::string("Cherry")
};

auto list = std::make_shared<ItemsControl>();
list->SetItemsSource(data);

// 设置项模板
list->SetItemTemplate([](const std::any& item) {
    auto text = std::any_cast<std::string>(item);
    return textBlock().Text(text);
});
```

## 核心属性

### ItemsSource - 数据源

```cpp
const std::vector<std::any>& GetItemsSource() const;
void SetItemsSource(std::vector<std::any> source);
```

绑定数据源，自动生成列表项。

**示例：**

```cpp
struct Person {
    std::string name;
    int age;
};

std::vector<std::any> people = {
    Person{"Alice", 25},
    Person{"Bob", 30},
    Person{"Charlie", 35}
};

list->SetItemsSource(people);
```

### ItemTemplate - 项模板

```cpp
using ItemTemplateFunc = std::function<std::shared_ptr<UIElement>(const std::any&)>;

ItemTemplateFunc GetItemTemplate() const;
void SetItemTemplate(ItemTemplateFunc templateFunc);
```

定义如何将数据项转换为 UI 元素。

**示例：**

```cpp
list->SetItemTemplate([](const std::any& item) {
    auto person = std::any_cast<Person>(item);
    
    return stackPanel()
        .Orientation(Orientation::Horizontal)
        .Children({
            textBlock().Text(person.name),
            textBlock().Text(" (Age: " + std::to_string(person.age) + ")")
        });
});
```

### DisplayMemberPath - 显示成员路径

```cpp
const std::string& GetDisplayMemberPath() const;
void SetDisplayMemberPath(std::string_view path);
```

指定显示对象的哪个属性（简化版模板）。

**示例：**

```cpp
// 如果数据项是字符串
std::vector<std::any> items = {
    std::string("Item 1"),
    std::string("Item 2")
};

list->SetItemsSource(items);
// DisplayMemberPath 为空时，直接使用 any_cast<std::string>
```

### AlternationCount - 交替计数

```cpp
int GetAlternationCount() const;
void SetAlternationCount(int count);
```

设置交替索引的循环周期，用于实现隔行变色等效果。

**示例：**

```cpp
// 启用隔行变色（0 和 1 交替）
list->SetAlternationCount(2);

list->SetItemTemplate([](const std::any& item) {
    auto text = textBlock().Text(std::any_cast<std::string>(item));
    
    // 根据交替索引设置背景色
    int index = ItemsControl::GetAlternationIndex(text.get());
    if (index % 2 == 0) {
        text->SetBackground(Color::White());
    } else {
        text->SetBackground(Color::LightGray());
    }
    
    return text;
});
```

## 主要方法

### AddItem - 添加项

```cpp
void AddItem(std::shared_ptr<UIElement> item);
```

手动添加一个 UI 元素作为列表项。

**示例：**

```cpp
list->AddItem(textBlock().Text("Item 1"));
list->AddItem(button().Content(textBlock().Text("Button Item")));
```

### RemoveItem - 移除项

```cpp
bool RemoveItem(UIElement* item);
```

移除指定的列表项，返回是否成功。

**示例：**

```cpp
auto item = textBlock().Text("Remove me");
list->AddItem(item);

// 移除
if (list->RemoveItem(item.get())) {
    std::cout << "Item removed" << std::endl;
}
```

### ClearItems - 清空列表

```cpp
void ClearItems();
```

移除所有列表项。

```cpp
list->ClearItems();  // 清空
```

### AppendItems - 批量添加

```cpp
void AppendItems(std::initializer_list<std::shared_ptr<UIElement>> items);
```

批量添加多个项。

**示例：**

```cpp
list->AppendItems({
    textBlock().Text("Item 1"),
    textBlock().Text("Item 2"),
    textBlock().Text("Item 3")
});
```

### InsertItem - 插入项

```cpp
void InsertItem(std::size_t index, std::shared_ptr<UIElement> item);
```

在指定位置插入项。

**示例：**

```cpp
list->InsertItem(0, textBlock().Text("First"));  // 插入到开头
```

### GetItemAt / GetItemCount

```cpp
UIElement* GetItemAt(std::size_t index) const;
std::size_t GetItemCount() const noexcept;
```

访问列表项。

**示例：**

```cpp
std::cout << "Total items: " << list->GetItemCount() << std::endl;

if (auto item = list->GetItemAt(0)) {
    std::cout << "First item exists" << std::endl;
}
```

### SetItemsPanel - 设置布局面板

```cpp
void SetItemsPanel(std::shared_ptr<PanelBase> panel);
std::shared_ptr<PanelBase> GetItemsPanel() const noexcept;
```

自定义列表项的布局容器（默认使用 StackPanel）。

**示例：**

```cpp
// 使用水平 StackPanel
auto panel = stackPanel().Orientation(Orientation::Horizontal);
list->SetItemsPanel(panel);

// 或使用 Grid
auto grid = std::make_shared<Grid>();
grid->ColumnDefinitions({
    ColumnDefinition::Auto(),
    ColumnDefinition::Star()
});
list->SetItemsPanel(grid);
```

### GetAlternationIndex - 获取交替索引（静态）

```cpp
static int GetAlternationIndex(DependencyObject* element);
```

获取元素的交替索引（附加属性）。

**示例：**

```cpp
list->SetItemTemplate([](const std::any& item) {
    auto text = textBlock().Text(std::any_cast<std::string>(item));
    
    // 稍后在其他地方获取索引
    int index = ItemsControl::GetAlternationIndex(text.get());
    
    return text;
});
```

## 完整示例

### 示例 1：简单字符串列表

```cpp
#include <fk/ui/ItemsControl.h>
#include <fk/ui/TextBlock.h>

int main() {
    auto list = std::make_shared<ItemsControl>();
    
    // 数据源
    std::vector<std::any> items = {
        std::string("Apple"),
        std::string("Banana"),
        std::string("Cherry")
    };
    
    list->SetItemsSource(items);
    
    // 项模板
    list->SetItemTemplate([](const std::any& item) {
        return textBlock()
            .Text(std::any_cast<std::string>(item))
            .FontSize(16);
    });
    
    // 添加到窗口...
}
```

### 示例 2：自定义数据结构

```cpp
struct Product {
    std::string name;
    double price;
    int stock;
};

std::vector<std::any> products = {
    Product{"Laptop", 999.99, 5},
    Product{"Mouse", 29.99, 50},
    Product{"Keyboard", 79.99, 20}
};

auto list = std::make_shared<ItemsControl>();
list->SetItemsSource(products);

list->SetItemTemplate([](const std::any& item) {
    auto product = std::any_cast<Product>(item);
    
    return stackPanel()
        .Orientation(Orientation::Horizontal)
        .Spacing(10)
        .Children({
            textBlock()
                .Text(product.name)
                .FontWeight(FontWeight::Bold),
            textBlock()
                .Text("$" + std::to_string(product.price)),
            textBlock()
                .Text("Stock: " + std::to_string(product.stock))
                .Foreground(product.stock > 0 ? Color::Green() : Color::Red())
        });
});
```

### 示例 3：隔行变色

```cpp
auto list = std::make_shared<ItemsControl>();
list->SetAlternationCount(2);  // 启用交替索引

std::vector<std::any> data = {
    std::string("Row 1"),
    std::string("Row 2"),
    std::string("Row 3"),
    std::string("Row 4")
};

list->SetItemsSource(data);

list->SetItemTemplate([](const std::any& item) {
    auto panel = stackPanel()
        .Padding(Thickness(10));
    
    auto text = textBlock()
        .Text(std::any_cast<std::string>(item));
    
    // 根据交替索引设置背景色
    int index = ItemsControl::GetAlternationIndex(panel.get());
    if (index % 2 == 0) {
        panel->SetBackground(Color::White());
    } else {
        panel->SetBackground(Color::FromRgb(240, 240, 240));
    }
    
    panel->AddChild(text);
    return panel;
});
```

### 示例 4：动态添加/删除

```cpp
auto list = std::make_shared<ItemsControl>();

// 动态添加按钮
auto addButton = button()
    .Content(textBlock().Text("Add Item"))
    .OnClick([list]() {
        static int count = 1;
        list->AddItem(
            textBlock().Text("Dynamic Item " + std::to_string(count++))
        );
    });

// 动态移除按钮
auto removeButton = button()
    .Content(textBlock().Text("Remove Last"))
    .OnClick([list]() {
        if (list->GetItemCount() > 0) {
            auto lastItem = list->GetItemAt(list->GetItemCount() - 1);
            list->RemoveItem(lastItem);
        }
    });

// 清空按钮
auto clearButton = button()
    .Content(textBlock().Text("Clear All"))
    .OnClick([list]() {
        list->ClearItems();
    });
```

### 示例 5：水平列表（自定义面板）

```cpp
auto list = std::make_shared<ItemsControl>();

// 设置水平布局
auto panel = stackPanel()
    .Orientation(Orientation::Horizontal)
    .Spacing(10);
list->SetItemsPanel(panel);

// 数据
std::vector<std::any> colors = {
    Color::Red(),
    Color::Green(),
    Color::Blue()
};

list->SetItemsSource(colors);

list->SetItemTemplate([](const std::any& item) {
    auto color = std::any_cast<Color>(item);
    
    return stackPanel()
        .Width(50)
        .Height(50)
        .Background(color);
});
```

### 示例 6：链式 API

```cpp
auto list = std::make_shared<ItemsControl>();

list->ItemsSource(dataVector)
    ->AlternationCount(2)
    ->ItemTemplate([](const std::any& item) {
        return textBlock().Text(std::any_cast<std::string>(item));
    })
    ->Width(300)
    ->Height(400);
```

## 数据绑定集成

### 与 ObservableObject 配合

```cpp
class ViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(std::vector<std::any>, Items);
};

// ViewModel
auto vm = std::make_shared<ViewModel>();
vm->SetItems({
    std::string("Item 1"),
    std::string("Item 2")
});

// 绑定
auto list = std::make_shared<ItemsControl>();
list->SetBinding(
    ItemsControl::ItemsSourceProperty(),
    binding::Binding()
        .Source(vm)
        .Path("Items")
);
```

## 最佳实践

### ✅ 推荐做法

**1. 使用 ItemsSource 而非手动添加**
```cpp
// ✅ 数据驱动
list->SetItemsSource(dataVector);
list->SetItemTemplate([](const std::any& item) { /*...*/ });

// ❌ 手动管理（不灵活）
for (const auto& data : dataVector) {
    list->AddItem(CreateItem(data));
}
```

**2. 项模板返回共享指针**
```cpp
list->SetItemTemplate([](const std::any& item) {
    return textBlock().Text(/*...*/);  // ✅ 返回 shared_ptr
});
```

**3. 合理使用交替索引**
```cpp
list->SetAlternationCount(2);  // 隔行变色
list->SetAlternationCount(3);  // 三色循环
```

**4. 清空时调用 ClearItems**
```cpp
list->ClearItems();  // ✅ 正确清理
```

### ❌ 避免的做法

**1. 混用 ItemsSource 和手动添加**
```cpp
list->SetItemsSource(data);
list->AddItem(item);  // ❌ 冲突！ItemsSource 会覆盖
```

**2. 在模板中修改数据源**
```cpp
list->SetItemTemplate([&data](const std::any& item) {
    data.push_back(/*...*/);  // ❌ 危险！可能导致迭代器失效
    return textBlock();
});
```

**3. 忘记设置 ItemTemplate**
```cpp
list->SetItemsSource(complexData);
// ❌ 没有设置模板，无法渲染！
```

## 常见问题

**Q: ItemsControl 和 Panel 有什么区别？**

A: `ItemsControl` 是**数据驱动**的，适合绑定数据源；`Panel` 是**手动管理**子元素的布局容器。

**Q: 如何实现动态更新？**

A: 重新设置 `ItemsSource` 或使用 `ObservableCollection`（未来支持）：
```cpp
// 方式 1：重新设置
data.push_back(newItem);
list->SetItemsSource(data);

// 方式 2：手动管理
list->AddItem(CreateItem(newItem));
```

**Q: ItemTemplate 可以返回 nullptr 吗？**

A: 不应该。返回 `nullptr` 会导致该项不显示或出错。

**Q: 如何获取点击的项？**

A: 在模板中为每项添加事件处理：
```cpp
list->SetItemTemplate([](const std::any& item) {
    auto btn = button()
        .Content(textBlock().Text(std::any_cast<std::string>(item)))
        .OnClick([item]() {
            std::cout << "Clicked: " << std::any_cast<std::string>(item) << std::endl;
        });
    return btn;
});
```

**Q: AlternationCount 的作用？**

A: 它定义交替索引的循环周期。`AlternationCount = 2` 时，索引为 0, 1, 0, 1, ...；`AlternationCount = 3` 时为 0, 1, 2, 0, 1, 2, ...

## 性能考虑

- **项数量**：数百项通常没问题，数千项考虑虚拟化（未来支持）
- **模板复杂度**：模板越复杂，性能开销越大
- **数据更新**：重新设置 ItemsSource 会重建所有项，性能开销 O(n)

**优化建议：**
- 简化项模板
- 避免频繁重建列表
- 考虑分页加载大数据集

## 相关类型

- **Control** - 父类
- **PanelBase** - 内部布局容器
- **ObservableObject** - 配合数据绑定
- **DependencyProperty** - 属性系统

## 总结

`ItemsControl` 是数据驱动的列表控件基类，提供：

✅ ItemsSource 数据绑定  
✅ ItemTemplate 项模板  
✅ DisplayMemberPath 显示属性  
✅ AlternationCount 交替样式  
✅ 动态添加/删除项

适用于列表、表格、数据展示等场景。
