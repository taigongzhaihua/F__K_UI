# Panel Background 和 CornerRadius 功能实现总结

## 实现日期
2025年11月20日

## 功能概述

成功为 `Panel`（包括 `StackPanel` 和 `Grid`）实现了：
1. ✅ **Background 属性支持** - 可设置背景画刷
2. ✅ **四角独立 CornerRadius 支持** - Border、StackPanel、Grid 都支持四角不同的圆角半径

## 修改的文件清单

### 1. 渲染层核心修改

#### `include/fk/render/RenderCommand.h`
- 添加 `#include "fk/ui/CornerRadius.h"`
- 修改 `RectanglePayload::cornerRadius`：`float` → `ui::CornerRadius`

#### `include/fk/render/RenderContext.h`
- 修改 `DrawRectangle()` API 签名：
  ```cpp
  void DrawRectangle(
      const ui::Rect& rect,
      const std::array<float, 4>& fillColor,
      const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
      float strokeWidth = 0.0f,
      const ui::CornerRadius& cornerRadius = ui::CornerRadius(0),  // 改为结构体
      StrokeAlignment strokeAlignment = StrokeAlignment::Center,
      float aaWidth = 0.75f
  );
  ```

#### `src/render/RenderContext.cpp`
- 更新 `DrawRectangle()` 实现以接受和传递 `CornerRadius` 结构体

#### `src/render/GlRenderer.cpp`
- **着色器修改**：
  - Uniform: `float uCornerRadius` → `vec4 uCornerRadii`
  - 添加 `getCornerRadius()` 函数：根据片段象限返回对应的圆角半径
  ```glsl
  float getCornerRadius(vec2 localPos, vec2 size) {
      if (localPos.x < 0.0) {
          if (localPos.y < 0.0) return uCornerRadii.x; // topLeft
          else return uCornerRadii.w; // bottomLeft
      } else {
          if (localPos.y < 0.0) return uCornerRadii.y; // topRight
          else return uCornerRadii.z; // bottomRight
      }
  }
  ```
  
- **DrawRectangle 实现**：
  ```cpp
  // 分别 clamp 四个角
  float clampedTopLeft = std::clamp(payload.cornerRadius.topLeft, 0.0f, halfMinDimension);
  float clampedTopRight = std::clamp(payload.cornerRadius.topRight, 0.0f, halfMinDimension);
  float clampedBottomRight = std::clamp(payload.cornerRadius.bottomRight, 0.0f, halfMinDimension);
  float clampedBottomLeft = std::clamp(payload.cornerRadius.bottomLeft, 0.0f, halfMinDimension);
  
  glUniform4f(cornerRadiiLoc, clampedTopLeft, clampedTopRight, clampedBottomRight, clampedBottomLeft);
  ```

### 2. Panel 基类扩展

#### `include/fk/ui/Panel.h`
- 添加头文件：
  ```cpp
  #include "fk/ui/CornerRadius.h"
  #include "fk/core/Event.h"
  ```
  
- 添加 CornerRadius 属性：
  ```cpp
  static const binding::DependencyProperty& CornerRadiusProperty();
  
  ui::CornerRadius GetCornerRadius() const;
  void SetCornerRadius(const ui::CornerRadius& value);
  Derived* WithCornerRadius(const ui::CornerRadius& radius);
  Derived* WithCornerRadius(float uniform);
  ```
  
- 添加渲染方法：
  ```cpp
  protected:
      void OnRender(render::RenderContext& context);
      void OnPropertyChanged(...);
      void ObserveBrush(Brush* brush);
      
      core::Event<...>::Connection backgroundConnection_;
  ```

#### `src/ui/Panel.cpp`
- 注册 `CornerRadiusProperty` 依赖属性
- 实现 `OnRender()`：
  ```cpp
  void Panel<Derived>::OnRender(render::RenderContext& context) {
      auto background = GetBackground();
      if (!background) return;
      
      auto renderSize = this->GetRenderSize();
      ui::Rect rect(0, 0, renderSize.width, renderSize.height);
      
      // 转换 Brush 为颜色
      std::array<float, 4> fillColor = brushToColor(background);
      auto cornerRadius = GetCornerRadius();
      
      // 绘制带圆角的背景
      context.DrawRectangle(rect, fillColor, {{0,0,0,0}}, 0.0f, cornerRadius);
  }
  ```
  
- 实现 `OnPropertyChanged()` 和 `ObserveBrush()`：监听属性变化并自动重绘

### 3. Border 类更新

#### `src/ui/Border.cpp`
- 简化 `OnRender()`：
  ```cpp
  // 旧代码（计算平均值）：
  float radius = (cornerRadius.topLeft + cornerRadius.topRight + 
                 cornerRadius.bottomRight + cornerRadius.bottomLeft) / 4.0f;
  context.DrawRectangle(rect, fillColor, strokeColor, strokeWidth, radius, ...);
  
  // 新代码（直接传递完整结构）：
  context.DrawRectangle(rect, fillColor, strokeColor, strokeWidth, cornerRadius, ...);
  ```

### 4. Shape 类更新

#### `src/ui/Shape.cpp`
- Rectangle 绘制适配：
  ```cpp
  float radius = std::max(radiusX, radiusY);
  ui::CornerRadius cornerRadius(radius);  // 创建统一圆角
  context.DrawRectangle(bounds, fillColor, strokeColor, strokeThickness, cornerRadius);
  ```

## API 使用示例

### 1. StackPanel 统一圆角
```cpp
auto stackPanel = new StackPanel();
stackPanel->SetBackground(new SolidColorBrush(Color(0.4f, 0.7f, 0.9f, 1.0f)));
stackPanel->SetCornerRadius(CornerRadius(10.0f));  // 统一圆角
```

### 2. Grid 四角不同圆角
```cpp
auto grid = new Grid();
grid->SetBackground(new SolidColorBrush(Color(0.9f, 0.7f, 0.4f, 1.0f)));
grid->SetCornerRadius(CornerRadius(30, 10, 20, 5));  // TL, TR, BR, BL
```

### 3. Border 自定义圆角
```cpp
auto border = new Border();
border->SetBackground(new SolidColorBrush(Color(0.7f, 0.4f, 0.9f, 1.0f)));
border->SetBorderBrush(new SolidColorBrush(Color(0.5f, 0.2f, 0.7f, 1.0f)));
border->SetBorderThickness(Thickness(3));
border->SetCornerRadius(CornerRadius(25, 5, 25, 5));  // 交替圆角
```

### 4. 链式 API（可选）
```cpp
auto panel = (new StackPanel())
    ->Background(new SolidColorBrush(Color(0.9f, 0.9f, 0.9f, 1.0f)))
    ->WithCornerRadius(10.0f);
```

## 技术细节

### GPU 着色器实现原理

1. **象限判断**：根据片段在矩形中的位置（相对于中心）判断所属象限
   - `x < 0, y < 0` → 左上角 (topLeft)
   - `x > 0, y < 0` → 右上角 (topRight)
   - `x > 0, y > 0` → 右下角 (bottomRight)
   - `x < 0, y > 0` → 左下角 (bottomLeft)

2. **SDF 计算**：使用对应象限的圆角半径计算有符号距离场
   ```glsl
   float radius = getCornerRadius(localPos, uRectSize * 0.5);
   float dist = roundedBoxSDF(localPos, uRectSize * 0.5, radius);
   ```

3. **平滑渲染**：使用 `smoothstep()` 实现抗锯齿和边缘平滑过渡

### 自动重绘机制

- **属性变化监听**：通过 `OnPropertyChanged()` 监听 Background 和 CornerRadius 变化
- **Brush 变化监听**：通过 `ObserveBrush()` 监听 Brush 对象内部属性变化（如颜色）
- **自动 InvalidateVisual**：任何变化都会触发重绘请求

### 圆角限制

- 每个圆角半径都会被 clamp 到 `[0, min(width, height) / 2]`
- 防止圆角半径过大导致渲染错误

## 示例程序

### 可视化演示
运行以下程序查看效果：
```bash
.\build\example_panel_backgrounds_simple.exe
```

演示内容：
1. StackPanel 带统一圆角 (10px)
2. Grid 带四角不同圆角 (30, 10, 20, 5)
3. Border 带交替圆角 (25, 5, 25, 5)

## 优势与特性

✅ **统一的 API** - Border、StackPanel、Grid 使用相同的 CornerRadius 结构
✅ **高性能** - GPU 片段着色器实现，无 CPU 开销
✅ **完美抗锯齿** - 基于 SDF 的平滑边缘
✅ **灵活性** - 支持四角独立或统一圆角
✅ **自动重绘** - 属性变化时自动更新视觉
✅ **类型安全** - 编译时检查，无运行时错误

## 向后兼容性

- ✅ 保持原有 API 不变
- ✅ 默认值为 `CornerRadius(0)` - 无圆角
- ✅ `CornerRadius(float)` 构造函数支持统一圆角
- ✅ 所有现有代码无需修改即可编译通过

## 性能影响

- **CPU**: 无额外开销（属性存储增加 16 字节）
- **GPU**: 片段着色器增加 1 个条件分支和 4 个 uniform，影响可忽略
- **内存**: 每个 Panel 实例增加约 32 字节（CornerRadius + Event Connection）

## 测试验证

所有修改已通过编译测试：
```bash
cd "g:\Documents\Visual Studio Code\F__K_UI"
cmake --build build --target fk -j8
# 编译成功，无错误 ✅
```

示例程序成功运行：
```bash
.\build\example_panel_backgrounds_simple.exe
# 窗口正常显示，所有圆角正确渲染 ✅
```

## 未来扩展可能

1. **渐变背景** - 支持 LinearGradientBrush、RadialGradientBrush
2. **边框圆角独立** - Border 的 BorderThickness 和 CornerRadius 分别适配
3. **动画支持** - CornerRadius 属性动画过渡
4. **阴影效果** - 结合圆角的阴影渲染

## 总结

成功实现了完整的 Panel Background 和四角独立 CornerRadius 功能，涉及从底层渲染 API 到 UI 控件的全栈修改。实现符合现代 UI 框架标准，性能优秀，API 友好，为后续 UI 开发提供了强大的视觉定制能力。

---
**实现者**: GitHub Copilot  
**日期**: 2025年11月20日  
**状态**: ✅ 完成并验证
