# Control 文档

## 概述

`Control` 是所有交互控件的基类，在 `FrameworkElement` 基础上添加了以下能力：
- 内容管理（Content）
- 焦点控制
- 填充与背景
- 前景色与字体样式

常见的派生类包括 Button、TextBox、CheckBox 等。

**继承链：** `UIElement` → `FrameworkElement` → `Control`

**命名空间：** `fk::ui`

**职责：**
- ✅ 管理控件内容
- ✅ 支持焦点管理
- ✅ 提供样式属性（Background、Foreground、FontSize 等）
- ✅ 为派生类提供通用能力

---

## 核心属性

### Content
- **类型：** `std::shared_ptr<UIElement>`
- **默认值：** `nullptr`
- **说明：** 控件的主要内容，通常是 TextBlock、Image、Panel 等
- **可绑定：** ✅ 是

**用法：**
```cpp
// 设置文本内容（自动包装为 TextBlock）
ui::button()->Content("点击我");

// 设置复杂内容
auto contentPanel = ui::stackPanel()
    ->Children({
        ui::image()->Source("icon.png"),
        ui::textBlock()->Text("带图标的按钮")
    });

ui::button()->Content(contentPanel);
```

### ContentTemplated
- **类型：** `bool`
- **默认值：** `false`
- **说明：** Content 是否通过模板化方式显示
- **用途：** 在 DataTemplate 场景中使用

---

## 焦点管理

### IsFocused
- **类型：** `bool`
- **默认值：** `false`
- **说明：** 控件是否具有焦点
- **只读：** 只能通过 `Focus()` 方法改变

```cpp
if (button->IsFocused()) {
    // 控件有焦点
}
```

### Focusable
- **类型：** `bool`
- **默认值：** `true` (对交互控件)
- **说明：** 控件是否可获得焦点
- **可绑定：** ✅ 是

```cpp
ui::button()->Focusable(true);
ui::textBlock()->Focusable(false);  // TextBlock 不能获得焦点
```

### Focus()
- **说明：** 设置控件为焦点
- **返回值：** `true` 成功，`false` 失败

```cpp
auto button = ui::button();
button->Focus();  // 设置焦点
```

### Blur()
- **说明：** 取消控件的焦点

```cpp
button->Blur();
```

---

## 样式属性

### Background
- **类型：** `Color`
- **默认值：** 依赖于具体控件
- **说明：** 控件的背景色
- **可绑定：** ✅ 是

```cpp
ui::button()
    ->Background(Color::FromRGB(200, 200, 200));

ui::button()
    ->Background(Color::FromRGBA(255, 0, 0, 128));  // 半透明红色
```

### Foreground
- **类型：** `Color`
- **默认值：** 依赖于具体控件
- **说明：** 控件的前景色（通常用于文本）
- **可绑定：** ✅ 是

```cpp
ui::button()
    ->Content("蓝色按钮")
    ->Foreground(Color::FromRGB(0, 0, 255));
```

### CornerRadius
- **类型：** `float`
- **默认值：** `0.0f`
- **说明：** 控件圆角半径（像素）

```cpp
ui::button()
    ->CornerRadius(5.0f);  // 5 像素圆角
```

### BorderBrush
- **类型：** `Color`
- **默认值：** 依赖于控件
- **说明：** 边框色

```cpp
ui::textBox()->BorderBrush(Color::FromRGB(0, 0, 0));
```

### BorderThickness
- **类型：** `float`
- **默认值：** `0.0f`
- **说明：** 边框宽度（像素）

```cpp
ui::textBox()->BorderThickness(1.0f);
```

### Padding
- **类型：** `Thickness`
- **默认值：** 依赖于控件
- **说明：** 内边距（Content 到边框的距离）

```cpp
ui::button()
    ->Padding({10, 5, 10, 5});  // 左, 上, 右, 下
```

---

## 字体和文本样式

### FontFamily
- **类型：** `std::string`
- **默认值：** `"Arial"`
- **说明：** 字体名称
- **可绑定：** ✅ 是

```cpp
ui::button()->FontFamily("Microsoft YaHei");
```

### FontSize
- **类型：** `float`
- **默认值：** `12.0f`
- **说明：** 字体大小（像素）
- **可绑定：** ✅ 是

```cpp
ui::button()->FontSize(14.0f);
```

### FontWeight
- **类型：** `FontWeight`
- **默认值：** `FontWeight::Normal`
- **说明：** 字体粗细
- **可选值：** Thin、Light、Normal、SemiBold、Bold、ExtraBold

```cpp
ui::button()->FontWeight(FontWeight::Bold);
```

### FontStyle
- **类型：** `FontStyle`
- **默认值：** `FontStyle::Normal`
- **说明：** 字体样式
- **可选值：** Normal、Italic、Oblique

```cpp
ui::textBlock()->FontStyle(FontStyle::Italic);
```

---

## 从 FrameworkElement 继承的属性

### Width / Height / Margin / MinWidth / MaxWidth 等
详见 [FrameworkElement.md](FrameworkElement.md)

---

## 典型控件列表

### Button
交互按钮，支持点击事件和 Command 绑定。
- **详见：** [Button.md](Button.md)

### TextBox
文本输入控件。

### CheckBox
复选框。

### ComboBox
下拉列表。

### RadioButton
单选按钮。

### Slider
滑块（取值范围）。

### ProgressBar
进度条。

---

## 完整使用示例

### 示例 1：样式化按钮

```cpp
auto styledButton = ui::button()
    ->Content("保存")
    ->Background(Color::FromRGB(0, 102, 204))    // 蓝色背景
    ->Foreground(Color::FromRGB(255, 255, 255))  // 白色文本
    ->FontSize(14.0f)
    ->FontWeight(FontWeight::Bold)
    ->Padding({15, 10, 15, 10})
    ->CornerRadius(5.0f)
    ->Width(100.0f)
    ->Height(40.0f)
    ->Focusable(true);
```

### 示例 2：焦点管理

```cpp
auto textBox = ui::textBox();
auto submitButton = ui::button()->Content("提交");

// 在 TextBox 中按 Enter 时移动焦点到按钮
textBox->OnKeyDown([submitButton](KeyEventArgs& e) {
    if (e.key == Key::Return) {
        submitButton->Focus();
    }
});
```

### 示例 3：主题一致的控件组

```cpp
// 定义一致的样式
auto applyTheme = [](Control* ctrl) {
    ctrl->FontFamily("Segoe UI");
    ctrl->FontSize(13.0f);
    ctrl->Padding({8, 6, 8, 6});
    ctrl->Background(Color::FromRGB(245, 245, 245));
    ctrl->BorderThickness(1.0f);
    ctrl->BorderBrush(Color::FromRGB(200, 200, 200));
};

// 应用到控件
auto btn1 = ui::button()->Content("按钮 1");
applyTheme(btn1.get());

auto btn2 = ui::button()->Content("按钮 2");
applyTheme(btn2.get());

auto textBox = ui::textBox();
applyTheme(textBox.get());
```

### 示例 4：Content 复杂布局

```cpp
auto complexButton = ui::button()
    ->Content(
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(8.0f)
            ->Padding({5, 5, 5, 5})
            ->Children({
                ui::image()
                    ->Source("download_icon.png")
                    ->Width(16.0f)
                    ->Height(16.0f),
                ui::textBlock()
                    ->Text("下载文件")
                    ->VerticalAlignment(VerticalAlignment::Center)
            })
    );
```

---

## 常见问题

### Q1：如何创建禁用的控件？
**A：** 使用 `IsEnabled` 属性（继承自 UIElement）：
```cpp
ui::button()
    ->Content("禁用按钮")
    ->IsEnabled(false);
```

### Q2：如何动态改变控件样式？
**A：** 在运行时改变样式属性：
```cpp
auto button = ui::button()->Content("按钮");

// 鼠标进入时改变背景色
button->OnMouseEnter([button](MouseEventArgs&) {
    button->Background(Color::FromRGB(100, 100, 100));
});

// 鼠标离开时恢复
button->OnMouseLeave([button](MouseEventArgs&) {
    button->Background(Color::FromRGB(200, 200, 200));
});
```

### Q3：Content 是否支持绑定？
**A：** 支持。可以绑定到 ViewModel 的属性：
```cpp
auto button = ui::button();
button->Bind(binding::Bind(viewModel->ButtonLabelProperty()));
```

### Q4：为什么某些控件的焦点改变不生效？
**A：** 检查以下几点：
1. 控件的 `Focusable` 是否为 `true`
2. 控件是否在可见的窗口中
3. 控件是否被禁用（IsEnabled = false）
4. 控件是否被隐藏（Visibility = Collapsed）

### Q5：Padding 和 Margin 的区别？
**A：**
- **Padding：** 内边距，Content 到控件边界的距离
- **Margin：** 外边距，控件到父容器的距离
- 设置位置：Padding 在 Control，Margin 在 FrameworkElement

---

## 样式最佳实践

1. **使用主题色：**
   ```cpp
   struct Theme {
       static constexpr Color PrimaryColor = Color::FromRGB(0, 102, 204);
       static constexpr Color AccentColor = Color::FromRGB(255, 102, 0);
       static constexpr Color TextColor = Color::FromRGB(32, 32, 32);
   };
   ```

2. **创建样式辅助函数：**
   ```cpp
   auto createPrimaryButton = [](const std::string& label) {
       return ui::button()
           ->Content(label)
           ->Background(Theme::PrimaryColor)
           ->Foreground(Color::White())
           ->FontWeight(FontWeight::Bold)
           ->Padding({15, 10, 15, 10});
   };
   ```

3. **使用数据绑定管理样式：**
   ```cpp
   // ViewModel 中定义样式属性
   FK_PROPERTY(Color, ButtonBackground);
   
   // View 中绑定
   button->Bind(binding::Bind(vm->ButtonBackgroundProperty()));
   ```

---

## 性能建议

- **频繁改变样式：** 避免在每帧都改变样式属性，改用事件驱动
- **批量设置：** 链式调用多个属性比多次赋值快

---

## 另见

- [Button.md](Button.md) - 按钮控件
- [TextBox.md](TextBox.md) - 文本输入
- [FrameworkElement.md](FrameworkElement.md) - 布局基础
- [UIElement.md](UIElement.md) - 基础元素
