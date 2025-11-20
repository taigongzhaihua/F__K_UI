# 建议的修复方案

## 问题确认

用户的反馈是正确的：**Y=118.4 确实不合理，视觉间距太大（60px）。**

## 问题根源

当前代码对**所有文本**（包括单行）都应用了 1.2 倍行高系数：

```cpp
// src/ui/TextBlock.cpp:128
float estimatedHeight = fontSize * 1.2f;  // 32 × 1.2 = 38.4
```

这导致：
- desiredSize = 38.4 + 40 (margin) = 78.4
- 但实际渲染高度只有 38.4
- 视觉间距 = 60px（太大！）

## 为什么 1.2 倍系数不适合单行文本？

### 行高系数的本意

行高系数是为了**多行文本的行间距**：

```
第一行文本
  ↕ 行间距 (line-height - font-size)
第二行文本
  ↕ 行间距
第三行文本
```

### 单行文本的问题

对于单行文本：
```
【空白】← 上半部分的行高预留（不需要！）
文本内容
【空白】← 下半部分的行高预留（不需要！）
```

## 建议的修复方案

### 方案 A：单行文本不使用行高系数（推荐）

```cpp
// src/ui/TextBlock.cpp
} else {
    // 不换行：单行文本
    float estimatedWidth = text.length() * fontSize * 0.6f;
    float estimatedHeight = fontSize;  // 改为 fontSize，去掉 * 1.2f
    
    return Size(
        std::min(estimatedWidth, availableSize.width),
        estimatedHeight
    );
}
```

**效果：**
- desiredSize.height = 32 + 40 = 72
- Text2.y = 20 + 72 + 0 = 92
- 视觉间距 = 40px（合理！）

**优点：**
- 简单直接
- 单行文本间距合理
- 多行文本仍保持行间距

**缺点：**
- 可能需要调整字形的实际高度估算

### 方案 B：减小单行文本的行高系数

```cpp
} else {
    // 不换行：单行文本
    float estimatedWidth = text.length() * fontSize * 0.6f;
    float estimatedHeight = fontSize * 1.0f;  // 改为 1.0，而不是 1.2
    
    return Size(
        std::min(estimatedWidth, availableSize.width),
        estimatedHeight
    );
}
```

**效果：**
- desiredSize.height = 32 + 40 = 72
- 与方案A相同

### 方案 C：添加 LineHeight 属性（最灵活，但更复杂）

```cpp
// 添加新的依赖属性
static const binding::DependencyProperty& LineHeightProperty();
float GetLineHeight() const { 
    return GetValue<float>(LineHeightProperty()); 
}

// 在 MeasureOverride 中使用
float lineHeightFactor = GetLineHeight();  // 默认 1.0 或 auto
if (lineHeightFactor == 0.0f) {
    // auto: 单行用 1.0，多行用 1.2
    lineHeightFactor = (textWrapping == TextWrapping::Wrap) ? 1.2f : 1.0f;
}
float estimatedHeight = fontSize * lineHeightFactor;
```

**优点：**
- 用户可控制
- 兼容性最好

**缺点：**
- 实现复杂
- API 增加

## 推荐实施

### 第一阶段：快速修复

使用**方案 A**，修改 `src/ui/TextBlock.cpp:128`：

```cpp
float estimatedHeight = fontSize;  // 去掉 * 1.2f
```

**原因：**
1. 最简单
2. 立即解决用户报告的问题
3. 符合用户期望

### 第二阶段：完善

如果需要更精确的控制，实施**方案 C**，添加 LineHeight 属性。

## 测试验证

修改后，运行测试应该显示：

```
Text1 "Hello, F K UI!":
  Desired: (308.8, 72)          ← 从 78.4 变为 72
  LayoutRect: (20, 20, 760, 72) ← 从 78.4 变为 72
  RenderSize: (268.8, 32)       ← 从 38.4 变为 32

Text2 "This is a simple example...":
  LayoutRect: (20, 92, 760, 56) ← 从 118.4 变为 92

实际间距: 40 像素  ← 从 60 变为 40
✅ 间距合理
```

## 其他考虑

### CSS 参考

CSS 的 line-height 只影响行内的垂直对齐，不会在单行元素上下添加额外空间：

```css
.single-line {
    font-size: 32px;
    line-height: 1.2;  /* 不会增加元素高度 */
}
```

### WPF 参考

WPF 的 TextBlock.LineHeight 默认是 NaN (auto)，在这种情况下不会添加额外空间。

## 结论

**用户的反馈是正确的。** 118.4 的Y坐标导致60px的视觉间距，这确实不合理。

**建议立即实施方案 A 的修复**，将单行文本的行高系数从 1.2 改为 1.0（即去掉系数）。

这将使视觉间距从 60px 减少到 40px（只是两个 margin），符合用户期望和标准排版实践。
