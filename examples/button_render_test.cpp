#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/render/RenderHost.h"
#include "fk/render/GlRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace fk;
using namespace fk::ui;
using namespace fk::render;

int main() {
    std::cout << "=== Button Render Test ===" << std::endl;

    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置 OpenGL 版本 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* glfwWindow = glfwCreateWindow(640, 480, "Button Render Test", nullptr, nullptr);
    if (!glfwWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(1); // 启用垂直同步

    // 初始化 GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    try {
        // 创建测试按钮（不需要 Window，直接测试渲染）
        std::cout << "Creating test buttons..." << std::endl;
        auto button1 = Button<>::Create()
            ->Width(120)
            ->Height(40)
            ->CornerRadius(8.0f)
            ->Background("#4CAF50")
            ->Foreground("#FFFFFF")
            ->BorderBrush("#2E7D32")
            ->BorderThickness(2.0f);

        auto button2 = Button<>::Create()
            ->Width(150)
            ->Height(50)
            ->CornerRadius(25.0f)  // 圆角按钮
            ->Background("#2196F3")
            ->Foreground("#FFFFFF")
            ->BorderThickness(0.0f);

        auto button3 = Button<>::Create()
            ->Width(100)
            ->Height(35)
            ->CornerRadius(4.0f)
            ->Background("#FFFFFF")
            ->Foreground("#000000")
            ->BorderBrush("#CCCCCC")
            ->BorderThickness(1.0f);

        std::cout << "Button 1: Green rounded rectangle (radius=8, border=2)" << std::endl;
        std::cout << "  Background: " << button1->GetBackground() << std::endl;
        std::cout << "  CornerRadius: " << button1->GetCornerRadius() << std::endl;
        std::cout << "  BorderThickness: " << button1->GetBorderThickness() << std::endl;

        std::cout << "Button 2: Blue pill button (radius=25, no border)" << std::endl;
        std::cout << "  Background: " << button2->GetBackground() << std::endl;
        std::cout << "  CornerRadius: " << button2->GetCornerRadius() << std::endl;

        std::cout << "Button 3: White square button (radius=4, border=1)" << std::endl;
        std::cout << "  Background: " << button3->GetBackground() << std::endl;
        std::cout << "  BorderBrush: " << button3->GetBorderBrush() << std::endl;

        std::cout << "\nButtons created successfully!" << std::endl;
        std::cout << "Note: Full rendering requires Window and RenderHost integration." << std::endl;

        std::cout << "\nWindow will close in 3 seconds..." << std::endl;
        
        // 简单的消息循环，显示窗口 3 秒
        auto startTime = std::chrono::steady_clock::now();
        while (!glfwWindowShouldClose(glfwWindow)) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - startTime).count();
            
            if (elapsed >= 3) {
                break;
            }
            
            glfwPollEvents();
            
            // 清空屏幕显示背景色
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            glfwSwapBuffers(glfwWindow);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
        return -1;
    }

    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    std::cout << "Test completed successfully!" << std::endl;
    return 0;
}
