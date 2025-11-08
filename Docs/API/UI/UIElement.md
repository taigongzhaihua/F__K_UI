# UIElement API 文档

## 概述

`UIElement` 是 F__K_UI 框架中所有可视化元素的根基类。它提供了基本的属性系统、事件处理、布局支持和命中测试功能。

**继承链：** `std::enable_shared_from_this` → `DependencyObject` + `Visual` → `FrameworkElement`

**命名空间：** `fk::ui`

---

## 核心属性（Dependency Properties）

### Visibility
- **类型：** `enum class Visibility { Visible, Hidden, Collapsed }`
- **默认值：** `Visibility::Visible`
- **说明：** 控制元素的可见性
  - `Visible`：正常显示
  - `Hidden`：隐藏但占用布局空间
  - `Collapsed`：隐藏且不占用布局空间

**用法：**
```cpp
auto elem = ui::textBlock()->Visibility(Visibility::Hidden);
```

### IsEnabled
- **类型：** `bool`
- **默认值：** `true`
- **说明：** 控制元素是否接收输入事件

**用法：**
```cpp
button->IsEnabled(false);  // 禁用按钮
```

### Opacity
- **类型：** `float`
- **默认值：** `1.0f`
- **范围：** `[0.0f, 1.0f]`
- **说明：** 元素的透明度，`0.0f` 完全透明，`1.0f` 完全不透明

**用法：**
```cpp
textBlock->Opacity(0.5f);  // 50% 透明
```

### ClipToBounds
- **类型：** `bool`
- **默认值：** `false`
- **说明：** 是否将子元素内容裁剪到元素边界内

**用法：**
```cpp
panel->ClipToBounds(true);
```

### Name
- **类型：** `std::string`
- **默认值：** `""` (空字符串)
- **说明：** 元素的唯一标识名称，用于查找元素和代码中的引用

**用法：**
```cpp
auto button = ui::button()->Name("SubmitButton");
auto foundButton = window->FindName("SubmitButton")->As<ui::Button<>>();
```

---

## 布局方法

### Size Measure(const Size& availableSize)
- **参数：** 可用的大小空间
- **返回值：** 元素所需的大小
- **说明：** 测量阶段，元素计算自身所需的尺寸

**用法（通常由容器调用）：**
```cpp
Size desiredSize = element->Measure(Size{800, 600});
```

### void Arrange(const Rect& finalRect)
- **参数：** 最终分配的矩形区域 `{x, y, width, height}`
- **说明：** 排列阶段，元素接受最终的位置和大小

**用法（通常由容器调用）：**
```cpp
element->Arrange(Rect{10, 10, 100, 50});
```

### void InvalidateMeasure()
- **说明：** 标记测量状态为无效，下一帧将重新测量
- **触发时机：** 元素宽度、高度等影响大小的属性变化时

### void InvalidateArrange()
- **说明：** 标记排列状态为无效，下一帧将重新排列
- **触发时机：** 元素位置、对齐方式等影响位置的属性变化时

### void InvalidateVisual()
- **说明：** 标记视觉呈现为无效，下一帧将重新渲染
- **触发时机：** 颜色、不透明度等影响外观但不影响布局的属性变化时

---

## 查询方法

### [[nodiscard]] bool IsMeasureValid() const noexcept
- **返回值：** 测量状态是否有效
- **用法：**
```cpp
if (!element->IsMeasureValid()) {
    element->Measure(availableSize);
}
```

### [[nodiscard]] bool IsArrangeValid() const noexcept
- **返回值：** 排列状态是否有效

### [[nodiscard]] const Size& DesiredSize() const noexcept
- **返回值：** 元素在测量阶段计算的所需大小

### [[nodiscard]] const Rect& LayoutSlot() const noexcept
- **返回值：** 元素最后一次排列分配的矩形区域

### [[nodiscard]] UIElement* FindName(const std::string& name)
- **参数：** 要查找的元素名称
- **返回值：** 找到的元素指针，未找到返回 `nullptr`
- **说明：** 在此元素及其子树中查找具有指定名称的元素

**用法：**
```cpp
auto submitBtn = window->FindName("SubmitButton");
if (submitBtn) {
    auto btn = submitBtn->As<ui::Button<>>();
}
```

### [[nodiscard]] const UIElement* FindName(const std::string& name) const
- **说明：** `const` 版本

---

## 类型转换辅助

### template\<typename T\> T* As() noexcept
- **模板参数：** 目标类型 `T`
- **返回值：** 动态转换后的指针，转换失败返回 `nullptr`
- **说明：** 安全的 `dynamic_cast` 包装

**用法：**
```cpp
auto button = element->As<ui::Button<>>();
if (button) {
    button->OnClick([](auto&) { /* ... */ });
}
```

### template\<typename T\> const T* As() const noexcept
- **说明：** `const` 版本

---

## 事件处理方法（虚函数）

这些方法可被子类重写以自定义行为：

### virtual bool OnMouseButtonDown(int button, double x, double y)
- **参数：**
  - `button`：鼠标按键（通常 0 = 左键，1 = 右键）
  - `x, y`：局部坐标（相对于元素左上角）
- **返回值：** `true` 表示事件已处理，停止冒泡；`false` 继续冒泡
- **触发时机：** 鼠标按键按下

### virtual bool OnMouseButtonUp(int button, double x, double y)
- **说明：** 鼠标按键释放

### virtual bool OnMouseMove(double x, double y)
- **说明：** 鼠标移动

### virtual bool OnMouseWheel(double xoffset, double yoffset, double mouseX, double mouseY)
- **参数：**
  - `xoffset, yoffset`：滚轮偏移
  - `mouseX, mouseY`：鼠标位置
- **说明：** 鼠标滚轮滚动

### virtual bool OnKeyDown(int key, int scancode, int mods)
- **参数：**
  - `key`：键码
  - `scancode`：扫描码
  - `mods`：修饰符（Shift、Ctrl、Alt）
- **说明：** 键盘按键按下

### virtual bool OnKeyUp(int key, int scancode, int mods)
- **说明：** 键盘按键释放

### virtual bool OnTextInput(unsigned int codepoint)
- **参数：** Unicode 码点
- **说明：** 文字输入事件

### virtual bool HitTest(double x, double y) const
- **参数：** 全局坐标
- **返回值：** 该点是否在元素的渲染边界内
- **说明：** 命中测试，用于判断鼠标事件是否应该被此元素处理

### virtual UIElement* HitTestChildren(double x, double y)
- **参数：** 局部坐标
- **返回值：** 在该位置的最上层子元素，未找到返回 `nullptr`
- **说明：** 用于事件路由，找到真正应该处理事件的子元素

---

## 事件信号

### core::Event\<UIElement&\> MeasureInvalidated
- **触发时机：** 调用 `InvalidateMeasure()` 时
- **参数：** 触发事件的元素本身

**用法：**
```cpp
element->MeasureInvalidated += [](UIElement& elem) {
    std::cout << "测量失效" << std::endl;
};
```

### core::Event\<UIElement&\> ArrangeInvalidated
- **触发时机：** 调用 `InvalidateArrange()` 时

---

## Visual 接口实现

这些方法实现了 `Visual` 接口，通常由渲染系统调用：

### virtual Rect GetRenderBounds() const override
- **返回值：** 元素的渲染边界（绝对坐标）

### virtual std::vector\<Visual*\> GetVisualChildren() const override
- **返回值：** 所有可视子元素的列表

### virtual bool HasRenderContent() const override
- **返回值：** 元素是否有需要渲染的内容

---

## 生命周期事件

### virtual void OnAttachedToLogicalTree() override
- **触发时机：** 元素被添加到逻辑树时
- **用途：** 初始化依赖于父元素或逻辑树的资源

### virtual void OnDetachedFromLogicalTree() override
- **触发时机：** 元素从逻辑树中移除时
- **用途：** 清理资源，解除事件订阅

---

## 使用示例

### 基础用法

```cpp
#include "fk/ui/TextBlock.h"
#include "fk/ui/Window.h"

auto window = ui::window()
    ->Title("UIElement Demo")
    ->Width(400)
    ->Height(300)
    ->Content(
        ui::textBlock()
            ->Text("Hello UIElement")
            ->Name("greeting")  // 设置名称便于查找
            ->FontSize(24.0f)
            ->Opacity(0.9f)     // 略微透明
    );

auto app = fk::Application();
app.AddWindow(window, "MainWindow");
```

### 查找并修改元素

```cpp
// 查找名为 "greeting" 的元素
auto greetingElem = window->FindName("greeting");
if (auto textBlock = greetingElem->As<ui::TextBlock>()) {
    textBlock->Foreground("#FF0000");  // 改为红色
}
```

### 自定义事件处理

```cpp
class CustomElement : public UIElement {
protected:
    bool OnMouseButtonDown(int button, double x, double y) override {
        std::cout << "鼠标在 (" << x << ", " << y << ") 按下" << std::endl;
        return true;  // 标记事件已处理
    }
};
```

---

## 性能考虑

- **频繁调用 `InvalidateVisual()`：** 可能降低性能，建议批量更新后一次性调用
- **深层嵌套的 `FindName()`：** O(n) 复杂度，频繁调用可考虑缓存结果
- **类型转换 `As<T>()`：** 底层使用 `dynamic_cast`，频繁调用有开销

---

## 另见

- [FrameworkElement.md](FrameworkElement.md) - 添加尺寸、对齐、边距支持
- [Panel.md](Panel.md) - 子元素容器
- [View.md](View.md) - 链式 API 模式

