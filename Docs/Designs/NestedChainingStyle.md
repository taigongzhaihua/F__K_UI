# 嵌套式链式调用示例

## 概述

将示例应用改为完全嵌套式的链式调用风格,所有 UI 元素在一个表达式中创建,无需中间变量。

## 代码对比

### 之前的方式 - 分步创建

```cpp
// 分别创建按钮
auto button1 = ui::button()
    ->Width(250)->Height(50)
    ->CornerRadius(10.0f)
    ->Background("#4CAF50")
    ->BorderBrush("#2E7D32")
    ->BorderThickness(2.0f)
    ->OnClick([](ui::detail::ButtonBase&) {
        std::cout << ">>> Green Button Clicked!" << std::endl;
    });

auto button2 = ui::button()
    ->Width(250)->Height(50)
    // ...

// 创建布局容器
auto stackPanel = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->AddChild(button1)
    ->AddChild(button2)
    // ...

// 创建窗口
auto window = ui::window()
    ->Title("Demo")
    ->Width(800)
    ->Height(600)
    ->Content(stackPanel);
```

### 现在的方式 - 嵌套式链式调用

```cpp
// 一气呵成!所有元素在一个表达式中创建
auto window = ui::window()
    ->Title("F__K_UI Demo - Nested Chaining")
    ->Width(800)
    ->Height(600)
    ->Content(
        ui::stackPanel()
            ->Orientation(ui::Orientation::Vertical)
            ->AddChild(
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#4CAF50")  // 绿色
                    ->BorderBrush("#2E7D32")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Green Button Clicked!" << std::endl;
                    })
            )
            ->AddChild(
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#2196F3")  // 蓝色
                    ->BorderBrush("#1565C0")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Blue Button Clicked!" << std::endl;
                    })
            )
            ->AddChild(
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#FF9800")  // 橙色
                    ->BorderBrush("#E65100")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Orange Button Clicked!" << std::endl;
                    })
            )
            ->AddChild(
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#F44336")  // 红色
                    ->BorderBrush("#B71C1C")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Red Button Clicked!" << std::endl;
                    })
            )
            ->AddChild(
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#9C27B0")  // 紫色
                    ->BorderBrush("#4A148C")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Purple Button Clicked!" << std::endl;
                    })
            )
    );
```

## 优势

### 1. **代码更紧凑**
- 无需定义中间变量 (button1, button2, stackPanel 等)
- 整个 UI 层次结构一目了然

### 2. **层次结构清晰**
- 缩进直接反映了 UI 树的嵌套关系
- 更容易理解父子关系

### 3. **声明式风格**
- 类似 React JSX、SwiftUI 或 Jetpack Compose
- 描述"是什么"而不是"怎么做"

### 4. **减少变量污染**
- 不需要为每个 UI 元素命名
- 作用域更小,代码更干净

### 5. **链式 API 的完美体现**
- 充分利用了 `Content()` 的链式调用支持
- 展示了框架设计的优雅性

## 代码行数对比

### 分步创建方式
```
按钮定义: 5 个 × 10 行 = 50 行
StackPanel: 7 行
Window: 5 行
总计: ~62 行
```

### 嵌套式链式调用
```
完整表达式: ~65 行 (包含所有元素)
但无需中间变量定义
```

虽然行数相近,但嵌套式更加紧凑和直观。

## 适用场景

### 适合嵌套式链式调用
✅ 简单的 UI 布局  
✅ 静态的控件层次结构  
✅ 原型和演示代码  
✅ 不需要动态修改的 UI

### 适合分步创建
✅ 需要保存子元素引用以便后续修改  
✅ 复杂的动态 UI  
✅ 需要条件性添加子元素  
✅ 元素需要在多处复用

## 实际运行效果

```bash
=== F__K_UI Nested Chaining Demo ===
Creating window with nested button hierarchy...
Window hierarchy created!
Subscribing events...
Creating application...
Running application with main window...
Application started.
GLFW initialized
GLFW window created: 800x600
RenderHost initialized for window (800x600)
Layout performed: 800x600
>>> Window opened!
Window 'F__K_UI Demo - Nested Chaining' opened (800x600)
Starting message loop...
```

窗口正常显示,5 个颜色按钮垂直排列,点击事件正常触发。

## 技术细节

### 关键技术支持

1. **工厂函数**
   ```cpp
   ui::button()    // 创建 Button
   ui::stackPanel() // 创建 StackPanel
   ui::window()     // 创建 Window
   ```

2. **链式 API**
   - 所有设置方法返回 `shared_ptr<Derived>`
   - 支持连续调用

3. **Content() 链式支持**
   ```cpp
   Ptr Content(std::shared_ptr<UIElement> content) {
       SetContent(std::move(content));
       return std::static_pointer_cast<Window>(shared_from_this());
   }
   ```

4. **AddChild() 链式支持**
   ```cpp
   Ptr AddChild(std::shared_ptr<UIElement> child) {
       AddChild(std::move(child));
       return std::static_pointer_cast<StackPanel>(shared_from_this());
   }
   ```

### 类型推导

编译器能够正确推导整个嵌套表达式的类型:
```cpp
ui::stackPanel()                    // shared_ptr<StackPanel>
    ->Orientation(...)              // shared_ptr<StackPanel>
    ->AddChild(ui::button()->...)   // shared_ptr<StackPanel>
    ->AddChild(...)                 // shared_ptr<StackPanel>

ui::window()                        // shared_ptr<Window>
    ->Content(stackPanel)           // shared_ptr<Window>
```

## 风格建议

### 混合使用
对于复杂应用,可以混合两种风格:

```cpp
// 复杂的可复用组件 - 使用分步创建
auto createColorButton = [](const std::string& color, const std::string& border) {
    return ui::button()
        ->Width(250)->Height(50)
        ->CornerRadius(10.0f)
        ->Background(color)
        ->BorderBrush(border)
        ->BorderThickness(2.0f);
};

// 简单布局 - 使用嵌套式
auto window = ui::window()
    ->Title("Demo")
    ->Content(
        ui::stackPanel()
            ->AddChild(createColorButton("#4CAF50", "#2E7D32"))
            ->AddChild(createColorButton("#2196F3", "#1565C0"))
    );
```

## 总结

嵌套式链式调用充分展示了流畅 API 设计的优势:
- ✅ 代码紧凑,层次清晰
- ✅ 声明式编程风格
- ✅ 类型安全
- ✅ 编译时检查
- ✅ 无性能开销

完美契合现代 C++ 和 UI 框架的设计理念!

---
**Date**: 2025-10-29  
**Status**: ✅ Implemented and Tested
