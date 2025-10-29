#pragma once

struct GLFWwindow;

namespace fk::ui {

class Window;

/**
 * @brief WindowInteropHelper - 封装原生窗口操作
 * 
 * 提供对底层 GLFW 窗口的访问和管理，将平台相关代码与 Window 类分离
 */
class WindowInteropHelper {
public:
    explicit WindowInteropHelper(Window* owner);
    ~WindowInteropHelper();

    /**
     * @brief 获取原生窗口句柄
     * @return GLFWwindow* 指针，如果窗口未创建则返回 nullptr
     */
    GLFWwindow* GetHandle() const { return handle_; }

    /**
     * @brief 确保窗口已创建
     * 
     * 如果窗口尚未创建，则创建它。这是延迟创建模式。
     */
    void EnsureHandle();

    /**
     * @brief 销毁原生窗口
     */
    void DestroyHandle();

    /**
     * @brief 检查窗口是否已创建
     */
    bool HasHandle() const { return handle_ != nullptr; }

private:
    void CreateNativeWindow();
    
    // GLFW 回调函数
    static void OnWindowClose(GLFWwindow* window);
    static void OnWindowResize(GLFWwindow* window, int width, int height);
    static void OnWindowFocus(GLFWwindow* window, int focused);
    static void OnWindowPos(GLFWwindow* window, int xpos, int ypos);
    static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);

    Window* owner_;
    GLFWwindow* handle_ = nullptr;
};

} // namespace fk::ui
