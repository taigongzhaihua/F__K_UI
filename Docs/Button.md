# Button 控件

支持圆角、自定义背景色的现代化按钮控件。

## 特性

- ✅ **圆角支持** - 可设置任意圆角半径
- ✅ **颜色自定义** - 支持背景色、前景色、边框颜色
- ✅ **边框样式** - 可自定义边框粗细和颜色
- ✅ **事件处理** - 点击事件支持
- ✅ **链式调用** - 流畅的 API 设计
- ✅ **动态样式** - 运行时修改样式属性
- ✅ **状态跟踪** - 自动跟踪鼠标悬停和按下状态

## 基本用法

```cpp
#include "fk/ui/Button.h"

using namespace fk::ui;

// 创建基础按钮
auto button = Button<>::Create()
    ->Width(120)
    ->Height(40);
```

## 圆角按钮（Material Design 风格）

```cpp
auto materialButton = Button<>::Create()
    ->CornerRadius(8.0f)           // 圆角半径 8px
    ->Background("#4CAF50")        // 绿色背景
    ->Foreground("#FFFFFF")        // 白色文字
    ->BorderThickness(0.0f)        // 无边框
    ->Width(150)
    ->Height(45);
```

## 带边框按钮

```cpp
auto borderedButton = Button<>::Create()
    ->CornerRadius(12.0f)
    ->Background("#FFFFFF")        // 白色背景
    ->Foreground("#2196F3")        // 蓝色文字
    ->BorderBrush("#2196F3")       // 蓝色边框
    ->BorderThickness(2.0f)        // 2px 边框
    ->Width(140)
    ->Height(42);
```

## 点击事件

```cpp
button->OnClick([](auto& btn) {
    std::cout << "按钮被点击！" << std::endl;
});
```

## 动态样式修改

```cpp
// 运行时修改样式
button->Background("#F44336");      // 改为红色（危险按钮）
button->CornerRadius(20.0f);       // 增大圆角
button->BorderThickness(3.0f);     // 加粗边框
```

## 按钮组示例

```cpp
auto panel = StackPanel::Create()
    ->Orientation(Orientation::Vertical);

auto saveButton = Button<>::Create()
    ->CornerRadius(4.0f)
    ->Background("#4CAF50")
    ->Foreground("#FFFFFF")
    ->Width(100)
    ->Height(35)
    ->OnClick([](auto& btn) {
        std::cout << "保存" << std::endl;
    });

auto cancelButton = Button<>::Create()
    ->CornerRadius(4.0f)
    ->Background("#F44336")
    ->Foreground("#FFFFFF")
    ->Width(100)
    ->Height(35)
    ->OnClick([](auto& btn) {
        std::cout << "取消" << std::endl;
    });

panel->AddChild(saveButton);
panel->AddChild(cancelButton);
```

## 属性说明

### 几何属性
- `CornerRadius(float)` - 圆角半径（像素），默认 0.0
- `BorderThickness(float)` - 边框粗细（像素），默认 1.0

### 颜色属性
所有颜色支持以下格式：
- `#RGB` - 短格式（如 `#F00`）
- `#RRGGBB` - 标准格式（如 `#FF5733`）
- `#RRGGBBAA` - 带透明度（如 `#FF5733FF`）

- `Background(string)` - 背景颜色，默认 `#CCCCCC`
- `Foreground(string)` - 前景色（文字颜色），默认 `#000000`
- `BorderBrush(string)` - 边框颜色，默认 `#999999`

### 状态属性（只读）
- `IsMouseOver()` - 鼠标是否悬停
- `IsPressed()` - 按钮是否被按下

### 内容属性
- `Content(UIElementPtr)` - 设置按钮内容（继承自 Control）

### 布局属性（继承）
- `Width(float)` - 宽度
- `Height(float)` - 高度
- `Margin(...)` - 外边距
- `HorizontalAlignment(...)` - 水平对齐
- `VerticalAlignment(...)` - 垂直对齐

## 事件

### Click 事件
当用户点击按钮时触发（鼠标按下并在按钮区域内释放）。

```cpp
button->OnClick([](detail::ButtonBase& btn) {
    // 处理点击
});

// 或直接访问事件对象
button->Click.Add([](detail::ButtonBase& btn) {
    // 处理点击
});
```

## 样式预设

### Material Design 按钮
```cpp
// 主要按钮
auto primaryButton = Button<>::Create()
    ->CornerRadius(4.0f)
    ->Background("#2196F3")
    ->Foreground("#FFFFFF")
    ->BorderThickness(0.0f)
    ->Width(120)
    ->Height(36);

// 次要按钮
auto secondaryButton = Button<>::Create()
    ->CornerRadius(4.0f)
    ->Background("#FFFFFF")
    ->Foreground("#2196F3")
    ->BorderBrush("#2196F3")
    ->BorderThickness(1.0f)
    ->Width(120)
    ->Height(36);
```

### Flat Design 按钮
```cpp
auto flatButton = Button<>::Create()
    ->CornerRadius(0.0f)           // 无圆角
    ->Background("#3498DB")
    ->Foreground("#FFFFFF")
    ->BorderThickness(0.0f)
    ->Width(100)
    ->Height(40);
```

### 圆形按钮（FAB）
```cpp
auto fabButton = Button<>::Create()
    ->CornerRadius(28.0f)          // 半径 = 高度的一半
    ->Background("#FF5722")
    ->Foreground("#FFFFFF")
    ->BorderThickness(0.0f)
    ->Width(56)
    ->Height(56);
```

## 验证规则

- **圆角半径**：必须 >= 0.0
- **边框粗细**：必须 >= 0.0
- **颜色格式**：必须符合 `#RGB`, `#RRGGBB`, 或 `#RRGGBBAA` 格式

无效值会被验证回调拒绝，属性不会更新。

## 实现细节

- **继承层次**：`Button<> → View<> → ButtonBase → ControlBase → FrameworkElement`
- **依赖属性**：所有样式属性都是依赖属性，支持绑定和继承
- **事件系统**：使用 `fk::core::Event<>` 实现类型安全的事件机制
- **CRTP 模式**：通过 CRTP 实现链式调用和类型安全

## 待实现功能

- [ ] 鼠标悬停/按下的视觉反馈（需要渲染系统支持）
- [ ] 禁用状态样式
- [ ] 图标支持
- [ ] 工具提示
- [ ] 键盘导航（Tab、Enter、Space）
- [ ] 焦点视觉效果
- [ ] 动画过渡效果

## 编译和运行

```bash
# 编译示例
cmake --build build --config Debug --target button_example

# 运行示例
./build/button_example
```

## 相关文件

- `include/fk/ui/Button.h` - Button 类定义
- `src/ui/Button.cpp` - Button 实现
- `examples/button_example.cpp` - 使用示例
