# StackPanel 修复和完善说明

本文档记录了 StackPanel 的各项修复和功能完善。

---

## 目录

1. [元素重叠问题修复](#1-元素重叠问题修复)
2. [Alignment 支持和 Spacing 优化 (2025-11-19)](#2-alignment-支持和-spacing-优化-2025-11-19)

---

## 1. 元素重叠问题修复

### 问题描述

在之前的版本中，StackPanel 中的所有子元素会重叠在一起，而不是按照预期堆叠排列。

## 问题示例

```cpp
auto stackPanel = new StackPanel();
stackPanel->SetOrientation(Orientation::Vertical);

auto text1 = new TextBlock()->Text("第一个元素");
auto text2 = new TextBlock()->Text("第二个元素");
auto text3 = new TextBlock()->Text("第三个元素");

stackPanel->AddChild(text1);
stackPanel->AddChild(text2);
stackPanel->AddChild(text3);
```

**问题**: 所有三个文本块会显示在相同的位置，互相重叠。

**预期行为**: 三个文本块应该垂直堆叠，每个元素占据不同的位置。

## 根本原因

问题出在 `UIElement::Arrange()` 方法的早期返回逻辑中：

```cpp
// 旧代码（有bug）
void UIElement::Arrange(const Rect& finalRect) {
    if (!arrangeDirty_ && !measureDirty_) {
        return; // 已经排列过
    }
    layoutRect_ = finalRect;
    // ...
}
```

当 StackPanel 尝试排列子元素时：
1. 第一个子元素被排列在 (0, 0)
2. 第二个子元素应该被排列在 (0, height1 + spacing)
3. 但是，如果第二个元素的 `arrangeDirty_` 和 `measureDirty_` 都是 false，`Arrange()` 会提前返回
4. 结果：第二个元素的 `layoutRect_` 没有更新，仍然保持在 (0, 0)
5. 所有元素都保持在相同位置，导致重叠

## 修复方案

在 `UIElement::Arrange()` 中增加了对位置是否改变的检查：

```cpp
// 新代码（已修复）
void UIElement::Arrange(const Rect& finalRect) {
    // 检查位置是否改变
    bool rectChanged = (layoutRect_.x != finalRect.x || 
                       layoutRect_.y != finalRect.y ||
                       layoutRect_.width != finalRect.width ||
                       layoutRect_.height != finalRect.height);
    
    // 只有在元素不脏且位置没有改变时才跳过
    if (!arrangeDirty_ && !measureDirty_ && !rectChanged) {
        return;
    }
    
    layoutRect_ = finalRect;
    // ...
}
```

现在，即使元素不是"脏"的，只要父容器传入了不同的位置，元素就会更新其 `layoutRect_`。

## 修复后的效果

修复后，StackPanel 中的元素将正确堆叠：

```
┌─────────────────────────┐
│ 第一个元素              │  <- (0, 0)
├─────────────────────────┤
│ 第二个元素              │  <- (0, height1 + spacing)
├─────────────────────────┤
│ 第三个元素              │  <- (0, height1 + height2 + 2*spacing)
└─────────────────────────┘
```

## 影响范围

此修复影响所有使用布局容器的场景：
- **StackPanel**: 垂直和水平堆叠
- **Grid**: 网格布局
- **其他 Panel**: 所有自定义面板实现

## 性能影响

修复保留了原有的性能优化。只有在以下情况下才会执行完整的排列逻辑：
- 元素被标记为需要测量或排列（`measureDirty_` 或 `arrangeDirty_` 为 true）
- 父容器传入的位置/尺寸与当前不同（`rectChanged` 为 true）

如果元素已经在正确的位置且不需要更新，仍然会跳过排列，保持高效。

## 测试方法

可以使用以下代码测试修复是否有效：

```cpp
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"

// 创建 StackPanel
auto panel = new StackPanel();
panel->SetOrientation(Orientation::Vertical);
panel->SetSpacing(10.0f);

// 添加子元素
for (int i = 0; i < 3; i++) {
    auto text = new TextBlock();
    text->SetText("元素 " + std::to_string(i + 1));
    panel->AddChild(text);
}

// 测量和排列
panel->Measure(Size(400, 600));
panel->Arrange(Rect(0, 0, 400, 600));

// 检查子元素位置
auto children = panel->GetChildren();
for (size_t i = 0; i < children.size(); i++) {
    auto rect = children[i]->GetLayoutRect();
    std::cout << "元素 " << (i + 1) << " 位置: (" 
              << rect.x << ", " << rect.y << ")" << std::endl;
}
```

预期输出：
```
元素 1 位置: (0, 0)
元素 2 位置: (0, <height1 + 10>)
元素 3 位置: (0, <height1 + height2 + 20>)
```

---

## 2. Alignment 支持和 Spacing 优化 (2025-11-19)

### 实现的功能

本次更新为 StackPanel 添加了完整的 Alignment 支持，并优化了 Spacing 行为，使其更符合直觉。

#### 2.1 Spacing 参与 Margin 折叠

**设计理念**: Spacing 表示"**最小间距保证**"，与 Margin 一起参与折叠（取最大值）。

**计算公式**:
```
实际间距 = max(前一个元素的尾部Margin, 当前元素的头部Margin, Spacing)
```

**行为示例**:

| 前Margin | 后Margin | Spacing | 实际间距 | 计算过程 | 说明 |
|----------|----------|---------|---------|----------|------|
| 10 | 15 | 0 | 15 | max(10, 15, 0) | 无 Spacing，Margin 折叠 |
| 10 | 15 | 20 | 20 | max(10, 15, 20) | Spacing 胜出 |
| 10 | 15 | 5 | 15 | max(10, 15, 5) | Margin 更大，Spacing 被吸收 |
| 30 | 5 | 20 | 30 | max(30, 5, 20) | 尊重大 Margin |
| 0 | 0 | 20 | 20 | max(0, 0, 20) | 仅 Spacing 生效 |

**语义解释**:
- `Spacing = 20` → "保证子元素间距**至少** 20 像素"
- 如果某个子元素的 Margin 更大（如 30），则尊重该 Margin
- 如果所有 Margin 都小于 Spacing，则间距由 Spacing 决定

#### 2.2 完整的 Alignment 支持

遵循 WPF 标准，实现了完整的 HorizontalAlignment 和 VerticalAlignment 支持。

##### 垂直 StackPanel - HorizontalAlignment

在垂直堆叠的 StackPanel 中，子元素的 **HorizontalAlignment** 有效（控制宽度和水平位置）：

| Alignment | 行为 | 宽度 | X 位置 |
|-----------|------|------|--------|
| **Stretch** (默认) | 拉伸填充 | 可用宽度 | margin.left |
| **Left** | 左对齐 | 期望宽度 | margin.left |
| **Center** | 居中对齐 | 期望宽度 | margin.left + (可用宽度 - 期望宽度) / 2 |
| **Right** | 右对齐 | 期望宽度 | margin.left + 可用宽度 - 期望宽度 |

**示例代码**:
```cpp
auto* panel = new StackPanel();
panel->SetOrientation(Orientation::Vertical);
panel->SetWidth(200);

auto* btnStretch = new Button();
btnStretch->SetWidth(100);
btnStretch->SetHorizontalAlignment(HorizontalAlignment::Stretch);
panel->AddChild(btnStretch);  // 宽度 = 200（拉伸）

auto* btnCenter = new Button();
btnCenter->SetWidth(100);
btnCenter->SetHorizontalAlignment(HorizontalAlignment::Center);
panel->AddChild(btnCenter);  // 宽度 = 100，水平居中
```

##### 水平 StackPanel - VerticalAlignment

在水平堆叠的 StackPanel 中，子元素的 **VerticalAlignment** 有效（控制高度和垂直位置）：

| Alignment | 行为 | 高度 | Y 位置 |
|-----------|------|------|--------|
| **Stretch** (默认) | 拉伸填充 | 可用高度 | margin.top |
| **Top** | 顶部对齐 | 期望高度 | margin.top |
| **Center** | 居中对齐 | 期望高度 | margin.top + (可用高度 - 期望高度) / 2 |
| **Bottom** | 底部对齐 | 期望高度 | margin.top + 可用高度 - 期望高度 |

**示例代码**:
```cpp
auto* panel = new StackPanel();
panel->SetOrientation(Orientation::Horizontal);
panel->SetHeight(200);

auto* btnStretch = new Button();
btnStretch->SetHeight(100);
btnStretch->SetVerticalAlignment(VerticalAlignment::Stretch);
panel->AddChild(btnStretch);  // 高度 = 200（拉伸）

auto* btnCenter = new Button();
btnCenter->SetHeight(100);
btnCenter->SetVerticalAlignment(VerticalAlignment::Center);
panel->AddChild(btnCenter);  // 高度 = 100，垂直居中
```

#### 2.3 性能优化

实现了 Alignment 查询结果缓存，避免在布局循环中重复查询：

```cpp
// 在循环开始处缓存查询结果
auto hAlign = child->GetHorizontalAlignment();
auto vAlign = child->GetVerticalAlignment();

// 后续直接使用缓存值
switch (hAlign) {
    case HorizontalAlignment::Stretch:
        // ...
}
```

### 与 WPF 的差异

#### 相同点
- ✅ Alignment 行为完全兼容 WPF
- ✅ 垂直 StackPanel 中 HorizontalAlignment 有效，VerticalAlignment 无效
- ✅ 水平 StackPanel 中 VerticalAlignment 有效，HorizontalAlignment 无效
- ✅ 堆叠方向给予子元素无限空间

#### 差异点

| 特性 | WPF | 本实现 | 说明 |
|------|-----|--------|------|
| **Margin 折叠** | 相加 | 取最大值 (CSS 风格) | 本实现更符合 CSS 习惯 |
| **Spacing 属性** | 无（通过 Margin 模拟） | 专门的 Spacing 属性 | 更方便统一设置间距 |
| **Spacing 语义** | N/A | "最小间距保证" | 参与 Margin 折叠 |

### 典型用例

#### 用例 1: 统一间距的按钮列表

```cpp
auto* panel = new StackPanel();
panel->SetOrientation(Orientation::Vertical);
panel->SetSpacing(15);  // 保证至少 15 像素间距

// 即使按钮有不同的 Margin，也能保证最小间距
panel->AddChild(CreateButton()->SetMargin(Thickness(0, 5, 0, 5)));   // 间距 15
panel->AddChild(CreateButton()->SetMargin(Thickness(0, 10, 0, 10))); // 间距 15
panel->AddChild(CreateButton()->SetMargin(Thickness(0, 0, 0, 0)));   // 间距 15
```

#### 用例 2: 混合对齐的表单布局

```cpp
auto* panel = new StackPanel();
panel->SetOrientation(Orientation::Vertical);
panel->SetWidth(400);
panel->SetSpacing(10);

// 标题 - 居中
auto* title = new TextBlock()->Text("用户信息");
title->SetHorizontalAlignment(HorizontalAlignment::Center);
panel->AddChild(title);

// 输入框 - 拉伸填充
auto* nameInput = new TextBox();
nameInput->SetHorizontalAlignment(HorizontalAlignment::Stretch);
panel->AddChild(nameInput);

// 按钮组 - 右对齐
auto* btnPanel = new StackPanel();
btnPanel->SetOrientation(Orientation::Horizontal);
btnPanel->SetHorizontalAlignment(HorizontalAlignment::Right);
btnPanel->AddChild(new Button()->Content("取消"));
btnPanel->AddChild(new Button()->Content("确定"));
panel->AddChild(btnPanel);
```

#### 用例 3: 工具栏布局

```cpp
auto* toolbar = new StackPanel();
toolbar->SetOrientation(Orientation::Horizontal);
toolbar->SetHeight(40);
toolbar->SetSpacing(5);

// 所有按钮垂直居中
auto* btn1 = new Button()->SetVerticalAlignment(VerticalAlignment::Center);
auto* btn2 = new Button()->SetVerticalAlignment(VerticalAlignment::Center);
auto* btn3 = new Button()->SetVerticalAlignment(VerticalAlignment::Center);

toolbar->AddChild(btn1);
toolbar->AddChild(btn2);
toolbar->AddChild(btn3);
```

### 测试验证

完整的测试用例位于 `test_stackpanel_alignment.cpp`，包含：

1. **Spacing 折叠测试** - 验证不同 Margin 和 Spacing 组合
2. **垂直 StackPanel Alignment 测试** - 测试所有 HorizontalAlignment 选项
3. **水平 StackPanel Alignment 测试** - 测试所有 VerticalAlignment 选项
4. **综合场景测试** - 实际应用示例验证

运行测试：
```bash
# 编译并运行测试
cmake --build build --target test_stackpanel_alignment
./build/test_stackpanel_alignment
```

### 代码注释

在 `src/ui/StackPanel.cpp` 中添加了详细的代码注释，说明：

- 无限空间设计原理
- Spacing 作为"最小间距保证"的语义
- Alignment 的 WPF 兼容规则
- Margin 折叠的 CSS 风格
- 性能优化考虑（Alignment 查询缓存）

### 总结

本次更新使 StackPanel 的功能更加完善：

- ✅ **直觉的 Spacing 行为**：作为"最小间距保证"，更符合开发者预期
- ✅ **完整的 Alignment 支持**：与 WPF 兼容，提供灵活的布局控制
- ✅ **性能优化**：缓存 Alignment 查询，减少重复调用
- ✅ **详细的文档和测试**：易于理解和维护

这些改进使 StackPanel 成为一个功能完整、行为可预测的布局容器。

## 相关文件

- `src/ui/UIElement.cpp` - 主要修复文件
- `include/fk/ui/UIElement.h` - UIElement 类定义
- `src/ui/StackPanel.cpp` - StackPanel 实现（未修改）

## 参考

- Issue: 上一个PR位置确实回到了左上角，但stackpanel中所有元素还是重叠在一起
- PR: #[当前PR编号]
- 提交: cedd96b - 修复StackPanel元素重叠问题 - 更新UIElement::Arrange逻辑
