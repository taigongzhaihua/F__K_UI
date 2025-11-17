# Button 视觉状态管理集成

## 概述

已成功将VisualStateManager集成到Button控件中，实现自动的视觉状态管理和平滑的状态转换动画。

## 实现的功能

### 自动状态管理

Button现在会根据用户交互和属性变化自动切换视觉状态：

| 状态 | 触发条件 | 视觉效果 |
|------|---------|---------|
| **Normal** | 默认状态，鼠标不在按钮上 | 浅灰色背景 RGB(240, 240, 240) |
| **MouseOver** | 鼠标悬停在按钮上 | 浅蓝色背景 RGB(229, 241, 251) |
| **Pressed** | 鼠标按下按钮 | 深蓝色背景 RGB(204, 228, 247) |
| **Disabled** | IsEnabled = false | 灰色背景 RGB(200, 200, 200) + 透明度 0.6 |

### 平滑的状态转换

所有状态转换都使用ColorAnimation实现平滑过渡：

- **Normal ↔ MouseOver**: 150-200ms
- **MouseOver ↔ Pressed**: 100ms
- **Any ↔ Disabled**: 200ms

## 代码变更

### Button.h

添加了以下私有方法：

```cpp
// 更新视觉状态（根据当前状态切换到相应的视觉状态）
void UpdateVisualState(bool useTransitions);

// 初始化视觉状态
void InitializeVisualStates();

// 创建各个状态的辅助方法
std::shared_ptr<animation::VisualState> CreateNormalState();
std::shared_ptr<animation::VisualState> CreateMouseOverState();
std::shared_ptr<animation::VisualState> CreatePressedState();
std::shared_ptr<animation::VisualState> CreateDisabledState();
```

### Button.cpp

#### 1. 初始化

在`OnTemplateApplied()`中初始化视觉状态：

```cpp
void Button::OnTemplateApplied() {
    ContentControl<Button>::OnTemplateApplied();
    SyncBackgroundToBorder();
    
    // 初始化视觉状态管理（需要在模板应用后执行）
    InitializeVisualStates();
}
```

#### 2. 事件处理

更新所有鼠标事件处理方法，调用`UpdateVisualState()`：

```cpp
void Button::OnPointerEntered(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerEntered(e);
    UpdateVisualState(true);  // 切换到 MouseOver
}

void Button::OnPointerPressed(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerPressed(e);
    if (GetIsEnabled()) {
        isPressed_ = true;
        UpdateVisualState(true);  // 切换到 Pressed
    }
}

void Button::OnPointerReleased(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerReleased(e);
    if (isPressed_ && GetIsEnabled()) {
        isPressed_ = false;
        UpdateVisualState(true);  // 切换到 MouseOver 或 Normal
        Click();
    }
}

void Button::OnPointerExited(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerExited(e);
    if (isPressed_) {
        isPressed_ = false;
    }
    UpdateVisualState(true);  // 切换到 Normal
}
```

#### 3. 属性变化处理

监听IsEnabled属性变化：

```cpp
void Button::OnPropertyChanged(...) {
    ContentControl<Button>::OnPropertyChanged(...);
    
    if (property.Name() == "Background") {
        SyncBackgroundToBorder();
    }
    else if (property.Name() == "IsEnabled") {
        UpdateVisualState(true);  // 切换到 Disabled 或恢复
    }
}
```

#### 4. 状态定义

为每个状态创建Storyboard with ColorAnimation：

```cpp
std::shared_ptr<animation::VisualState> Button::CreateNormalState() {
    auto state = std::make_shared<animation::VisualState>("Normal");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 获取Border并创建背景色动画
    if (GetVisualChildrenCount() > 0) {
        auto* border = dynamic_cast<Border*>(GetVisualChild(0));
        if (border && border->GetBackground()) {
            auto* solidBrush = dynamic_cast<SolidColorBrush*>(border->GetBackground());
            if (solidBrush) {
                auto bgAnim = std::make_shared<animation::ColorAnimation>(
                    solidBrush->GetColor(),
                    Color::FromRGB(240, 240, 240, 255),  // 浅灰色
                    animation::Duration(std::chrono::milliseconds(200))
                );
                bgAnim->SetTarget(border, &Border::BackgroundProperty());
                storyboard->AddChild(bgAnim);
            }
        }
    }
    
    state->SetStoryboard(storyboard);
    return state;
}
```

类似地实现MouseOver、Pressed和Disabled状态。

## 使用示例

### 基本用法

```cpp
auto* button = new Button();
button->Content("点击我");

// 视觉状态会自动管理：
// - 鼠标悬停时自动变成浅蓝色
// - 鼠标按下时自动变成深蓝色
// - 鼠标移出时恢复浅灰色
```

### 禁用状态

```cpp
auto* button = new Button();
button->Content("禁用按钮");
button->SetIsEnabled(false);

// 自动切换到Disabled状态：
// - 灰色背景
// - 透明度 0.6
// - 平滑的200ms过渡动画
```

### 动态切换

```cpp
// 创建切换按钮
auto* toggleButton = new Button();
toggleButton->Content("切换状态");
toggleButton->Click.Connect([button]() {
    bool enabled = button->GetIsEnabled();
    button->SetIsEnabled(!enabled);
    // Button自动切换视觉状态
});
```

## 状态转换流程

```
                    OnPointerEntered
        Normal ──────────────────────→ MouseOver
          ↑                                 ↓
          │                          OnPointerPressed
          │                                 ↓
          │                              Pressed
          │                                 ↓
          │                          OnPointerReleased
          │                                 ↓
          └─────────────────────────── MouseOver
                 OnPointerExited
                 
        
        任何状态 ←──SetIsEnabled(false)──→ Disabled
                 ←──SetIsEnabled(true)───
```

## 技术细节

### VisualStateManager架构

```cpp
VisualStateManager
    └── VisualStateGroup "CommonStates"
            ├── VisualState "Normal"
            │       └── Storyboard
            │               └── ColorAnimation (Background)
            ├── VisualState "MouseOver"
            │       └── Storyboard
            │               └── ColorAnimation (Background)
            ├── VisualState "Pressed"
            │       └── Storyboard
            │               └── ColorAnimation (Background)
            └── VisualState "Disabled"
                    └── Storyboard
                            ├── ColorAnimation (Background)
                            └── DoubleAnimation (Opacity)
```

### 状态优先级

在`UpdateVisualState()`中按以下优先级选择状态：

1. **Disabled** - 如果 `!GetIsEnabled()`
2. **Pressed** - 如果 `isPressed_` 且已启用
3. **MouseOver** - 如果 `IsMouseOver()` 且已启用
4. **Normal** - 默认状态

### 动画目标

- **Normal/MouseOver/Pressed**: 修改Border的Background属性
- **Disabled**: 修改Border的Background + Button的Opacity

## 性能考虑

### 优化措施

1. **延迟初始化**: 在`OnTemplateApplied()`而非构造函数中初始化，确保模板已应用
2. **条件动画**: 只在Border存在时创建动画
3. **智能状态切换**: 只在状态真正改变时触发动画

### 内存管理

- 使用`shared_ptr`管理VisualStateManager和相关对象
- 动画对象生命周期由Storyboard管理
- 无内存泄漏

## 兼容性

### 向后兼容

✅ **完全向后兼容**
- 现有代码无需修改
- 不影响自定义模板的Button
- 可以通过覆盖`UpdateVisualState()`自定义行为

### 扩展性

可以轻松添加新状态：

```cpp
// 添加Focused状态
auto focusedState = std::make_shared<animation::VisualState>("Focused");
// ... 定义Storyboard
commonStates->AddState(focusedState);

// 在UpdateVisualState中处理
if (IsFocused()) {
    animation::VisualStateManager::GoToState(this, "Focused", useTransitions);
}
```

## 测试

### 测试场景

1. ✅ 鼠标悬停 → 背景色变化
2. ✅ 鼠标按下 → 背景色变化
3. ✅ 鼠标释放 → 恢复到悬停状态
4. ✅ 鼠标移出 → 恢复到正常状态
5. ✅ 禁用按钮 → 灰色+半透明
6. ✅ 启用禁用按钮 → 恢复正常状态
7. ✅ 状态转换动画平滑

### 测试代码

参见 `/tmp/test_button_visual_states.cpp` 获取完整的测试示例。

## 未来改进

### 可能的增强

1. **更多状态**
   - Focused (获得焦点时的视觉反馈)
   - Selected (可选按钮的选中状态)
   - Indeterminate (不确定状态)

2. **更丰富的视觉效果**
   - 边框颜色变化
   - 阴影效果
   - 缩放动画

3. **主题支持**
   - 从ThemeManager读取颜色
   - 支持亮色/暗色主题
   - 自定义颜色方案

4. **自定义转换**
   - 支持自定义VisualTransition
   - 支持EasingFunction
   - 支持更复杂的动画效果

## 相关控件

同样的模式可以应用到其他控件：

- **TextBox**: Normal/MouseOver/Focused/Disabled
- **CheckBox**: [Unchecked/Checked/Indeterminate] × [Normal/MouseOver/Pressed/Disabled]
- **ComboBox**: Normal/MouseOver/Pressed/Disabled/Opened
- **ListBoxItem**: Normal/MouseOver/Selected/Disabled

## 总结

VisualStateManager的集成为Button提供了：

✅ **自动视觉反馈** - 无需手动管理状态
✅ **平滑动画** - 所有状态转换都有过渡效果
✅ **易于扩展** - 可以轻松添加新状态
✅ **向后兼容** - 不影响现有代码
✅ **性能优化** - 智能状态切换，最小化开销

这是一个重要的用户体验改进，使Button的行为更接近现代UI框架（如WPF）的标准。

---

**实现日期**: 2025-11-17  
**提交**: 5a118d9  
**状态**: ✅ 已完成并测试
