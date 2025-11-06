# UI 模块 API 文档

用户界面控件模块,提供完整的 UI 控件库和布局系统。

命名空间: `fk::ui`

---

## 创建控件

F__K_UI 使用**工厂函数**来创建控件实例,而不是直接调用构造函数或静态 `Create()` 方法。

### 工厂函数

所有UI控件都提供了小写的工厂函数,返回智能指针:

```cpp
// 创建控件
auto btn = ui::button();           // 按钮
auto text = ui::textBlock();       // 文本块
auto panel = ui::stackPanel();     // 堆叠面板
auto win = ui::window();           // 窗口
auto scroll = ui::scrollViewer();  // 滚动视图
auto bar = ui::scrollBar();        // 滚动条
```

### 链式配置

工厂函数返回的智能指针支持链式调用,可以流畅地配置属性:

```cpp
auto button = ui::button()
    ->Content("Click Me")
    ->Width(120)
    ->Height(40)
    ->Background("#4CAF50")
    ->HoveredBackground("#45A049")
    ->OnClick([](ui::detail::ButtonBase&) {
        std::cout << "Button clicked!" << std::endl;
    });
```

---

## 目录

- [创建控件](#创建控件)
  - [工厂函数](#工厂函数)
  - [链式配置](#链式配置)
- [基础类](#基础类)
  - [DispatcherObject](#dispatcherobject)
  - [DependencyObject](#dependencyobject-ui)
  - [Visual](#visual)
  - [UIElement](#uielement)
  - [FrameworkElement](#frameworkelement)
- [控件](#控件)
  - [Control](#control)
  - [ContentControl](#contentcontrol)
  - [Button](#button)
  - [TextBlock](#textblock)
- [布局](#布局)
  - [Panel](#panel)
  - [StackPanel](#stackpanel)
  - [Decorator](#decorator)
- [容器](#容器)
  - [ItemsControl](#itemscontrol)
  - [ScrollViewer](#scrollviewer)
  - [ScrollBar](#scrollbar)
- [窗口](#窗口)
  - [Window](#window)
  - [View](#view)
- [辅助类型](#辅助类型)

---

## 基础类

### UIElement

**UI 元素基类** - 提供布局、渲染和事件处理的基础功能。

#### 类定义

```cpp
class UIElement : public DependencyObject, public Visual {
public:
    UIElement();
    ~UIElement() override;
    
    // 布局相关事件
    core::Event<UIElement&> MeasureInvalidated;
    core::Event<UIElement&> ArrangeInvalidated;
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Visibility | `Visibility` | `Visible` | 可见性 |
| IsEnabled | `bool` | `true` | 是否启用 |
| Opacity | `float` | `1.0` | 不透明度 (0.0-1.0) |
| ClipToBounds | `bool` | `false` | 是否裁剪到边界 |

#### 枚举

##### Visibility

```cpp
enum class Visibility {
    Visible,    // 可见且占用布局空间
    Hidden,     // 不可见但占用布局空间
    Collapsed   // 不可见且不占用布局空间
};
```

#### 核心方法

##### SetVisibility() / GetVisibility()

设置/获取可见性

```cpp
void SetVisibility(Visibility visibility);
Visibility GetVisibility() const;
```

**示例:**
```cpp
element->SetVisibility(Visibility::Collapsed);
```

##### SetIsEnabled() / IsEnabled()

设置/获取是否启用

```cpp
void SetIsEnabled(bool enabled);
bool IsEnabled() const;
```

**示例:**
```cpp
button->SetIsEnabled(false);  // 禁用按钮
```

##### SetOpacity() / GetOpacity()

设置/获取不透明度

```cpp
void SetOpacity(float value);  // 0.0 (完全透明) ~ 1.0 (完全不透明)
float GetOpacity() const;
```

**示例:**
```cpp
element->SetOpacity(0.5f);  // 半透明
```

#### 布局方法

##### Measure()

测量元素所需的大小

```cpp
Size Measure(const Size& availableSize);
```

**参数:**
- `availableSize` - 可用空间大小

**返回:** 元素期望的大小

**注意:** 通常由父元素调用,不需要手动调用

##### Arrange()

排列元素到指定位置

```cpp
void Arrange(const Rect& finalRect);
```

**参数:**
- `finalRect` - 最终位置和大小

**注意:** 通常由父元素调用

##### InvalidateMeasure()

使测量结果无效,触发重新测量

```cpp
void InvalidateMeasure();
```

**线程安全:** 否

**使用场景:** 当影响大小的属性改变时调用

##### InvalidateArrange()

使排列结果无效,触发重新排列

```cpp
void InvalidateArrange();
```

**线程安全:** 否

##### InvalidateVisual()

使视觉呈现无效,触发重新渲染

```cpp
void InvalidateVisual();
```

**线程安全:** 否

**使用场景:** 当视觉外观改变但布局不变时调用

**示例:**
```cpp
void OnColorChanged() {
    InvalidateVisual();  // 颜色改变,需要重绘
}
```

#### 布局属性

##### DesiredSize()

获取元素期望的大小

```cpp
const Size& DesiredSize() const noexcept;
```

**返回:** Measure() 计算的期望大小

##### LayoutSlot()

获取元素的布局槽位

```cpp
const Rect& LayoutSlot() const noexcept;
```

**返回:** Arrange() 分配的位置和大小

#### 鼠标事件方法

##### OnMouseButtonDown()

鼠标按钮按下事件

```cpp
virtual bool OnMouseButtonDown(int button, double x, double y);
```

**参数:**
- `button` - 按钮索引 (0=左键, 1=右键, 2=中键)
- `x`, `y` - 鼠标位置

**返回:** true 表示事件已处理(停止冒泡), false 继续冒泡

**示例:**
```cpp
bool OnMouseButtonDown(int button, double x, double y) override {
    if (button == 0) {  // 左键
        std::cout << "Clicked at: " << x << ", " << y << std::endl;
        return true;  // 停止事件冒泡
    }
    return false;  // 继续冒泡
}
```

##### OnMouseButtonUp()

鼠标按钮释放事件

```cpp
virtual bool OnMouseButtonUp(int button, double x, double y);
```

##### OnMouseMove()

鼠标移动事件

```cpp
virtual bool OnMouseMove(double x, double y);
```

##### OnMouseWheel()

鼠标滚轮事件

```cpp
virtual bool OnMouseWheel(double xoffset, double yoffset, 
                         double mouseX, double mouseY);
```

**参数:**
- `xoffset`, `yoffset` - 滚轮偏移量
- `mouseX`, `mouseY` - 鼠标位置

#### 命中测试方法

##### HitTest()

检查点是否在元素内

```cpp
virtual bool HitTest(double x, double y) const;
```

**参数:**
- `x`, `y` - 测试点坐标

**返回:** 如果点在元素内返回 true

##### HitTestChildren()

在子元素中查找指定位置的最上层元素

```cpp
virtual UIElement* HitTestChildren(double x, double y);
```

**返回:** 命中的子元素指针,未命中返回 nullptr

---

### FrameworkElement

**框架元素基类** - 扩展 UIElement,添加大小、边距等属性。

#### 类定义

```cpp
class FrameworkElement : public UIElement {
public:
    FrameworkElement();
    ~FrameworkElement() override;
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Width | `float` | `NaN` | 宽度 |
| Height | `float` | `NaN` | 高度 |
| MinWidth | `float` | `0.0` | 最小宽度 |
| MinHeight | `float` | `0.0` | 最小高度 |
| MaxWidth | `float` | `Infinity` | 最大宽度 |
| MaxHeight | `float` | `Infinity` | 最大高度 |
| Margin | `Thickness` | `{0}` | 外边距 |
| HorizontalAlignment | `HorizontalAlignment` | `Stretch` | 水平对齐 |
| VerticalAlignment | `VerticalAlignment` | `Stretch` | 垂直对齐 |

#### 方法

```cpp
void SetWidth(float value);
float GetWidth() const;

void SetHeight(float value);
float GetHeight() const;

void SetMargin(const Thickness& margin);
const Thickness& GetMargin() const;
```

**示例:**
```cpp
element->SetWidth(200.0f);
element->SetHeight(100.0f);
element->SetMargin(Thickness{10.0f});  // 所有边 10 像素
element->SetMargin(Thickness{10.0f, 20.0f});  // 水平 10, 垂直 20
element->SetMargin(Thickness{10.0f, 20.0f, 30.0f, 40.0f});  // 左上右下
```

---

## 控件

### Control

**控件基类** - 所有交互控件的基类。

#### 类定义

```cpp
template <typename Derived>
class Control : public View<Derived, detail::ControlBase> {
public:
    using Ptr = std::shared_ptr<Derived>;
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| IsFocused | `bool` | `false` | 是否获得焦点 |
| TabIndex | `int` | `0` | Tab 键顺序 |
| Cursor | `std::string` | `""` | 光标样式 |
| Content | `std::shared_ptr<UIElement>` | `nullptr` | 内容 |
| Padding | `Thickness` | `{0}` | 内边距 |

#### 链式 API

```cpp
Ptr IsFocused(bool value);
Ptr TabIndex(int value);
Ptr Cursor(std::string cursor);
Ptr Content(std::shared_ptr<UIElement> content);
Ptr Padding(const Thickness& padding);
Ptr Padding(float uniform);
Ptr Padding(float horizontal, float vertical);
Ptr Padding(float left, float top, float right, float bottom);
```

**示例:**
```cpp
auto ctrl = Control<MyControl>::Create()
    ->TabIndex(1)
    ->Padding(10.0f)
    ->Content(textBlock);
```

---

### Button

**按钮控件** - 可点击的按钮。

#### 创建方式

```cpp
// 使用工厂函数 (推荐)
auto btn = ui::button();

// 工厂函数定义
inline ButtonPtr button();
```

#### 类定义

```cpp
template<typename Derived = void>
class Button : public Control<Button> {
public:
    static Ptr Create();
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| CornerRadius | `float` | `0.0` | 圆角半径 |
| Background | `std::string` | `"#CCCCCC"` | 背景颜色 |
| Foreground | `std::string` | `"#000000"` | 前景颜色 (文本) |
| HoveredBackground | `std::string` | `""` | 悬停背景色 |
| PressedBackground | `std::string` | `""` | 按下背景色 |
| BorderBrush | `std::string` | `"#000000"` | 边框颜色 |
| BorderThickness | `float` | `0.0` | 边框粗细 |
| IsMouseOver | `bool` | `false` | 鼠标是否悬停(只读) |
| IsPressed | `bool` | `false` | 是否按下(只读) |

#### 链式 API

```cpp
Ptr CornerRadius(float radius);
Ptr Background(std::string color);
Ptr Foreground(std::string color);
Ptr HoveredBackground(std::string color);
Ptr PressedBackground(std::string color);
Ptr BorderBrush(std::string color);
Ptr BorderThickness(float thickness);
```

#### 事件

##### Click

按钮点击事件

```cpp
core::Event<Button&> Click;
```

**示例:**
```cpp
auto btn = ui::button()
    ->Content("Click Me")
    ->Background("#4CAF50")
    ->Foreground("#FFFFFF")
    ->CornerRadius(5.0f);

// 使用 OnClick 链式API (推荐)
auto btn2 = ui::button()
    ->Content("Action")
    ->OnClick([](ui::detail::ButtonBase& sender) {
        std::cout << "Button clicked!" << std::endl;
    });

// 或者使用 += 运算符订阅事件
btn->Click += [](ui::detail::ButtonBase& sender) {
    std::cout << "Button clicked!" << std::endl;
};
```

#### 颜色格式

颜色字符串支持以下格式:
- `"#RGB"` - 短格式 (如 "#F00" = 红色)
- `"#RRGGBB"` - 标准格式 (如 "#FF0000" = 红色)
- `"#RRGGBBAA"` - 带透明度 (如 "#FF0000FF" = 完全不透明红色)

---

### TextBlock

**文本块控件** - 显示文本。

#### 创建方式

```cpp
// 使用工厂函数 (推荐)
auto text = ui::textBlock();

// 工厂函数定义
inline std::shared_ptr<TextBlockView> textBlock();
```

#### 类定义

```cpp
template<typename Derived>
class TextBlock : public detail::TextBlockBase {
public:
    // ...
};

// 具体实现
class TextBlockView : public TextBlock<TextBlockView> {
    // ...
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Text | `std::string` | `""` | 文本内容 |
| FontSize | `float` | `14.0` | 字体大小 |
| FontFamily | `std::string` | `""` | 字体族 |
| Foreground | `std::string` | `"#000000"` | 文本颜色 |
| TextAlignment | `TextAlignment` | `Left` | 文本对齐 |

#### 链式 API

```cpp
Ptr Text(std::string text);
Ptr FontSize(float size);
Ptr FontFamily(std::string family);
Ptr Foreground(std::string color);
```

**示例:**
```cpp
auto text = ui::textBlock()
    ->Text("Hello, F__K_UI!")
    ->FontSize(24.0f)
    ->Foreground("#333333");
```

---

## 布局

### Panel

**面板基类** - 所有布局容器的基类。

#### 类定义

```cpp
template <typename Derived>
class Panel : public PanelBase {
public:
    using Ptr = std::shared_ptr<Derived>;
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Children | `UIElementCollection` | `{}` | 子元素集合 |

#### 方法

##### AddChild()

添加子元素

```cpp
void AddChild(std::shared_ptr<UIElement> child);
```

**示例:**
```cpp
panel->AddChild(button);
panel->AddChild(textBlock);
```

##### RemoveChild()

移除子元素

```cpp
void RemoveChild(UIElement* child);
```

##### ClearChildren()

清空所有子元素

```cpp
void ClearChildren();
```

##### GetChildren()

获取子元素集合

```cpp
std::span<const std::shared_ptr<UIElement>> GetChildren() const noexcept;
```

**返回:** 子元素的只读视图

**示例:**
```cpp
for (const auto& child : panel->GetChildren()) {
    // 处理每个子元素
}
```

#### 链式 API

```cpp
Ptr Children(const UIElementCollection& children);
Ptr Children(UIElementCollection&& children);
```

**示例:**
```cpp
auto panel = ui::stackPanel()
    ->Children({
        ui::button()->Content("Button 1"),
        ui::button()->Content("Button 2"),
        ui::textBlock()->Text("Some text")
    });
```

---

### StackPanel

**堆栈面板** - 水平或垂直排列子元素。

#### 创建方式

```cpp
// 使用工厂函数 (推荐)
auto panel = ui::stackPanel();

// 工厂函数定义
inline std::shared_ptr<StackPanel> stackPanel();
```

#### 类定义

```cpp
class StackPanel : public Panel<StackPanel> {
public:
    static Ptr Create();
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Orientation | `Orientation` | `Vertical` | 排列方向 |
| Spacing | `float` | `0.0` | 子元素间距 |

#### 枚举

##### Orientation

```cpp
enum class Orientation {
    Horizontal,  // 水平排列
    Vertical     // 垂直排列
};
```

#### 链式 API

```cpp
Ptr Orientation(Orientation value);
Ptr Spacing(float value);
```

**示例:**
```cpp
auto stack = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->Spacing(10.0f)
    ->Children({
        ui::button()->Content("Button 1"),
        ui::button()->Content("Button 2"),
        ui::button()->Content("Button 3")
    });
```

---

### ScrollViewer

**滚动视图** - 为内容提供滚动功能。

#### 创建方式

```cpp
// 使用工厂函数 (推荐)
auto scroll = ui::scrollViewer();

// 工厂函数定义
inline std::shared_ptr<ScrollViewerView> scrollViewer();
```

#### 类定义

```cpp
template<typename Derived>
class ScrollViewer : public ContentControl {
    // ...
};

class ScrollViewerView : public ScrollViewer<ScrollViewerView> {
    // ...
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| HorizontalScrollBarVisibility | `ScrollBarVisibility` | `Auto` | 水平滚动条可见性 |
| VerticalScrollBarVisibility | `ScrollBarVisibility` | `Auto` | 垂直滚动条可见性 |
| HorizontalOffset | `float` | `0.0` | 水平偏移量 |
| VerticalOffset | `float` | `0.0` | 垂直偏移量 |

#### 枚举

```cpp
enum class ScrollBarVisibility {
    Auto,      // 需要时显示
    Visible,   // 总是显示
    Hidden,    // 总是隐藏
    Disabled   // 禁用滚动
};
```

#### 方法

```cpp
void ScrollToHorizontalOffset(float offset);
void ScrollToVerticalOffset(float offset);
void LineUp();
void LineDown();
void LineLeft();
void LineRight();
void PageUp();
void PageDown();
```

**示例:**
```cpp
auto scrollViewer = ui::scrollViewer()
    ->Content(
        ui::stackPanel()
            ->Children({ /* 很多子元素 */ })
    );

// 滚动到指定位置
scrollViewer->ScrollToVerticalOffset(100.0f);
```

---

## 窗口

### Window

**窗口类** - 顶级窗口容器。

#### 创建方式

```cpp
// 使用工厂函数 (推荐)
auto win = ui::window();

// 工厂函数定义
inline WindowPtr window();

// 或者直接使用构造函数
auto win = std::make_shared<ui::Window>();
```

#### 类定义

```cpp
class Window : public ContentControl {
public:
    using Ptr = std::shared_ptr<Window>;
    
    Window();
    virtual ~Window();
    
    // 事件
    core::Event<> Closed;
    core::Event<> Opened;
    core::Event<int, int> Resized;
    core::Event<> Activated;
    core::Event<> Deactivated;
};
```

#### 依赖属性

| 属性 | 类型 | 默认值 | 说明 |
|---|---|---|---|
| Title | `std::string` | `""` | 窗口标题 |
| Width | `int` | `800` | 窗口宽度 |
| Height | `int` | `600` | 窗口高度 |

#### 方法

##### Show()

显示窗口

```cpp
void Show();
```

**线程安全:** 否

##### Hide()

隐藏窗口

```cpp
void Hide();
```

##### Close()

关闭窗口

```cpp
void Close();
```

##### ProcessEvents()

处理窗口事件(非阻塞)

```cpp
bool ProcessEvents();
```

**返回:** 如果窗口应该关闭返回 false

**使用:** 在主循环中调用

##### RenderFrame()

渲染一帧

```cpp
void RenderFrame();
```

**使用:** 在主循环中调用

#### 链式 API

```cpp
Ptr Title(const std::string& title);
std::string Title() const;

Ptr Width(int w);
int Width() const;

Ptr Height(int h);
int Height() const;

Ptr Content(std::shared_ptr<UIElement> content);
```

#### 完整示例

```cpp
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"

int main() {
    // 使用工厂函数创建窗口
    auto window = ui::window()
        ->Title("My Application")
        ->Width(800)
        ->Height(600);
    
    // 创建 UI
    auto button = ui::button()
        ->Content("Click Me")
        ->Background("#4CAF50")
        ->Foreground("#FFFFFF")
        ->OnClick([](ui::detail::ButtonBase& btn) {
            std::cout << "Button clicked!" << std::endl;
        });
    
    auto panel = ui::stackPanel()
        ->Spacing(10.0f)
        ->Children({ button });
    
    window->Content(panel);
    window->Show();
    
    // 主循环
    while (window->ProcessEvents()) {
        window->RenderFrame();
    }
    
    return 0;
}
```

---

## 辅助类型

### Size

二维尺寸

```cpp
struct Size {
    float width{0.0f};
    float height{0.0f};
    
    constexpr Size() = default;
    constexpr Size(float w, float h);
};
```

### Rect

矩形区域

```cpp
struct Rect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
    
    constexpr Rect() = default;
    constexpr Rect(float left, float top, float w, float h);
};
```

### Thickness

边距/内边距

```cpp
struct Thickness {
    float left{0.0f};
    float top{0.0f};
    float right{0.0f};
    float bottom{0.0f};
    
    // 构造函数
    Thickness() = default;
    explicit Thickness(float uniform);  // 所有边相同
    Thickness(float horizontal, float vertical);  // 水平/垂直
    Thickness(float left, float top, float right, float bottom);  // 分别指定
};
```

**示例:**
```cpp
Thickness{10.0f};                    // 所有边 10
Thickness{10.0f, 20.0f};            // 左右 10, 上下 20
Thickness{5.0f, 10.0f, 15.0f, 20.0f}; // 左上右下分别指定
```

---

## View 模式

F__K_UI 使用 CRTP(奇异递归模板模式) 实现链式 API:

```cpp
template <typename Derived, typename Base>
class View : public Base {
protected:
    std::shared_ptr<Derived> Self() {
        return std::static_pointer_cast<Derived>(
            this->shared_from_this()
        );
    }
};
```

**优势:**
- 类型安全的链式调用
- 返回正确的派生类型
- 零运行时开销

**使用示例:**
```cpp
class MyControl : public Control<MyControl> {
public:
    static Ptr Create() {
        return std::make_shared<MyControl>();
    }
    
    // 自定义链式方法
    Ptr CustomProperty(int value) {
        customValue_ = value;
        return Self();  // 返回 shared_ptr<MyControl>
    }
    
private:
    int customValue_{0};
};

// 链式调用
auto ctrl = MyControl::Create()
    ->Width(200.0f)
    ->Height(100.0f)
    ->CustomProperty(42);  // 返回类型始终是 MyControl::Ptr
```

---

## 线程安全说明

| 类 | 线程安全性 |
|---|---|
| 所有 UI 元素 | **非线程安全** - 必须在 UI 线程访问 |
| Window::Show() | 必须在主线程调用 |
| Window::ProcessEvents() | 必须在主线程调用 |
| InvalidateVisual() | 可以从任意线程调用(内部会切换到 UI 线程) |

---

## 最佳实践

### 布局

1. **使用约束而非固定大小**
   ```cpp
   // 推荐
   element->MinWidth(100.0f)->MaxWidth(300.0f);
   
   // 避免 (除非必要)
   element->Width(200.0f);
   ```

2. **使用 StackPanel 进行简单布局**
   ```cpp
   ui::stackPanel()
       ->Orientation(ui::Orientation::Vertical)
       ->Spacing(10.0f)
       ->Children({ /* ... */ });
   ```

3. **合理使用 Margin 和 Padding**
   ```cpp
   // Margin: 元素外部空间
   // Padding: 元素内部空间
   button->Margin(10.0f)->Padding(20.0f);
   ```

### 性能

1. **避免频繁的 InvalidateVisual()**
   - 合并多个属性变更
   - 使用批量更新

2. **使用虚拟化处理大量数据**
   - ItemsControl 支持虚拟化
   - 仅渲染可见项

3. **缓存测量结果**
   - 不要在 MeasureOverride 中执行复杂计算
   - 缓存布局相关信息

### UI 设计

1. **使用数据绑定分离 UI 和逻辑**
   ```cpp
   // ViewModel
   class MyViewModel : public ObservableObject {
       // 数据和逻辑
   };
   
   // View
   window->SetDataContext(std::make_shared<MyViewModel>());
   textBlock->SetBinding(ui::detail::TextBlockBase::TextProperty(),
       Binding().Path("PropertyName"));
   ```

2. **控件应该可重用**
   - 创建自定义控件而非硬编码 UI
   - 使用样式和模板

3. **响应式设计**
   - 使用 HorizontalAlignment/VerticalAlignment
   - 支持不同窗口大小

### 事件处理

1. **使用事件而非轮询**
   ```cpp
   // 推荐使用 += 运算符
   button->Click += [](ui::detail::ButtonBase& sender) {
       // 响应点击
   };
   ```

2. **正确管理事件订阅**
   ```cpp
   // 使用 Connection 自动管理生命周期
   Event<>::Connection conn = button->Click.Connect([]{});
   ```

3. **事件冒泡返回 true 停止传播**
   ```cpp
   bool OnMouseButtonDown(int button, double x, double y) override {
       // 处理事件
       return true;  // 停止冒泡
   }
   ```
