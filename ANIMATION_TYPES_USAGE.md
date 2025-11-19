# 动画类型使用指南

VisualStateBuilder 现在支持4种动画类型,对应WPF中最常用的动画:

## 1. ColorAnimation - 颜色动画

用于动画化颜色属性(如Background, Foreground, BorderBrush等)。

```cpp
auto group = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("MouseOver")
        ->ColorAnimation("RootBorder", "Background.Color")
            ->To(Color::FromRGB(229, 241, 251))
            ->Duration(150)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

## 2. DoubleAnimation - 数值动画

用于动画化double类型属性(如Opacity, Width, Height等)。

```cpp
auto group = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("Disabled")
        ->DoubleAnimation("RootBorder", "Opacity")
            ->From(1.0)
            ->To(0.5)
            ->Duration(200)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

## 3. PointAnimation - 点位置动画

用于动画化Point类型属性(如Canvas.Left/Top组合,或自定义位置属性)。

```cpp
auto group = VisualStateBuilder::CreateGroup("MoveStates")
    ->State("MovedRight")
        ->PointAnimation("MovableElement", "Position")
            ->From(ui::Point{0, 0})
            ->To(ui::Point{100, 50})
            ->Duration(300)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

**使用场景**:
- 元素位置移动动画
- 路径动画的起点/终点
- 自定义位置属性的动画

## 4. ThicknessAnimation - 厚度动画

用于动画化Thickness类型属性(如Margin, Padding, BorderThickness等)。

```cpp
auto group = VisualStateBuilder::CreateGroup("SpacingStates")
    ->State("Expanded")
        ->ThicknessAnimation("ContentPanel", "Margin")
            ->From(Thickness(10))
            ->To(Thickness(20, 30, 20, 30))
            ->Duration(250)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

**使用场景**:
- Margin动画(扩展/收缩边距)
- Padding动画(改变内边距)
- BorderThickness动画(边框粗细变化)

## 完整示例 - 复杂按钮状态

```cpp
auto buttonStates = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("Normal")
        ->ColorAnimation("RootBorder", "Background.Color")
            ->ToBinding(Control<Button>::BackgroundProperty())
            ->Duration(200)
        ->EndAnimation()
        ->DoubleAnimation("RootBorder", "Opacity")
            ->To(1.0)
            ->Duration(200)
        ->EndAnimation()
        ->ThicknessAnimation("ContentPresenter", "Margin")
            ->To(Thickness(0))
            ->Duration(150)
        ->EndAnimation()
    ->EndState()
    
    ->State("MouseOver")
        ->ColorAnimation("RootBorder", "Background.Color")
            ->ToBinding(Button::MouseOverBackgroundProperty())
            ->Duration(150)
        ->EndAnimation()
        ->ColorAnimation("RootBorder", "BorderBrush.Color")
            ->To(Color::FromRGB(100, 150, 200))
            ->Duration(150)
        ->EndAnimation()
    ->EndState()
    
    ->State("Pressed")
        ->ColorAnimation("RootBorder", "Background.Color")
            ->ToBinding(Button::PressedBackgroundProperty())
            ->Duration(100)
        ->EndAnimation()
        ->ThicknessAnimation("ContentPresenter", "Margin")
            ->To(Thickness(1, 2, -1, 0))  // 模拟按下效果
            ->Duration(100)
        ->EndAnimation()
    ->EndState()
    
    ->State("Disabled")
        ->ColorAnimation("RootBorder", "Background.Color")
            ->To(Color::FromRGB(200, 200, 200))
            ->Duration(200)
        ->EndAnimation()
        ->DoubleAnimation("RootBorder", "Opacity")
            ->To(0.6)
            ->Duration(200)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

## 与WPF对比

| F__K_UI | WPF | 用途 |
|---------|-----|------|
| `ColorAnimation` | `ColorAnimation` | 颜色插值 |
| `DoubleAnimation` | `DoubleAnimation` | 数值插值 |
| `PointAnimation` | `PointAnimation` | 点位置插值 |
| `ThicknessAnimation` | `ThicknessAnimation` | 厚度插值 |

## 未来扩展

可以考虑添加的动画类型:
- **关键帧动画** (KeyFrame) - 非线性动画
- **Int32Animation** - 整数动画
- **RectAnimation** - 矩形动画
- **VectorAnimation** - 向量动画

## 技术细节

所有动画类型都继承自 `Animation<T>` 模板类,提供:
- 线性插值 (`Interpolate`)
- 加法运算 (`Add`)
- 持续时间控制
- From/To值设置
- 目标对象绑定

动画通过 `Storyboard` 管理,在 `VisualState` 切换时自动播放。
