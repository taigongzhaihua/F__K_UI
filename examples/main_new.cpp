#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ContentControl.h"
#include "fk/render/RenderHost.h"
#include "fk/render/GlRenderer.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

using namespace fk;

int main() {
    std::cout << "=== F__K_UI Window Demo ===" << std::endl;
    std::cout << "Testing: Window -> Content -> RenderHost -> GlRenderer" << std::endl;
    std::cout << std::endl;

    // 1. 创建 UI 树
    std::cout << "1. Creating UI tree..." << std::endl;
    
    auto stackPanel = std::make_shared<ui::StackPanel>();
    stackPanel->Orientation(ui::Orientation::Vertical);

    // 添加一些子元素 (流式API链式调用)
    for (int i = 0; i < 3; ++i) {
        auto content = std::make_shared<ui::ContentControl>();
        stackPanel->AddChild(content);
    }
    
    std::cout << "   - Root: StackPanel (Vertical)" << std::endl;
    std::cout << "   - Child count: " << stackPanel->GetChildren().size() << std::endl;
    std::cout << std::endl;

    // 2. 创建渲染器和渲染主机
    std::cout << "2. Creating renderer and render host..." << std::endl;
    auto renderer = std::make_shared<render::GlRenderer>();
    auto renderHost = std::make_shared<render::RenderHost>(renderer);
    std::cout << "   ✓ Renderer created" << std::endl;
    std::cout << std::endl;

    // 3. 创建窗口
    std::cout << "3. Creating window..." << std::endl;
    auto window = ui::window()
        ->Title("F__K_UI Demo Window")
        ->Width(800)
        ->Height(600);

    // 设置内容和渲染系统
    window->SetContent(stackPanel);
    window->SetRenderHost(renderHost);

    // 订阅窗口事件
    window->Closed += []() {
        std::cout << "   [Event] Window closed" << std::endl;
    };
    
    window->Resized += [](int w, int h) {
        std::cout << "   [Event] Window resized to " << w << "x" << h << std::endl;
    };

    // 4. 显示窗口
    std::cout << "4. Showing window..." << std::endl;
    window->Show();
    std::cout << std::endl;

    // 5. 初始化渲染器（需要 OpenGL 上下文）
    std::cout << "5. Initializing renderer..." << std::endl;
    render::RendererInitParams initParams;
    initParams.nativeSurfaceHandle = window->GetNativeHandle();
    initParams.initialSize = {800, 600};
    initParams.pixelRatio = 1.0f;
    initParams.enableDebugLayer = true;
    initParams.rendererName = "GlRenderer";
    
    try {
        renderHost->Initialize(initParams);
        std::cout << "   ✓ Renderer initialized successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "   ✗ Renderer initialization failed: " << e.what() << std::endl;
        return -1;
    }
    std::cout << std::endl;

    // 6. 主循环
    std::cout << "6. Starting render loop..." << std::endl;
    std::cout << "   Press ESC to exit" << std::endl;
    std::cout << std::endl;

    GLFWwindow* nativeWindow = window->GetNativeHandle();
    int frameCount = 0;

    while (!glfwWindowShouldClose(nativeWindow)) {
        glfwPollEvents();

        // 渲染
        renderHost->RequestRender();

        render::FrameContext frameContext;
        frameContext.elapsedSeconds = glfwGetTime();
        frameContext.deltaSeconds = 0.016;
        frameContext.clearColor = {0.1f, 0.1f, 0.15f, 1.0f}; // 深蓝色背景
        frameContext.frameIndex = frameCount;
        
        renderHost->RenderFrame(frameContext, *stackPanel);

        glfwSwapBuffers(nativeWindow);

        // 每60帧打印一次状态
        if (++frameCount % 60 == 0) {
            std::cout << "   Frame " << frameCount << " rendered" << std::endl;
        }

        // ESC 退出
        if (glfwGetKey(nativeWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            window->Close();
        }
    }

    // 7. 清理
    std::cout << std::endl;
    std::cout << "7. Shutting down..." << std::endl;
    renderHost->Shutdown();

    std::cout << "   ✓ Demo complete" << std::endl;
    return 0;
}
