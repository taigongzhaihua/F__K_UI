# DependencyProperty 重构 - API 迁移指南

## 概述

为了保持架构一致性，我们将 `Window`、`StackPanel` 等类的独有属性从普通成员变量重构为 `DependencyProperty`，与 `FrameworkElement` 的属性系统统一。

## 架构原理

### 为什么使用 DependencyProperty?

`FrameworkElement` 的所有布局属性（Width、Height、Margin、Padding 等）都使用 DependencyProperty 实现：

```cpp
// FrameworkElement.h
static const binding::DependencyProperty& WidthProperty();
static const binding::DependencyProperty& HeightProperty();

float GetWidth() const {
    return std::any_cast<float>(GetValue(WidthProperty()));
}

void SetWidth(float value) {
    SetValue(WidthProperty(), value);
}
```

作为 `FrameworkElement` 的派生类，`Window`、`StackPanel` 等也应该使用相同的属性系统：

**优势：**
- ✅ **架构统一**：所有属性使用相同的存储和访问机制
- ✅ **数据绑定支持**：未来可实现属性绑定 `{Binding Path=Width}`
- ✅ **属性变更通知**：自动触发布局更新和事件
- ✅ **样式系统**：支持样式和模板设置属性
- ✅ **动画支持**：属性值可以动画化

## API 变更清单

### Window 窗口大小属性设计

**设计原则：** Window 的 `Width()/Height()` 直接控制窗口大小（int 类型），隐藏父类的布局大小方法（float 类型）。这符合 WPF 的设计，也更符合直觉。

#### 最终设计

Window 类**隐藏/重写**父类继承的 `Width()/Height()` 方法：

| API | 类型 | 用途 | 来源 |
|-----|------|------|------|
| `Width(int)` | int | 设置窗口宽度（像素） | Window 重写 |
| `Height(int)` | int | 设置窗口高度（像素） | Window 重写 |
| `Width() const` | int | 获取窗口宽度 | Window 重写 |
| `Height() const` | int | 获取窗口高度 | Window 重写 |

**设计理由：**
1. ✅ **符合 WPF**：WPF 中 `Window.Width/Height` 直接就是窗口大小（double 类型）
2. ✅ **API 简洁**：不需要区分 `Width` 和 `WindowWidth`，用户不会困惑
3. ✅ **类型安全**：窗口大小使用 int（像素），布局大小使用 float（可以是 NaN）
4. ✅ **向后兼容**：Window 的使用者不需要关心父类的布局方法

#### 迁移示例

**最新代码（推荐）:**
```cpp
auto window = ui::window()
    ->Title("My Window")
    ->Width(800)      // ✅ 窗口宽度（int）
    ->Height(600);    // ✅ 窗口高度（int）

int w = window->Width();   // ✅ 获取窗口宽度
int h = window->Height();  // ✅ 获取窗口高度
```

**注意：** 父类 View 的 `Width(float)/Height(float)` 方法被隐藏，Window 只暴露 int 版本。

`Title` 属性已改为 DependencyProperty，但 API 没有变化（向后兼容）。

**实现细节：**
```cpp
// Window.h
static const binding::DependencyProperty& TitleProperty();
Ptr Title(const std::string& title);   // Setter
std::string Title() const;             // Getter

// Window.cpp
const binding::DependencyProperty& Window::TitleProperty() {
    static const auto& prop = DependencyProperty::Register(
        "Title", typeid(std::string), typeid(Window),
        PropertyMetadata(
            std::string("Untitled Window"),  // 默认值
            &Window::TitlePropertyChanged    // 变更回调
        )
    );
    return prop;
}

std::string Window::Title() const {
    return std::any_cast<std::string>(GetValue(TitleProperty()));
}

Window::Ptr Window::Title(const std::string& title) {
    SetValue(TitleProperty(), title);
    return std::static_pointer_cast<Window>(shared_from_this());
}
```

**用法不变：**
```cpp
auto window = ui::window()->Title("My Window");
std::string title = window->Title();  // "My Window"
```

### 3. StackPanel.Orientation 依赖属性

`Orientation` 属性已改为 DependencyProperty，API 没有变化。

**实现细节：**
```cpp
// StackPanel.h
static const binding::DependencyProperty& OrientationProperty();
ui::Orientation Orientation() const;                       // Getter
std::shared_ptr<StackPanel> Orientation(ui::Orientation);  // Setter

// StackPanel.cpp
const binding::DependencyProperty& StackPanel::OrientationProperty() {
    static const auto& prop = DependencyProperty::Register(
        "Orientation", typeid(ui::Orientation), typeid(StackPanel),
        PropertyMetadata(
            ui::Orientation::Vertical,           // 默认垂直
            &StackPanel::OrientationPropertyChanged
        )
    );
    return prop;
}

void StackPanel::OrientationPropertyChanged(
    DependencyObject* d,
    const DependencyPropertyChangedEventArgs& e
) {
    auto* panel = static_cast<StackPanel*>(d);
    // 方向改变，重新布局
    panel->InvalidateMeasure();
    panel->InvalidateArrange();
}
```

**用法不变：**
```cpp
auto panel = std::make_shared<StackPanel>()
    ->Orientation(ui::Orientation::Horizontal);

auto orientation = panel->Orientation();  // Horizontal
```

## 完整迁移检查清单

### 代码修改

- [x] **StackPanel.Orientation** → DependencyProperty
  - [x] 添加 `OrientationProperty()` 注册
  - [x] 重写 getter/setter 使用 GetValue/SetValue
  - [x] 添加 `OrientationPropertyChanged` 回调
  - [x] 更新 `MeasureOverride/ArrangeOverride` 使用新方法

- [x] **Window.Title** → DependencyProperty
  - [x] 添加 `TitleProperty()` 注册
  - [x] 重写 getter/setter
  - [x] 添加 `TitlePropertyChanged` 回调同步到 GLFW
  - [x] 移除 `title_` 成员变量

- [x] **Window.Width/Height** → DependencyProperty (隐藏父类 float 版本)
  - [x] 添加 `WidthProperty()`/`HeightProperty()` 注册
  - [x] 重写 getter/setter 为 int 版本
  - [x] 添加 `SizePropertyChanged` 回调
  - [x] 移除 `width_`/`height_` 成员变量
  - [x] 更新构造函数（移除初始化列表）
  - [x] 更新 `Show()`, `PerformLayout()`, `OnNativeWindowResize()` 使用新方法

### 示例代码更新

- [x] `examples/main.cpp` - 使用 `Width/Height`
- [x] `examples/main_new.cpp` - 使用 `Width/Height`
- [x] `examples/test_window.cpp` - 使用 `Width/Height`
- [x] `examples/phase2_demo.cpp` - 使用 `Width/Height`
- [x] `examples/main_new.cpp` - 更新为 `WindowWidth/WindowHeight`
- [x] `examples/test_window.cpp` - 更新为 `WindowWidth/WindowHeight`
- [x] `examples/phase2_demo.cpp` - 更新为 `WindowWidth/WindowHeight`

### 编译验证

- [x] fk_ui 库编译通过
- [x] fk_example 编译通过
- [x] phase2_demo 编译通过
- [x] phase3_demo 编译通过
- [x] test_getter 编译通过
- [x] test_window_width 编译通过

## 属性变更回调机制

DependencyProperty 支持属性变更回调，当属性值改变时自动触发：

```cpp
// 示例：StackPanel.Orientation 改变时自动重新布局
void StackPanel::OrientationPropertyChanged(
    DependencyObject* d,
    const DependencyPropertyChangedEventArgs& e
) {
    auto* panel = static_cast<StackPanel*>(d);
    
    // 自动触发布局失效
    panel->InvalidateMeasure();
    panel->InvalidateArrange();
    
    // 可选：获取旧值和新值
    auto oldOrientation = std::any_cast<ui::Orientation>(e.OldValue());
    auto newOrientation = std::any_cast<ui::Orientation>(e.NewValue());
    
    std::cout << "Orientation changed from " 
              << (int)oldOrientation << " to " << (int)newOrientation << std::endl;
}
```

**应用场景：**
- **StackPanel.Orientation**：方向改变 → 触发布局更新
- **Window.Title**：标题改变 → 同步到原生窗口标题
- **Window.WindowWidth/WindowHeight**：大小改变 → 调用 `glfwSetWindowSize()` → 触发布局更新

## 性能考虑

DependencyProperty 的性能开销：

1. **属性访问开销**：`std::any_cast` 有一定开销，但在非热路径（属性设置）可接受
2. **存储开销**：使用 `std::unordered_map<const DependencyProperty*, std::any>` 存储，比直接成员变量多一些开销
3. **变更通知**：属性改变时调用回调函数

**优化建议：**
- ✅ 布局计算中频繁访问的属性（如 StackPanel.Orientation）可以缓存
- ✅ 属性变更回调中避免复杂逻辑，只触发失效标记
- ✅ 对于渲染热路径，考虑缓存属性值

## 未来规划

### 待重构属性

- [ ] **ContentControl.Content** → DependencyProperty + 流式 API
  - 当前：`SetContent(UIElement*)`/`GetContent()`
  - 目标：`Content(std::shared_ptr<UIElement>)`/`Content() const`

- [ ] **Decorator.Child** → DependencyProperty + 流式 API
  - 当前：`SetChild(UIElement*)`/`GetChild()`
  - 目标：`Child(std::shared_ptr<UIElement>)`/`Child() const`

### 数据绑定支持（长期）

```cpp
// 未来可能的绑定语法
auto textBox = ui::textbox();
auto slider = ui::slider();

// 双向绑定：textBox.Text ↔ slider.Value
textBox->Bind(TextBox::TextProperty(), slider, Slider::ValueProperty());
```

## 总结

**核心原则：** 所有 FrameworkElement 派生类的属性都应使用 DependencyProperty，保持架构一致性。

**主要收益：**
1. ✅ **统一的属性系统**：所有属性使用相同的机制
2. ✅ **自动变更通知**：属性改变自动触发回调
3. ✅ **未来扩展性**：支持数据绑定、样式、动画

**迁移建议：**
- 优先迁移高频使用的属性（如 Window.WindowWidth/WindowHeight）
- 逐步重构，保持代码可用性
- 更新所有示例代码和文档

**参考：**
- `Docs/API-Consistency-Check.md` - API 一致性检查
- `Docs/Fluent-API-Refactoring.md` - 流式 API 重构总结
- `Docs/DependencyProperty-Refactoring-Plan.md` - 依赖属性重构详细方案
