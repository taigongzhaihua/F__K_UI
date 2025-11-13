# Phase 4.2: VisualStateManager 系统

**开发日期**: 2025-11-13  
**状态**: ✅ 完成  
**优先级**: 高

---

## 📊 概述

VisualStateManager（视觉状态管理器）是一个强大的状态管理系统，允许控件在不同的视觉状态之间切换，并定义状态之间的转换动画。这对于创建交互式UI控件（如按钮、复选框等）至关重要。

---

## ✅ 已实现的功能

### 1. VisualState - 视觉状态

**文件**: `include/fk/animation/VisualState.h`

**核心功能**:
- 状态名称标识
- 关联的故事板（定义状态的视觉表现）

**API 示例**:
```cpp
auto normalState = std::make_shared<VisualState>("Normal");

// 创建故事板定义状态的视觉效果
auto storyboard = std::make_shared<Storyboard>();
auto animation = std::make_shared<DoubleAnimation>(
    1.0, 1.0, Duration(std::chrono::milliseconds(300))
);
animation->SetTarget(element, &Element::OpacityProperty());
storyboard->AddChild(animation);

normalState->SetStoryboard(storyboard);
```

**代码量**: 约 30 行

---

### 2. VisualTransition - 状态转换

**文件**: `include/fk/animation/VisualTransition.h`

**核心功能**:
- 定义从一个状态到另一个状态的转换
- 支持通配符（空字符串表示任意状态）
- 可配置转换持续时间
- 可配置缓动函数
- 可选的自定义故事板

**转换匹配优先级**:
1. **精确匹配**: From="StateA", To="StateB"
2. **部分匹配**: From="StateA", To="" 或 From="", To="StateB"
3. **默认转换**: From="", To="" (匹配所有转换)

**API 示例**:
```cpp
// 精确转换：从 Normal 到 MouseOver
auto transition = std::make_shared<VisualTransition>();
transition->SetFrom("Normal");
transition->SetTo("MouseOver");
transition->SetGeneratedDuration(Duration(std::chrono::milliseconds(200)));

auto easing = std::make_shared<QuadraticEase>();
easing->SetEasingMode(EasingMode::EaseOut);
transition->SetGeneratedEasingFunction(easing);

// 默认转换：适用于所有状态转换
auto defaultTransition = std::make_shared<VisualTransition>();
defaultTransition->SetGeneratedDuration(Duration(std::chrono::milliseconds(300)));
```

**代码量**: 约 60 行

---

### 3. VisualStateGroup - 视觉状态组

**文件**: `include/fk/animation/VisualStateGroup.h`

**核心功能**:
- 管理一组相关的视觉状态
- 跟踪当前状态
- 管理状态转换集合
- 查找最佳转换

**常见状态组**:
- **CommonStates**: Normal, MouseOver, Pressed, Disabled
- **FocusStates**: Focused, Unfocused
- **ValidationStates**: Valid, Invalid

**API 示例**:
```cpp
auto commonStates = std::make_shared<VisualStateGroup>("CommonStates");

// 添加状态
commonStates->AddState(normalState);
commonStates->AddState(mouseOverState);
commonStates->AddState(pressedState);

// 添加转换
commonStates->AddTransition(transition1);
commonStates->AddTransition(transition2);

// 查找状态
auto state = commonStates->FindState("Normal");

// 查找最佳转换
auto transition = commonStates->FindBestTransition("Normal", "MouseOver");

// 获取当前状态
auto currentState = commonStates->GetCurrentState();
```

**代码量**: 约 120 行

---

### 4. VisualStateManager - 视觉状态管理器

**文件**: `include/fk/animation/VisualStateManager.h`, `src/animation/VisualStateManager.cpp`

**核心功能**:
- 管理多个状态组
- 执行状态转换
- 触发状态改变事件
- 应用转换动画
- 对象关联管理

**API 示例**:
```cpp
// 静态方法：状态转换
bool success = VisualStateManager::GoToState(
    element, 
    "MouseOver",  // 目标状态名
    true          // 使用转换动画
);

// 创建和配置管理器
auto manager = std::make_shared<VisualStateManager>();

// 添加状态组
manager->AddStateGroup(commonStates);
manager->AddStateGroup(focusStates);

// 关联到对象
VisualStateManager::SetVisualStateManager(element, manager);

// 订阅事件
manager->CurrentStateChanging.Connect([](VisualStateGroup* group,
                                         VisualState* from,
                                         VisualState* to) {
    std::cout << "状态即将改变: " << from->GetName() 
              << " → " << to->GetName() << "\n";
});

manager->CurrentStateChanged.Connect([](VisualStateGroup* group,
                                        VisualState* from,
                                        VisualState* to) {
    std::cout << "状态已改变\n";
});
```

**事件**:
- `CurrentStateChanging` - 状态改变前触发
- `CurrentStateChanged` - 状态改变后触发

**代码量**: 约 180 行

---

## 📊 代码统计

| 模块 | 文件数 | 代码行数 |
|------|-------|---------|
| VisualState | 1 | 30 |
| VisualTransition | 1 | 60 |
| VisualStateGroup | 1 | 120 |
| VisualStateManager | 2 | 180 |
| 演示程序 | 1 | 320 |
| **总计** | **6** | **710** |

---

## 🎨 完整使用示例

### 示例 1: 按钮状态管理

```cpp
// 创建按钮元素
auto button = std::make_shared<Button>();

// 创建 VisualStateManager
auto manager = std::make_shared<VisualStateManager>();
VisualStateManager::SetVisualStateManager(button.get(), manager);

// 创建 CommonStates 状态组
auto commonStates = std::make_shared<VisualStateGroup>("CommonStates");

// Normal 状态
auto normalState = std::make_shared<VisualState>("Normal");
auto normalStoryboard = std::make_shared<Storyboard>();

auto normalOpacity = std::make_shared<DoubleAnimation>(
    button->GetOpacity(), 1.0, Duration(300ms)
);
normalOpacity->SetTarget(button.get(), &Button::OpacityProperty());
normalStoryboard->AddChild(normalOpacity);

normalState->SetStoryboard(normalStoryboard);
commonStates->AddState(normalState);

// MouseOver 状态
auto mouseOverState = std::make_shared<VisualState>("MouseOver");
auto mouseOverStoryboard = std::make_shared<Storyboard>();

auto mouseOverOpacity = std::make_shared<DoubleAnimation>(
    button->GetOpacity(), 0.8, Duration(200ms)
);
mouseOverOpacity->SetTarget(button.get(), &Button::OpacityProperty());
mouseOverStoryboard->AddChild(mouseOverOpacity);

mouseOverState->SetStoryboard(mouseOverStoryboard);
commonStates->AddState(mouseOverState);

// Pressed 状态
auto pressedState = std::make_shared<VisualState>("Pressed");
auto pressedStoryboard = std::make_shared<Storyboard>();

auto pressedOpacity = std::make_shared<DoubleAnimation>(
    button->GetOpacity(), 0.6, Duration(100ms)
);
pressedOpacity->SetTarget(button.get(), &Button::OpacityProperty());
pressedStoryboard->AddChild(pressedOpacity);

pressedState->SetStoryboard(pressedStoryboard);
commonStates->AddState(pressedState);

// 添加状态组到管理器
manager->AddStateGroup(commonStates);

// 状态转换
VisualStateManager::GoToState(button.get(), "MouseOver", true);
```

### 示例 2: 自定义转换

```cpp
// 创建转换：Normal → MouseOver
auto transition = std::make_shared<VisualTransition>();
transition->SetFrom("Normal");
transition->SetTo("MouseOver");
transition->SetGeneratedDuration(Duration(std::chrono::milliseconds(250)));

// 使用弹性缓动
auto easing = std::make_shared<ElasticEase>(3, 5.0);
easing->SetEasingMode(EasingMode::EaseOut);
transition->SetGeneratedEasingFunction(easing);

commonStates->AddTransition(transition);

// 创建默认转换（适用于所有其他状态转换）
auto defaultTransition = std::make_shared<VisualTransition>();
defaultTransition->SetGeneratedDuration(Duration(std::chrono::milliseconds(300)));

auto linearEase = std::make_shared<LinearEase>();
defaultTransition->SetGeneratedEasingFunction(linearEase);

commonStates->AddTransition(defaultTransition);
```

### 示例 3: 状态改变事件

```cpp
// 订阅状态改变事件
manager->CurrentStateChanging.Connect([](VisualStateGroup* group,
                                         VisualState* from,
                                         VisualState* to) {
    std::cout << "状态即将改变:\n";
    std::cout << "  组: " << group->GetName() << "\n";
    std::cout << "  从: " << (from ? from->GetName() : "None") << "\n";
    std::cout << "  到: " << (to ? to->GetName() : "None") << "\n";
});

manager->CurrentStateChanged.Connect([](VisualStateGroup* group,
                                        VisualState* from,
                                        VisualState* to) {
    std::cout << "状态已改变完成\n";
});

// 执行状态转换（将触发事件）
VisualStateManager::GoToState(button.get(), "Pressed", true);
```

---

## 🎯 技术亮点

### 1. WPF 兼容设计

完全兼容 WPF 的 VisualStateManager API：
- 相同的类名和方法名
- 相同的状态组概念
- 相同的转换优先级逻辑

### 2. 灵活的转换匹配

支持三级转换匹配：
- **精确匹配**: 最高优先级，完全指定源和目标状态
- **部分匹配**: 中等优先级，使用通配符
- **默认转换**: 最低优先级，适用于所有转换

### 3. 事件驱动架构

提供完整的事件系统：
- `CurrentStateChanging` - 状态改变前
- `CurrentStateChanged` - 状态改变后

允许外部代码响应状态变化。

### 4. 故事板集成

每个状态可以关联一个故事板：
- 定义状态的视觉表现
- 支持多个并行动画
- 自动管理动画生命周期

### 5. 对象关联管理

使用静态映射管理对象和管理器的关联：
- 线程安全（使用 mutex）
- 自动清理
- 支持多对象管理

---

## 🔧 演示程序

**文件**: `examples/visual_state_demo.cpp`

**测试场景**:
1. 基础状态转换（按钮的 Normal/MouseOver/Pressed）
2. 自定义状态转换（精确匹配、默认转换）
3. 状态改变事件（订阅和触发）

**运行方式**:
```bash
cd build
./visual_state_demo
```

**输出示例**:
```
╔═══════════════════════════════════════════════════════════╗
║    F__K_UI Phase 4.2 - VisualStateManager 演示程序       ║
║         Visual State Manager Demo                        ║
╚═══════════════════════════════════════════════════════════╝

============================================================
 测试 1: 基础状态转换
============================================================

创建模拟按钮控件
初始不透明度: 1
初始缩放: 1

已创建3个视觉状态:
  - Normal: 不透明度=1.0, 缩放=1.0
  - MouseOver: 不透明度=0.8, 缩放=1.05
  - Pressed: 不透明度=0.6, 缩放=0.95

转换到 Normal 状态...
  转换成功
  当前状态: Normal

转换到 MouseOver 状态...
  转换成功
  当前状态: MouseOver

...
```

---

## ✅ 测试结果

所有测试场景全部通过：

- ✓ 基础状态转换工作正常
- ✓ 自定义转换查找正常
- ✓ 状态改变事件正常
- ✓ 转换优先级匹配正确

---

## 🔜 应用场景

### 控件状态管理

VisualStateManager 非常适合管理 UI 控件的交互状态：

**Button 控件**:
- Normal, MouseOver, Pressed, Disabled
- Focus States: Focused, Unfocused

**TextBox 控件**:
- Normal, Focused, Disabled
- Validation States: Valid, Invalid

**CheckBox 控件**:
- Unchecked, Checked, Indeterminate
- MouseOver 状态

**自定义控件**:
- 任意数量的自定义状态
- 灵活的状态转换
- 复杂的动画效果

---

## 📈 Phase 4 总体进度

| 子阶段 | 状态 | 完成度 | 代码量 |
|--------|------|--------|--------|
| 4.1 动画基础 | ✅ | 100% | 1660 行 |
| 4.1 扩展（关键帧） | ✅ | 100% | 940 行 |
| 4.2 VisualStateManager | ✅ | 100% | 710 行 |
| 4.3 资源管理 | ⏳ | 0% | 0 行 |
| 4.4 性能优化 | ⏳ | 0% | 0 行 |
| **总计** | 🚧 | **70%** | **3310 行** |

---

## 🔜 下一步工作

### Phase 4.3: 高级资源管理

计划实现：
- 增强 ResourceDictionary
- 主题系统
- 动态资源
- 资源继承

预计时间：1 周  
预计代码量：1000-1500 行

---

**文档版本**: 1.0  
**最后更新**: 2025-11-13  
**作者**: F__K_UI 开发团队
