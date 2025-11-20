# Panel背景和圆角功能实现说明

## 概述

本次更新为 `StackPanel`、`Grid` 和 `Border` 控件添加了背景渲染和四个独立圆角的支持，使UI设计更加灵活和美观。

## 新增功能

### 1. 背景渲染
- **StackPanel** 和 **Grid** 现在可以显示Background属性
- 之前这些Panel的Background属性虽然存在但不会渲染
- 现在可以为布局容器设置彩色背景

### 2. 独立圆角支持
所有Panel和Border现在支持四个角独立设置圆角半径：
- **左上角** (TopLeft)
- **右上角** (TopRight)  
- **右下角** (BottomRight)
- **左下角** (BottomLeft)

## API使用方法

### 设置统一圆角

```cpp
auto panel = new StackPanel();
panel->Background(new SolidColorBrush(Color{0.2f, 0.6f, 0.9f, 1.0f}));
panel->CornerRadius(20.0f);  // 所有角都是20px圆角
```

### 设置独立圆角

```cpp
auto grid = new Grid();
grid->Background(new SolidColorBrush(Color{0.3f, 0.8f, 0.4f, 1.0f}));
// 参数顺序：左上, 右上, 右下, 左下
grid->CornerRadius(5.0f, 20.0f, 35.0f, 50.0f);
```

### Border 示例

```cpp
auto border = new Border();
border->Background(new SolidColorBrush(Color{1.0f, 0.7f, 0.2f, 1.0f}));
border->BorderBrush(new SolidColorBrush(Color{0.8f, 0.4f, 0.1f, 1.0f}));
border->BorderThickness(3.0f);
border->CornerRadius(10.0f, 30.0f, 50.0f, 20.0f);
```

## 技术实现

### 渲染层改进

1. **RectanglePayload结构** - 从单个圆角值扩展为四个独立字段：
   ```cpp
   float cornerRadiusTopLeft{0.0f};
   float cornerRadiusTopRight{0.0f};
   float cornerRadiusBottomRight{0.0f};
   float cornerRadiusBottomLeft{0.0f};
   ```

2. **片段着色器改进** - 根据像素所在象限选择对应的圆角半径：
   ```glsl
   float r;
   if (p.x > 0.0) {
       if (p.y > 0.0) {
           r = radius.z; // 右下
       } else {
           r = radius.y; // 右上
       }
   } else {
       if (p.y > 0.0) {
           r = radius.w; // 左下
       } else {
           r = radius.x; // 左上
       }
   }
   ```

### Panel基类扩展

在 `Panel<Derived>` 模板类中添加：
- `CornerRadiusProperty()` - 依赖属性
- `GetCornerRadius()` / `SetCornerRadius()` - 属性访问器
- `CornerRadius(...)` - 流式API链式调用方法

### 控件OnRender实现

StackPanel 和 Grid 都实现了 `OnRender` 方法：
```cpp
void StackPanel::OnRender(render::RenderContext& context) {
    auto background = GetBackground();
    if (!background) return;
    
    auto renderSize = GetRenderSize();
    Rect rect(0, 0, renderSize.width, renderSize.height);
    auto cornerRadius = GetCornerRadius();
    
    std::array<float, 4> fillColor = brushToColor(background);
    context.DrawRectangle(rect, fillColor, {0,0,0,0}, 0.0f,
                         cornerRadius.topLeft, cornerRadius.topRight,
                         cornerRadius.bottomRight, cornerRadius.bottomLeft);
}
```

## 测试程序

### 单元测试
`test_panel_background_corners.cpp` - 验证属性设置的正确性

运行测试：
```bash
cd build
./test_panel_background_corners
```

### 可视化演示
`demo_panel_backgrounds.cpp` - 展示6个不同的圆角配置示例

运行演示（需要OpenGL支持）：
```bash
cd build
./demo_panel_backgrounds
```

## 示例效果

1. **统一圆角** - 适合常规UI元素
2. **波浪圆角** - 创造动感效果 (大小交替)
3. **递增圆角** - 创建渐变视觉效果
4. **混合圆角** - 实现独特的设计需求
5. **无圆角** - 保持传统矩形外观

## 兼容性

- ✅ 完全向后兼容
- ✅ 默认圆角为0（保持原有行为）
- ✅ 可选择性使用新功能
- ✅ 不影响现有代码

## 文件改动列表

### 头文件
- `include/fk/render/RenderCommand.h`
- `include/fk/render/RenderContext.h`
- `include/fk/ui/Panel.h`
- `include/fk/ui/StackPanel.h`
- `include/fk/ui/Grid.h`

### 实现文件
- `src/render/RenderContext.cpp`
- `src/render/GlRenderer.cpp`
- `src/render/RenderList.cpp`
- `src/ui/Panel.cpp`
- `src/ui/StackPanel.cpp`
- `src/ui/Grid.cpp`
- `src/ui/Border.cpp`

### 测试文件
- `test_panel_background_corners.cpp` (新增)
- `demo_panel_backgrounds.cpp` (新增)

## 总结

此次更新显著增强了UI框架的视觉表现能力，使开发者能够创建更加美观和现代的用户界面。四个独立圆角的支持为设计师提供了前所未有的灵活性，可以实现各种创意设计效果。
