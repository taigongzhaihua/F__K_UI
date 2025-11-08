# FrameworkElement 文档

## 概述

`FrameworkElement` 是 UI 框架的核心基础类，集成了布局系统（Measure/Arrange）、尺寸管理、对齐与边距等功能。几乎所有可视元素都继承自它。

**继承链：** `UIElement` → `FrameworkElement`

**命名空间：** `fk::ui`

**职责：**
- ✅ 管理元素尺寸（Width、Height、Min/Max Constraints）
- ✅ 支持布局计算（Measure、Arrange）
- ✅ 提供边距和对齐属性
- ✅ 支持数据绑定

---

## 尺寸管理

### 固定尺寸属性

#### Width
- **类型：** `float`
- **默认值：** `NaN` (自动计算)
- **说明：** 元素的固定宽度（单位：像素）
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::button()->Width(100.0f);  // 固定 100 像素宽

ui::button()->Width(NaN());   // 自动宽度
```

#### Height
- **类型：** `float`
- **默认值：** `NaN` (自动计算)
- **说明：** 元素的固定高度（单位：像素）
- **可绑定：** ✅ 是

```cpp
ui::button()->Height(40.0f);
```

### 约束属性

#### MinWidth / MaxWidth
- **类型：** `float`
- **默认值：** `0.0f` 和 `∞`
- **说明：** 最小/最大宽度约束
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::button()
    ->MinWidth(50.0f)
    ->MaxWidth(300.0f);  // 宽度范围：50-300 像素
```

#### MinHeight / MaxHeight
- **类型：** `float`
- **默认值：** `0.0f` 和 `∞`
- **说明：** 最小/最大高度约束

```cpp
ui::stackPanel()
    ->MinHeight(100.0f)
    ->MaxHeight(500.0f);
```

### 尺寸计算属性

#### DesiredSize
- **类型：** `SizeF` (只读)
- **说明：** 元素在 Measure 阶段计算出的所需尺寸

```cpp
auto size = element->DesiredSize();
printf("所需尺寸：%.1f x %.1f\n", size.width, size.height);
```

#### ActualWidth / ActualHeight
- **类型：** `float` (只读)
- **说明：** 元素在 Arrange 阶段分配到的实际尺寸

```cpp
button->Measure(SizeF{200, 50});
button->Arrange(RectF{{0, 0}, {200, 50}});
printf("实际尺寸：%.1f x %.1f\n", 
       button->ActualWidth(), button->ActualHeight());
```

---

## 对齐与边距

### HorizontalAlignment
- **类型：** `HorizontalAlignment` (枚举)
- **默认值：** `HorizontalAlignment::Stretch`
- **说明：** 元素在父容器中的水平对齐方式
- **可选值：**
  - `HorizontalAlignment::Left` - 左对齐
  - `HorizontalAlignment::Center` - 居中
  - `HorizontalAlignment::Right` - 右对齐
  - `HorizontalAlignment::Stretch` - 拉伸填满宽度
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::button()
    ->Content("居中按钮")
    ->HorizontalAlignment(HorizontalAlignment::Center);
```

### VerticalAlignment
- **类型：** `VerticalAlignment` (枚举)
- **默认值：** `VerticalAlignment::Stretch`
- **说明：** 元素在父容器中的竖直对齐方式
- **可选值：**
  - `VerticalAlignment::Top` - 顶部对齐
  - `VerticalAlignment::Center` - 居中
  - `VerticalAlignment::Bottom` - 底部对齐
  - `VerticalAlignment::Stretch` - 拉伸填满高度
- **可绑定：** ✅ 是

```cpp
ui::button()
    ->VerticalAlignment(VerticalAlignment::Center);
```

### Margin
- **类型：** `Thickness` (结构体)
- **默认值：** `{0, 0, 0, 0}`
- **说明：** 外边距（元素到父容器的距离）
- **可绑定：** ✅ 是

**Thickness 结构：**
```cpp
struct Thickness {
    float left;    // 左边距
    float top;     // 上边距
    float right;   // 右边距
    float bottom;  // 下边距
};
```

**用法：**
```cpp
ui::button()
    ->Margin({10, 5, 10, 5});  // L, T, R, B

ui::button()
    ->Margin({10});  // 四个方向都是 10 像素

ui::button()
    ->Margin({10, 20});  // 左右 10，上下 20
```

---

## 流体布局（Fluid Layout）

### FluidWidth / FluidHeight
- **类型：** `float`
- **默认值：** `NaN` (不使用流体尺寸)
- **说明：** 相对于父容器的百分比宽度/高度 (0.0 - 1.0)
- **可绑定：** ✅ 是

**用法：**
```cpp
ui::stackPanel()
    ->Width(400.0f)  // 父容器宽 400
    ->Children({
        ui::button()
            ->FluidWidth(0.5f),   // 占父容器宽度的 50% = 200 像素
        ui::button()
            ->FluidWidth(0.5f)    // 占父容器宽度的 50% = 200 像素
    });
```

**场景：** 响应式布局，元素宽度需要随父容器改变

---

## 布局计算方法

### Measure(SizeF availableSize)
- **参数：** `availableSize` - 父容器可用的尺寸
- **说明：** 测量阶段，计算元素的所需尺寸
- **触发时机：** 
  - 布局失效时（`InvalidateMeasure()`）
  - 布局系统主动调用

**调用流程：**
```cpp
// 1. 元素调用 Measure，告诉元素有多少空间可用
element->Measure(SizeF{500, 300});

// 2. 元素内部调用 MeasureOverride，计算所需尺寸
// 3. 结果保存在 DesiredSize 中
auto desired = element->DesiredSize();  // 例如：{200, 50}
```

### Arrange(RectF finalRect)
- **参数：** `finalRect` - 分配给元素的最终矩形区域
- **说明：** 排列阶段，设置元素的最终位置和尺寸
- **触发时机：**
  - Measure 之后
  - 布局失效时

**调用流程：**
```cpp
// 1. 元素调用 Arrange，分配最终位置和尺寸
RectF rect{{0, 0}, {200, 50}};  // 位置 (0,0)，尺寸 200x50
element->Arrange(rect);

// 2. 元素内部调用 ArrangeOverride，执行排列逻辑
// 3. ActualWidth/ActualHeight 被更新为实际尺寸
```

### InvalidateMeasure()
- **说明：** 标记元素的测量为失效
- **效果：** 下一帧将重新调用 Measure
- **触发条件：**
  - 元素尺寸约束改变
  - 子元素集合改变

```cpp
element->Width(300.0f);  // 宽度改变
element->InvalidateMeasure();  // 标记重新测量
```

### InvalidateArrange()
- **说明：** 标记元素的排列为失效
- **效果：** 下一帧将重新调用 Arrange

```cpp
element->Margin({10, 10, 10, 10});
element->InvalidateArrange();
```

---

## 布局虚拟函数（重写点）

### SizeF MeasureOverride(SizeF availableSize)
- **说明：** 派生类重写此方法实现自定义测量逻辑
- **返回值：** 元素所需的尺寸
- **约束：** 返回值应该 ≤ `availableSize`

**示例：**
```cpp
class CustomControl : public FrameworkElement {
protected:
    SizeF MeasureOverride(SizeF availableSize) override {
        // 1. 测量子元素
        if (auto child = GetChild()) {
            child->Measure(availableSize);
        }
        
        // 2. 计算所需尺寸
        SizeF desired{100, 50};  // 最少需要 100x50
        
        // 3. 约束到可用空间
        return {
            std::min(desired.width, availableSize.width),
            std::min(desired.height, availableSize.height)
        };
    }
};
```

### SizeF ArrangeOverride(SizeF finalSize)
- **说明：** 派生类重写此方法实现自定义排列逻辑
- **参数：** `finalSize` - 分配的最终尺寸
- **返回值：** 实际使用的尺寸（通常是 `finalSize`）

**示例：**
```cpp
class CustomControl : public FrameworkElement {
protected:
    SizeF ArrangeOverride(SizeF finalSize) override {
        // 1. 排列子元素
        if (auto child = GetChild()) {
            RectF childRect{{0, 0}, finalSize};
            child->Arrange(childRect);
        }
        
        // 2. 返回实际使用的尺寸
        return finalSize;
    }
};
```

---

## 完整使用示例

### 示例 1：固定尺寸和对齐

```cpp
auto layout = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(10.0f)
    ->Width(500.0f)
    ->Height(400.0f)
    ->Children({
        // 标题 - 充满宽度，居中对齐
        ui::textBlock()
            ->Text("应用标题")
            ->FontSize(24.0f)
            ->HorizontalAlignment(HorizontalAlignment::Center),
        
        // 内容区 - 填充剩余空间
        ui::stackPanel()
            ->VerticalAlignment(VerticalAlignment::Stretch),
        
        // 底部按钮 - 固定尺寸，右对齐
        ui::button()
            ->Content("确定")
            ->Width(100.0f)
            ->Height(40.0f)
            ->HorizontalAlignment(HorizontalAlignment::Right)
            ->Margin({0, 10, 10, 10})
    });
```

### 示例 2：约束尺寸

```cpp
auto responsiveControl = ui::stackPanel()
    ->MinWidth(200.0f)   // 最小宽度 200
    ->MaxWidth(800.0f)   // 最大宽度 800
    ->Width(400.0f)      // 首选宽度 400
    ->MinHeight(100.0f)  // 最小高度 100
    ->MaxHeight(600.0f); // 最大高度 600
```

### 示例 3：流体布局（响应式）

```cpp
auto responsiveGrid = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()},
        ColumnDefinition{GridLength::Star()}
    })
    ->Children({
        ui::button()->Content("A") | cell(0, 0),
        ui::button()->Content("B") | cell(0, 1),
        ui::button()->Content("C") | cell(0, 2)
    });

// 当父容器尺寸改变时，Grid 自动调整列宽
```

### 示例 4：边距和对齐

```cpp
auto centeredForm = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Spacing(15.0f)
    ->Width(300.0f)
    ->HorizontalAlignment(HorizontalAlignment::Center)
    ->VerticalAlignment(VerticalAlignment::Center)
    ->Margin({20, 20, 20, 20})
    ->Children({
        ui::textBlock()
            ->Text("登录表单")
            ->FontSize(18.0f)
            ->FontWeight(FontWeight::Bold),
        
        ui::textBox()
            ->Margin({0, 5, 0, 5}),  // 上下间隔
        
        ui::textBox()
            ->Margin({0, 5, 0, 5}),
        
        ui::button()
            ->Content("登录")
            ->Width(100.0f)
            ->HorizontalAlignment(HorizontalAlignment::Center)
            ->Margin({0, 10, 0, 0})
    });
```

---

## 从 UIElement 继承的属性

### Visibility
- **类型：** `Visibility`
- **默认值：** `Visibility::Visible`
- **可选值：** Visible、Hidden、Collapsed

```cpp
element->Visibility(Visibility::Collapsed);  // 隐藏且不占空间
```

### Opacity
- **类型：** `float`
- **默认值：** `1.0f`
- **说明：** 不透明度 (0.0 = 完全透明)

```cpp
element->Opacity(0.5f);  // 50% 透明
```

### IsEnabled
- **类型：** `bool`
- **默认值：** `true`

```cpp
element->IsEnabled(false);  // 禁用元素
```

---

## 常见问题

### Q1：Width/Height 为 NaN 时会怎样？
**A：** 元素会根据内容自动计算尺寸（前提是父容器允许）。例如，Button 的尺寸由其 Content 决定。

### Q2：Width 和 FluidWidth 如何冲突时的优先级？
**A：** 通常 Width（固定值）优先级更高。设置 Width 会覆盖 FluidWidth。

### Q3：Measure 必须在 Arrange 之前吗？
**A：** 是的。正确顺序是：
1. Measure（计算所需尺寸）
2. Arrange（分配最终位置和尺寸）

### Q4：频繁调用 InvalidateMeasure 会影响性能吗？
**A：** 会。频繁失效会导致频繁重新计算布局。应该：
- 批量修改尺寸属性后再一次性失效
- 避免在每帧都改变尺寸

### Q5：如何实现居中布局？
**A：**
```cpp
ui::grid()
    ->Children({
        ui::button()
            ->Content("居中按钮")
            ->HorizontalAlignment(HorizontalAlignment::Center)
            ->VerticalAlignment(VerticalAlignment::Center)
    });
```

---

## 最佳实践

1. **明确指定尺寸：**
   ```cpp
   // ✅ 推荐：明确指定
   ui::button()->Width(100.0f)->Height(40.0f);
   
   // ⚠️ 隐式：依赖内容计算
   ui::button()->Content("Click");
   ```

2. **使用约束而不是硬编码：**
   ```cpp
   // ✅ 推荐：灵活响应不同屏幕
   ui::stackPanel()
       ->MinWidth(200.0f)
       ->MaxWidth(800.0f);
   
   // ❌ 避免：硬编码尺寸
   ui::stackPanel()->Width(500.0f);
   ```

3. **合理使用对齐：**
   ```cpp
   // ✅ 推荐：使用 Alignment 而不是 Margin
   element->VerticalAlignment(VerticalAlignment::Center);
   
   // ⚠️ 脆弱：手动计算 Margin
   element->Margin({0, (parentHeight - elemHeight) / 2, 0, 0});
   ```

---

## 性能建议

- **布局层次深：** 避免过深的嵌套（> 10 层），会导致布局计算成本增加
- **频繁尺寸变化：** 如果元素尺寸频繁改变，考虑使用虚拟化
- **大量元素：** > 1000 个元素时，布局计算成本显著增加

---

## 另见

- [UIElement.md](UIElement.md) - 基础元素
- [Grid.md](Grid.md) - 二维布局
- [StackPanel.md](StackPanel.md) - 线性布局
- [Control.md](Control.md) - 控件基类
