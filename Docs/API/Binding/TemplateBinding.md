# TemplateBinding

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/TemplateBinding.h` |
| **继承自** | `Binding` |

## 描述

`TemplateBinding` 是一种特殊的绑定，用于在 `ControlTemplate` 中将模板内元素的属性绑定到应用模板的控件（`TemplatedParent`）的属性。

**WPF 对应**：`{TemplateBinding Property}`

主要特点：
- 继承自 `Binding`，复用绑定基础设施
- 自动将绑定源设置为 `TemplatedParent`
- 默认使用 `OneWay` 绑定模式
- 简化 ControlTemplate 内的属性绑定语法

## 类定义

```cpp
namespace fk::binding {

class TemplateBinding : public Binding {
public:
    explicit TemplateBinding(const DependencyProperty& sourceProperty);
    ~TemplateBinding() override = default;

    [[nodiscard]] bool IsTemplateBinding() const noexcept override;
    [[nodiscard]] const DependencyProperty* GetTemplateBindingSourceProperty() const noexcept override;
    [[nodiscard]] std::unique_ptr<Binding> Clone() const override;

private:
    const DependencyProperty* sourceProperty_{nullptr};
};

} // namespace fk::binding
```

---

## 构造函数

### TemplateBinding()

```cpp
explicit TemplateBinding(const DependencyProperty& sourceProperty);
```

构造一个模板绑定。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `sourceProperty` | `const DependencyProperty&` | 要绑定的 TemplatedParent 属性 |

**说明**：
- 自动使用属性名称作为 `Path`
- 默认设置 `BindingMode::OneWay`
- `Source` 不在构造时设置，而是在应用绑定时由 `BindingExpression` 自动设置为 `TemplatedParent`

**示例**：
```cpp
// 创建绑定到 Button.Background 属性的 TemplateBinding
fk::TemplateBinding bgBinding(Button::BackgroundProperty());
```

---

## 公共方法

### IsTemplateBinding

```cpp
[[nodiscard]] bool IsTemplateBinding() const noexcept override;
```

标记这是一个 TemplateBinding。

**返回值**：始终返回 `true`

**说明**：重写自 `Binding`，用于在绑定应用时识别模板绑定

### GetTemplateBindingSourceProperty

```cpp
[[nodiscard]] const DependencyProperty* GetTemplateBindingSourceProperty() const noexcept override;
```

获取源属性（TemplatedParent 的属性）。

**返回值**：源依赖属性的指针

**示例**：
```cpp
auto binding = fk::TemplateBinding(Button::BackgroundProperty());
const auto* prop = binding.GetTemplateBindingSourceProperty();
std::cout << "绑定到: " << prop->Name() << std::endl;  // "Background"
```

### Clone

```cpp
[[nodiscard]] std::unique_ptr<Binding> Clone() const override;
```

克隆 TemplateBinding 对象。

**返回值**：新的 `TemplateBinding` 实例（作为 `Binding` 指针）

**说明**：用于复制模板定义时克隆绑定配置

---

## 工作原理

### 绑定流程

```
┌─────────────────────────────────────────────────────────────┐
│                    ControlTemplate 定义                      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  创建 TemplateBinding                                        │
│  - 指定 sourceProperty（如 Button::BackgroundProperty()）    │
│  - 自动设置 Path = "Background"                              │
│  - 默认 Mode = OneWay                                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  应用模板到控件                                              │
│  - Control.ApplyTemplate()                                  │
│  - 模板元素的 TemplatedParent 设置为控件                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  BindingExpression 创建                                      │
│  - 检测 IsTemplateBinding() == true                         │
│  - 自动设置 Source = TemplatedParent                        │
│  - 激活绑定，开始属性同步                                    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  运行时同步                                                  │
│  - TemplatedParent 属性变更                                  │
│  - 自动更新模板内元素的对应属性                              │
└─────────────────────────────────────────────────────────────┘
```

### 与普通 Binding 的区别

| 特性 | TemplateBinding | 普通 Binding |
|------|-----------------|--------------|
| 源设置 | 自动设置为 TemplatedParent | 需要显式设置 Source 或使用 DataContext |
| 默认模式 | OneWay | 取决于属性元数据 |
| 使用场景 | ControlTemplate 内部 | 任何绑定场景 |
| 路径设置 | 自动使用属性名 | 需要显式指定 Path |

---

## 使用示例

### 基本用法：绑定 Background

```cpp
#include "fk/binding/TemplateBinding.h"

// 在 ControlTemplate 中
auto controlTemplate = std::make_shared<ControlTemplate>();

controlTemplate->SetVisualTree([]{
    auto border = std::make_shared<Border>();
    
    // 绑定 Border.Background 到 TemplatedParent.Background
    border->SetBinding(
        Border::BackgroundProperty(),
        fk::TemplateBinding(Control::BackgroundProperty())
    );
    
    return border;
});

// 应用模板
button->SetTemplate(controlTemplate);
// 现在 Border.Background 自动跟随 Button.Background
```

### 绑定多个属性

```cpp
auto controlTemplate = std::make_shared<ControlTemplate>();

controlTemplate->SetVisualTree([]{
    auto border = std::make_shared<Border>();
    
    // 绑定 Background
    border->SetBinding(
        Border::BackgroundProperty(),
        fk::TemplateBinding(Control::BackgroundProperty())
    );
    
    // 绑定 BorderBrush
    border->SetBinding(
        Border::BorderBrushProperty(),
        fk::TemplateBinding(Control::BorderBrushProperty())
    );
    
    // 绑定 BorderThickness
    border->SetBinding(
        Border::BorderThicknessProperty(),
        fk::TemplateBinding(Control::BorderThicknessProperty())
    );
    
    // 创建内容呈现器
    auto contentPresenter = std::make_shared<ContentPresenter>();
    
    // 绑定 Content
    contentPresenter->SetBinding(
        ContentPresenter::ContentProperty(),
        fk::TemplateBinding(ContentControl::ContentProperty())
    );
    
    border->SetChild(contentPresenter);
    return border;
});
```

### 完整的 Button 模板示例

```cpp
auto buttonTemplate = std::make_shared<ControlTemplate>();

buttonTemplate->SetVisualTree([]{
    // 外层 Border
    auto border = std::make_shared<Border>();
    border->SetBinding(
        Border::BackgroundProperty(),
        fk::TemplateBinding(Button::BackgroundProperty())
    );
    border->SetBinding(
        Border::BorderBrushProperty(),
        fk::TemplateBinding(Button::BorderBrushProperty())
    );
    border->SetBinding(
        Border::CornerRadiusProperty(),
        fk::TemplateBinding(Button::CornerRadiusProperty())
    );
    border->SetBinding(
        Border::PaddingProperty(),
        fk::TemplateBinding(Button::PaddingProperty())
    );
    
    // 内容呈现器
    auto contentPresenter = std::make_shared<ContentPresenter>();
    contentPresenter->SetBinding(
        ContentPresenter::ContentProperty(),
        fk::TemplateBinding(Button::ContentProperty())
    );
    contentPresenter->SetHorizontalAlignment(HorizontalAlignment::Center);
    contentPresenter->SetVerticalAlignment(VerticalAlignment::Center);
    
    border->SetChild(contentPresenter);
    return border;
});

// 应用到 Button
auto button = std::make_shared<Button>();
button->SetTemplate(buttonTemplate);
button->SetBackground(Colors::Blue);
button->SetContent("Click Me");
// Border 自动使用蓝色背景，内容显示 "Click Me"
```

### 检查 TemplateBinding 类型

```cpp
fk::Binding binding;
binding.Path("Name");

fk::TemplateBinding templateBinding(Control::BackgroundProperty());

// 区分绑定类型
if (binding.IsTemplateBinding()) {
    std::cout << "这是 TemplateBinding" << std::endl;
} else {
    std::cout << "这是普通 Binding" << std::endl;
}

if (templateBinding.IsTemplateBinding()) {
    const auto* prop = templateBinding.GetTemplateBindingSourceProperty();
    std::cout << "TemplateBinding 源属性: " << prop->Name() << std::endl;
}
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `sourceProperty_` | `const DependencyProperty*` | 指向 TemplatedParent 属性的指针 |

---

## 注意事项

1. **仅用于 ControlTemplate**：TemplateBinding 设计用于 ControlTemplate 内部，在其他场景可能无法正常工作

2. **单向绑定**：默认且通常是 OneWay，不支持将模板元素的值写回到 TemplatedParent

3. **性能**：比等效的普通 Binding 更高效，因为直接引用属性而非路径解析

4. **克隆支持**：在模板被多个控件使用时，每个控件会获得绑定的克隆

---

## 相关类

- [Binding](Binding.md) - 基类，通用数据绑定
- [BindingExpression](BindingExpression.md) - 处理 TemplateBinding 的实际绑定逻辑
- [DependencyProperty](DependencyProperty.md) - 依赖属性定义

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
