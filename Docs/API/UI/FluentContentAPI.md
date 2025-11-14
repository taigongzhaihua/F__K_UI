# Fluent Content API 改进

## 概述

为了提高 F__K_UI 框架的易用性，我们为 `ContentControl` 类添加了更流畅的 API，使得声明式 UI 编写更加直观和简洁。

## 问题背景

在之前的实现中，`Window::Content` 方法接受 `std::any` 类型的参数，这导致用户在编写 UI 代码时需要手动进行类型包装，使代码不够直观：

```cpp
// 旧方式 - 不够直观
auto mainWindow = std::make_shared<fk::ui::Window>();
mainWindow->Content(std::any(new fk::ui::TextBlock()));
```

## 改进方案

### 1. 添加 UIElement* 重载

在 `ContentControl` 模板类中添加了一个新的重载方法，直接接受 `UIElement*` 指针：

```cpp
template<typename Derived>
class ContentControl : public Control<Derived> {
public:
    // 原有方法 - 保持向后兼容
    Derived* Content(const std::any& value) {
        SetContent(value);
        return static_cast<Derived*>(this);
    }
    
    // 新增流畅 API - 直接接受 UIElement*
    Derived* Content(UIElement* element) {
        SetContent(std::any(element));
        return static_cast<Derived*>(this);
    }
};
```

### 2. 修复内部实现问题

同时修复了以下问题：

1. **GetContent() 的 any_cast 错误**：避免嵌套的 `std::any`
2. **GetContentTemplate() 的鲁棒性**：添加异常处理

## 使用示例

### 基本用法

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/TextBlock.h"

int main() {
    fk::Application app;
    auto mainWindow = std::make_shared<fk::ui::Window>();
    
    // 现在可以直接传递 UIElement* - 简洁直观！
    mainWindow->Title("Hello F__K_UI")
        ->Width(800)
        ->Height(600)
        ->Content(new fk::ui::TextBlock()->Text("Hello, World!"));
    
    app.Run(mainWindow);
    return 0;
}
```

### 链式调用

新 API 完美支持链式调用：

```cpp
auto window = std::make_shared<fk::ui::Window>();
window->Title("My App")
    ->Width(800)
    ->Height(600)
    ->Content(
        (new fk::ui::TextBlock())
            ->Text("Welcome!")
            ->FontSize(24.0f)
            ->FontWeight(fk::ui::FontWeight::Bold)
            ->TextAlignment(fk::ui::TextAlignment::Center)
    );
```

### 动态内容

```cpp
// 创建复杂的 UI 元素
auto* textBlock = new fk::ui::TextBlock();
textBlock->Text("Dynamic Content")
    ->FontFamily("Arial")
    ->FontSize(18.0f);

// 直接设置为内容
window->Content(textBlock);
```

## API 对比

| 场景 | 旧方式 | 新方式 |
|------|--------|--------|
| 基本使用 | `Content(std::any(new TextBlock()))` | `Content(new TextBlock())` |
| 链式调用 | 需要先包装成 any | 直接传递指针 |
| 代码可读性 | 较差，有技术细节干扰 | 优秀，意图清晰 |
| 学习曲线 | 需要理解 std::any | 符合直觉 |

## 向后兼容性

此改进完全向后兼容：

- ✅ 原有的 `Content(const std::any&)` 方法仍然存在
- ✅ 现有代码无需修改即可继续工作
- ✅ 新代码可以选择使用更简洁的方式

## 性能影响

性能影响微乎其微：

- 新方法内部只是将指针包装成 `std::any`，然后调用原有方法
- 没有额外的内存分配或复制
- 编译器可以内联优化

## 其他受益的类

由于使用了 CRTP 模式，以下类都自动获得了这个改进：

- `Window`
- `Button`
- `ScrollViewer`
- 所有继承自 `ContentControl` 的类

## 示例程序

参考 `examples/fluent_api_demo.cpp` 查看完整的使用示例。

## 技术细节

### 实现位置
- `include/fk/ui/ContentControl.h`

### 相关修复
1. `GetContent()` - 避免嵌套 any
2. `GetContentTemplate()` - 添加异常处理

### 测试验证
- ✅ 单元测试通过
- ✅ 现有测试未受影响
- ✅ 演示程序正常运行

## 未来改进

可以考虑为其他接受 `std::any` 参数的方法添加类似的重载，进一步提高框架的易用性。
