# 关于"border位于文字下方"的澄清

## 用户反馈

用户说："**修复前**，border明显位于文字下方"

但根据我的分析，修复前：
- Border 渲染在 y=0
- TextBlock 渲染在 y=12

在屏幕坐标系中（y 轴向下），y=0 在上方，y=12 在下方。所以 TextBlock 应该在 Border **下方**。

## 可能的误解场景

### 场景1：视觉效果 vs 坐标

也许用户看到的是 Border 的**底边**：

```
修复前的视觉效果（错误）：
y=0  ┌─────────────────────┐
     │ Border             │
     │                     │
y=12 │                     │  TextBlock "Click Me"
     │                     │
     └─────────────────────┘  ← Border 的底边在 y=50
```

在这种情况下，TextBlock 被渲染在 y=12，恰好在 Border 内部的中间位置，但由于双重变换，位置不对。

### 场景2：用户看到的是不同的场景

也许用户看到的不是简单的 Button，而是在某个布局中，Border 和 TextBlock 的渲染顺序不同？

### 场景3：我的分析有误

让我重新检查修复前的着色器行为...

## 重新分析修复前的行为

修复前着色器：
```glsl
vec2 pos = aPos + uOffset;
```

关键问题：**uOffset 在什么时候被设置？**

根据命令序列：
```
命令 #0: SetTransform(0, 0)
命令 #1: SetTransform(0, 0)
命令 #2: DrawRectangle (Border) - payload.y=0
         着色器使用：pos.y = 0 + 0 = 0
         
命令 #3: SetTransform(11, 6)
命令 #4: SetTransform(11, 6)
命令 #5: DrawText (TextBlock) - payload.y=6
         着色器使用：pos.y = 6 + 6 = 12
```

所以：
- Border 实际渲染在屏幕 y=0（靠上）
- TextBlock 实际渲染在屏幕 y=12（靠下）

## 问题

如果 Border 在 y=0，TextBlock 在 y=12，那么 Border 应该在文字**上方**，而不是下方。

但用户说"border位于文字下方"。

### 可能的解释

1. **用户看到的是 Border 的矩形框包围了文字**：Border 是一个矩形，从 y=0 到 y=50，而 TextBlock 在 y=12，所以文字在 Border 内部。也许用户说的"border位于文字下方"是指"border的底边在文字下方"？

2. **我对 payload 坐标的理解有误**：也许 RenderContext::DrawRectangle 传入的不是全局坐标？让我重新检查...

3. **还有其他的变换逻辑**：也许在某个地方还有其他的坐标变换？

## 需要进一步确认

我需要：
1. 确认修复前用户实际看到的视觉效果
2. 确认 Border 和 TextBlock 的绝对屏幕坐标
3. 理解用户说"border位于文字下方"的确切含义

## 当前修复的正确性

无论如何，当前的修复是正确的：
- 移除了双重变换
- 所有测试通过
- Border 和 TextBlock 正确对齐

修复后：
- Border 在 y=0
- TextBlock 在 y=6（正确的 padding 偏移）

这是符合预期的布局。
