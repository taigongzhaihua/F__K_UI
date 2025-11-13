# Renderer (UI)

## 概述

`Renderer` 是 UI 模块中的渲染器类，作为渲染后端的桥接器，负责协调 UI 元素和底层渲染系统之间的交互。它管理渲染命令的生成和执行。

**注意**：这是 UI 模块的 `Renderer` 类。渲染系统的主渲染器请参阅 [Render::Renderer](../Render/Renderer.md)。

## 命名空间

```cpp
namespace fk::ui
```

## 主要功能

### 1. 渲染管理

管理 UI 渲染流程：

```cpp
class Renderer {
public:
    // 渲染 UI 元素树
    void Render(const std::shared_ptr<Visual>& root);
    
    // 渲染单个元素
    void RenderElement(const std::shared_ptr<UIElement>& element);
    
    // 清空渲染缓冲
    void Clear();
};
```

**使用示例：**

```cpp
auto renderer = std::make_shared<Renderer>();

// 渲染整个 UI 树
auto window = std::make_shared<Window>();
// ... 构建 UI ...
renderer->Render(window);

// 渲染单个元素
auto button = std::make_shared<Button>();
renderer->RenderElement(button);
```

### 2. 渲染后端集成

与渲染后端交互：

```cpp
// 设置渲染后端
void SetRenderBackend(const std::shared_ptr<RenderBackend>& backend);

// 获取渲染后端
std::shared_ptr<RenderBackend> GetRenderBackend() const;

// 提交渲染命令到后端
void Submit();
```

**使用示例：**

```cpp
auto renderer = std::make_shared<Renderer>();

// 创建并设置 OpenGL 后端
auto glBackend = std::make_shared<GlRenderBackend>();
renderer->SetRenderBackend(glBackend);

// 渲染并提交
renderer->Render(root);
renderer->Submit();  // 将命令发送到 GPU
```

### 3. 渲染上下文

管理渲染上下文：

```cpp
// 获取当前渲染上下文
RenderContext& GetContext();

// 推入新的渲染状态
void PushState();

// 弹出渲染状态
void PopState();
```

**使用示例：**

```cpp
auto renderer = std::make_shared<Renderer>();

// 渲染带变换的元素
renderer->PushState();
auto& context = renderer->GetContext();
context.Transform = element->GetTransform();
context.Opacity = element->GetOpacity();

renderer->RenderElement(element);

renderer->PopState();
```

### 4. 命令构建

构建渲染命令：

```cpp
// 添加绘制命令
void AddDrawCommand(const DrawCommand& command);

// 批量添加命令
void AddDrawCommands(const std::vector<DrawCommand>& commands);

// 获取命令缓冲区
const std::vector<DrawCommand>& GetCommandBuffer() const;
```

**使用示例：**

```cpp
auto renderer = std::make_shared<Renderer>();

// 手动创建绘制命令
DrawCommand cmd;
cmd.Type = DrawCommandType::Rectangle;
cmd.Bounds = Rect(10, 10, 100, 50);
cmd.Brush = std::make_shared<SolidColorBrush>(Colors::Blue);

renderer->AddDrawCommand(cmd);
```

### 5. 视口管理

管理渲染视口：

```cpp
// 设置视口
void SetViewport(const Rect& viewport);

// 获取视口
Rect GetViewport() const;

// 设置裁剪区域
void SetClipRect(const Rect& clip);
```

**使用示例：**

```cpp
auto renderer = std::make_shared<Renderer>();

// 设置渲染视口（通常是窗口大小）
renderer->SetViewport(Rect(0, 0, 800, 600));

// 设置裁剪区域（用于滚动视图等）
renderer->SetClipRect(Rect(10, 10, 400, 300));

renderer->Render(root);
```

## 渲染流程

### 基本渲染循环

```cpp
class Application {
    void RenderLoop() {
        auto renderer = std::make_shared<Renderer>();
        auto backend = std::make_shared<GlRenderBackend>();
        renderer->SetRenderBackend(backend);
        
        while (running_) {
            // 1. 处理输入
            ProcessInput();
            
            // 2. 更新逻辑
            Update(deltaTime);
            
            // 3. 渲染
            renderer->Clear();
            renderer->Render(rootVisual_);
            renderer->Submit();
            
            // 4. 交换缓冲区
            backend->SwapBuffers();
        }
    }
};
```

### 分层渲染

```cpp
void RenderWithLayers(const std::shared_ptr<Renderer>& renderer,
                     const std::shared_ptr<Visual>& root) {
    // 渲染背景层
    renderer->PushState();
    RenderBackgroundLayer(renderer, root);
    renderer->PopState();
    
    // 渲染内容层
    renderer->PushState();
    RenderContentLayer(renderer, root);
    renderer->PopState();
    
    // 渲染装饰层
    renderer->PushState();
    RenderDecorationLayer(renderer, root);
    renderer->PopState();
    
    // 提交所有命令
    renderer->Submit();
}
```

## 完整示例

### 示例 1：基本渲染设置

```cpp
#include <fk/ui/Renderer.h>
#include <fk/render/GlRenderer.h>
#include <fk/ui/Window.h>

class MyApplication : public Application {
private:
    std::shared_ptr<Renderer> renderer_;
    std::shared_ptr<Window> mainWindow_;
    
public:
    void Initialize() {
        // 创建渲染器
        renderer_ = std::make_shared<Renderer>();
        
        // 设置渲染后端
        auto glBackend = std::make_shared<GlRenderBackend>();
        glBackend->Initialize();
        renderer_->SetRenderBackend(glBackend);
        
        // 创建主窗口
        mainWindow_ = std::make_shared<Window>();
        mainWindow_->SetTitle("我的应用");
        mainWindow_->SetSize(Size(800, 600));
        
        // 构建 UI
        BuildUI();
    }
    
    void Render() {
        // 清空
        renderer_->Clear();
        
        // 设置视口
        auto size = mainWindow_->GetSize();
        renderer_->SetViewport(Rect(0, 0, size.Width, size.Height));
        
        // 渲染
        renderer_->Render(mainWindow_);
        
        // 提交
        renderer_->Submit();
    }
};
```

### 示例 2：自定义渲染

```cpp
class CustomRenderer : public Renderer {
public:
    // 重写渲染方法以添加自定义效果
    void RenderElement(const std::shared_ptr<UIElement>& element) override {
        // 应用自定义着色器
        if (auto control = std::dynamic_pointer_cast<Control>(element)) {
            ApplyCustomShader(control);
        }
        
        // 调用基类渲染
        Renderer::RenderElement(element);
        
        // 后处理效果
        ApplyPostProcessing(element);
    }
    
private:
    void ApplyCustomShader(const std::shared_ptr<Control>& control) {
        // 设置自定义着色器参数
        auto& context = GetContext();
        context.Shader = customShader_;
        context.ShaderParams["glow"] = control->GetGlowIntensity();
    }
    
    void ApplyPostProcessing(const std::shared_ptr<UIElement>& element) {
        // 应用后处理效果，如模糊、阴影等
        if (element->GetDropShadow()) {
            AddShadowEffect(element);
        }
    }
};
```

### 示例 3：性能优化渲染

```cpp
class OptimizedRenderer : public Renderer {
private:
    std::unordered_map<UIElement*, DrawCommand> commandCache_;
    
public:
    void RenderElement(const std::shared_ptr<UIElement>& element) override {
        // 检查缓存
        auto it = commandCache_.find(element.get());
        if (it != commandCache_.end() && !element->IsDirty()) {
            // 使用缓存的命令
            AddDrawCommand(it->second);
            return;
        }
        
        // 生成新命令
        auto commands = GenerateCommands(element);
        
        // 缓存命令
        if (!commands.empty()) {
            commandCache_[element.get()] = commands[0];
        }
        
        // 添加到渲染队列
        AddDrawCommands(commands);
    }
    
    void InvalidateCache(UIElement* element) {
        commandCache_.erase(element);
    }
};
```

## 性能考虑

1. **命令批处理**：合并相似的绘制命令以减少状态切换
2. **脏矩形**：只重绘变化的区域
3. **命令缓存**：缓存静态元素的渲染命令
4. **GPU 卸载**：尽可能将工作转移到 GPU

## 最佳实践

1. **状态管理**：使用 `PushState`/`PopState` 保持渲染状态一致
2. **资源管理**：及时释放不再使用的渲染资源
3. **错误处理**：处理渲染失败的情况
4. **性能监控**：跟踪渲染时间和命令数量

## 与渲染系统的关系

```
UI::Renderer (本类)
  └─ 生成渲染命令
      └─ RenderBackend
          └─ Render::Renderer
              └─ GPU 执行
```

## 相关类

- `RenderBackend` - 渲染后端接口
- `DrawCommand` - 绘制命令
- `RenderContext` - 渲染上下文
- [Render::Renderer](../Render/Renderer.md) - 底层渲染器

## 另请参阅

- [Visual](Visual.md) - 视觉树基类
- [UIElement](UIElement.md) - UI 元素基类
- [RenderBackend](RenderBackend.md) - 渲染后端
- [Render::Renderer](../Render/Renderer.md) - 渲染系统
