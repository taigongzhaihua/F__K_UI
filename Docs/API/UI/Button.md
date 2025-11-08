# Button API 文档

## 概述

`Button` 是一个交互式控件，允许用户通过点击触发命令。支持圆角、自定义颜色、悬停/按下状态反馈，以及 MVVM 风格的命令绑定。

**继承链：** `UIElement` → `FrameworkElement` → `ControlBase` → `Button<Derived>`

**命名空间：** `fk::ui`

**模板参数：** `Derived = void`（通常省略，自动推导）

---

## 核心属性（Dependency Properties）

### Background
- **类型：** `std::string`（16 进制颜色码）
- **默认值：** `"#0078D4"`（Fluent Accent Blue）
- **格式：** `"#RRGGBB"` 或 `"#RRGGBBAA"`
- **说明：** 按钮的背景颜色

**用法：**
```cpp
button->Background("#4CAF50");  // 绿色
```

### Foreground
- **类型：** `std::string`
- **默认值：** `"#FFFFFF"`（白色）
- **说明：** 按钮内容（通常为文字）的前景色

**用法：**
```cpp
button->Foreground("#FF0000");  // 红色文字
```

### CornerRadius
- **类型：** `float`
- **默认值：** `4.0f`
- **说明：** 按钮四个角的圆角半径（像素）

**用法：**
```cpp
button->CornerRadius(8.0f);  // 8 像素的圆角
```

### BorderBrush
- **类型：** `std::string`
- **默认值：** `"#0078D400"`（透明）
- **说明：** 按钮边框的颜色

**用法：**
```cpp
button->BorderBrush("#000000");  // 黑色边框
```

### BorderThickness
- **类型：** `float`
- **默认值：** `0.0f`
- **说明：** 边框宽度（像素）

**用法：**
```cpp
button->BorderThickness(2.0f);  // 2 像素边框
```

### HoveredBackground
- **类型：** `std::string`
- **默认值：** `""` (空，自动计算)
- **说明：** 鼠标悬停时的背景颜色，空值时自动在基础颜色上微妙变亮（乘以 1.1）

**用法：**
```cpp
button->HoveredBackground("#FF5733");  // 明确指定悬停色
```

### PressedBackground
- **类型：** `std::string`
- **默认值：** `""` (空，自动计算)
- **说明：** 鼠标按下时的背景颜色，空值时自动在基础颜色上变暗（乘以 0.8）

**用法：**
```cpp
button->PressedBackground("#003D82");  // 明确指定按下色
```

### IsMouseOver
- **类型：** `bool`
- **默认值：** `false`
- **只读：** 通常由按钮内部更新，不应手动设置
- **说明：** 鼠标是否在按钮上方

### IsPressed
- **类型：** `bool`
- **默认值：** `false`
- **只读：** 通常由按钮内部更新
- **说明：** 鼠标是否按下

### Command
- **类型：** `ICommand::Ptr` (shared_ptr\<ICommand\>)
- **默认值：** `nullptr`
- **说明：** 绑定的命令对象，点击时执行该命令
- **绑定支持：** ✅ 支持数据绑定

**用法：**
```cpp
// 创建一个简单命令
auto command = ui::relayCommand(
    [](const std::any& param) {
        std::cout << "执行命令" << std::endl;
    },
    [](const std::any& param) {
        return true;  // CanExecute
    }
);

button->Command(command);

// 或使用数据绑定
button->Command(bind("MyCommand"));
```

### CommandParameter
- **类型：** `std::any`
- **默认值：** 空
- **说明：** 传递给命令的参数
- **绑定支持：** ✅ 支持数据绑定

**用法：**
```cpp
button->CommandParameter(42);
button->Command(command);  // 点击时调用 command->Execute(42)
```

---

## 内容设置方法

### Ptr Content(ContentPtr content)
- **参数：** 作为按钮内容的 `UIElement`
- **返回值：** 返回 `this`，支持链式调用
- **说明：** 设置按钮的内容为一个 UI 元素

**用法：**
```cpp
button->Content(
    ui::stackPanel()
        ->Children({
            ui::textBlock()->Text("下载"),
            ui::textBlock()->Text("✓")
        })
);
```

### Ptr Content(const std::string& text)
- **参数：** 文本字符串
- **返回值：** 返回 `this`
- **说明：** 便捷方法，自动创建 `TextBlock` 作为内容
  - 字体大小：14.0f
  - 颜色：继承按钮的 `Foreground` 颜色

**用法：**
```cpp
button->Content("点击我");  // 快速添加文本内容
```

### Ptr Content(binding::Binding binding)
- **参数：** 数据绑定对象
- **说明：** 将内容绑定到 ViewModel 属性

---

## 事件方法

### Ptr OnClick(std::function\<void(ButtonBase&)\> handler)
- **参数：** 点击事件处理函数
- **返回值：** 返回 `this`
- **说明：** 订阅按钮点击事件（当没有绑定 `Command` 时触发）

**用法：**
```cpp
button->OnClick([](ButtonBase& btn) {
    std::cout << "按钮被点击！" << std::endl;
});
```

### core::Event\<ButtonBase&\> Click
- **说明：** 底层点击事件信号
- **触发时机：** 鼠标在按钮上方松开时
- **优先级：** 如果绑定了 `Command`，优先执行命令，然后触发 `Click` 事件

**用法：**
```cpp
button->Click += [](ButtonBase& btn) { /* ... */ };
```

---

## 便捷方法

### Ptr Command(ICommand::Ptr command)
- **参数：** 命令对象指针
- **返回值：** 返回 `this`
- **说明：** 设置按钮的命令

### Ptr Command(binding::Binding binding)
- **参数：** 数据绑定
- **说明：** 绑定命令到 ViewModel 属性

### Ptr CommandParameter(std::any parameter)
- **参数：** 任意类型的参数
- **返回值：** 返回 `this`

### Ptr CommandParameter(binding::Binding binding)
- **参数：** 数据绑定
- **说明：** 绑定参数到 ViewModel 属性

---

## 从 Control 继承的属性和方法

### Padding
```cpp
button->Padding(12.0f, 8.0f, 12.0f, 9.0f);  // 左、上、右、下
button->Padding(10.0f);  // 统一内边距
```

### Content（只读 getter）
```cpp
auto content = button->Content();
```

### IsFocused
```cpp
button->IsFocused(true);
```

---

## 从 UIElement 继承的属性

- `Width`, `Height` - 尺寸
- `Opacity` - 透明度
- `IsEnabled` - 是否启用
- `Visibility` - 可见性
- `Name` - 元素名称
- `HorizontalAlignment`, `VerticalAlignment` - 对齐方式
- 更多详见 [UIElement.md](UIElement.md)

---

## 命令系统集成

### 自动启用/禁用

当按钮绑定了命令时，按钮的 `IsEnabled` 属性会自动根据命令的 `CanExecute()` 结果同步：

```cpp
auto command = ui::relayCommand(
    [](const std::any& p) { /* ... */ },
    [](const std::any& p) {
        return condition;  // 返回 false 时按钮自动禁用
    }
);

button->Command(command);
// 当 condition 变化时，按钮自动启用/禁用

// 通知 CanExecute 状态变化
if (auto cmd = std::dynamic_pointer_cast<ui::RelayCommand>(command)) {
    cmd->RaiseCanExecuteChanged();  // 按钮状态同步更新
}
```

---

## 样式属性查询

### std::string GetActualBackground() const
- **返回值：** 根据当前状态（Normal/Hover/Pressed）计算的实际背景颜色
- **说明：** 用于获取渲染器使用的最终颜色

---

## 工厂函数

### static Ptr Create()
```cpp
auto button = Button<>::Create();
```

### 全局便捷函数
```cpp
auto button = ui::button()
    ->Content("点击")
    ->Width(100)
    ->Height(40);
```

---

## 完整使用示例

### 简单按钮

```cpp
auto button = ui::button()
    ->Content("保存")
    ->Width(100)
    ->Height(40)
    ->Background("#4CAF50")
    ->Foreground("#FFFFFF")
    ->OnClick([](auto&) {
        std::cout << "保存成功" << std::endl;
    });
```

### 带命令的 MVVM 按钮

```cpp
// ViewModel 中
class MyViewModel : public fk::ObservableObject {
public:
    MyViewModel() {
        SetCanSave(true);
    }
    
    FK_PROPERTY(bool, CanSave)
    
    void SaveData() {
        std::cout << "数据已保存" << std::endl;
        SetCanSave(false);
    }
};

// UI 中
auto viewModel = std::make_shared<MyViewModel>();

auto saveCommand = ui::relayCommand(
    [viewModel](const std::any&) {
        viewModel->SaveData();
    },
    [viewModel](const std::any&) {
        return viewModel->GetCanSave();
    }
);

auto button = ui::button()
    ->Content("保存")
    ->Command(saveCommand);

mainWindow->SetDataContext(
    std::static_pointer_cast<fk::INotifyPropertyChanged>(viewModel)
);
```

### 复杂内容按钮

```cpp
auto complexButton = ui::button()
    ->Width(150)
    ->Height(60)
    ->CornerRadius(8.0f)
    ->Content(
        ui::stackPanel()
            ->Orientation(ui::Orientation::Vertical)
            ->Spacing(5.0f)
            ->Children({
                ui::textBlock()
                    ->Text("下载文件")
                    ->FontSize(14.0f),
                ui::textBlock()
                    ->Text("1.5 MB")
                    ->FontSize(12.0f)
                    ->Foreground("#CCCCCC")
            })
    )
    ->OnClick([](auto&) {
        std::cout << "开始下载..." << std::endl;
    });
```

---

## 视觉反馈

按钮在不同状态下的视觉变化：

| 状态 | 背景色计算 | 触发条件 |
|------|----------|--------|
| Normal | `Background` 属性值 | 初始状态 |
| Hovered | `HoveredBackground` 或 `Background * 1.1` | 鼠标进入 |
| Pressed | `PressedBackground` 或 `Background * 0.8` | 鼠标按下 |
| Disabled | 自动灰化（`IsEnabled=false`） | 通常由命令 `CanExecute` 返回 false 触发 |

---

## 性能考虑

- **频繁更改按钮外观：** 每次属性变化会触发重绘，考虑批量更新
- **大量按钮列表：** 使用虚拟化容器（如 `ItemsControl` 与虚拟化）
- **命令频繁调用 `RaiseCanExecuteChanged()`：** 如果状态变化频繁，考虑使用防抖

---

## 另见

- [ICommand.md](../Core/ICommand.md) - 命令系统
- [Control.md](Control.md) - 控件基类
- [TextBlock.md](TextBlock.md) - 文本显示

