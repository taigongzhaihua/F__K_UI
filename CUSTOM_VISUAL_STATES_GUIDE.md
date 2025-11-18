# 自定义Button视觉状态指南

## 概述

本指南说明如何通过重定义`ControlTemplate`来自定义Button的视觉状态机，类似于WPF/WinUI中在XAML中定义`VisualStateManager.VisualStateGroups`。

## 背景

在之前的实现中，Button的视觉状态是硬编码在Button.cpp中的：
- 状态定义在`CreateNormalState()`、`CreateMouseOverState()`等私有方法中
- 无法通过模板自定义
- 不支持声明式定义

现在的改进允许你：
- ✅ 在`ControlTemplate`中定义视觉状态
- ✅ 使用声明式的`VisualStateBuilder` API
- ✅ 像WPF/WinUI一样在"布局"中定义状态
- ✅ 保持向后兼容（如果模板中没有定义状态，使用默认状态）

## 三种使用方式

### 方式1：使用默认视觉状态（无需任何代码）

```cpp
auto button = new Button();
button->Content("默认按钮");
// 自动使用Button内置的默认状态
```

**默认状态包括：**
- **Normal**: 浅灰色背景 `RGB(240, 240, 240)`
- **MouseOver**: 浅蓝色背景 `RGB(229, 241, 251)`
- **Pressed**: 深蓝色背景 `RGB(204, 228, 247)`
- **Disabled**: 灰色背景 `RGB(200, 200, 200)`，透明度0.6

**优点：**
- 零配置，开箱即用
- 所有Button有统一的外观
- 适合快速原型开发

### 方式2：使用VisualStateBuilder（推荐）

这是**推荐的方式**，提供类似WPF/WinUI的声明式API。

```cpp
// 创建自定义模板
auto tmpl = new ControlTemplate();
tmpl->SetTargetType(typeid(Button))
    ->SetFactory([]() -> UIElement* {
        // 定义视觉树
        auto border = new Border();
        border->Background(TemplateBinding(Control<Button>::BackgroundProperty()))
              ->BorderBrush(new SolidColorBrush(Color::FromRGB(100, 100, 200)))
              ->BorderThickness(2.0f)
              ->Padding(15.0f, 8.0f, 15.0f, 8.0f)
              ->CornerRadius(8.0f);
        
        border->Child(
            (new ContentPresenter<>())
                ->SetHAlign(HorizontalAlignment::Center)
                ->SetVAlign(VerticalAlignment::Center)
        );
        
        return border;
    });

// 使用VisualStateBuilder定义视觉状态
// 注意：这里的动画目标应该是模板中的元素（如Border）
auto dummyBrush = new SolidColorBrush(Color::FromRGB(255, 255, 255));

auto stateGroup = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("Normal")
        ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
            ->To(Color::FromRGB(255, 200, 200))  // 浅红色
            ->Duration(250)
        ->EndAnimation()
    ->EndState()
    ->State("MouseOver")
        ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
            ->To(Color::FromRGB(255, 150, 150))  // 中红色
            ->Duration(200)
        ->EndAnimation()
    ->EndState()
    ->State("Pressed")
        ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
            ->To(Color::FromRGB(200, 100, 100))  // 深红色
            ->Duration(100)
        ->EndAnimation()
    ->EndState()
    ->State("Disabled")
        ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
            ->To(Color::FromRGB(180, 180, 180))  // 灰色
            ->Duration(250)
        ->EndAnimation()
    ->EndState()
    ->Build();

// 将视觉状态组添加到模板
tmpl->AddVisualStateGroup(stateGroup);

// 应用到Button
auto button = new Button();
button->Content("自定义红色主题按钮");
button->SetTemplate(tmpl);
```

**VisualStateBuilder API说明：**

| 方法 | 说明 |
|------|------|
| `CreateGroup(name)` | 创建一个新的状态组（如"CommonStates"） |
| `State(name)` | 开始定义一个新状态（如"Normal", "MouseOver"） |
| `ColorAnimation(target, property)` | 为当前状态添加颜色动画 |
| `DoubleAnimation(target, property)` | 为当前状态添加双精度浮点数动画 |
| `From(value)` | 设置动画的起始值 |
| `To(value)` | 设置动画的目标值 |
| `Duration(ms)` | 设置动画持续时间（毫秒） |
| `EndAnimation()` | 结束当前动画的定义 |
| `EndState()` | 结束当前状态的定义 |
| `Build()` | 完成构建并返回`VisualStateGroup` |

**优点：**
- 声明式API，代码简洁
- 类似WPF/WinUI的XAML语法
- 链式调用，易于阅读和维护
- 适合大多数自定义场景

### 方式3：手动创建VisualState对象（最灵活）

```cpp
// 创建模板
auto tmpl = new ControlTemplate();
tmpl->SetTargetType(typeid(Button));

// 定义视觉树（省略...）

// 手动创建状态组
auto commonStates = std::make_shared<VisualStateGroup>("CommonStates");

// 创建Normal状态
auto normalState = std::make_shared<VisualState>("Normal");
auto normalStoryboard = std::make_shared<Storyboard>();

auto normalAnim = std::make_shared<ColorAnimation>();
normalAnim->SetTo(Color::FromRGB(200, 255, 200));  // 浅绿色
normalAnim->SetDuration(Duration(std::chrono::milliseconds(250)));
normalAnim->SetTarget(targetBrush, &SolidColorBrush::ColorProperty());
normalStoryboard->AddChild(normalAnim);

normalState->SetStoryboard(normalStoryboard);
commonStates->AddState(normalState);

// 创建其他状态（MouseOver, Pressed, Disabled）...

// 将状态组添加到模板
tmpl->AddVisualStateGroup(commonStates);

// 应用到Button
auto button = new Button();
button->SetTemplate(tmpl);
```

**优点：**
- 完全控制每个细节
- 可以实现复杂的动画组合
- 适合高级定制需求

**缺点：**
- 代码冗长
- 需要深入理解动画系统

## 与WPF/WinUI的对应关系

### WPF/WinUI XAML

```xml
<ControlTemplate TargetType="Button">
    <Border x:Name="border" 
            Background="{TemplateBinding Background}"
            BorderBrush="Blue" 
            BorderThickness="2">
        <ContentPresenter/>
        
        <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
                <VisualState x:Name="Normal">
                    <Storyboard>
                        <ColorAnimation Storyboard.TargetName="border"
                                      Storyboard.TargetProperty="(Border.Background).(SolidColorBrush.Color)"
                                      To="#FFC8C8"
                                      Duration="0:0:0.2"/>
                    </Storyboard>
                </VisualState>
                <VisualState x:Name="MouseOver">
                    <Storyboard>
                        <ColorAnimation Storyboard.TargetName="border"
                                      Storyboard.TargetProperty="(Border.Background).(SolidColorBrush.Color)"
                                      To="#FF9696"
                                      Duration="0:0:0.15"/>
                    </Storyboard>
                </VisualState>
            </VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
    </Border>
</ControlTemplate>
```

### F__K_UI C++ (使用VisualStateBuilder)

```cpp
auto tmpl = new ControlTemplate();
tmpl->SetTargetType(typeid(Button))
    ->SetFactory([]() -> UIElement* {
        auto border = new Border();
        border->Background(TemplateBinding(Control<Button>::BackgroundProperty()))
              ->BorderBrush(new SolidColorBrush(Color::Blue))
              ->BorderThickness(2.0f)
              ->Child(new ContentPresenter<>());
        return border;
    });

auto borderBrush = new SolidColorBrush(Color::White);

tmpl->AddVisualStateGroup(
    VisualStateBuilder::CreateGroup("CommonStates")
        ->State("Normal")
            ->ColorAnimation(borderBrush, &SolidColorBrush::ColorProperty())
                ->To(Color::FromRGB(255, 200, 200))
                ->Duration(200)
            ->EndAnimation()
        ->EndState()
        ->State("MouseOver")
            ->ColorAnimation(borderBrush, &SolidColorBrush::ColorProperty())
                ->To(Color::FromRGB(255, 150, 150))
                ->Duration(150)
            ->EndAnimation()
        ->EndState()
        ->Build()
);
```

**对应关系：**

| WPF/WinUI XAML | F__K_UI C++ |
|----------------|-------------|
| `<ControlTemplate>` | `new ControlTemplate()` |
| `<VisualStateManager.VisualStateGroups>` | `->AddVisualStateGroup(...)` |
| `<VisualStateGroup x:Name="...">` | `VisualStateBuilder::CreateGroup("...")` |
| `<VisualState x:Name="...">` | `->State("...")` |
| `<Storyboard>` | （自动创建） |
| `<ColorAnimation To="..." Duration="..."/>` | `->ColorAnimation(...)->To(...)->Duration(...)` |

## 状态机工作原理

### 状态转换逻辑

Button会根据用户交互和属性变化自动切换视觉状态：

```
                    鼠标进入 (OnPointerEntered)
        Normal ──────────────────────→ MouseOver
          ↑                                 ↓
          │                          鼠标按下 (OnPointerPressed)
          │                                 ↓
          │                              Pressed
          │                                 ↓
          │                          鼠标释放 (OnPointerReleased)
          │                                 ↓
          └─────────────────────────── MouseOver
                  鼠标离开 (OnPointerExited)
                  
        
        任何状态 ←──SetIsEnabled(false)──→ Disabled
                 ←──SetIsEnabled(true)───
```

### 状态优先级

在`Button::UpdateVisualState()`中，状态按以下优先级选择：

1. **Disabled** - 如果 `!GetIsEnabled()`
2. **Pressed** - 如果 `isPressed_` 且已启用
3. **MouseOver** - 如果 `IsMouseOver()` 且已启用
4. **Normal** - 默认状态

### 初始化流程

1. Button构造时设置默认模板
2. 模板应用时（`OnTemplateApplied()`）：
   - 检查模板是否定义了视觉状态
   - 如果有，使用模板中的状态
   - 如果没有，创建默认状态
3. 创建`VisualStateManager`并添加状态组
4. 根据当前状态进入相应的视觉状态

## 常见用例

### 用例1：创建自定义主题

```cpp
// 创建可复用的主题模板
ControlTemplate* CreateDarkThemeButtonTemplate() {
    auto tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    tmpl->SetFactory([]() -> UIElement* {
        // 深色主题的视觉树
        auto border = new Border();
        border->Background(TemplateBinding(Control<Button>::BackgroundProperty()))
              ->BorderBrush(new SolidColorBrush(Color::FromRGB(60, 60, 60)))
              ->BorderThickness(1.0f)
              ->Padding(10.0f, 5.0f, 10.0f, 5.0f)
              ->CornerRadius(4.0f)
              ->Child(new ContentPresenter<>());
        return border;
    });
    
    auto dummyBrush = new SolidColorBrush(Color::Black);
    
    tmpl->AddVisualStateGroup(
        VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
                    ->To(Color::FromRGB(45, 45, 45))  // 深灰色
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            ->State("MouseOver")
                ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
                    ->To(Color::FromRGB(60, 60, 60))  // 稍亮
                    ->Duration(150)
                ->EndAnimation()
            ->EndState()
            ->State("Pressed")
                ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
                    ->To(Color::FromRGB(30, 30, 30))  // 更深
                    ->Duration(100)
                ->EndAnimation()
            ->EndState()
            ->State("Disabled")
                ->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
                    ->To(Color::FromRGB(70, 70, 70))  // 灰色
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            ->Build()
    );
    
    return tmpl;
}

// 使用
auto button1 = new Button();
button1->SetTemplate(CreateDarkThemeButtonTemplate());

auto button2 = new Button();
button2->SetTemplate(CreateDarkThemeButtonTemplate());
```

### 用例2：为不同类型的按钮定义不同的状态

```cpp
// 主要按钮（Primary Button）
auto primaryTemplate = new ControlTemplate();
// ... 蓝色主题的状态定义

// 危险按钮（Danger Button）
auto dangerTemplate = new ControlTemplate();
// ... 红色主题的状态定义

// 应用
auto saveButton = new Button();
saveButton->Content("保存");
saveButton->SetTemplate(primaryTemplate);

auto deleteButton = new Button();
deleteButton->Content("删除");
deleteButton->SetTemplate(dangerTemplate);
```

### 用例3：添加复杂的多属性动画

```cpp
auto stateGroup = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("MouseOver")
        // 同时动画化多个属性
        ->ColorAnimation(border, &Border::BackgroundProperty())
            ->To(Color::FromRGB(200, 200, 255))
            ->Duration(200)
        ->EndAnimation()
        ->DoubleAnimation(border, &Border::OpacityProperty())
            ->To(0.9)
            ->Duration(200)
        ->EndAnimation()
    ->EndState()
    ->Build();
```

## 最佳实践

### 1. 优先使用VisualStateBuilder

```cpp
// ✅ 推荐：清晰、简洁
auto group = VisualStateBuilder::CreateGroup("CommonStates")
    ->State("Normal")
        ->ColorAnimation(...)
        ->EndAnimation()
    ->EndState()
    ->Build();

// ❌ 避免：冗长、容易出错
auto group = std::make_shared<VisualStateGroup>("CommonStates");
auto state = std::make_shared<VisualState>("Normal");
auto storyboard = std::make_shared<Storyboard>();
// ... 更多代码
```

### 2. 为状态定义有意义的名称

```cpp
// ✅ 推荐：使用标准名称
->State("Normal")
->State("MouseOver")
->State("Pressed")
->State("Disabled")

// ❌ 避免：自定义名称（除非有特殊需求）
->State("Default")
->State("Hover")
```

### 3. 保持动画持续时间一致

```cpp
// ✅ 推荐：相似的动画使用相似的持续时间
->State("Normal")->...->Duration(200)->...
->State("MouseOver")->...->Duration(150)->...
->State("Pressed")->...->Duration(100)->...  // 按下动画更快

// ❌ 避免：持续时间差异过大
->State("Normal")->...->Duration(2000)->...  // 太慢
->State("Pressed")->...->Duration(10)->...   // 太快
```

### 4. 考虑性能

```cpp
// ✅ 推荐：只动画化需要改变的属性
->State("MouseOver")
    ->ColorAnimation(...)  // 只改变颜色
    ->EndAnimation()
->EndState()

// ⚠️ 注意：避免过多的同时动画
->State("MouseOver")
    ->ColorAnimation(...)
    ->DoubleAnimation(...)
    ->DoubleAnimation(...)  // 可能影响性能
    // ...
```

### 5. 创建可复用的模板

```cpp
// ✅ 推荐：创建工厂函数
ControlTemplate* CreateCustomButtonTemplate(const Color& primaryColor) {
    auto tmpl = new ControlTemplate();
    // ... 使用primaryColor参数化
    return tmpl;
}

// 使用
button1->SetTemplate(CreateCustomButtonTemplate(Color::Blue));
button2->SetTemplate(CreateCustomButtonTemplate(Color::Red));
```

## 向后兼容性

所有现有代码继续工作，不需要任何修改：

```cpp
// 这些代码无需改变
auto button = new Button();
button->Content("按钮");
// 自动使用默认视觉状态
```

如果你想使用自定义状态，只需要：
1. 创建`ControlTemplate`
2. 使用`AddVisualStateGroup()`添加状态组
3. 应用到Button

## 注意事项

### 1. 动画目标

当前示例中使用`dummyBrush`作为动画目标是一个临时解决方案。在实际使用中，你应该：
- 动画化模板实例中的实际元素
- 或使用命名元素（通过`x:Name`）

### 2. 状态组名称

Button默认查找名为`"CommonStates"`的状态组。如果你使用其他名称，状态转换可能不会生效。

### 3. 必需的状态

虽然你可以只定义部分状态，但建议定义所有四个基本状态：
- Normal
- MouseOver
- Pressed
- Disabled

### 4. 线程安全

视觉状态的创建和应用应该在UI线程上进行。

## 总结

新的视觉状态系统提供了：

✅ **灵活性** - 可以在模板中完全自定义按钮的视觉反馈

✅ **声明式API** - `VisualStateBuilder`提供类似XAML的体验

✅ **向后兼容** - 现有代码无需修改

✅ **可复用性** - 模板可以在多个Button实例间共享

✅ **类似WPF/WinUI** - 熟悉WPF的开发者可以快速上手

这使得F__K_UI在保持C++性能优势的同时，提供了现代UI框架的开发体验！

## 参考示例

完整的示例代码请查看：
- `examples/button_custom_visual_states_demo.cpp` - 完整演示程序
- `include/fk/animation/VisualStateBuilder.h` - Builder API文档
- `include/fk/ui/ControlTemplate.h` - ControlTemplate扩展

---

**文档版本**: 1.0  
**创建日期**: 2025-11-18  
**作者**: F__K_UI Team
