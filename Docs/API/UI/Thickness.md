# Thickness - 边距和填充

## 概述

`Thickness` 是一个简单但重要的结构体，用于表示元素的边距（Margin）和填充（Padding），支持独立设置四个方向的厚度值。

## 结构定义

```cpp
struct Thickness {
    float left{0.0f};
    float top{0.0f};
    float right{0.0f};
    float bottom{0.0f};
    
    constexpr Thickness() = default;
    constexpr explicit Thickness(float uniform);
    constexpr Thickness(float horizontal, float vertical);
    constexpr Thickness(float leftValue, float topValue, float rightValue, float bottomValue);
};
```

## 构造方式

### 1. 默认构造（全零）

```cpp
fk::Thickness t;  // left=0, top=0, right=0, bottom=0
```

### 2. 统一厚度

```cpp
fk::Thickness padding(10.0f);
// left=10, top=10, right=10, bottom=10
```

### 3. 水平和竖直分离

```cpp
fk::Thickness margin(5.0f, 10.0f);
// left=5, top=10, right=5, bottom=10
```

### 4. 四个独立值

```cpp
fk::Thickness inset(10.0f, 15.0f, 20.0f, 25.0f);
// left=10, top=15, right=20, bottom=25
```

## 常见使用场景

### 1. 元素边距

```cpp
#include "fk/ui/Button.h"
#include "fk/ui/Thickness.h"

auto button = fk::ui::Button::Create();

// 四周各 10px 边距
button->SetMargin(fk::Thickness(10.0f));

// 水平 5px，竖直 15px
button->SetMargin(fk::Thickness(5.0f, 15.0f));

// 自定义
button->SetMargin(fk::Thickness(10.0f, 5.0f, 10.0f, 5.0f));
```

### 2. 内部填充

```cpp
auto panel = fk::ui::Panel::Create();

// 内部填充
panel->SetPadding(fk::Thickness(15.0f));

// 仅顶部填充
panel->SetPadding(fk::Thickness(0.0f, 20.0f, 0.0f, 0.0f));
```

### 3. 边框厚度

```cpp
auto control = fk::ui::Control::Create();

// 边框厚度
control->SetBorderThickness(fk::Thickness(2.0f));
```

### 4. 不对称样式

```cpp
// 左侧突出
auto notification = fk::ui::Panel::Create();
notification->SetMargin(fk::Thickness(0.0f, 10.0f, 10.0f, 10.0f));

// 底部强调
auto underline = fk::ui::Panel::Create();
underline->SetBorderThickness(fk::Thickness(0.0f, 0.0f, 0.0f, 3.0f));
```

## 常见模式

### 1. 响应式边距

```cpp
class ResponsiveUI {
private:
    std::shared_ptr<UIElement> container_;
    
public:
    void ApplyMargin() {
        float spacing = GetScreenSize() > 1024 ? 20.0f : 10.0f;
        container_->SetMargin(fk::Thickness(spacing));
    }
};
```

### 2. 组件化间距

```cpp
class DesignSystem {
public:
    static fk::Thickness Spacing(int level) {
        switch (level) {
            case 1: return fk::Thickness(4.0f);   // 小间距
            case 2: return fk::Thickness(8.0f);   // 中间距
            case 3: return fk::Thickness(16.0f);  // 大间距
            case 4: return fk::Thickness(24.0f);  // 超大间距
            default: return fk::Thickness(0.0f);
        }
    }
};

// 使用
button->SetMargin(DesignSystem::Spacing(2));  // 8px 间距
```

### 3. 对称布局

```cpp
void CreateCenteredForm() {
    auto form = fk::ui::Panel::Create();
    
    // 顶部和底部间距相同
    form->SetMargin(fk::Thickness(0.0f, 30.0f));
    
    // 内部填充
    form->SetPadding(fk::Thickness(20.0f));
}
```

### 4. 链式 API 与 Thickness

```cpp
auto button = fk::ui::Button::Create()
    ->Text("Click me")
    ->Margin(fk::Thickness(10.0f))
    ->Padding(fk::Thickness(5.0f, 10.0f));
```

### 5. 计算有效尺寸

```cpp
float GetEffectiveWidth(float contentWidth, const fk::Thickness& margin) {
    return contentWidth + margin.left + margin.right;
}

float GetEffectiveHeight(float contentHeight, const fk::Thickness& margin) {
    return contentHeight + margin.top + margin.bottom;
}

// 使用
auto button = fk::ui::Button::Create();
auto margin = fk::Thickness(10.0f, 5.0f, 10.0f, 5.0f);
float effectiveWidth = GetEffectiveWidth(100.0f, margin);  // 120.0f
```

## 进阶应用

### 1. 构建复杂布局

```cpp
void SetupDialog() {
    auto dialog = fk::ui::Window::Create();
    
    // 外层边距（窗口到屏幕边缘）
    dialog->SetMargin(fk::Thickness(20.0f));
    
    auto content = fk::ui::Panel::Create();
    
    // 内层填充（内容到边框）
    content->SetPadding(fk::Thickness(15.0f));
    dialog->AddChild(content);
}
```

### 2. 动画边距变化

```cpp
class AnimatedLayout {
private:
    std::shared_ptr<UIElement> element_;
    fk::core::Clock& clock_ = fk::core::Clock::Instance();
    
public:
    void AnimateMargin(const fk::Thickness& from, const fk::Thickness& to, double duration) {
        // 简单线性插值
        auto elapsed = clock_.GetDeltaSeconds();
        double progress = elapsed / duration;
        
        if (progress >= 1.0) {
            element_->SetMargin(to);
            return;
        }
        
        fk::Thickness current(
            Lerp(from.left, to.left, progress),
            Lerp(from.top, to.top, progress),
            Lerp(from.right, to.right, progress),
            Lerp(from.bottom, to.bottom, progress)
        );
        
        element_->SetMargin(current);
    }
    
private:
    float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }
};
```

### 3. 修复元素重叠

```cpp
void PreventOverlap(
    std::shared_ptr<UIElement> first,
    std::shared_ptr<UIElement> second,
    float gap) {
    
    // 给第一个元素添加右边距
    first->SetMargin(fk::Thickness(0.0f, 0.0f, gap, 0.0f));
    
    // 给第二个元素添加左边距
    second->SetMargin(fk::Thickness(gap, 0.0f, 0.0f, 0.0f));
}
```

## 最佳实践

### 1. 使用常量定义间距

```cpp
// ✅ 推荐
constexpr float SMALL_SPACING = 8.0f;
constexpr float MEDIUM_SPACING = 16.0f;
constexpr float LARGE_SPACING = 24.0f;

button->SetMargin(fk::Thickness(MEDIUM_SPACING));

// ❌ 避免
button->SetMargin(fk::Thickness(16.0f));  // 魔法数字
button->SetMargin(fk::Thickness(16.0f));  // 重复定义
```

### 2. 区分 Margin 和 Padding

```cpp
// ✅ 清晰的使用
element->SetMargin(fk::Thickness(10.0f));   // 外部间距
element->SetPadding(fk::Thickness(5.0f));   // 内部间距

// ❌ 混淆
// 不要混乱地应用 margin 和 padding
```

### 3. 对称性考虑

```cpp
// ✅ 对称布局
auto layout = fk::Thickness(15.0f, 10.0f, 15.0f, 10.0f);

// 或更简洁
auto layout = fk::Thickness(15.0f, 10.0f);  // 等价

// ❌ 不必要的复杂
auto layout = fk::Thickness(15.0f, 10.0f, 14.0f, 10.0f);  // 为什么？
```

### 4. 响应式值

```cpp
fk::Thickness GetResponsiveMargin() {
    if (IsDesktopSize()) {
        return fk::Thickness(24.0f);
    } else if (IsTabletSize()) {
        return fk::Thickness(16.0f);
    } else {
        return fk::Thickness(8.0f);
    }
}
```

## 常见问题

### Q: Margin 和 Padding 的区别？
**A:** 
- **Margin**: 元素外部间距，不属于元素本身
- **Padding**: 元素内部间距，属于元素内容区域

### Q: 可以使用负的 Thickness 吗？
**A:** 技术上可以，但不推荐。会导致布局重叠和不可预测的行为。

### Q: 如何快速创建只有某个方向的 Thickness？
**A:** 
```cpp
fk::Thickness OnlyLeft(10.0f, 0.0f, 0.0f, 0.0f);
fk::Thickness OnlyRight(0.0f, 0.0f, 10.0f, 0.0f);
fk::Thickness OnlyTop(0.0f, 10.0f, 0.0f, 0.0f);
fk::Thickness OnlyBottom(0.0f, 0.0f, 0.0f, 10.0f);
```

### Q: 浮点数精度会影响布局吗？
**A:** 通常不会，但在大量计算或缩放时可能产生视觉偏差。

## 相关文档

- [UIElement.md](./UIElement.md) - UI 元素基类
- [FrameworkElement.md](./FrameworkElement.md) - 布局框架
- [Panel.md](./Panel.md) - 容器控件
