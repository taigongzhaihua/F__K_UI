# ScrollViewer Phase 3 实施计划

## 概述

Phase 3 的目标是实现 ScrollBar 的完整渲染和交互功能，包括 Track、Thumb 和 RepeatButton 组件。

## 架构设计

### 组件层次

```
ScrollBar (容器)
└── Track (轨道)
    ├── DecreaseRepeatButton (向上/左按钮)
    ├── Thumb (可拖动滑块)
    └── IncreaseRepeatButton (向下/右按钮)
```

### WPF 对应关系

| 组件 | WPF 类 | 功能 |
|------|--------|------|
| ScrollBar | System.Windows.Controls.Primitives.ScrollBar | 滚动条容器 |
| Track | System.Windows.Controls.Primitives.Track | 轨道布局 |
| Thumb | System.Windows.Controls.Primitives.Thumb | 可拖动滑块 |
| RepeatButton | System.Windows.Controls.Primitives.RepeatButton | 重复按钮 |

## 实施步骤

### Step 1: RepeatButton 基础实现 ⏳

**目标：** 创建可以重复触发点击的按钮

**文件：**
- `include/fk/ui/RepeatButton.h`
- `src/ui/RepeatButton.cpp`

**功能：**
- 继承自 ButtonBase
- 按住时重复触发 Click 事件
- 可配置延迟（Delay）和间隔（Interval）
- 支持水平和垂直方向

**关键属性：**
- `Delay`: 首次重复前的延迟（默认 500ms）
- `Interval`: 重复间隔（默认 33ms）

### Step 2: Thumb 基础实现 ⏳

**目标：** 创建可拖动的滑块

**文件：**
- `include/fk/ui/Thumb.h`
- `src/ui/Thumb.cpp`

**功能：**
- 继承自 Control
- 支持鼠标拖动
- 触发拖动事件（DragStarted、DragDelta、DragCompleted）
- 限制拖动范围

**事件：**
- `DragStarted`: 开始拖动
- `DragDelta`: 拖动中（提供偏移量）
- `DragCompleted`: 完成拖动

### Step 3: Track 布局实现 ⏳

**目标：** 实现滚动条的三段式布局

**文件：**
- `include/fk/ui/Track.h`
- `src/ui/Track.cpp`

**功能：**
- 管理三个子元素的布局
- 根据 Value/Minimum/Maximum/ViewportSize 计算 Thumb 位置和大小
- 支持水平和垂直方向
- 点击轨道区域滚动

**布局逻辑：**
```
Orientation::Vertical:
  ┌─────────────┐
  │  Decrease   │ <- DecreaseRepeatButton
  ├─────────────┤
  │             │ <- 可点击区域（上）
  ├─────────────┤
  │    Thumb    │ <- 滑块
  ├─────────────┤
  │             │ <- 可点击区域（下）
  ├─────────────┤
  │  Increase   │ <- IncreaseRepeatButton
  └─────────────┘
```

### Step 4: ScrollBar 完整渲染 ⏳

**目标：** 更新 ScrollBar 使用 Track 布局

**更新文件：**
- `include/fk/ui/ScrollBar.h`
- `src/ui/ScrollBar.cpp`

**功能：**
- 创建和管理 Track、Thumb、RepeatButton
- 实现 OnApplyTemplate（模板应用）
- 连接事件处理
- 渲染滚动条

**布局：**
- 使用 Track 进行三段式布局
- Thumb 大小反映 ViewportSize
- Thumb 位置反映 Value

### Step 5: 鼠标交互 ⏳

**目标：** 实现完整的鼠标交互

**功能：**
- Thumb 拖动改变 Value
- RepeatButton 点击滚动
- Track 点击跳转到位置
- 鼠标悬停视觉反馈

**事件处理：**
```cpp
// Thumb 拖动
thumb->DragDelta.Connect([](float horizontal, float vertical) {
    // 计算新的 Value
    // 更新 ScrollBar
});

// RepeatButton 点击
decreaseButton->Click.Connect([]() {
    scrollBar->LineUp();
});

// Track 点击
track->MouseDown.Connect([](MouseButtonEventArgs& e) {
    // 计算点击位置
    // 跳转到该位置
});
```

### Step 6: ScrollViewer 集成 ⏳

**目标：** 连接 ScrollBar 和 ScrollContentPresenter

**更新文件：**
- `src/ui/ScrollViewer.cpp`

**功能：**
- 在 ScrollViewer 中创建 ScrollBar
- 连接 ScrollBar.ValueChanged 到 ScrollContentPresenter
- 根据 extent/viewport 更新 ScrollBar 范围
- 根据 ScrollBarVisibility 控制显示

**同步逻辑：**
```
ScrollBar.Value 改变
    ↓
ScrollContentPresenter.SetVerticalOffset(value)
    ↓
内容滚动 + ScrollChanged 事件
    ↓
ScrollViewer.OnScrollContentPresenterChanged()
    ↓
更新 ScrollBar 状态（但不触发 ValueChanged）
```

## 实施优先级

1. **高优先级（必须）：**
   - RepeatButton 基础实现
   - Thumb 基础实现
   - Track 布局实现
   - ScrollBar 渲染

2. **中优先级（重要）：**
   - 鼠标交互
   - ScrollViewer 集成
   - 事件处理

3. **低优先级（优化）：**
   - 视觉状态（Hover、Pressed）
   - 动画效果
   - 性能优化

## 技术考虑

### 1. 坐标转换

拖动 Thumb 时需要将鼠标移动距离转换为 Value 的变化：

```cpp
float CalculateValueFromThumbDelta(float delta) {
    float trackLength = GetTrackLength();
    float thumbLength = CalculateThumbLength();
    float range = Maximum - Minimum;
    
    // delta / (trackLength - thumbLength) = valueChange / range
    float valueChange = (delta / (trackLength - thumbLength)) * range;
    return valueChange;
}
```

### 2. Thumb 大小计算

Thumb 大小应反映 ViewportSize 占总范围的比例：

```cpp
float CalculateThumbLength() {
    float trackLength = GetTrackLength();
    float range = Maximum - Minimum + ViewportSize;
    float ratio = ViewportSize / range;
    return std::max(MIN_THUMB_LENGTH, trackLength * ratio);
}
```

### 3. Thumb 位置计算

Thumb 位置应反映 Value 在范围中的位置：

```cpp
float CalculateThumbPosition() {
    float trackLength = GetTrackLength();
    float thumbLength = CalculateThumbLength();
    float range = Maximum - Minimum;
    
    if (range <= 0) return 0;
    
    float ratio = (Value - Minimum) / range;
    return ratio * (trackLength - thumbLength);
}
```

### 4. 防止循环更新

ScrollBar 和 ScrollContentPresenter 之间需要防止循环更新：

```cpp
// ScrollBar 中
void SetValue(float value) {
    if (isUpdatingFromContent_) return;  // 防止循环
    
    isUpdatingFromScrollViewer_ = true;
    // 更新 value
    ValueChanged(oldValue, newValue);
    isUpdatingFromScrollViewer_ = false;
}

// ScrollViewer 中
void OnScrollContentPresenterChanged() {
    scrollBar_->isUpdatingFromContent_ = true;
    scrollBar_->SetValue(newValue);
    scrollBar_->isUpdatingFromContent_ = false;
}
```

## 测试计划

### 单元测试

1. **RepeatButton 测试**
   - 按住触发重复事件
   - Delay 和 Interval 配置
   - 释放停止重复

2. **Thumb 测试**
   - 拖动事件触发
   - 位置计算正确
   - 边界限制

3. **Track 测试**
   - 布局计算正确
   - Thumb 大小和位置
   - 点击区域处理

4. **ScrollBar 测试**
   - Value 范围限制
   - ViewportSize 影响 Thumb
   - 事件触发正确

### 集成测试

1. **ScrollBar + ScrollContentPresenter**
   - 拖动 Thumb 滚动内容
   - 点击 RepeatButton 滚动
   - 点击 Track 跳转

2. **完整 ScrollViewer**
   - 内容超出时显示滚动条
   - 滚动条自动更新
   - 程序化滚动同步

### 视觉测试

1. 创建示例应用
2. 测试不同尺寸和方向
3. 测试鼠标交互反馈
4. 测试边界情况

## 预期成果

Phase 3 完成后：

- ✅ 可见的滚动条
- ✅ 可拖动的 Thumb
- ✅ 可点击的按钮和轨道
- ✅ 完整的滚动交互
- ✅ ScrollBar 和 ScrollContentPresenter 双向同步

## 时间估计

- Step 1-2 (RepeatButton + Thumb): 2-3 小时
- Step 3 (Track): 2-3 小时
- Step 4 (ScrollBar): 1-2 小时
- Step 5 (交互): 2-3 小时
- Step 6 (集成): 1-2 小时
- 测试和调试: 2-3 小时

**总计：** 10-16 小时

## 下一步行动

开始实现 Step 1: RepeatButton 基础功能
