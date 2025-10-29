# Window.Content() 链式调用支持

## 问题描述

之前的 `Window` 类继承自 `ContentControl`,而 `ContentControl` 继承自 `Control<ContentControl>`,这导致 `Content()` 方法返回 `shared_ptr<ContentControl>` 而不是 `shared_ptr<Window>`。

**继承链**:
```
Window → ContentControl → Control<ContentControl>
                          ↓
                    Content() 返回 ContentControl::Ptr
```

这使得无法流畅地进行链式调用:
```cpp
auto window = ui::window()
    ->Title("Demo")
    ->Width(800)
    ->Height(600)
    ->Content(stackPanel);  // ❌ 返回 ContentControl*,无法继续调用 Window 的方法
```

## 解决方案

在 `Window` 类中重写 `Content()` 方法,返回 `Window::Ptr`:

```cpp
// include/fk/ui/Window.h
class Window : public ContentControl {
public:
    using Ptr = std::shared_ptr<Window>;
    
    // Content 链式调用 (重写以返回 Window::Ptr)
    Ptr Content(std::shared_ptr<UIElement> content) {
        SetContent(std::move(content));
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
    // 其他方法...
};
```

## 效果

现在可以完全流畅的链式调用:

```cpp
auto window = ui::window()
    ->Title("F__K_UI Demo - Multiple Buttons")
    ->Width(800)
    ->Height(600)
    ->Content(stackPanel);  // ✅ 返回 Window*,继续链式调用
```

## 技术细节

### CRTP 模式
`Control<Derived>` 使用 CRTP (Curiously Recurring Template Pattern):
```cpp
template<typename Derived = void>
class Control : public ControlBase {
    Ptr Content(ContentPtr content) {
        this->SetContent(std::move(content));
        return this->Self();  // 返回派生类指针
    }
};
```

### 类型转换
`Window::Content()` 使用 `std::static_pointer_cast` 进行安全的向下转型:
```cpp
return std::static_pointer_cast<Window>(shared_from_this());
```

### 方法重写
- 调用父类的 `SetContent()` 保持功能一致
- 返回正确的派生类类型支持链式调用
- 使用移动语义优化性能

## 示例

### 多按钮布局
```cpp
auto button1 = ui::button()
    ->Width(250)->Height(50)
    ->CornerRadius(10.0f)
    ->Background("#4CAF50")
    ->BorderBrush("#2E7D32")
    ->BorderThickness(2.0f);

auto button2 = ui::button()
    ->Width(250)->Height(50)
    ->CornerRadius(10.0f)
    ->Background("#2196F3")
    ->BorderBrush("#1565C0")
    ->BorderThickness(2.0f);

auto stackPanel = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->AddChild(button1)
    ->AddChild(button2);

// 完整的链式调用
auto window = ui::window()
    ->Title("Demo")
    ->Width(800)
    ->Height(600)
    ->Content(stackPanel);  // ✅ 支持链式调用

window->Show();
```

## 扩展性

如果将来有其他类继承自 `ContentControl`,也需要类似的重写:

```cpp
class MyCustomWindow : public ContentControl {
public:
    using Ptr = std::shared_ptr<MyCustomWindow>;
    
    // 重写 Content() 返回正确类型
    Ptr Content(std::shared_ptr<UIElement> content) {
        SetContent(std::move(content));
        return std::static_pointer_cast<MyCustomWindow>(shared_from_this());
    }
};
```

## 测试验证

### 编译测试
```bash
cd build
mingw32-make fk_example
```

### 运行测试
```bash
./fk_example.exe
```

### 预期输出
```
=== F__K_UI Multi-Button Demo ===
Step 1: Creating multiple buttons...
Created 5 colorful buttons
Step 2: Creating StackPanel for vertical layout...
Step 3: Creating window...
Step 4: Subscribing events...
...
>>> Window opened!
```

## 总结

通过在 `Window` 中重写 `Content()` 方法:
- ✅ 保持了 API 的流畅性
- ✅ 支持完整的链式调用
- ✅ 类型安全
- ✅ 性能优化(移动语义)
- ✅ 易于扩展

---
**Date**: 2025-01-29  
**Status**: ✅ Implemented and Tested
