# 窗口调整大小时视口更新修复

## 问题描述

用户报告：拉伸窗口时，视口大小会更新，但似乎没有将新的视口大小传递到坐标变换类中，导致文字会被拉伸。

## 根本原因

在 `Window::RenderFrame()` 中，当窗口尺寸改变时：

1. ✅ 调用 `glfwGetFramebufferSize()` 获取新的窗口尺寸
2. ✅ 调用 `glViewport()` 设置 OpenGL 视口
3. ✅ 使用新尺寸进行布局计算
4. ❌ **但没有调用 `renderer_->Resize()` 更新渲染器的视口尺寸**

这导致渲染器内部的 `viewportSize_` 变量保持旧值，而着色器的 `uViewport` uniform 使用这个旧值进行坐标变换，导致渲染变形。

## 问题影响

### 着色器坐标变换

**顶点着色器（vertexShaderSource）：**
```glsl
uniform vec2 uViewport;

void main() {
    vec2 pos = aPos + uOffset;
    // 转换到 NDC (-1 到 1)
    vec2 ndc = (pos / uViewport) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    // ...
}
```

**文本着色器（textVertexShaderSource）：**
```glsl
uniform vec2 uViewport;

void main() {
    vec2 pos = vertex.xy + uOffset;
    vec2 ndc = (pos / uViewport) * 2.0 - 1.0;
    ndc.y = -ndc.y;
    gl_Position = vec4(ndc, 0.0, 1.0);
    // ...
}
```

### 问题场景

```
初始窗口: 800x600
  renderer_->viewportSize_ = (800, 600) ✅
  uViewport uniform = (800, 600) ✅
  渲染正常 ✅

用户调整窗口: 1024x768
  Window 获取新尺寸: 1024x768 ✅
  glViewport(0, 0, 1024, 768) ✅
  布局使用新尺寸 ✅
  但 renderer_->viewportSize_ 仍然是 (800, 600) ❌
  uViewport uniform 仍然是 (800, 600) ❌
  
结果:
  - 顶点位置计算错误
  - 文字和图形被拉伸/压缩
  - 显示变形 ❌
```

## 实施的修复

### 修改文件：`src/ui/Window.cpp`

在 `Window::RenderFrame()` 中，初始化渲染器后立即更新视口大小：

```cpp
void Window::RenderFrame() {
    // ... 获取窗口尺寸 ...
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    if (renderer_ && renderList_) {
        // 初始化渲染器（如果还没初始化）
        if (!renderer_->IsInitialized()) {
            render::RendererInitParams params;
            params.initialSize.width = static_cast<std::uint32_t>(width);
            params.initialSize.height = static_cast<std::uint32_t>(height);
            renderer_->Initialize(params);
        }
        
        // 【新增】更新渲染器视口大小（处理窗口调整）
        render::Extent2D newSize{
            static_cast<std::uint32_t>(width),
            static_cast<std::uint32_t>(height)
        };
        renderer_->Resize(newSize);
        
        // ... 继续渲染 ...
    }
}
```

## 修复效果

### GlRenderer::Resize() 的作用

```cpp
void GlRenderer::Resize(const Extent2D& size) {
    viewportSize_ = size;  // 更新内部视口尺寸
    glViewport(0, 0, size.width, size.height);  // 更新 OpenGL 视口
}
```

### GlRenderer::BeginFrame() 使用更新后的视口

```cpp
void GlRenderer::BeginFrame(const FrameContext& ctx) {
    // ... 其他初始化 ...
    
    // 设置视口 uniform（使用更新后的 viewportSize_）
    int viewportLoc = glGetUniformLocation(shaderProgram_, "uViewport");
    glUniform2f(viewportLoc, 
        static_cast<float>(viewportSize_.width),   // ✅ 使用新尺寸
        static_cast<float>(viewportSize_.height)); // ✅ 使用新尺寸
}
```

### 修复后的流程

```
用户调整窗口: 1024x768
  Window 获取新尺寸: 1024x768 ✅
  glViewport(0, 0, 1024, 768) ✅
  renderer_->Resize({1024, 768}) ✅ 【新增】
    → viewportSize_ = (1024, 768) ✅
  布局使用新尺寸 ✅
  BeginFrame() 设置 uViewport = (1024, 768) ✅
  
结果:
  - 顶点位置计算正确 ✅
  - 文字和图形比例正确 ✅
  - 显示正常 ✅
```

## 技术细节

### 坐标变换流程

**1. 布局坐标（Layout Coordinates）**
```
元素在布局系统中的位置和尺寸
例如：Text1.layoutRect = (10, 20, 760, 38.4)
```

**2. 世界坐标（World Coordinates）**
```
应用变换后的坐标
vertex.xy = layoutRect.xy + offset
例如：pos = (10, 20) + (0, 0) = (10, 20)
```

**3. 归一化设备坐标（NDC）**
```
转换到 [-1, 1] 范围
ndc = (pos / viewport) * 2.0 - 1.0
例如：
  旧视口 (800, 600): ndc.x = (10 / 800) * 2 - 1 = -0.975
  新视口 (1024, 768): ndc.x = (10 / 1024) * 2 - 1 = -0.9805
```

**4. 屏幕坐标（Screen Coordinates）**
```
OpenGL 将 NDC 映射到屏幕
screenX = (ndc.x + 1) * 0.5 * screenWidth
```

### 为什么需要每帧调用 Resize()？

虽然 `Resize()` 内部也调用 `glViewport()`，但：

1. **更新内部状态**：`viewportSize_` 必须与实际窗口尺寸同步
2. **着色器 uniform**：`uViewport` 在 `BeginFrame()` 中设置，依赖 `viewportSize_`
3. **每帧检查**：窗口尺寸可能随时改变，需要每帧确保同步

## 测试

### 测试场景

1. **初始窗口**：800x600
   - 文字渲染位置正确 ✅
   - 布局正确 ✅

2. **拉伸窗口**：1024x768
   - 文字不被拉伸 ✅
   - 保持正确的宽高比 ✅
   - 坐标变换正确 ✅

3. **缩小窗口**：640x480
   - 文字不被压缩 ✅
   - 布局自适应 ✅
   - 坐标变换正确 ✅

## 相关代码

### GlRenderer 的视口管理

- `Initialize()`: 初始化视口尺寸
- `Resize()`: 更新视口尺寸
- `BeginFrame()`: 使用视口尺寸设置着色器 uniform

### 着色器 uniform

- `uViewport`: 视口尺寸，用于坐标变换
- `uOffset`: 变换偏移，用于布局定位

## 总结

**问题**：窗口调整大小时，渲染器的视口尺寸没有更新，导致坐标变换使用旧的视口尺寸，造成渲染变形。

**修复**：在每帧渲染前调用 `renderer_->Resize()` 更新视口尺寸。

**效果**：窗口调整大小时，文字和图形保持正确的比例，不会被拉伸或压缩。

---

**修改的文件：**
- `src/ui/Window.cpp` - 在 RenderFrame() 中添加 Resize() 调用
