# Visual State Manager 实现状态

## 当前实现情况

### ✅ 已实现的基础架构

1. **VisualStateManager** (`include/fk/animation/VisualStateManager.h`)
   - 视觉状态管理器核心类
   - `GoToState()` - 状态转换方法
   - 状态组管理
   - 状态改变事件（CurrentStateChanging, CurrentStateChanged）

2. **VisualState** (`include/fk/animation/VisualState.h`)
   - 表示单个视觉状态
   - 包含状态名称和Storyboard
   - 定义状态的视觉表现

3. **VisualStateGroup** (`include/fk/animation/VisualStateGroup.h`)
   - 状态组（如CommonStates包含Normal/MouseOver/Pressed/Disabled）
   - 管理多个相关状态
   - 状态转换查找

4. **VisualTransition** (`include/fk/animation/VisualTransition.h`)
   - 定义状态间的转换效果
   - 支持自定义转换动画
   - 支持EasingFunction

5. **演示程序** (`examples/visual_state_demo.cpp`)
   - 展示VisualStateManager的完整用法
   - 包含Normal/MouseOver/Pressed状态示例
   - 演示状态转换和事件

### ❌ 未集成到UI控件

虽然VisualStateManager框架完整，但**没有UI控件实际使用它**。

#### Button当前的状态管理

**手动状态跟踪：**
```cpp
// Button.cpp
void Button::OnPointerPressed(PointerEventArgs& e) {
    if (this->GetIsEnabled()) {
        isPressed_ = true;              // 手动设置
        this->InvalidateVisual();       // 只是重绘
    }
}
```

**问题：**
- 只有`isPressed_`布尔值，没有视觉反馈
- 不使用VisualStateManager
- 背景色不会根据状态改变
- 悬停/按下/禁用状态没有视觉区别

#### Control的状态跟踪

Control基类已经跟踪鼠标和焦点状态：

```cpp
// Control.h
bool IsMouseOver() const { return isMouseOver_; }  // ✓ 已跟踪
bool IsFocused() const { return isFocused_; }      // ✓ 已跟踪

protected:
    void OnPointerEntered(PointerEventArgs& e) override {
        isMouseOver_ = true;   // ✓ 自动更新
    }
    
    void OnPointerExited(PointerEventArgs& e) override {
        isMouseOver_ = false;  // ✓ 自动更新
    }
```

**但是：** 这些状态变化**不会触发**VisualStateManager的状态转换。

## 需要的集成工作

要让Button等控件自动使用VisualStateManager，需要：

### 1. 在Button构造函数中设置状态

```cpp
Button::Button() {
    // ... 现有代码 ...
    
    // 创建VisualStateManager
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(this, manager);
    
    // 创建CommonStates状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");
    
    // 定义Normal状态
    auto normalState = CreateNormalState();
    commonStates->AddState(normalState);
    
    // 定义MouseOver状态
    auto mouseOverState = CreateMouseOverState();
    commonStates->AddState(mouseOverState);
    
    // 定义Pressed状态
    auto pressedState = CreatePressedState();
    commonStates->AddState(pressedState);
    
    // 定义Disabled状态
    auto disabledState = CreateDisabledState();
    commonStates->AddState(disabledState);
    
    // 添加状态组
    manager->AddStateGroup(commonStates);
    
    // 初始状态
    animation::VisualStateManager::GoToState(this, "Normal", false);
}
```

### 2. 在状态改变时调用GoToState

```cpp
void Button::OnPointerEntered(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerEntered(e);
    UpdateVisualState(true);  // 根据当前状态转换
}

void Button::OnPointerPressed(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerPressed(e);
    if (GetIsEnabled()) {
        isPressed_ = true;
        UpdateVisualState(true);
    }
}

void Button::OnPointerReleased(PointerEventArgs& e) {
    ContentControl<Button>::OnPointerReleased(e);
    if (isPressed_ && GetIsEnabled()) {
        isPressed_ = false;
        UpdateVisualState(true);
        Click();
    }
}

void Button::UpdateVisualState(bool useTransitions) {
    if (!GetIsEnabled()) {
        animation::VisualStateManager::GoToState(this, "Disabled", useTransitions);
    } else if (isPressed_) {
        animation::VisualStateManager::GoToState(this, "Pressed", useTransitions);
    } else if (IsMouseOver()) {
        animation::VisualStateManager::GoToState(this, "MouseOver", useTransitions);
    } else {
        animation::VisualStateManager::GoToState(this, "Normal", useTransitions);
    }
}
```

### 3. 为每个状态创建Storyboard

```cpp
std::shared_ptr<animation::VisualState> Button::CreateNormalState() {
    auto state = std::make_shared<animation::VisualState>("Normal");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 背景色动画：恢复到默认颜色
    auto bgAnim = std::make_shared<animation::ColorAnimation>(
        /* current */ GetBackground()->GetColor(),
        /* target */ Color::FromRGB(240, 240, 240, 255),  // 浅灰色
        animation::Duration(std::chrono::milliseconds(200))
    );
    bgAnim->SetTarget(this, &Button::BackgroundProperty());
    storyboard->AddChild(bgAnim);
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreateMouseOverState() {
    auto state = std::make_shared<animation::VisualState>("MouseOver");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 背景色动画：鼠标悬停时变亮
    auto bgAnim = std::make_shared<animation::ColorAnimation>(
        /* current */ GetBackground()->GetColor(),
        /* target */ Color::FromRGB(229, 241, 251, 255),  // 浅蓝色
        animation::Duration(std::chrono::milliseconds(150))
    );
    bgAnim->SetTarget(this, &Button::BackgroundProperty());
    storyboard->AddChild(bgAnim);
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreatePressedState() {
    auto state = std::make_shared<animation::VisualState>("Pressed");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 背景色动画：按下时变暗
    auto bgAnim = std::make_shared<animation::ColorAnimation>(
        /* current */ GetBackground()->GetColor(),
        /* target */ Color::FromRGB(204, 228, 247, 255),  // 深蓝色
        animation::Duration(std::chrono::milliseconds(100))
    );
    bgAnim->SetTarget(this, &Button::BackgroundProperty());
    storyboard->AddChild(bgAnim);
    
    state->SetStoryboard(storyboard);
    return state;
}

std::shared_ptr<animation::VisualState> Button::CreateDisabledState() {
    auto state = std::make_shared<animation::VisualState>("Disabled");
    auto storyboard = std::make_shared<animation::Storyboard>();
    
    // 背景色动画：禁用时变灰
    auto bgAnim = std::make_shared<animation::ColorAnimation>(
        /* current */ GetBackground()->GetColor(),
        /* target */ Color::FromRGB(200, 200, 200, 255),  // 灰色
        animation::Duration(std::chrono::milliseconds(200))
    );
    bgAnim->SetTarget(this, &Button::BackgroundProperty());
    storyboard->AddChild(bgAnim);
    
    // 透明度动画：禁用时半透明
    auto opacityAnim = std::make_shared<animation::DoubleAnimation>(
        /* current */ GetOpacity(),
        /* target */ 0.6,
        animation::Duration(std::chrono::milliseconds(200))
    );
    opacityAnim->SetTarget(this, &Button::OpacityProperty());
    storyboard->AddChild(opacityAnim);
    
    state->SetStoryboard(storyboard);
    return state;
}
```

### 4. 处理IsEnabled属性变化

```cpp
void Button::OnPropertyChanged(const binding::DependencyProperty& property, 
                               const std::any& oldValue, 
                               const std::any& newValue,
                               binding::ValueSource oldSource,
                               binding::ValueSource newSource) {
    ContentControl<Button>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
    
    if (property.Name() == "Background") {
        SyncBackgroundToBorder();
    } else if (property.Name() == "IsEnabled") {
        // IsEnabled改变时更新视觉状态
        UpdateVisualState(true);
    }
}
```

## 效果对比

### Before (当前实现)

```cpp
auto* button = new Button();
button->Content("点击我");

// 问题：
// - 鼠标悬停：没有视觉反馈
// - 鼠标按下：没有视觉反馈（只有isPressed_标志）
// - 禁用状态：没有视觉反馈
```

### After (集成VisualStateManager后)

```cpp
auto* button = new Button();
button->Content("点击我");

// 自动效果：
// - 鼠标悬停：背景色变亮（浅蓝色）+ 平滑过渡
// - 鼠标按下：背景色变暗（深蓝色）+ 快速过渡
// - 禁用状态：背景色变灰 + 半透明 + 平滑过渡
// - 所有状态转换都有动画效果
```

## 其他控件

同样的模式可以应用到其他控件：

- **TextBox**: Normal/MouseOver/Focused/Disabled
- **CheckBox**: Unchecked/Checked/Indeterminate × (Normal/MouseOver/Pressed/Disabled)
- **ComboBox**: Normal/MouseOver/Pressed/Disabled/Focused
- **ListBoxItem**: Normal/MouseOver/Selected/Disabled

## 依赖

实现Button的状态管理需要：

1. ✅ **VisualStateManager** - 已实现
2. ✅ **VisualState/VisualStateGroup** - 已实现
3. ⚠ **ColorAnimation** - 需要检查是否已实现
4. ✅ **DoubleAnimation** - 已实现（用于Opacity）
5. ✅ **Storyboard** - 已实现

唯一可能缺少的是`ColorAnimation`，用于平滑过渡背景色。如果没有，可以：
- 直接设置颜色（无动画）
- 或先实现ColorAnimation

## 实现优先级

建议的实现顺序：

1. **高优先级**：Button的基础状态管理
   - Normal/MouseOver/Pressed/Disabled
   - 即使没有动画也应该有颜色变化

2. **中优先级**：添加过渡动画
   - 实现ColorAnimation
   - 平滑的状态转换

3. **低优先级**：扩展到其他控件
   - TextBox, CheckBox, ListBoxItem等

## 总结

**当前状态：**
- ✅ VisualStateManager框架完整
- ✅ 演示程序展示了如何使用
- ❌ 没有UI控件实际集成
- ❌ Button等控件状态没有视觉反馈

**需要做的：**
- 在Button中集成VisualStateManager
- 定义4个基础状态及其视觉效果
- 在事件处理中触发状态转换
- （可选）实现ColorAnimation用于平滑过渡

这是一个重要的用户体验改进，但需要一定的开发工作。

---

*文档日期: 2025-11-17*  
*状态: 待实现*
