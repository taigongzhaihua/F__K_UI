# ToggleButton 模板自定义指南

## 概述

ToggleButton 控件使用了高度可定制的 ControlTemplate 系统。本指南介绍如何理解和自定义 ToggleButton 的视觉模板。

## 当前模板架构

### 模板结构

ToggleButton 的默认模板包含以下元素：

```
Border (RootBorder)
  └─ ContentPresenter
```

- **RootBorder**: 根容器，提供背景、边框和圆角
- **ContentPresenter**: 显示 ToggleButton 的内容（文本、图标等）

### 视觉状态组

ToggleButton 使用两个独立的视觉状态组：

#### 1. CommonStates（通用交互状态）

控制鼠标交互的视觉反馈：

- **Normal**: 正常状态
  - 透明度：1.0
  - 边框颜色：绑定到 BorderBrush 属性

- **MouseOver**: 鼠标悬停
  - 透明度：0.85（降低透明度提供视觉反馈）
  - 边框颜色：白色高亮

- **Pressed**: 按下状态
  - 透明度：0.7（更明显的视觉变化）
  - 边框颜色：白色

- **Disabled**: 禁用状态
  - 背景色：灰色 (200, 200, 200)
  - 透明度：0.5

#### 2. CheckStates（选中状态）

控制 ToggleButton 的选中/未选中状态：

- **Unchecked**: 未选中
  - 背景色：绑定到 Background 属性

- **Checked**: 选中
  - 背景色：绑定到 CheckedBackground 属性

- **Indeterminate**: 不确定状态（三态模式）
  - 背景色：灰色 (120, 120, 120)
  - 透明度：0.8

## 模板设计原则

### 1. 状态组独立性

CommonStates 和 CheckStates 是独立的状态组，可以同时生效：

- **CommonStates** 控制交互反馈（透明度、边框）
- **CheckStates** 控制背景色（基于选中状态）

这种设计允许两种状态叠加，例如：
- 选中且鼠标悬停：显示 Checked 背景色 + MouseOver 透明度和边框效果
- 未选中且按下：显示 Unchecked 背景色 + Pressed 透明度和边框效果

### 2. 属性绑定

模板使用 `ToBinding()` 方法将动画目标值绑定到依赖属性：

```cpp
->ColorAnimation("RootBorder", "Background.Color")
->ToBinding(Control<ToggleButton>::BackgroundProperty())
```

这意味着：
- 用户可以在运行时更改属性值，视觉状态会自动使用新值
- 模板不需要硬编码颜色值
- 支持主题切换和动态样式

### 3. 动画时长

不同状态转换使用不同的动画时长：

- **快速交互**（50ms）：MouseOver、Pressed
  - 提供即时的视觉反馈
  
- **中等速度**（100ms）：Normal、Disabled
  - 平滑的状态恢复
  
- **较慢过渡**（150ms）：CheckStates
  - 更明显的状态变化，用户可以清楚地看到选中/未选中

## 自定义模板

### 方法 1：修改现有模板属性

最简单的自定义方式是修改 ToggleButton 的依赖属性：

```cpp
auto toggleBtn = new ToggleButton();
toggleBtn->Background(Color::FromRGB(100, 100, 200))        // 未选中背景
         ->CheckedBackground(Color::FromRGB(50, 200, 50))   // 选中背景
         ->BorderBrush(Color::FromRGB(80, 80, 180))         // 边框颜色
         ->BorderThickness(2.0f);                           // 边框粗细
```

### 方法 2：创建自定义 ControlTemplate

如果需要完全自定义外观，可以创建新的 ControlTemplate：

```cpp
auto customTemplate = new ControlTemplate();
customTemplate->SetTargetType(typeid(ToggleButton))
    ->SetFactory([]() -> UIElement* {
        // 创建自定义的视觉树
        return (new Border())
            ->Name("CustomRoot")
            ->CornerRadius(20.0f)  // 圆形按钮
            ->Child(
                (new Grid())
                    ->Children({
                        // 添加图标
                        (new Border())
                            ->Name("Icon")
                            ->Width(16.0f)
                            ->Height(16.0f),
                        // 添加文本
                        (new ContentPresenter<>())
                    })
            );
    })
    ->AddVisualStateGroup(
        // 定义自定义的视觉状态...
    );

toggleBtn->SetTemplate(customTemplate);
```

### 方法 3：继承并重写默认模板

创建 ToggleButton 的派生类（类似 CheckBox）：

```cpp
class CustomToggleButton : public ToggleButton {
public:
    CustomToggleButton() {
        // 设置自定义默认模板
        if (!GetTemplate()) {
            SetTemplate(CreateCustomTemplate());
        }
    }

private:
    static ControlTemplate* CreateCustomTemplate() {
        // 创建自定义模板...
    }
};
```

## 模板优化建议

### 当前模板的优点

✅ **双状态组架构**：CommonStates 和 CheckStates 独立工作，提供灵活的视觉效果  
✅ **属性绑定**：使用 ToBinding() 支持动态主题和样式  
✅ **平滑动画**：所有状态转换都有适当的动画效果  
✅ **良好的视觉反馈**：透明度和边框变化提供清晰的交互提示  
✅ **简洁高效**：单一 Border + ContentPresenter，性能优异  

### 可能的改进方向

1. **添加 IndeterminateBackground 属性**
   - 目前 Indeterminate 状态使用硬编码的灰色
   - 可以添加依赖属性允许用户自定义

2. **可选的图标支持**
   - 在 Checked 状态显示勾选图标
   - 在 Unchecked 状态隐藏图标

3. **更多的悬停效果选项**
   - 添加 MouseOverBorderBrush 属性
   - 添加 PressedBorderBrush 属性

4. **缩放动画**
   - 在按下时添加轻微的缩放效果（0.95x）
   - 提供更明显的按下反馈

## 示例：添加缩放动画

如果想要在按下时添加缩放效果：

```cpp
->State("Pressed")
    ->DoubleAnimation("RootBorder", "Opacity")
    ->To(0.7)
    ->Duration(50)
    ->EndAnimation()
    ->DoubleAnimation("RootBorder", "ScaleX")  // 添加 X 轴缩放
    ->To(0.95)
    ->Duration(50)
    ->EndAnimation()
    ->DoubleAnimation("RootBorder", "ScaleY")  // 添加 Y 轴缩放
    ->To(0.95)
    ->Duration(50)
    ->EndAnimation()
    ->EndState()
```

## 结论

ToggleButton 的当前模板设计已经相当完善，遵循了 WPF 的最佳实践：

1. **职责分离**：不同的状态组管理不同方面的视觉效果
2. **可扩展性**：通过依赖属性和绑定支持自定义
3. **性能优化**：简洁的视觉树和高效的动画
4. **用户友好**：清晰的视觉反馈和平滑的过渡效果

对于大多数使用场景，默认模板已经足够。如果需要特殊的外观，可以通过修改属性或创建自定义模板来实现。

## 参考

- CheckBox 控件：演示了如何通过继承 ToggleButton 并提供自定义模板来创建新控件
- Button 控件：演示了基础的 CommonStates 实现
- VisualStateManager：视觉状态管理的核心系统
