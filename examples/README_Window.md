# fk::ui::Window 使用示例

这是一个使用 `fk::ui::Window` 类的完整示例程序。

## 示例代码 (examples/main.cpp)

```cpp
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/render/RenderHost.h"
#include "fk/render/GlRenderer.h"
#include <GLFW/glfw3.h>

using namespace fk;

int main() {
    // 1. 创建 UI 树
    auto rootPanel = std::make_shared<ui::StackPanel>();
    rootPanel->SetOrientation(ui::Orientation::Vertical);
    
    for (int i = 0; i < 3; ++i) {
        auto child = std::make_shared<ui::ContentControl>();
        rootPanel->AddChild(child);
    }

    // 2. 创建渲染器和渲染主机
    auto renderer = std::make_unique<render::GlRenderer>();
    render::RenderHostConfig config;
    config.renderer = std::move(renderer);
    auto renderHost = std::make_shared<render::RenderHost>(std::move(config));

    // 3. 创建窗口（使用流式 API）
    auto window = ui::window()
        ->Title("F__K_UI Demo")
        ->Width(800)
        ->Height(600);
    
    // 4. 设置窗口内容和渲染系统
    window->SetContent(rootPanel);
    window->SetRenderHost(renderHost);

    // 5. 订阅窗口事件
    window->Opened += []() {
        std::cout << "Window opened!" << std::endl;
    };
    
    window->Closed += []() {
        std::cout << "Window closed!" << std::endl;
    };
    
    window->Resized += [](int w, int h) {
        std::cout << "Window resized to " << w << "x" << h << std::endl;
    };

    // 6. 显示窗口
    window->Show();

    // 7. 初始化渲染器（需要 OpenGL 上下文）
    render::RendererInitParams initParams;
    initParams.nativeSurfaceHandle = window->GetNativeHandle();
    initParams.initialSize = {800, 600};
    initParams.pixelRatio = 1.0f;
    initParams.enableDebugLayer = true;
    initParams.rendererName = "GlRenderer";
    
    renderHost->Initialize(initParams);

    // 8. 主循环
    GLFWwindow* nativeWindow = window->GetNativeHandle();
    int frameCount = 0;
    
    while (!glfwWindowShouldClose(nativeWindow)) {
        glfwPollEvents();

        // ESC 退出
        if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window->Close();
        }

        // 渲染
        renderHost->RequestRender();
        
        render::FrameContext frameCtx;
        frameCtx.elapsedSeconds = glfwGetTime();
        frameCtx.deltaSeconds = 0.016;
        frameCtx.clearColor = {0.15f, 0.2f, 0.3f, 1.0f};
        frameCtx.frameIndex = frameCount++;
        
        renderHost->RenderFrame(frameCtx, *rootPanel);
        glfwSwapBuffers(nativeWindow);
    }

    // 9. 清理
    renderHost->Shutdown();
    return 0;
}
```

## 关键特性

### 1. 流式 API
```cpp
auto window = ui::window()
    ->Title("My App")
    ->Width(1024)
    ->Height(768);
```

### 2. Getter/Setter 重载
```cpp
window->Title("New Title");  // 设置标题，返回 Ptr 支持链式调用
std::string title = window->Title();  // 获取标题

window->Width(1024);  // 设置宽度
int width = window->Width();  // 获取宽度
```

### 3. 内容管理
```cpp
auto content = std::make_shared<ui::StackPanel>();
window->SetContent(content);  // 设置内容
auto current = window->GetContent();  // 获取内容
```

### 4. 渲染集成
```cpp
auto renderHost = std::make_shared<render::RenderHost>(...);
window->SetRenderHost(renderHost);  // 关联渲染主机

// 获取本地窗口句柄用于渲染器初始化
GLFWwindow* handle = window->GetNativeHandle();
```

### 5. 事件订阅
```cpp
window->Opened += []() { /* 窗口打开 */ };
window->Closed += []() { /* 窗口关闭 */ };
window->Resized += [](int w, int h) { /* 尺寸改变 */ };
window->Activated += []() { /* 获得焦点 */ };
window->Deactivated += []() { /* 失去焦点 */ };
```

### 6. 自动功能
- **自动 GLFW 初始化**: 第一个窗口创建时自动初始化
- **自动布局**: 内容设置或窗口尺寸改变时自动执行 Measure/Arrange
- **自动视口更新**: 窗口尺寸改变时自动更新渲染器视口
- **自动资源清理**: 析构函数自动销毁 GLFW 窗口

## 编译和运行

```powershell
cd build
cmake --build .
.\fk_example.exe
```

## 输出示例

```
=== F__K_UI Window Example ===
Using: fk::ui::Window with content and rendering

1. Creating UI tree...
   - Root: StackPanel (Vertical)
   - Child count: 3

2. Creating renderer and render host...
   ✓ Renderer and RenderHost created

3. Creating window...
   ✓ Window created: F__K_UI Demo (800x600)

4. Showing window...
Created native window: 800x600
Layout performed: 800x600
   [Event] Window opened
Window 'F__K_UI Demo' opened (800x600)

5. Initializing renderer...
   ✓ Renderer initialized successfully

6. Starting render loop...
   Press ESC or close window to exit

   Frame 60 rendered
   Frame 120 rendered
   ...
```

## 注意事项

1. 窗口在 `Show()` 时才真正创建 GLFW 窗口
2. 必须在窗口显示后才能获取 `GetNativeHandle()`
3. 需要保持消息循环运行 (`glfwPollEvents()`)
4. 按 ESC 或关闭窗口退出程序
