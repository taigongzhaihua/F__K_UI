# Enums - 枚举定义

## 概述

`Enums.h` 定义了 F__K UI 框架中的核心枚举类型，用于表示布局方向、对齐方式、可见性等常见 UI 属性。

## 核心枚举

### Orientation - 布局方向

```cpp
enum class Orientation {
    Horizontal,  // 水平
    Vertical     // 垂直
};
```

用于 `StackPanel`、`ScrollBar` 等需要指定方向的控件。

**使用场景：**

| 控件 | Horizontal | Vertical |
|------|-----------|----------|
| StackPanel | 从左到右排列 | 从上到下排列 |
| ScrollBar | 水平滚动 | 竖直滚动 |
| Slider | 水平拖动范围 | 竖直拖动范围 |

**示例：**
```cpp
#include "fk/ui/StackPanel.h"
#include "fk/ui/Enums.h"

auto panel = fk::ui::StackPanel::Create();

// 水平排列子元素
panel->SetOrientation(fk::ui::Orientation::Horizontal);

// 竖直排列子元素
panel->SetOrientation(fk::ui::Orientation::Vertical);
```

### HorizontalAlignment - 水平对齐

```cpp
enum class HorizontalAlignment {
    Left,     // 左对齐
    Center,   // 居中
    Right,    // 右对齐
    Stretch   // 拉伸填充
};
```

控制元素在可用宽度内的水平位置。

**示例：**
```cpp
auto button = fk::ui::Button::Create();
button->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Center);
```

### VerticalAlignment - 竖直对齐

```cpp
enum class VerticalAlignment {
    Top,      // 顶部对齐
    Center,   // 居中
    Bottom,   // 底部对齐
    Stretch   // 拉伸填充
};
```

控制元素在可用高度内的竖直位置。

**示例：**
```cpp
auto label = fk::ui::TextBlock::Create();
label->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
```

### Visibility - 可见性

```cpp
enum class Visibility {
    Visible,    // 可见且占用布局空间
    Hidden,     // 不可见但仍占用布局空间
    Collapsed   // 不可见且不占用布局空间
};
```

控制元素的显示状态和布局行为。

**行为对比：**

| 值 | 显示 | 占用空间 | 接收事件 |
|----|------|---------|---------|
| Visible | ✓ | ✓ | ✓ |
| Hidden | ✗ | ✓ | ✗ |
| Collapsed | ✗ | ✗ | ✗ |

**示例：**
```cpp
auto element = fk::ui::UIElement::Create();

// 完全隐藏，不占用空间
element->SetVisibility(fk::ui::Visibility::Collapsed);

// 隐藏但占用空间（保持布局）
element->SetVisibility(fk::ui::Visibility::Hidden);

// 显示
element->SetVisibility(fk::ui::Visibility::Visible);
```

## 常见模式

### 1. 动态切换可见性

```cpp
class UIController {
private:
    std::shared_ptr<UIElement> modal_;
    
public:
    void ShowModal() {
        modal_->SetVisibility(fk::ui::Visibility::Visible);
    }
    
    void HideModal() {
        modal_->SetVisibility(fk::ui::Visibility::Collapsed);
    }
};
```

### 2. 响应式布局

```cpp
void SetupResponsiveLayout() {
    auto header = fk::ui::StackPanel::Create();
    header->SetOrientation(fk::ui::Orientation::Horizontal);
    
    auto sidebar = fk::ui::Panel::Create();
    auto content = fk::ui::Panel::Create();
    
    // 在小屏幕上隐藏侧边栏
    bool isSmallScreen = GetScreenWidth() < 800;
    sidebar->SetVisibility(
        isSmallScreen ? fk::ui::Visibility::Collapsed 
                      : fk::ui::Visibility::Visible
    );
}
```

### 3. 水平和竖直对齐组合

```cpp
void CenterElement(std::shared_ptr<UIElement> element) {
    element->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Center);
    element->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
}

// 或使用链式 API
void CenterElementChained(std::shared_ptr<UIElement> element) {
    element->HorizontalAlignment(fk::ui::HorizontalAlignment::Center)
           ->VerticalAlignment(fk::ui::VerticalAlignment::Center);
}
```

### 4. 条件可见性

```cpp
class FormView {
private:
    std::shared_ptr<UIElement> errorMessage_;
    
public:
    void ValidateAndShow(const std::string& input) {
        if (IsValid(input)) {
            errorMessage_->SetVisibility(fk::ui::Visibility::Collapsed);
        } else {
            errorMessage_->SetVisibility(fk::ui::Visibility::Visible);
        }
    }
};
```

### 5. 线性布局方向

```cpp
class LayoutManager {
public:
    void ConfigureStackPanel(
        std::shared_ptr<StackPanel> panel,
        fk::ui::Orientation direction) {
        
        panel->SetOrientation(direction);
        
        // 根据方向调整对齐
        if (direction == fk::ui::Orientation::Horizontal) {
            panel->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Stretch);
            panel->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
        } else {
            panel->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Center);
            panel->SetVerticalAlignment(fk::ui::VerticalAlignment::Stretch);
        }
    }
};
```

## 最佳实践

### 1. 使用 Collapsed 而不是隐藏

```cpp
// ✅ 推荐：完全隐藏不占用空间
element->SetVisibility(fk::ui::Visibility::Collapsed);

// ❌ 避免：隐藏但留下空白
element->SetVisibility(fk::ui::Visibility::Hidden);
```

### 2. 对齐与 Stretch 的关系

```cpp
// ✅ 如果想填充可用空间
element->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Stretch);

// ❌ 避免手动设置宽度同时使用 Stretch
element->SetWidth(100);  // 冲突
element->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Stretch);
```

### 3. 方向与子元素对齐

```cpp
// ✅ 水平面板中子元素竖直居中
panel->SetOrientation(fk::ui::Orientation::Horizontal);
for (auto& child : children) {
    child->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
}

// ❌ 避免混淆
// 不要在水平面板中设置水平对齐
```

## 常见问题

### Q: Visibility::Hidden 的用途是什么？
**A:** 用于临时隐藏元素但保持布局完整，例如动画过渡或保留空间。

### Q: 如何切换对齐方式？
**A:** 直接调用 `SetHorizontalAlignment()` 或 `SetVerticalAlignment()`，会自动重新布局。

### Q: Orientation 只用于 StackPanel 吗？
**A:** 不仅如此，任何需要方向属性的控件都可以使用，如 ScrollBar。

### Q: 如何让元素既不可见也不占用空间？
**A:** 使用 `Visibility::Collapsed`。

## 相关文档

- [FrameworkElement.md](../UI/FrameworkElement.md) - 布局基类
- [StackPanel.md](../UI/StackPanel.md) - 堆叠布局
- [Grid.md](../UI/Grid.md) - 网格布局
- [UIElement.md](../UI/UIElement.md) - UI 元素基类
