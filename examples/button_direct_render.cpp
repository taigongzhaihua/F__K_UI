#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fk/ui/Button.h"
#include "fk/render/GlRenderer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderList.h"

using namespace fk;
using namespace fk::ui;
using namespace fk::render;

// 辅助函数：添加圆角矩形渲染命令
void addRoundedRect(RenderCommandBuffer& buffer, float x, float y, float w, float h, 
                    float r, float g, float b, float a, float cornerRadius) {
    RectanglePayload payload;
    payload.rect = ui::Rect(x, y, w, h);
    payload.color = {r, g, b, a};
    payload.cornerRadius = cornerRadius;
    
    buffer.AddCommand(RenderCommand(CommandType::DrawRectangle, payload));
}

int main() {
    std::cout << "=== Button Direct Render Test ===" << std::endl;

    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Button Render Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    try {
        // 创建渲染器
        GlRenderer renderer;
        
        RendererInitParams params;
        params.initialSize = {800, 600};
        renderer.Initialize(params);
        
        std::cout << "Renderer initialized successfully" << std::endl;
        std::cout << "Viewport: " << params.initialSize.width << "x" << params.initialSize.height << std::endl;
        std::cout << "Drawing rounded rectangles..." << std::endl;
        std::cout << "Button positions:" << std::endl;
        std::cout << "  Button 1: (50, 50) 200x60" << std::endl;
        std::cout << "  Button 2: (50, 130) 250x50" << std::endl;
        std::cout << "  Button 3: (50, 200) 180x45" << std::endl;

        // 主循环
        int frameCount = 0;
        auto startTime = std::chrono::steady_clock::now();
        
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 开始帧
            FrameContext frameCtx;
            frameCtx.clearColor = {0.15f, 0.15f, 0.15f, 1.0f};
            renderer.BeginFrame(frameCtx);

            // 创建渲染命令缓冲区
            auto commandBuffer = std::make_unique<RenderCommandBuffer>();
            
            // 添加多个圆角矩形按钮
            
            // 按钮 1: 绿色圆角矩形 (圆角半径 8)
            addRoundedRect(*commandBuffer, 50, 50, 200, 60, 
                          0.298f, 0.686f, 0.314f, 1.0f,  // #4CAF50
                          8.0f);
            
            // 按钮 2: 蓝色药丸按钮 (圆角半径 25)
            addRoundedRect(*commandBuffer, 50, 130, 250, 50,
                          0.129f, 0.588f, 0.953f, 1.0f,  // #2196F3
                          25.0f);
            
            // 按钮 3: 白色方形按钮 (圆角半径 4)
            addRoundedRect(*commandBuffer, 50, 200, 180, 45,
                          1.0f, 1.0f, 1.0f, 1.0f,        // #FFFFFF
                          4.0f);
            
            // 按钮 4: 红色按钮 (圆角半径 12)
            addRoundedRect(*commandBuffer, 50, 265, 220, 55,
                          0.957f, 0.263f, 0.212f, 1.0f,  // #F44336
                          12.0f);
            
            // 按钮 5: 橙色按钮 (圆角半径 20)
            addRoundedRect(*commandBuffer, 50, 340, 200, 50,
                          1.0f, 0.596f, 0.0f, 1.0f,      // #FF9800
                          20.0f);
            
            // 按钮 6: 紫色按钮 (圆角半径 0 - 方形)
            addRoundedRect(*commandBuffer, 50, 410, 180, 50,
                          0.612f, 0.153f, 0.690f, 1.0f,  // #9C27B0
                          0.0f);
            
            // 按钮 7: 半透明蓝色 (圆角半径 15)
            addRoundedRect(*commandBuffer, 320, 50, 200, 60,
                          0.129f, 0.588f, 0.953f, 0.7f,  // #2196F3 + alpha
                          15.0f);
            
            // 按钮 8: 青色按钮 (圆角半径 10)
            addRoundedRect(*commandBuffer, 320, 130, 220, 55,
                          0.0f, 0.737f, 0.831f, 1.0f,    // #00BCD4
                          10.0f);

            // 创建渲染列表并执行绘制
            RenderList renderList;
            renderList.SetCommandBuffer(commandBuffer.get());
            renderer.Draw(renderList);

            renderer.EndFrame();
            glfwSwapBuffers(window);

            frameCount++;
            
            // 5 秒后自动退出
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - startTime).count();
            
            if (elapsed >= 5) {
                std::cout << "Rendered " << frameCount << " frames in " << elapsed << " seconds" << std::endl;
                std::cout << "FPS: " << (frameCount / (float)elapsed) << std::endl;
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        renderer.Shutdown();
        std::cout << "Renderer shutdown successfully" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}
