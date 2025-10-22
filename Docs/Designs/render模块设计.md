# fk::render 模块设计

## 1. 模块目标
- 为 UI 框架提供可扩展的渲染后端抽象，首个实现基于 OpenGL + GLFW + FreeType。
- 实现从视觉树到 GPU 绘制命令的转换，支持矢量图形、文本、图像等元素。
- 将渲染与 UI 逻辑解耦，方便替换为 Vulkan/DirectX 等其他后端。

## 2. 体系结构概览
```
Visual Tree (fk::ui)
    │
    ▼
RenderTreeBuilder (fk::render)
    │ 生成 RenderNode
    ▼
RenderScene / RenderList
    │
    ├─ RenderCommandBuffer
    │
    └─ ResourceCache (纹理/字体/几何体)
    ▼
IRenderer → 后端实现（GlRenderer, StubRenderer...）
    │
    ├─ Surface/SwapChain
    ├─ ShaderManager
    ├─ GlyphAtlas
    └─ DrawCall 组装
```

## 3. 核心组件

### 3.1 IRenderer 接口
- 最小职责集：
  ```cpp
  class IRenderer {
  public:
      virtual ~IRenderer() = default;

      virtual void Initialize(const RendererInitParams& params) = 0;
      virtual void Resize(const Size& size) = 0;
      virtual void BeginFrame(const FrameContext& ctx) = 0;
      virtual void Draw(const RenderList& list) = 0;
      virtual void EndFrame() = 0;
      virtual void Shutdown() = 0;
  };
  ```
- `RendererInitParams` 包含窗口句柄、像素比例、调试标志等。
- `FrameContext` 提供帧计时、背景颜色等参数。

### 3.2 RenderTreeBuilder
- 输入：`fk::ui::Visual` 树，遍历生成 `RenderNode`。
- `RenderNode` 类型：
  - `GeometryNode`：包含路径/填充/描边。
  - `TextNode`：字符内容、字体、字号、颜色。
  - `ImageNode`：纹理引用、裁剪信息。
  - `LayerNode`：透明度、混合模式、遮罩。
- 将 `RenderNode` 转为 `RenderCommand`：
  - `SetClip`, `SetTransform`, `DrawPath`, `DrawGlyphRun`, `DrawImage`, `PushLayer`, `PopLayer`。
- 支持批处理：合并相同材质、相同状态的命令。

### 3.3 资源管理
- `ResourceCache`
  - 管理 GPU 资源（纹理、缓冲、着色器），提供引用计数与惰性加载。
  - 按 `ResourceKey` 缓存，避免重复创建。
- `GlyphAtlas`
  - 基于 FreeType 渲染字形到图集纹理。
  - 提供 `GetGlyph(const GlyphKey&)`，返回位置、UV、Advance 等信息。
- `Brush`/`Pen`
  - 渲染层面的画刷 (SolidColor, Gradient) 与画笔。
  - 映射到 GPU uniform/texture。

### 3.4 后端实现（GlRenderer）
- 依赖 glad + OpenGL 核心 Profile。
- 构建顶点缓冲、索引缓冲，使用实例化减少 draw call。
- 着色器：
  - `basic_fill.vert/frag`: 绘制纯色、渐变。
  - `text_render.frag`: 采样 glyph atlas。
  - `image.frag`: 纹理绘制。
- 状态管理：
  - `PipelineState` 结构记录混合模式、深度测试、裁剪。
  - 实现状态缓存，避免重复调用 gl 状态。

### 3.5 平台抽象
- `IRenderSurface`
  - 描述渲染目标，提供 `SwapBuffers`。
  - GLFW 窗口实现 `GlfwRenderSurface`，内部调用 `glfwSwapBuffers`。
- `SwapChain`
  - 对未来多后端的统一抽象，OpenGL 实现可简单委托给 surface。
- `RenderContext`
  - 管理 OpenGL 上下文，提供 `MakeCurrent`, `DoneCurrent`。

### 3.6 与 UI 的协作
- `fk::ui::Window` 在初始化时创建 `RenderHost`：
  - 持有 `IRenderer`、`RenderTreeBuilder`、`RenderScene`。
  - 监听布局/视觉树变化，标记 `DirtyRegion`。
- 帧循环：
  1. UI 层在 `LayoutPass` 后调用 `RenderHost::RequestRender()`。
  2. `RenderHost` 在 `BeginFrame` 中生成新的 `RenderList`。
  3. 调用 `IRenderer::Draw(list)`，提交 GPU 命令。

## 4. 数据结构
- `RenderList`
  - 包含 `std::vector<RenderCommand>`。
  - 记录批次分割点、层级信息。
- `RenderCommand`
  - `{ CommandType type; State state; Geometry geometry; }`
  - `State` 包含变换矩阵、剪裁矩形、brush/pen ID。
- `RenderBatch`
  - 分组 draw call，减少状态切换。

## 5. FreeType 集成
- `FontManager`
  - 加载字体文件（ttf/otf），创建 `Face`。
  - 管理 fallback 字体列表。
- `GlyphRasterizer`
  - 使用 FreeType 渲染 glyph，生成位图。
  - 对字形进行加粗、斜体仿真（可选）。
- `GlyphAtlas`
  - 多级图集（512x512/1024x1024），使用空间分配算法（如 skyline）。
  - 定期 GC：移除最近未使用 glyph。

## 6. 性能优化
- **命令批处理**：合并相同材质/剪裁/变换的绘制命令。
- **脏矩形更新**：只重建/绘制受影响区域。
- **多线程准备**：RenderTreeBuilder 可在 UI 线程生成 `RenderList` 后交给渲染线程提交。
- **纹理复用**：ResourceCache 管理纹理生命周期，避免重复创建。
- **GPU 动态分配**：使用 `glBufferSubData` 或持久映射更新缓冲。

## 7. 调试与工具
- `RenderDebugger`
  - 导出 RenderList JSON 供调试。
  - 可视化脏矩形和命中测试区域。
- `StatsCollector`
  - 统计 draw call 数量、批次数、纹理切换次数。
- 着色器热更新（可选）：监听文件变化并重新编译。

## 8. 与其他模块的依赖
- 依赖 `fk::core`：日志、计时、事件、Dispatcher。
- 依赖 `fk::ui` 提供的 `Visual` 接口及布局结果。
- 不直接依赖 `fk::binding`，但 RenderTreeBuilder 需要通过 UI 元素读取属性（如 Brush）。

## 9. 测试策略
- 单元测试：
  - RenderTreeBuilder：给定 Visual 树，验证生成的 RenderCommand 序列。
  - GlyphAtlas：添加/访问 glyph，验证缓存命中与定位。
- 集成测试：
  - 启动最小窗口，绘制基本形状并截屏对比（可使用像素差异阈值）。
  - 文本渲染正确性（宽度、baseline 对齐）。
- 性能基准：
  - 大量控件、文本、滚动场景，记录帧率与渲染时间。

## 10. 实现路线
1. 定义 `IRenderer`、`RenderCommand`、`RenderList` 等核心接口。
2. 完成 `RenderTreeBuilder`，支持基本几何和文本节点。
3. 实现 `GlRenderer`：
   - OpenGL 上下文封装
   - 简单批处理
   - GlyphAtlas + 文本绘制
4. 与 `fk::ui::Window` 接口对接，建立渲染循环。
5. 添加调试工具与性能统计。
6. 逐步扩展到阴影、渐变、Blur 等高级效果。

---
> 此设计为 render 模块的总体蓝图，具体类图与接口需要与 UI 模块进一步对齐，以确保 Visual 属性能准确转换为渲染命令。