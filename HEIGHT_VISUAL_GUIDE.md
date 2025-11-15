# 32号字为什么布局高度是 78.4 - 可视化指南

## 快速答案

```
78.4 = 行高(38.4) + 上边距(20) + 下边距(20)
     = (32 × 1.2) + 20 + 20
```

## 可视化布局

```
┌─────────────────────────────────────┐
│                                     │ ← margin.top = 20px
│         (20px 空白区域)              │
├─────────────────────────────────────┤ ← Text1 内容区域开始
│                                     │
│   Hello, F K UI!  ← 字体32px        │
│                                     │ ← 内容高度 38.4px
│   (实际渲染高度 38.4px)             │    (32 × 1.2 行高系数)
│                                     │
├─────────────────────────────────────┤ ← Text1 内容区域结束
│                                     │
│         (20px 空白区域)              │ ← margin.bottom = 20px
│                                     │
└─────────────────────────────────────┘
 ↑                                   ↑
 0                                  78.4
 
总高度 = 20 + 38.4 + 20 = 78.4px
```

## 三层结构分解

### 第一层：字体大小 (32px)
```
fontSize = 32
```
这只是字符的**设计尺寸**，不等于实际占用空间。

### 第二层：行高 (38.4px)
```
lineHeight = fontSize × 1.2 = 32 × 1.2 = 38.4
```
包含：
- 字符本身：约 32px
- 额外空间：6.4px（用于升部/降部/行间距）

```
      ↑ 升部空间 (ascender)
   ───┼───
  b  d│h  ← 字符上部
   ───┼───
  a  e│o  ← 字符主体（约32px）
   ───┼───
  g  p│q  ← 字符下部
   ───┼───
      ↓ 降部空间 (descender)
      
  总高度：38.4px
```

### 第三层：布局高度 (78.4px)
```
layoutHeight = lineHeight + margin.top + margin.bottom
             = 38.4 + 20 + 20
             = 78.4
```

## 代码调用链

```
1. 用户设置
   ────────────────────────
   text1->FontSize(32)
   text1->Margin(fk::Thickness(20))


2. Measure 阶段
   ────────────────────────
   UIElement::Measure()
      ↓
   FrameworkElement::MeasureCore()
      ↓ (减去 margin)
      ↓ availableSize - margin = ...
      ↓
   TextBlock::MeasureOverride()
      ↓ estimatedHeight = 32 × 1.2 = 38.4
      ↓ return Size(width, 38.4)
      ↓
   FrameworkElement::MeasureCore()
      ↓ (加上 margin)
      ↓ desiredSize.height = 38.4 + 20 + 20
      ↓ return Size(width, 78.4)  ← desiredSize
      ↓
   保存为 desiredSize_ = 78.4


3. Arrange 阶段
   ────────────────────────
   StackPanel::ArrangeOverride()
      ↓
   childDesired = child->GetDesiredSize()
      ↓ childDesired.height = 78.4
      ↓
   计算下一个元素位置
      ↓ offset += 78.4
      ↓ offset = 20 + 78.4 = 98.4
      ↓
   加上 margin 折叠
      ↓ Text2.y = 98.4 + 20 = 118.4
```

## 为什么不是其他值？

### 如果是 32？
```
❌ 32 = 只有字体大小，没有行高和边距
不符合排版标准，字符会紧贴在一起
```

### 如果是 38.4？
```
❌ 38.4 = 只有行高，没有边距
元素会紧贴，没有外部间距
```

### 如果是 72？
```
❌ 72 = 32 + 20 + 20（没有行高系数）
违反了排版规则，行间距不足
```

### ✅ 78.4 是正确的
```
✅ 78.4 = (32 × 1.2) + 20 + 20
包含了行高和边距，符合标准排版
```

## 行高系数 1.2 的来源

这是业界标准：

| 平台/技术 | 默认行高 |
|----------|---------|
| CSS | `line-height: 1.2` 或 `120%` |
| WPF | `LineHeight = FontSize × 1.33` |
| Android | `lineSpacingMultiplier = 1.0-1.2` |
| iOS | `lineHeight ≈ fontSize × 1.2` |
| 本项目 | `fontSize × 1.2` |

代码位置：`src/ui/TextBlock.cpp` 第 123、128 行
```cpp
float lineHeight = fontSize * 1.2f;
```

## 实际测试验证

运行 `detailed_spacing_test` 输出：

```
Text1 配置:
  字体大小: 32

布局结果:
  Desired: (308.8, 78.4)      ← 包含 margin 的期望尺寸
  LayoutRect: (20, 20, 760, 78.4)
  RenderSize: (268.8, 38.4)   ← 实际内容尺寸（行高）

计算过程:
  38.4 (行高) + 20 (top margin) + 20 (bottom margin) = 78.4 ✅
```

## 总结

**32号字的布局高度是 78.4 的原因：**

1. **字体大小 32** - 字符设计尺寸
2. **行高系数 1.2** - 标准排版规则
3. **行高 38.4** = 32 × 1.2
4. **边距 40** = 20 (top) + 20 (bottom)
5. **总高度 78.4** = 38.4 + 40

这是**完全正确且符合标准**的布局行为！
