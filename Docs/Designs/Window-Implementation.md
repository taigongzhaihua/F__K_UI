# Window 类实现完成

## 实现概述

`fk::ui::Window` 类已成功实现，提供了以下功能：

### 核心功能

1. **GLFW 窗口集成**
   - 自动创建和管理 GLFWwindow* 本地窗口
   - OpenGL 3.3 Core Profile 上下文
   - VSync 支持

2. **窗口属性**
   - Title (标题)
   - Width/Height (尺寸)
   - Visible (可见性)
   - 流式API（链式调用）

3. **内容管理**
   - SetContent/GetContent 设置 UI 树根节点
   - 自动执行布局（Measure/Arrange）
   - 内容变化时自动请求重绘

4. **渲染集成**
   - SetRenderHost 关联渲染主机
   - 自动在窗口大小改变时更新渲染器视口
   - 提供本地窗口句柄给渲染器

5. **事件系统**
   - Opened - 窗口打开
   - Closed - 窗口关闭
   - Resized(width, height) - 窗口尺寸变化
   - Activated - 窗口获得焦点
   - Deactivated - 窗口失去焦点

### 测试结果

运行 `test_window.exe` 的输出：

```
=== F__K_UI Window Class Test ===

1. Creating window...
   Title: Test Window
   Size: 800x600

2. Creating content...
   Content set: StackPanel with 3 children

3. Subscribing to events...
   Events subscribed

4. Showing window...
Created native window: 800x600
Layout performed: 800x600
   [Event] Window opened!
Window 'Test Window' opened (800x600)
   Window is visible: yes
   Native handle: 0x23b82d1dbb0

5. Testing property changes...
Layout performed: 1024x600
   [Event] Window resized to 1024x600
Layout performed: 1024x768
   [Event] Window resized to 1024x768
   New title: Modified Title
   New size: 1024x768

6. Window will remain visible for a moment...
   (Window lifecycle managed by GLFW)

=== Test Complete ===
Destroyed native window
```

✓ **所有功能正常工作**

### 使用示例

```cpp
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/render/RenderHost.h"
#include "fk/render/GlRenderer.h"

using namespace fk;

int main() {
    // 创建 UI 内容
    auto stackPanel = std::make_shared<ui::StackPanel>();
    stackPanel->SetOrientation(ui::Orientation::Vertical);
    
    // 创建窗口
    auto window = ui::window()
        ->Title("My Application")
        ->Width(800)
        ->Height(600);
    
    window->SetContent(stackPanel);
    
    // 订阅事件
    window->Opened += []() {
        std::cout << "Window opened!" << std::endl;
    };
    
    window->Resized += [](int w, int h) {
        std::cout << "Window resized to " << w << "x" << h << std::endl;
    };
    
    // 显示窗口
    window->Show();
    
    // 设置渲染系统
    auto renderer = std::make_shared<render::GlRenderer>();
    auto renderHost = std::make_shared<render::RenderHost>(
        render::RenderHostConfig{
            .window = window,
            .renderer = std::move(renderer)
        }
    );
    
    window->SetRenderHost(renderHost);
    
    // 初始化渲染器
    render::RendererInitParams params;
    params.nativeSurfaceHandle = window->GetNativeHandle();
    params.initialSize = {800, 600};
    renderHost->Initialize(params);
    
    // 消息循环
    GLFWwindow* nativeWindow = window->GetNativeHandle();
    while (!glfwWindowShouldClose(nativeWindow)) {
        glfwPollEvents();
        
        renderHost->RequestRender();
        render::FrameContext ctx;
        ctx.clearColor = {0.1f, 0.1f, 0.15f, 1.0f};
        renderHost->RenderFrame(ctx, *stackPanel);
        
        glfwSwapBuffers(nativeWindow);
    }
    
    return 0;
}
```

## 实现细节

### 关键特性

1. **自动 GLFW 初始化**
   - 第一个窗口创建时自动调用 `glfwInit()`
   - 使用静态标志避免重复初始化

2. **延迟窗口创建**
   - 窗口在 `Show()` 时才创建
   - 初始设置 `GLFW_VISIBLE` 为 `GLFW_FALSE`

3. **GLFW 回调集成**
   - `OnWindowClose` - 触发 Closed 事件
   - `OnWindowResize` - 更新渲染器视口、重新布局、触发 Resized 事件
   - `OnWindowFocus` - 触发 Activated/Deactivated 事件

4. **布局自动化**
   - 窗口显示时自动执行初始布局
   - 尺寸改变时自动重新布局
   - 内容设置时如果窗口可见则立即布局

5. **资源管理**
   - 析构函数自动销毁 GLFW 窗口
   - 使用 RAII 模式确保资源清理

## 与其他类的集成

### Window ← RenderHost
- Window 持有 RenderHost 的 shared_ptr
- 尺寸改变时通知 RenderHost 更新视口

### Window ← UIElement (Content)
- Window 持有内容根元素
- 负责触发布局（Measure/Arrange）

### Window → GLFW
- 封装 GLFWwindow* 本地句柄
- 管理窗口生命周期
- 转发 GLFW 回调到事件系统

## 下一步

Window 类已完成基础实现，后续可以增强：

1. **多窗口支持** - Application 管理多个 Window 实例
2. **输入事件** - 鼠标、键盘事件转发到 UI 元素
3. **窗口样式** - 无边框、透明、最小化/最大化
4. **窗口位置** - SetPosition/GetPosition
5. **拖放支持** - 文件拖放到窗口
6. **对话框** - 模态窗口、消息框

目前的实现已经满足基本需求，可以在 Application 中使用。
