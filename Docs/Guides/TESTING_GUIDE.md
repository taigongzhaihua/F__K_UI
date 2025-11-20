# 修复测试指南

## 问题描述（修复前）

在 `example/main.cpp` 程序中观察到：
- **TextBlock（文字"Click Me"）**显示在上方
- **Border（按钮的矩形框）**显示在下方
- Border 的 y 坐标大约是 TextBlock 的 2 倍
- 这导致文字和矩形框错位，视觉效果不正确

## 问题原因

**双重变换**：
1. `RenderContext` 已经将坐标转换为全局坐标
2. 着色器又使用 `vec2 pos = aPos + uOffset` 再次应用变换
3. 导致某些元素的坐标被错误地变换两次

## 修复内容

**提交 4d2c3fc** 修改了 `src/render/GlRenderer.cpp`：

```glsl
// 修复前
uniform vec2 uOffset;
void main() {
    vec2 pos = aPos + uOffset;  // 双重变换！
    ...
}

// 修复后
void main() {
    vec2 pos = aPos;  // aPos 已经是全局坐标
    ...
}
```

同时移除了所有设置 `uOffset` uniform 的代码。

## 预期效果（修复后）

运行 `example/main.cpp` 应该看到：

```
┌────────────────────────────┐
│  Hello, F K UI!            │  ← TextBlock1（蓝色，大字）
│                            │
│  This is a simple...       │  ← TextBlock2（灰色，小字）
│                            │
│  ┏━━━━━━━━━━━━┓            │
│  ┃ Click Me   ┃            │  ← Button（绿色背景，白色文字）
│  ┗━━━━━━━━━━━━┛            │     Border 和 TextBlock 正确对齐
│                            │
└────────────────────────────┘
```

**正确的布局**：
- Button 的 Border（矩形框）应该包围文字
- TextBlock "Click Me" 应该在 Border 内部居中
- Border 和 TextBlock 应该在同一个位置，而不是分离的

## 测试步骤

1. **编译项目**：
   ```bash
   cd build
   cmake ..
   make example_app
   ```

2. **运行程序**：
   ```bash
   ./example_app
   ```

3. **验证效果**：
   - 检查 Button 是否显示为一个完整的按钮
   - 文字 "Click Me" 应该在按钮矩形内部
   - 按钮应该有绿色背景（从代码中的 `->Background(Brushes::Green())`）
   - 文字应该是白色

## 如果问题依然存在

如果修复后问题仍然存在，请提供：
1. 运行 `example/main.cpp` 的截图
2. Button 显示的具体位置和状态
3. 是否有任何错误或警告消息

这将帮助我进一步诊断和修复问题。

## 相关测试

项目中还包含了多个测试程序来验证修复：
- `test_fixed_renderer` - 验证修复后的坐标
- `test_before_fix_analysis` - 分析修复前的行为
- `test_global_coordinates` - 显示完整场景的坐标

这些测试都应该通过，并且输出显示正确的坐标对齐。
