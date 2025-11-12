# Border 设计文档

## 类概述

Border 是一个装饰器控件，为单个子元素提供边框和背景。它是最常用的布局辅助控件之一，支持圆角、边框样式和背景设置。

## 继承关系

```
Object
  └─ DependencyObject
      └─ Visual
          └─ UIElement
              └─ FrameworkElement
                  └─ Decorator
                      └─ Border
```

## 核心职责

1. **装饰功能**
   - 为子元素提供边框
   - 设置背景色或背景图
   - 支持圆角边框

2. **布局管理**
   - 测量和排列单个子元素
   - 考虑边框厚度和内边距
   - 处理子元素的对齐

3. **渲染**
   - 渲染背景
   - 渲染边框
   - 应用圆角效果

## 主要属性

### BorderBrush / BorderThickness
**职责**：定义边框的颜色和厚度
**实现状态**：✅ 已实现
**类型**：Brush / Thickness

### Background
**职责**：设置背景画刷
**实现状态**：✅ 已实现
**类型**：Brush

### CornerRadius
**职责**：设置圆角半径
**实现状态**：✅ 已实现
**类型**：CornerRadius

### Padding
**职责**：设置内边距
**实现状态**：✅ 已实现
**类型**：Thickness

### Child
**职责**：Border的单个子元素
**实现状态**：✅ 已实现
**类型**：UIElement

## 成员函数职责

### MeasureOverride(availableSize)
**职责**：测量Border及其子元素所需的大小
**实现状态**：✅ 已实现
**实现细节**：
- 计算边框和内边距占用的空间
- 为子元素提供减去装饰空间后的可用大小
- 返回子元素大小加上装饰空间

### ArrangeOverride(finalSize)
**职责**：排列子元素
**实现状态**：✅ 已实现
**实现细节**：
- 计算子元素的实际布局区域（排除边框和内边距）
- 调用子元素的Arrange方法

### OnRender(context)
**职责**：渲染边框和背景
**实现状态**：✅ 已实现
**实现细节**：
- 先渲染背景
- 再渲染边框
- 应用圆角裁剪

## 实现状态

### 已实现功能 ✅

- ✅ 基本的边框渲染
- ✅ 背景渲染
- ✅ 圆角支持
- ✅ 内边距处理
- ✅ 单个子元素布局

### 简单实现须扩充 ⚠️

- ⚠️ **边框样式**：目前只支持实线，虚线、点线等样式未实现
- ⚠️ **渐变背景**：渐变画刷支持较简单
- ⚠️ **阴影效果**：缺少DropShadow等效果
- ⚠️ **圆角优化**：圆角渲染性能可优化

### 未实现功能 ❌

- ❌ **边框图像**：BorderImage（使用九宫格图像作为边框）
- ❌ **多边框**：多层边框支持
- ❌ **边框动画**：边框属性的平滑过渡
- ❌ **复杂形状裁剪**：非矩形裁剪路径

## 实现原理

### 布局计算

```
可用大小 = 父元素提供的大小
边框空间 = BorderThickness
内边距空间 = Padding
子元素可用大小 = 可用大小 - 边框空间 - 内边距空间

Border最终大小 = 子元素大小 + 边框空间 + 内边距空间
```

### 渲染顺序

1. **背景渲染**：填充整个Border区域（包含边框内部）
2. **子元素渲染**：在内容区域渲染子元素
3. **边框渲染**：在边缘绘制边框线

### 圆角实现

- 使用裁剪路径限制渲染区域
- 圆角矩形的四个角使用贝塞尔曲线或圆弧
- 考虑边框厚度对圆角的影响（内外半径）

## 扩展方向

### 短期扩展

1. **边框样式**
   ```cpp
   enum class BorderStyle {
       Solid,      // 实线
       Dashed,     // 虚线
       Dotted,     // 点线
       Double      // 双线
   };
   ```

2. **渐变边框**
   - 支持LinearGradientBrush作为BorderBrush
   - 支持RadialGradientBrush

3. **阴影效果**
   ```cpp
   struct DropShadow {
       Color color;
       float offsetX, offsetY;
       float blurRadius;
   };
   ```

### 中期扩展

1. **边框图像**
   ```cpp
   BorderImage borderImage;  // 九宫格图像边框
   BorderImageSlice slice;   // 切片参数
   ```

2. **不同边的独立样式**
   ```cpp
   BorderStyle leftStyle, topStyle, rightStyle, bottomStyle;
   ```

3. **裁剪到内容**
   ```cpp
   bool ClipToBounds;  // 裁剪溢出的子元素
   ```

### 长期扩展

1. **复杂路径裁剪**
   - 支持任意Geometry作为裁剪路径
   - 椭圆、多边形等形状

2. **视觉效果**
   - 模糊效果
   - 光泽效果
   - 3D边框效果

## 性能考虑

### 当前性能

1. **渲染性能**：中等
   - 简单矩形边框：非常快
   - 圆角边框：需要额外的裁剪路径计算

2. **布局性能**：优秀
   - 只有一个子元素，布局计算简单

### 优化建议

1. **圆角缓存**
   - 缓存圆角路径，避免重复计算
   - 只在CornerRadius变化时重新计算

2. **渲染批处理**
   - 将多个Border的背景/边框合并到一个绘制调用

3. **脏区域优化**
   - 只在边框属性变化时重绘边框
   - 子元素变化时不重绘边框

## 设计决策

### 1. 为什么继承Decorator而非直接继承FrameworkElement？

**决策**：Border继承Decorator基类

**理由**：
- Decorator提供单子元素的标准模式
- 代码复用（其他装饰器如Viewbox也使用）
- 语义清晰

### 2. 为什么使用Thickness而非四个独立属性？

**决策**：BorderThickness和Padding使用Thickness结构

**理由**：
- 统一的四边设置API
- 支持对称设置（left/right, top/bottom）
- 减少属性数量

**权衡**：
- ✅ API简洁
- ⚠️ 四边独立样式需要额外支持

### 3. 圆角的实现方式

**决策**：使用CornerRadius结构定义四个角的半径

**理由**：
- 灵活性（每个角可独立设置）
- 常见的UI需求
- 与其他UI框架一致

**实现**：
- 使用裁剪路径而非直接绘制
- 保证子元素也被圆角裁剪

## 使用示例

### 基本用法

```cpp
auto border = std::make_shared<Border>();
border->SetBorderBrush(Brushes::Black);
border->SetBorderThickness(Thickness(1));
border->SetBackground(Brushes::LightGray);
border->SetChild(textBlock);
```

### 圆角边框

```cpp
border->SetCornerRadius(CornerRadius(5));  // 所有角5px半径
// 或者
border->SetCornerRadius(CornerRadius(10, 10, 0, 0));  // 只有上方两角圆角
```

### 带内边距

```cpp
border->SetPadding(Thickness(10));  // 四边10px内边距
border->SetPadding(Thickness(10, 5, 10, 5));  // 左右10px，上下5px
```

### 组合使用

```cpp
auto card = std::make_shared<Border>();
card->SetBackground(Brushes::White);
card->SetBorderBrush(Brushes::Gray);
card->SetBorderThickness(Thickness(1));
card->SetCornerRadius(CornerRadius(8));
card->SetPadding(Thickness(16));
card->SetChild(content);
```

## 测试策略

### 单元测试

1. **布局测试**
   - 测试边框和内边距对布局的影响
   - 测试圆角不影响布局计算
   - 测试子元素对齐

2. **渲染测试**
   - 测试背景渲染
   - 测试边框渲染
   - 测试圆角效果

3. **属性测试**
   - 测试各属性的get/set
   - 测试属性变更通知

### 视觉测试

1. 截图对比测试
2. 不同圆角半径的渲染效果
3. 不同边框厚度的效果

### 性能测试

1. 大量Border的渲染性能
2. 复杂圆角的性能影响

## 相关文档

- [Border API文档](../../API/UI/Border.md)
- [FrameworkElement 设计文档](./FrameworkElement.md)
- [Thickness 设计文档](./Thickness.md)
- [CornerRadius 设计文档](./CornerRadius.md)

## 总结

Border是一个简单但非常实用的装饰器控件。当前实现覆盖了基本功能，包括边框、背景和圆角。未来可以扩展边框样式、阴影效果和边框图像等高级功能。作为最常用的布局辅助控件之一，Border的性能优化（特别是圆角渲染）值得持续关注。
