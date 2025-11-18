# 使用TargetName定义视觉状态 - 使用指南

## 问题背景

在之前的实现中，定义视觉状态时需要手动传递动画目标对象：

```cpp
// ❌ 不推荐：手动传递对象
auto dummyBrush = new SolidColorBrush(Color::White);
->ColorAnimation(dummyBrush, &SolidColorBrush::ColorProperty())
```

这不符合WPF/WinUI的设计模式。在WPF中使用`Storyboard.TargetName`来引用模板中的元素：

```xml
<ColorAnimation Storyboard.TargetName="border" 
                Storyboard.TargetProperty="Background.Color"/>
```

## 改进后的API

现在`VisualStateBuilder`支持使用TargetName：

```cpp
// ✅ 推荐：使用TargetName
->ColorAnimation("border", "Background.Color")
->DoubleAnimation("border", "Opacity")
```

## 完整示例

### WPF/WinUI XAML

```xml
<ControlTemplate TargetType="Button">
    <Border x:Name="RootBorder" 
            Background="{TemplateBinding Background}"
            BorderBrush="Blue">
        <ContentPresenter/>
        
        <VisualStateManager.VisualStateGroups>
            <VisualStateGroup x:Name="CommonStates">
                <VisualState x:Name="Normal">
                    <Storyboard>
                        <ColorAnimation 
                            Storyboard.TargetName="RootBorder"
                            Storyboard.TargetProperty="Background.Color"
                            To="#F0F0F0"
                            Duration="0:0:0.2"/>
                    </Storyboard>
                </VisualState>
                <VisualState x:Name="MouseOver">
                    <Storyboard>
                        <ColorAnimation 
                            Storyboard.TargetName="RootBorder"
                            Storyboard.TargetProperty="Background.Color"
                            To="#E5F1FB"
                            Duration="0:0:0.15"/>
                    </Storyboard>
                </VisualState>
            </VisualStateGroup>
        </VisualStateManager.VisualStateGroups>
    </Border>
</ControlTemplate>
```

### F__K_UI C++ (使用TargetName)

```cpp
#include "fk/ui/Button.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/animation/VisualStateBuilder.h"

// 创建自定义按钮模板
ControlTemplate* CreateCustomButtonTemplate() {
    auto tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(Button));
    
    // 定义视觉树，注意设置元素名称
    tmpl->SetFactory([]() -> UIElement* {
        auto border = new Border();
        border->SetName("RootBorder");  // ⭐ 设置名称，供TargetName引用
        border->Background(TemplateBinding(Control<Button>::BackgroundProperty()));
        border->BorderBrush(new SolidColorBrush(Color::FromRGB(0, 0, 255)));
        border->BorderThickness(1.0f);
        border->Padding(10.0f, 5.0f, 10.0f, 5.0f);
        border->CornerRadius(4.0f);
        
        auto presenter = new ContentPresenter<>();
        presenter->SetHAlign(HorizontalAlignment::Center);
        presenter->SetVAlign(VerticalAlignment::Center);
        
        border->Child(presenter);
        return border;
    });
    
    // 使用VisualStateBuilder和TargetName定义状态
    tmpl->AddVisualStateGroup(
        VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation("RootBorder", "Background.Color")  // ⭐ 使用名称引用
                    ->To(Color::FromRGB(240, 240, 240))
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            
            ->State("MouseOver")
                ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(229, 241, 251))
                    ->Duration(150)
                ->EndAnimation()
            ->EndState()
            
            ->State("Pressed")
                ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(204, 228, 247))
                    ->Duration(100)
                ->EndAnimation()
            ->EndState()
            
            ->State("Disabled")
                ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(200, 200, 200))
                    ->Duration(200)
                ->EndAnimation()
                ->DoubleAnimation("RootBorder", "Opacity")  // 可以添加多个动画
                    ->To(0.6)
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            
            ->Build()
    );
    
    return tmpl;
}

// 使用自定义模板
void CreateButton() {
    auto button = new Button();
    button->SetTemplate(CreateCustomButtonTemplate());
    button->Content("自定义按钮");
}
```

## API说明

### VisualStateBuilder::ColorAnimation

```cpp
// 方式1：使用TargetName（推荐）
->ColorAnimation(targetName, propertyPath)
```

**参数：**
- `targetName` (string): 模板中元素的名称（通过`SetName()`设置）
- `propertyPath` (string): 属性路径，如 `"Background.Color"`, `"BorderBrush.Color"`

**示例：**
```cpp
->ColorAnimation("RootBorder", "Background.Color")
->ColorAnimation("InnerBorder", "BorderBrush.Color")
```

### VisualStateBuilder::DoubleAnimation

```cpp
// 方式1：使用TargetName（推荐）
->DoubleAnimation(targetName, propertyPath)
```

**参数：**
- `targetName` (string): 模板中元素的名称
- `propertyPath` (string): 属性路径，如 `"Opacity"`, `"Width"`, `"Height"`

**示例：**
```cpp
->DoubleAnimation("RootBorder", "Opacity")
->DoubleAnimation("ContentArea", "Width")
```

## 关键步骤

### 1. 在模板中设置元素名称

```cpp
tmpl->SetFactory([]() -> UIElement* {
    auto border = new Border();
    border->SetName("RootBorder");  // ⭐ 必须设置名称
    // ...
    return border;
});
```

### 2. 使用TargetName引用元素

```cpp
->ColorAnimation("RootBorder", "Background.Color")  // ⭐ 使用相同的名称
```

### 3. 指定完整的属性路径

```cpp
"Background.Color"      // 访问Background画刷的Color属性
"BorderBrush.Color"     // 访问BorderBrush画刷的Color属性
"Opacity"               // 直接访问Opacity属性
```

## 与直接对象方式的对比

### 方式1：TargetName（推荐）

✅ **优点：**
- 不需要创建临时对象
- 符合WPF/WinUI的设计模式
- 代码更清晰，意图更明确
- 自动利用TemplateBinding机制

❌ **缺点：**
- 需要设置元素名称
- 属性路径需要正确指定

**示例：**
```cpp
->ColorAnimation("RootBorder", "Background.Color")
```

### 方式2：直接对象（向后兼容）

✅ **优点：**
- 可以直接操作任何对象
- 不依赖于名称查找

❌ **缺点：**
- 需要手动创建和管理临时对象
- 代码冗长
- 不符合XAML模式

**示例：**
```cpp
auto brush = new SolidColorBrush(Color::White);
->ColorAnimation(brush, &SolidColorBrush::ColorProperty())
```

## 实现状态

### ✅ 已实现

1. **VisualStateBuilder API扩展**
   - `ColorAnimation(targetName, propertyPath)`
   - `DoubleAnimation(targetName, propertyPath)`

2. **Storyboard附加属性**
   - `SetTargetName(timeline, name)`
   - `GetTargetName(timeline)`
   - `SetTargetProperty(timeline, path)`
   - `GetTargetProperty(timeline)`

3. **TargetName信息存储**
   - TargetName和PropertyPath正确保存
   - 可以在运行时访问

### ⚠️ 部分实现

**TargetName运行时解析**

当前状态：
- Storyboard.Begin()中添加了解析钩子
- 但实际解析逻辑需要访问UIElement的FindName方法
- 由于类型转换限制，完整实现需要更多工作

临时解决方案：
- Button可以在InitializeVisualStates中手动解析
- 查找模板中的命名元素并设置动画目标

### 🔧 待完善

1. **自动TargetName解析**
   - 在Storyboard.Begin()时自动查找目标元素
   - 需要实现ControlTemplate到UIElement的转换
   - 需要访问模板实例化后的元素树

2. **属性路径解析**
   - 解析"Background.Color"等嵌套属性路径
   - 需要实现属性路径解析器

3. **错误处理**
   - TargetName不存在时的错误提示
   - 属性路径无效时的错误提示

## 当前推荐用法

在完整的TargetName解析实现之前，推荐以下方式：

### 选项1：在Button中手动解析（最佳）

修改Button::InitializeVisualStates()来解析TargetName并设置实际目标：

```cpp
void Button::InitializeVisualStates() {
    // 加载模板中的视觉状态
    if (LoadVisualStatesFromTemplate()) {
        // 获取模板根元素
        auto* root = GetTemplateRoot();
        if (!root) return;
        
        // 获取VisualStateManager
        auto* manager = VisualStateManager::GetVisualStateManager(this);
        if (!manager) return;
        
        // 遍历所有状态组
        for (auto& group : manager->GetStateGroups()) {
            // 遍历每个状态
            for (auto& state : group->GetStates()) {
                auto* storyboard = state->GetStoryboard();
                if (!storyboard) continue;
                
                // 设置模板根，用于解析TargetName
                Storyboard::SetTemplateRoot(storyboard.get(), root);
                
                // 遍历storyboard中的所有动画
                for (auto& child : storyboard->GetChildren()) {
                    std::string targetName = Storyboard::GetTargetName(child.get());
                    if (targetName.empty()) continue;
                    
                    // 在模板中查找命名元素
                    auto* target = ControlTemplate::FindName(targetName, root);
                    if (target) {
                        // 设置实际目标
                        Storyboard::SetTarget(child.get(), target);
                        // TODO: 解析PropertyPath并设置
                    }
                }
            }
        }
        
        UpdateVisualState(false);
    }
}
```

### 选项2：继续使用直接对象方式

在完整实现之前，可以继续使用原来的直接对象方式：

```cpp
auto brush = ... // 获取或创建目标brush
->ColorAnimation(brush, &SolidColorBrush::ColorProperty())
```

但这不是长期方案。

## 路线图

### Phase 1: TargetName API（已完成 ✅）
- VisualStateBuilder支持TargetName参数
- Storyboard附加属性存储TargetName
- 信息正确保存和传递

### Phase 2: 手动解析（部分完成 ⚠️）
- Button中添加手动解析逻辑
- 查找命名元素
- 设置动画目标

### Phase 3: 自动解析（待实现 🔧）
- Storyboard.Begin()自动解析TargetName
- 实现属性路径解析器
- 完整的错误处理

### Phase 4: 优化（待实现 🔧）
- 缓存解析结果
- 性能优化
- 更好的错误提示

## 示例代码

完整的可运行示例请参考：
- `examples/button_targetname_demo.cpp` - TargetName方式示例（待创建）
- `test_targetname_resolution.cpp` - TargetName解析测试（待创建）

## 总结

**当前状态：**
- ✅ TargetName API已实现并可用
- ✅ 信息正确存储
- ⚠️ 需要手动解析（或继续使用直接对象方式）

**推荐做法：**
1. 使用TargetName API定义视觉状态
2. 在控件中添加手动解析逻辑
3. 或等待自动解析功能完善

**长期目标：**
- 完全自动的TargetName解析
- 像WPF一样透明的使用体验
- 不需要手动干预

---

**文档版本**: 1.0
**创建日期**: 2025-11-18  
**状态**: TargetName API已实现，解析功能部分完成
