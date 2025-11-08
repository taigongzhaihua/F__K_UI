# TextBlock 文档

## 概述

`TextBlock` 是用于显示只读文本的基础控件，支持文本样式、字体、对齐、剪裁等功能。TextBlock 是显示数据最常用的控件，常用于标签、提示、表格单元格等场景。

**继承链：** `UIElement` → `FrameworkElement` → `Control` → `TextBlock`

**命名空间：** `fk::ui`

**特性：**
- ✅ 支持富文本样式（字体、大小、颜色、加粗、斜体）
- ✅ 灵活的文本对齐（水平、竖直）
- ✅ 自动/手动行折行
- ✅ 文本剪裁与省略号
- ✅ 数据绑定支持

---

## 核心属性

### Text
- **类型：** `std::string`
- **默认值：** `""`
- **说明：** 显示的文本内容
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::textBlock()->Text("Hello World");
// 或数据绑定
ui::textBlock()->Bind(binding::Bind(viewModel->TextProperty()));
```

### FontFamily
- **类型：** `std::string`
- **默认值：** `"Arial"`
- **说明：** 字体名称（如 "Arial"、"宋体"、"微软雅黑"）
- **可绑定：** ✅ 是

```cpp
ui::textBlock()->FontFamily("Microsoft YaHei");
```

### FontSize
- **类型：** `float`
- **默认值：** `12.0f`
- **说明：** 字体大小（单位：像素）
- **可绑定：** ✅ 是

```cpp
ui::textBlock()->FontSize(16.0f);
```

### FontWeight
- **类型：** `FontWeight` (枚举)
- **默认值：** `FontWeight::Normal`
- **说明：** 字体粗细程度
- **可选值：** 
  - `FontWeight::Thin` - 细体
  - `FontWeight::Light` - 轻体
  - `FontWeight::Normal` - 正常
  - `FontWeight::SemiBold` - 半粗
  - `FontWeight::Bold` - 粗体
  - `FontWeight::ExtraBold` - 特粗

```cpp
ui::textBlock()->FontWeight(FontWeight::Bold);
```

### FontStyle
- **类型：** `FontStyle` (枚举)
- **默认值：** `FontStyle::Normal`
- **说明：** 字体样式
- **可选值：**
  - `FontStyle::Normal` - 正常
  - `FontStyle::Italic` - 斜体
  - `FontStyle::Oblique` - 倾斜

```cpp
ui::textBlock()->FontStyle(FontStyle::Italic);
```

### Foreground
- **类型：** `Color`
- **默认值：** `Color::Black()`
- **说明：** 文本颜色
- **可绑定：** ✅ 是

```cpp
ui::textBlock()->Foreground(Color::FromRGB(255, 0, 0));  // 红色
```

### TextAlignment
- **类型：** `TextAlignment` (枚举)
- **默认值：** `TextAlignment::Left`
- **说明：** 水平对齐方式
- **可选值：**
  - `TextAlignment::Left` - 左对齐
  - `TextAlignment::Center` - 居中
  - `TextAlignment::Right` - 右对齐
  - `TextAlignment::Justify` - 两端对齐

```cpp
ui::textBlock()->TextAlignment(TextAlignment::Center);
```

### VerticalAlignment (继承)
- **类型：** `VerticalAlignment` (枚举)
- **默认值：** `VerticalAlignment::Top`
- **说明：** 竖直对齐方式
- **可选值：**
  - `VerticalAlignment::Top` - 顶部
  - `VerticalAlignment::Center` - 居中
  - `VerticalAlignment::Bottom` - 底部
  - `VerticalAlignment::Stretch` - 拉伸

```cpp
ui::textBlock()->VerticalAlignment(VerticalAlignment::Center);
```

---

## 文本折行与剪裁

### TextWrapping
- **类型：** `TextWrapping` (枚举)
- **默认值：** `TextWrapping::NoWrap`
- **说明：** 行折行行为
- **可选值：**
  - `TextWrapping::NoWrap` - 不折行，文本超出时溢出
  - `TextWrapping::Wrap` - 自动折行
  - `TextWrapping::WrapWithOverflow` - 折行但长单词可溢出

```cpp
ui::textBlock()
    ->Text("这是一个很长的文本，会自动折行显示...")
    ->TextWrapping(TextWrapping::Wrap)
    ->Width(200.0f);  // 限制宽度以触发折行
```

### TextTrimming
- **类型：** `TextTrimming` (枚举)
- **默认值：** `TextTrimming::None`
- **说明：** 超出容器时的剪裁方式
- **可选值：**
  - `TextTrimming::None` - 不剪裁
  - `TextTrimming::CharacterEllipsis` - 末尾省略号（字符）
  - `TextTrimming::WordEllipsis` - 末尾省略号（单词）

```cpp
ui::textBlock()
    ->Text("这是一个很长的文本，会被剪裁...")
    ->TextTrimming(TextTrimming::WordEllipsis)
    ->Width(100.0f)
    ->MaxLines(1);  // 单行显示
```

### MaxLines
- **类型：** `int`
- **默认值：** `std::numeric_limits<int>::max()` (无限制)
- **说明：** 最大行数
- **说明：** 超过此行数的内容将被隐藏或剪裁

```cpp
ui::textBlock()
    ->Text("多行文本...")
    ->MaxLines(3)        // 最多显示 3 行
    ->TextTrimming(TextTrimming::WordEllipsis);
```

### LineHeight
- **类型：** `float`
- **默认值：** `0.0f` (自动计算)
- **说明：** 单位行高（像素）

```cpp
ui::textBlock()
    ->Text("行1\n行2\n行3")
    ->LineHeight(25.0f);  // 每行 25 像素
```

---

## 装饰与效果

### TextDecorations
- **类型：** `TextDecorationCollection` (std::vector\<TextDecoration\>)
- **默认值：** `{}` (无装饰)
- **说明：** 文本装饰（下划线、删除线等）
- **可选值：**
  - `TextDecoration::Underline` - 下划线
  - `TextDecoration::Strikethrough` - 删除线
  - `TextDecoration::Overline` - 上划线

```cpp
ui::textBlock()
    ->Text("带下划线的文本")
    ->TextDecorations({TextDecoration::Underline});
```

### Opacity (继承)
- **类型：** `float`
- **默认值：** `1.0f`
- **说明：** 不透明度 (0.0 = 完全透明, 1.0 = 完全不透明)

```cpp
ui::textBlock()
    ->Text("半透明文本")
    ->Opacity(0.5f);  // 50% 透明
```

---

## 布局相关属性 (继承)

### Width / Height
- **类型：** `float`
- **默认值：** `NaN` (自动)
- **说明：** 固定宽度/高度

```cpp
ui::textBlock()->Width(200.0f)->Height(100.0f);
```

### MinWidth / MaxWidth / MinHeight / MaxHeight
- **类型：** `float`
- **默认值：** 默认无限制
- **说明：** 尺寸约束

```cpp
ui::textBlock()
    ->Text("受约束的文本")
    ->MinWidth(100.0f)
    ->MaxWidth(300.0f);
```

### Margin (继承)
- **类型：** `Thickness`
- **默认值：** `{0, 0, 0, 0}`
- **说明：** 外边距

```cpp
ui::textBlock()->Margin({10, 5, 10, 5});  // L, T, R, B
```

### Padding (继承)
- **类型：** `Thickness`
- **默认值：** `{0, 0, 0, 0}`
- **说明：** 内边距

```cpp
ui::textBlock()->Padding({5, 5, 5, 5});
```

---

## 完整使用示例

### 示例 1：简单标签

```cpp
auto label = ui::textBlock()
    ->Text("用户名:")
    ->FontFamily("Arial")
    ->FontSize(12.0f)
    ->Foreground(Color::FromRGB(64, 64, 64));
```

### 示例 2：标题文本

```cpp
auto title = ui::textBlock()
    ->Text("欢迎来到应用")
    ->FontSize(24.0f)
    ->FontWeight(FontWeight::Bold)
    ->Foreground(Color::FromRGB(0, 102, 204))
    ->TextAlignment(TextAlignment::Center)
    ->Margin({0, 10, 0, 20});
```

### 示例 3：多行文本显示

```cpp
auto description = ui::textBlock()
    ->Text("这是一个详细的产品描述文本，"
           "会根据容器宽度自动折行，"
           "并在超过最大行数时使用省略号结尾...")
    ->FontSize(13.0f)
    ->TextWrapping(TextWrapping::Wrap)
    ->MaxLines(5)
    ->TextTrimming(TextTrimming::WordEllipsis)
    ->Width(400.0f)
    ->LineHeight(20.0f);
```

### 示例 4：状态文本

```cpp
auto statusText = ui::textBlock()
    ->Text("加载中...")
    ->Foreground(Color::FromRGB(255, 102, 0))  // 橙色
    ->FontStyle(FontStyle::Italic)
    ->Opacity(0.8f);
```

### 示例 5：数据绑定示例

```cpp
// ViewModel
class StatusViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, StatusMessage);
    FK_PROPERTY(Color, StatusColor);
    
    void UpdateStatus(const std::string& msg, const Color& color) {
        SetStatusMessage(msg);
        SetStatusColor(color);
    }
};

// View
auto vm = std::make_shared<StatusViewModel>();
auto statusDisplay = ui::textBlock()
    ->Bind(binding::Bind(vm->StatusMessageProperty()))
    ->Bind(binding::Bind(vm->StatusColorProperty()));
```

---

## 常见问题

### Q1：如何实现带图标的文本？
**A：** 使用 StackPanel 组合 Image 和 TextBlock：
```cpp
ui::stackPanel()
    ->Orientation(Orientation::Horizontal)
    ->Spacing(5.0f)
    ->Children({
        ui::image()->Source("icon.png")->Width(16)->Height(16),
        ui::textBlock()->Text("带图标的标签")
    });
```

### Q2：如何为文本添加阴影效果？
**A：** 当前框架直接支持的阴影有限，可考虑：
1. 用两个 TextBlock 叠加（一个偏移+半透明作为阴影）
2. 将阴影效果集成到渲染层

```cpp
// 简单阴影效果
auto shadow = ui::textBlock()
    ->Text(text)
    ->Opacity(0.3f)
    ->Margin({1, 1, 0, 0});  // 向右下偏移 1 像素

auto actual = ui::textBlock()->Text(text);

// 叠加显示
```

### Q3：如何实现超链接文本？
**A：** 设计一个 HyperlinkButton 或在 TextBlock 上添加点击事件处理：
```cpp
auto hyperlink = ui::button()
    ->Content("点击此链接")
    ->Background(Color::Transparent())
    ->Foreground(Color::FromRGB(0, 102, 204))
    ->OnClick([](Button* btn) { /* 打开链接 */ });
```

### Q4：TextWrapping 和 TextTrimming 的区别？
**A：**
- `TextWrapping` - 控制是否折到新行
- `TextTrimming` - 控制超出容器时的剪裁方式（省略号等）

两者常配合使用。

### Q5：如何在 TextBlock 中显示换行符？
**A：** 在字符串中使用 `\n`：
```cpp
ui::textBlock()->Text("第一行\n第二行\n第三行");
```

---

## 性能考虑

- **频繁更新文本：** 每次 Text 属性变化都会触发重新测量和渲染
- **长文本：** 包含大量字符的文本会增加渲染成本
- **字体加载：** 使用多种字体会增加内存占用和初始化时间
- **建议：** 对频繁变化的文本使用虚拟化列表（ItemsControl）而不是多个 TextBlock

---

## 另见

- [UIElement.md](UIElement.md) - 基础元素
- [Button.md](Button.md) - 交互按钮
- [Control.md](Control.md) - 控件基类
