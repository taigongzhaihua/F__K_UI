# Children() 批量接口示例

## 新增功能

为 `StackPanel` 添加了 `Children()` 接口,支持批量添加子元素:

```cpp
// include/fk/ui/StackPanel.h

// 流式API: 批量添加子元素 (接受初始化列表)
std::shared_ptr<StackPanel> Children(std::initializer_list<std::shared_ptr<UIElement>> children);

// 流式API: 批量添加子元素 (接受 vector)
std::shared_ptr<StackPanel> Children(const std::vector<std::shared_ptr<UIElement>>& children);
```

## 使用对比

### 方式 1: 使用 AddChild() - 逐个添加

```cpp
auto stackPanel = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->AddChild(
        ui::button()->Width(250)->Height(50)->Background("#4CAF50")
    )
    ->AddChild(
        ui::button()->Width(250)->Height(50)->Background("#2196F3")
    )
    ->AddChild(
        ui::button()->Width(250)->Height(50)->Background("#FF9800")
    )
    ->AddChild(
        ui::button()->Width(250)->Height(50)->Background("#F44336")
    )
    ->AddChild(
        ui::button()->Width(250)->Height(50)->Background("#9C27B0")
    );
```

**特点**:
- ✅ 每个子元素都是独立的链式调用
- ❌ 需要多次调用 `AddChild()`
- ❌ 视觉上不够紧凑

### 方式 2: 使用 Children() - 批量添加 (推荐!)

```cpp
auto stackPanel = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->Children({
        ui::button()->Width(250)->Height(50)->Background("#4CAF50"),
        ui::button()->Width(250)->Height(50)->Background("#2196F3"),
        ui::button()->Width(250)->Height(50)->Background("#FF9800"),
        ui::button()->Width(250)->Height(50)->Background("#F44336"),
        ui::button()->Width(250)->Height(50)->Background("#9C27B0")
    });
```

**特点**:
- ✅ 一次性添加所有子元素
- ✅ 使用初始化列表语法,更简洁
- ✅ 子元素之间用逗号分隔,类似数组
- ✅ 视觉上更紧凑,类似 React/SwiftUI

### 方式 3: 使用 vector

```cpp
std::vector<std::shared_ptr<ui::UIElement>> buttons;
buttons.push_back(ui::button()->Background("#4CAF50"));
buttons.push_back(ui::button()->Background("#2196F3"));
buttons.push_back(ui::button()->Background("#FF9800"));

auto stackPanel = ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->Children(buttons);
```

**特点**:
- ✅ 适合动态构建子元素列表
- ✅ 可以条件性添加元素
- ✅ 适合循环生成

## 完整示例

### 使用 Children() 的嵌套链式调用

```cpp
auto window = ui::window()
    ->Title("F__K_UI Demo - Nested Chaining")
    ->Width(800)
    ->Height(600)
    ->Content(
        ui::stackPanel()
            ->Orientation(ui::Orientation::Vertical)
            ->Children({
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#4CAF50")  // 绿色
                    ->BorderBrush("#2E7D32")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Green Button Clicked!" << std::endl;
                    }),
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#2196F3")  // 蓝色
                    ->BorderBrush("#1565C0")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Blue Button Clicked!" << std::endl;
                    }),
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#FF9800")  // 橙色
                    ->BorderBrush("#E65100")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Orange Button Clicked!" << std::endl;
                    }),
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#F44336")  // 红色
                    ->BorderBrush("#B71C1C")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Red Button Clicked!" << std::endl;
                    }),
                ui::button()
                    ->Width(250)->Height(50)
                    ->CornerRadius(10.0f)
                    ->Background("#9C27B0")  // 紫色
                    ->BorderBrush("#4A148C")
                    ->BorderThickness(2.0f)
                    ->OnClick([](ui::detail::ButtonBase&) {
                        std::cout << ">>> Purple Button Clicked!" << std::endl;
                    })
            })
    );
```

### 动态构建示例

```cpp
// 动态生成按钮列表
std::vector<std::shared_ptr<ui::UIElement>> createColorButtons() {
    std::vector<std::shared_ptr<ui::UIElement>> buttons;
    
    std::vector<std::pair<std::string, std::string>> colors = {
        {"#4CAF50", "#2E7D32"},  // 绿色
        {"#2196F3", "#1565C0"},  // 蓝色
        {"#FF9800", "#E65100"},  // 橙色
        {"#F44336", "#B71C1C"},  // 红色
        {"#9C27B0", "#4A148C"}   // 紫色
    };
    
    for (const auto& [bg, border] : colors) {
        buttons.push_back(
            ui::button()
                ->Width(250)->Height(50)
                ->CornerRadius(10.0f)
                ->Background(bg)
                ->BorderBrush(border)
                ->BorderThickness(2.0f)
        );
    }
    
    return buttons;
}

// 使用
auto window = ui::window()
    ->Content(
        ui::stackPanel()
            ->Orientation(ui::Orientation::Vertical)
            ->Children(createColorButtons())
    );
```

## 实现细节

### 初始化列表版本
```cpp
std::shared_ptr<StackPanel> Children(
    std::initializer_list<std::shared_ptr<UIElement>> children
) {
    for (auto& child : children) {
        Panel::AddChild(child);
    }
    return Self();
}
```

- 接受 `std::initializer_list`
- 支持 `{elem1, elem2, elem3}` 语法
- 适合编译时已知的元素列表

### Vector 版本
```cpp
std::shared_ptr<StackPanel> Children(
    const std::vector<std::shared_ptr<UIElement>>& children
) {
    for (const auto& child : children) {
        Panel::AddChild(child);
    }
    return Self();
}
```

- 接受 `std::vector`
- 支持动态构建的列表
- 适合运行时生成的元素

## 与其他框架对比

### React (JSX)
```jsx
<Stack orientation="vertical">
  <Button background="#4CAF50" />
  <Button background="#2196F3" />
  <Button background="#FF9800" />
</Stack>
```

### SwiftUI
```swift
VStack {
    Button { } .background(Color.green)
    Button { } .background(Color.blue)
    Button { } .background(Color.orange)
}
```

### Jetpack Compose
```kotlin
Column {
    Button(colors = ButtonDefaults.buttonColors(containerColor = Color.Green))
    Button(colors = ButtonDefaults.buttonColors(containerColor = Color.Blue))
    Button(colors = ButtonDefaults.buttonColors(containerColor = Color.Orange))
}
```

### F__K_UI (C++)
```cpp
ui::stackPanel()
    ->Orientation(ui::Orientation::Vertical)
    ->Children({
        ui::button()->Background("#4CAF50"),
        ui::button()->Background("#2196F3"),
        ui::button()->Background("#FF9800")
    })
```

## 优势总结

### Children() vs AddChild()

| 特性 | Children() | AddChild() |
|------|-----------|-----------|
| 语法简洁性 | ✅ 更简洁 | ❌ 较啰嗦 |
| 视觉紧凑性 | ✅ 更紧凑 | ❌ 较松散 |
| 批量操作 | ✅ 一次调用 | ❌ 多次调用 |
| 初始化列表 | ✅ 支持 | ❌ 不支持 |
| 动态列表 | ✅ 支持 vector | ✅ 可用循环 |
| 链式调用 | ✅ 支持 | ✅ 支持 |

## 性能考虑

两种方式性能基本相同,因为:
1. 都是调用 `Panel::AddChild()`
2. `Children()` 只是循环调用
3. 编译器可能会内联优化

## 推荐使用场景

### 使用 Children({...})
- ✅ 静态的子元素列表
- ✅ 编译时已知的元素
- ✅ 追求代码简洁性

### 使用 Children(vector)
- ✅ 动态生成的子元素
- ✅ 条件性添加元素
- ✅ 循环生成元素

### 使用 AddChild()
- ✅ 单个元素添加
- ✅ 需要在不同位置添加
- ✅ 需要保存子元素引用

## 编译和运行

```bash
cd build
mingw32-make fk_example
./fk_example.exe
```

输出:
```
=== F__K_UI Nested Chaining Demo ===
Creating window with nested button hierarchy...
Window hierarchy created!
>>> Window opened!
```

窗口正常显示,5 个颜色按钮垂直排列,功能完美!

## 总结

`Children()` 接口:
- ✅ 更符合现代 UI 框架风格
- ✅ 代码更简洁紧凑
- ✅ 支持初始化列表和 vector
- ✅ 保持链式调用的流畅性
- ✅ 与 React/SwiftUI 等框架风格一致

完美提升了 API 的易用性和表达力!

---
**Date**: 2025-10-29  
**Status**: ✅ Implemented and Tested
