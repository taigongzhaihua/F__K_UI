#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fk/render/GlRenderer.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/RenderCommandBuffer.h"
#include "fk/render/RenderList.h"

using namespace fk;
using namespace fk::render;

int main() {
    std::cout << "=== Simple Render Test ===" << std::endl;

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Render Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
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

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << std::endl;

    try {
        // 获取实际窗口大小
        int actualWidth, actualHeight;
        glfwGetFramebufferSize(window, &actualWidth, &actualHeight);
        std::cout << "Framebuffer size: " << actualWidth << "x" << actualHeight << std::endl;
        
        GlRenderer renderer;
        RendererInitParams params;
        params.initialSize = {static_cast<uint32_t>(actualWidth), static_cast<uint32_t>(actualHeight)};
        renderer.Initialize(params);

        std::cout << "Viewport size: " << actualWidth << "x" << actualHeight << std::endl;
        std::cout << "Drawing rectangles:" << std::endl;
        std::cout << "  1. Red (10,10) 100x50" << std::endl;
        std::cout << "  2. Green (10,80) 100x50" << std::endl;
        std::cout << "  3. Blue (10,150) 100x50" << std::endl;
        std::cout << "  4. Yellow (10,220) 100x50" << std::endl;
        std::cout << "Window will stay open for 10 seconds..." << std::endl;

        auto startTime = std::chrono::steady_clock::now();
        int frameCount = 0;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // 开始帧
            FrameContext frameCtx;
            frameCtx.clearColor = {0.1f, 0.1f, 0.1f, 1.0f};  // 深灰色背景
            renderer.BeginFrame(frameCtx);

            // 创建命令缓冲区
            auto commandBuffer = std::make_unique<RenderCommandBuffer>();

            // 绘制 4 个小矩形，垂直排列在左侧
            
            // 1. 红色
            RectanglePayload payload1;
            payload1.rect = ui::Rect(10, 10, 100, 50);
            payload1.color = {1.0f, 0.0f, 0.0f, 1.0f};
            payload1.cornerRadius = 5.0f;
            commandBuffer->AddCommand(RenderCommand(CommandType::DrawRectangle, payload1));

            // 2. 绿色
            RectanglePayload payload2;
            payload2.rect = ui::Rect(10, 80, 100, 50);
            payload2.color = {0.0f, 1.0f, 0.0f, 1.0f};
            payload2.cornerRadius = 5.0f;
            commandBuffer->AddCommand(RenderCommand(CommandType::DrawRectangle, payload2));

            // 3. 蓝色
            RectanglePayload payload3;
            payload3.rect = ui::Rect(10, 150, 100, 50);
            payload3.color = {0.0f, 0.0f, 1.0f, 1.0f};
            payload3.cornerRadius = 5.0f;
            commandBuffer->AddCommand(RenderCommand(CommandType::DrawRectangle, payload3));
            
            // 4. 黄色
            RectanglePayload payload4;
            payload4.rect = ui::Rect(10, 220, 100, 50);
            payload4.color = {1.0f, 1.0f, 0.0f, 1.0f};
            payload4.cornerRadius = 5.0f;
            commandBuffer->AddCommand(RenderCommand(CommandType::DrawRectangle, payload4));

            RenderList renderList;
            renderList.SetCommandBuffer(commandBuffer.get());
            renderer.Draw(renderList);

            renderer.EndFrame();
            glfwSwapBuffers(window);

            frameCount++;

            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - startTime).count();

            if (elapsed >= 10) {  // 10 秒后退出
                std::cout << "Frames: " << frameCount << ", FPS: " << (frameCount / 10.0f) << std::endl;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        renderer.Shutdown();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Test completed!" << std::endl;
    return 0;
}
