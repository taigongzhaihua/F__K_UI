# 链式绑定语法 (Chainable Binding Syntax)

## 概述

F__K_UI 现在支持更简洁的链式绑定语法，让数据绑定代码更加简洁和直观。

## 功能说明

### 传统语法

之前，要将 TextBlock 的 Text 属性绑定到 ViewModel 的属性，需要这样写：

```cpp
auto textBlock = ui::textBlock();

Binding binding;
binding.Path("PropertyName");
textBlock->SetBinding(TextBlockBase::TextProperty(), binding);
```

### 新的链式语法

现在，你可以使用更简洁的链式语法：

```cpp
auto textBlock = ui::textBlock()
    ->Text(bind("PropertyName"))
    ->FontSize(16.0f)
    ->Foreground("#FF0078D4");
```

## 实现原理

### 1. `bind()` 辅助函数

新增了全局辅助函数 `bind()`，它接受属性路径字符串并返回配置好的 `Binding` 对象：

```cpp
// 在 fk::binding::Binding.h 中
namespace fk {
    inline Binding bind(std::string path) {
        Binding b;
        b.Path(std::move(path));
        return b;
    }
}
```

### 2. `Text(Binding)` 重载

在 `TextBlock` 类中添加了新的 `Text()` 方法重载，接受 `Binding` 对象：

```cpp
// 在 fk::ui::TextBlock 中
Ptr Text(binding::Binding binding) {
    SetBinding(TextProperty(), std::move(binding));
    return Self();
}
```

## 使用示例

### 基本用法

```cpp
// 创建 TextBlock 并绑定到 ViewModel 属性
auto nameText = ui::textBlock()->Text(bind("UserName"));
auto ageText = ui::textBlock()->Text(bind("UserAge"));
auto emailText = ui::textBlock()->Text(bind("UserEmail"));
```

### 链式调用

```cpp
// 绑定 + 设置样式，一气呵成
auto titleText = ui::textBlock()
    ->Text(bind("Title"))
    ->FontSize(24.0f)
    ->Foreground("#FF2E86DE")
    ->FontFamily("Arial");
```

### 高级绑定配置

如果需要更高级的绑定配置（如转换器、绑定模式等），仍然可以使用传统语法：

```cpp
Binding binding;
binding.Path("PropertyName")
       .Mode(BindingMode::TwoWay)
       .Converter(myConverter);
textBlock->SetBinding(TextProperty(), binding);
```

或者，你可以先配置 Binding 对象，再传递给链式方法：

```cpp
Binding binding;
binding.Path("Price")
       .Mode(BindingMode::OneWay)
       .Converter(std::make_shared<CurrencyConverter>());

auto priceText = ui::textBlock()
    ->Text(std::move(binding))
    ->FontSize(18.0f);
```

## 优势

1. **更简洁**：减少了样板代码，一行代码完成绑定和样式设置
2. **可读性强**：代码意图更加清晰
3. **链式调用**：可以与其他 fluent API 方法无缝链接
4. **类型安全**：编译时检查，避免运行时错误
5. **向后兼容**：传统语法仍然有效，不影响现有代码

## 扩展性

这种模式可以扩展到其他控件和属性：

```cpp
// 未来可以支持的扩展
auto button = ui::button()
    ->Content(bind("ButtonText"))
    ->IsEnabled(bind("CanExecute"));

auto textBox = ui::textBox()
    ->Text(bind("InputValue"))
    ->PlaceholderText("Enter value...");
```

## 编译和运行

### 前提条件
- 已编译 F__K_UI 库 (libfk.a)
- C++23 编译器
- CMake 3.10+

### 构建步骤

```bash
cd samples/binding_syntax_demo
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### 运行示例

```bash
.\binding_syntax_demo.exe
```

## 示例输出

运行示例后会显示一个窗口，展示新旧语法的对比：

```
=== Binding Syntax Demo ===
Demonstrating: textBlock()->Text(bind("PropertyName"))

=== Syntax Comparison ===
Traditional syntax:
  Binding binding;
  binding.Path("PropertyName");
  textBlock->SetBinding(TextProperty(), binding);

New chainable syntax:
  textBlock()->Text(bind("PropertyName"))

Both syntaxes work the same way internally!
The bind() function creates a Binding object with Path set.
The Text(Binding) overload calls SetBinding() internally.
```

## 技术细节

- **头文件位置**: `include/fk/binding/Binding.h` 和 `include/fk/ui/TextBlock.h`
- **命名空间**: `fk::bind()` 和 `fk::ui::TextBlock::Text(Binding)`
- **返回类型**: `std::shared_ptr<Derived>` 支持链式调用
- **性能**: 内部实现与传统语法完全相同，无额外开销

## 兼容性

- ✅ 与传统绑定语法完全兼容
- ✅ 支持所有现有的 Binding 功能
- ✅ 可以与其他 fluent API 方法混合使用
- ✅ 不破坏现有代码

## 未来计划

- [ ] 为其他控件属性添加类似的绑定重载
- [ ] 支持更多绑定场景（如 RelativeSource, ElementName）
- [ ] 提供更多便捷的辅助函数

## 参考

- [Binding API 文档](../Docs/API/Binding.md)
- [Fluent API 设计](../Docs/Designs/Fluent-API-Refactoring.md)
- [TextBlock API](../Docs/API/TextBlock.md)
