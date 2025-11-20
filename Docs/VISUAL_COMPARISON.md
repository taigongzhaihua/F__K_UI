# Button 坐标修复 - 可视化对比

## 问题：TextBlock 的 y 坐标是 Border 的 2 倍

### 修复前（错误）

```
坐标系统（y 轴向下）:
0 ─────────────────────────────
  │                             
  │  ┌─────────────────────┐   
  │  │ Border (0, 0)       │   ← Border 在正确位置
  │  │  Background: 灰色   │
  │  │  Padding: 10, 5     │
6 ─  │                     │
  │  │                     │
  │  └─────────────────────┘
  │
  │  
12─     "Click Me"              ← TextBlock 错误地在 y=12
  │                                (应该在 y=6，但被双重变换了)
  │
```

**问题**：
- Border 绘制在 y=0 ✓
- TextBlock **应该**绘制在 y=6（Border 的 padding.top）
- 但由于双重变换，TextBlock **实际**绘制在 y=12
- 12 = 6 × 2（这就是"y坐标大约是两倍"的原因！）

### 修复后（正确）

```
坐标系统（y 轴向下）:
0 ─────────────────────────────
  │                             
  │  ┌─────────────────────┐   
  │  │ Border (0, 0)       │   ← Border 在正确位置
  │  │  Background: 灰色   │
  │  │  Padding: 10, 5     │
6 ─  │                     │
  │  │  "Click Me"         │   ← TextBlock 在正确位置 y=6
  │  │                     │
  │  └─────────────────────┘
  │
```

**正确行为**：
- Border 绘制在 y=0 ✓
- TextBlock 绘制在 y=6 ✓
- 完美对齐！

## 技术细节

### 坐标变换流程

#### 修复前（双重变换）

```
1. TextBlock::OnRender
   └─> DrawText(Point(0, 0))  // 局部坐标

2. RenderContext::DrawText
   └─> globalPos = TransformPoint(Point(0, 0))
       = Point(0, 0) + 累积变换(11, 6)
       = Point(11, 6)  // 第一次变换 ✓
   └─> payload.bounds = (11, 6)

3. 渲染器执行
   └─> uOffset = (11, 6)  // 从 SetTransform 命令
   └─> 着色器：pos = payload.bounds + uOffset
       = (11, 6) + (11, 6)
       = (22, 12)  // 第二次变换 ✗ 错误！
```

#### 修复后（单次变换）

```
1. TextBlock::OnRender
   └─> DrawText(Point(0, 0))  // 局部坐标

2. RenderContext::DrawText
   └─> globalPos = TransformPoint(Point(0, 0))
       = Point(0, 0) + 累积变换(11, 6)
       = Point(11, 6)  // 唯一的变换 ✓
   └─> payload.bounds = (11, 6)

3. 渲染器执行
   └─> 着色器：pos = payload.bounds
       = (11, 6)  // 直接使用全局坐标 ✓ 正确！
```

## 布局细节

### Button 内部结构

```
Button
├─ Border (模板根)
│  ├─ Background: 浅灰色 (240, 240, 240)
│  ├─ BorderBrush: 灰色 (172, 172, 172)
│  ├─ BorderThickness: 1
│  ├─ Padding: (10, 5, 10, 5)  ← 左、上、右、下
│  ├─ CornerRadius: 3
│  │
│  └─ Child: ContentPresenter
│     ├─ HorizontalAlignment: Center
│     ├─ VerticalAlignment: Center
│     │
│     └─ Child: TextBlock
│        └─ Text: "Click Me"
```

### 布局计算

```
1. Border::ArrangeOverride
   └─> 子元素布局位置 = (
         borderThickness.left + padding.left,
         borderThickness.top + padding.top
       )
       = (1 + 10, 1 + 5)
       = (11, 6)
   └─> ContentPresenter.layoutRect = (11, 6, width, height)

2. ContentPresenter::ArrangeOverride
   └─> TextBlock.layoutRect = (0, 0, width, height)
       // 相对于 ContentPresenter 的坐标

3. 全局坐标计算
   └─> TextBlock 全局位置 = Button(0, 0) + Border(0, 0) + ContentPresenter(11, 6) + TextBlock(0, 0)
       = (11, 6)
```

## 实际测试输出

### 修复前

```
=== 模拟渲染器执行（模拟着色器行为：pos = aPos + uOffset）===
命令 #3: DrawRectangle
  payload坐标: (0, 0)
  uOffset: (0, 0)
  **最终渲染位置**: (0, 0)

命令 #6: DrawText 'Click Me'
  payload坐标: (11, 6)
  uOffset: (11, 6)
  **最终渲染位置**: (22, 12)  ← 错误！
```

### 修复后

```
=== 修复后的渲染器行为（着色器：pos = aPos，不加 uOffset）===
命令 #3: DrawRectangle (Border)
  payload坐标（全局）: (0, 0)
  **最终渲染位置**: (0, 0)  ✓

命令 #6: DrawText 'Click Me' (TextBlock)
  payload坐标（全局）: (11, 6)
  **最终渲染位置**: (11, 6)  ✓ 正确！
```

## 结论

修复前后的对比清楚地展示了问题：
- **修复前**: TextBlock 的 y 坐标 (12) 是正确值 (6) 的 2 倍
- **修复后**: TextBlock 的 y 坐标 (6) 与 Border 的 padding 完美对齐

通过移除渲染器着色器中的 uOffset，我们消除了双重变换，使得 Border 和 TextBlock 正确对齐。

---

**相关文档**:
- [COORDINATE_FIX_SUMMARY_中文.md](./COORDINATE_FIX_SUMMARY_中文.md) - 详细技术总结
- [BUTTON_FIX_SUMMARY_中文.md](./BUTTON_FIX_SUMMARY_中文.md) - Button 渲染修复历史
