# fk::render 模块类设计

## 1. 总览

本文件在《render模块设计.md》总体蓝图基础上，列出核心类的职责、协作关系以及与其他模块的接口约定，作为后续实现的类图指南。

```
fk::ui::Visual Tree
        │
        ▼
RenderTreeBuilder ──► RenderScene ──► RenderCommandBuffer ──► IRenderer
        │                                 │                        │
        └─ ResourceProvider ◄─────────────┴─ ResourceCache ◄───────┘
```

## 2. 渲染入口

### 2.1 RenderHost
- 位置：`fk::render::RenderHost`
- 依赖：`fk::ui::Window`、`RenderTreeBuilder`、`IRenderer`
- 主要职责：
  - 维护渲染循环状态（是否需要重绘、帧计数、背景色）。
  - 持有 `RenderTreeBuilder` 与 `IRenderer`，协调两者之间的帧提交流程。
  - 监听窗口大小变化，调用 `IRenderer::Resize`。
  - 对外暴露 `RequestRender()`、`RenderFrame(const FrameContext&)`。
- 关键属性：
  - `std::unique_ptr<RenderTreeBuilder> treeBuilder_`
  - `std::unique_ptr<IRenderer> renderer_`
  - `FrameStats stats_`

### 2.2 FrameStats
- 记录每帧耗时、draw call 数量、脏矩形数量。
- 被 `RenderHost` 更新、对外提供只读访问。

## 3. 命令管线

### 3.1 RenderTreeBuilder
- 输入：`fk::ui::Visual` 根节点、布局结果。
- 输出：`RenderScene`（包含 `RenderCommandBuffer` 和资源引用）。
- 方法：
  - `void Rebuild(const ui::Visual& visualRoot, RenderScene& scene)`。
  - 内部递归遍历 visual tree，生成 `RenderNode`，并下推变换/剪裁栈。
  - 将 `RenderNode` 转换为 `RenderCommand`，写入 `RenderCommandBuffer`。
- 依赖：
  - `BrushResolver`：解析 UI Brush 为渲染层资源。
  - `GeometryBuilder`：把 UI 形状转换成顶点/索引缓冲描述。

### 3.2 RenderScene
- 作为一次帧构建的完整结果。
- 成员：
  - `RenderCommandBuffer commandBuffer_`
  - `ResourceSet resourceSet_`（记录本帧引用的纹理、着色器等）
  - `RectF dirtyRegion_`
- 提供 `Reset()`、`Commit()`。

### 3.3 RenderCommandBuffer
- 仅负责顺序存储命令，不关心执行。
- 核心类型：
  ```cpp
  struct RenderCommand {
      CommandType type;
      PipelineState state;
      CommandPayload payload; // std::variant<PathPayload, TextPayload, ImagePayload, ...>
  };
  ```
- 支持批处理：内部维护 `BeginBatch/EndBatch` API，允许合并同类型命令。

### 3.4 PipelineState
- 表示 GPU 状态配置：混合模式、深度/模板、剪裁矩形、变换矩阵、brush/pen 引用。
- 可哈希，方便用于批处理和状态缓存。

## 4. 资源与缓存

### 4.1 ResourceCache
- 模板类：`ResourceCache<ResourceKey, Resource>`。
- 功能：引用计数、生命周期管理、懒加载。
- 对应子类：
  - `TextureCache`
  - `ShaderCache`
  - `GeometryCache`
  - `FontCache`
- 线程安全策略：主线程读写、渲染线程只读；必要时通过 `core::Dispatcher` 同步。

### 4.2 GlyphAtlas
- 负责字体字形到纹理图集的排布与查询。
- 方法：
  - `GlyphDescriptor AllocateGlyph(FontFace&, GlyphIndex)`
  - `void UploadPending(IRenderer&)`
- 依赖 `FontManager` 获取 FreeType glyph 位图。

### 4.3 FontManager / GlyphRasterizer
- `FontManager`：
  - 管理字体资源，提供 `FontFaceHandle LoadFont(std::string path)`。
  - 管理 fallback 链。
- `GlyphRasterizer`：
  - 对 `FontFaceHandle` 和字号进行 glyph 渲染，返回灰度/SDF 位图。

## 5. 渲染器抽象

### 5.1 IRenderer
- 关键接口（见设计文档 3.1）。
- 额外提供：
  - `IRenderSurface& Surface()` 返回渲染目标。
  - `ResourceUploader& Uploader()` 提供 GPU 资源上传入口。

### 5.2 RenderSurface & SwapChain
- `IRenderSurface`
  - `void Present()`
  - `Size PixelSize() const`
- `GlfwRenderSurface` 实现：持有 `GLFWwindow*`，封装 `glfwSwapBuffers`。
- `SwapChain`
  - 对多后端的统一接口，OpenGL 实现可直通 surface。

### 5.3 GlRenderer
- 继承 `IRenderer`，OpenGL 实现。
- 成员：
  - `GLContext context_`
  - `ShaderManager shaderManager_`
  - `BufferPool vertexBufferPool_`
  - `StateCache stateCache_`
- 关键流程：
  - `BeginFrame`：清空状态、绑定默认帧缓冲。
  - `Draw`：遍历 `RenderCommandBuffer`，转换为 gl draw call。
  - `EndFrame`：提交 buffer，调用 `Surface().Present()`。

## 6. RenderCommand 族

### 6.1 CommandType 枚举
- `SetClip`
- `SetTransform`
- `DrawGeometry`
- `DrawGlyphRun`
- `DrawImage`
- `PushLayer`
- `PopLayer`

### 6.2 Payload 结构
- `GeometryPayload`：`GeometryHandle`, `Pen`, `Brush`
- `GlyphRunPayload`：`GlyphRunHandle`, `Color`
- `ImagePayload`：`TextureHandle`, `RectF dst`, `RectF src`
- `LayerPayload`：`float opacity`, `BlendMode`

## 7. 辅助组件

### 7.1 BrushResolver
- 把 UI 层的 Brush/Pen 映射到渲染层 `BrushResource`。
- 支持 solid color、线性渐变、图片填充。

### 7.2 GeometryBuilder
- 将 UI 几何（矩形、圆角、路径）转换为渲染器可消费的顶点格式。
- 输出 `GeometryHandle`，供 `RenderCommand` 引用。

### 7.3 RenderDebugger
- 提供：
  - `DumpRenderList(const RenderCommandBuffer&, std::ostream&)`
  - `Visualize(FrameStats, DebugFlags)`

### 7.4 StatsCollector
- 被 `RenderHost` 和 `IRenderer` 调用，用于记录：draw call 数、提交命令数、上传纹理大小等。
- 支持通过 `core::Event` 发布统计数据。

## 8. UI 协作接口

### 8.1 VisualAdapter
- 如果 UI 层未定义 `Visual`，可用适配器解耦。
- 提供 `GetChildCount`、`GetChildAt`、`GetLayoutSlot` 等抽象访问接口。

### 8.2 DirtyRegionTracker
- UI 层在布局或属性变更时调用 `MarkDirty(RectF)`。
- `RenderHost` 在 `RenderFrame` 时读取脏区，决定是否完全重建或局部更新。

## 9. 线程与同步

- 默认单线程：UI 线程负责构建、渲染；后续可扩展为双线程（UI 构建、渲染线程提交）。
- 若启用双线程：
  - `RenderScene` 需可跨线程拥有者。
  - `ResourceUploader` 负责同步 GPU 上传（通过 command queue）。

## 10. 未来扩展点

- **多后端**：实现 `VkRenderer` / `DxRenderer`；`IRenderer` 接口保持稳定。
- **特效系统**：引入 `EffectNode`、`EffectProcessor`。
- **层级缓存**：支持 `RenderLayerCache`，在 UI 层或渲染层缓存部分视觉树。
- **GPU 纹理资源热更新**：针对图像/着色器在调试时的热替换。

---
以上类设计覆盖 render 模块主要职责及其与 UI 的协作方式，建议在实现前补充 UML 类图，以确保模块间接口清晰。