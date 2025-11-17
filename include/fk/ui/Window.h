#pragma once

#include "fk/ui/ContentControl.h"
#include "fk/binding/DependencyProperty.h"
#include <string>
#include <memory>

// 前向声明
namespace fk::render {
    class GlRenderer;
    class RenderList;
    class TextRenderer;
}

namespace fk::ui {

/**
 * @brief 窗口枚举
 */
enum class WindowState {
    Normal,      // 正常状态
    Minimized,   // 最小化
    Maximized    // 最大化
};

enum class WindowStartupLocation {
    Manual,          // 手动指定位置
    CenterScreen,    // 屏幕中心
    CenterOwner      // 相对于父窗口居中
};

/**
 * @brief 窗口类
 * 
 * 职责：
 * - 顶层窗口容器
 * - 窗口标题和图标
 * - 窗口状态（最小化、最大化、正常）
 * - 窗口位置和大小
 * - 模态对话框支持
 * 
 * 继承：ContentControl
 * WPF 对应：Window
 */
class Window : public ContentControl<Window> {
public:
    Window();
    virtual ~Window();

    // ========== 依赖属性 ==========
    
    /// Width 属性：窗口宽度（重写FrameworkElement的Width以添加回调）
    static const binding::DependencyProperty& WidthProperty();
    
    /// Height 属性：窗口高度（重写FrameworkElement的Height以添加回调）
    static const binding::DependencyProperty& HeightProperty();
    
    /// Title 属性：窗口标题
    static const binding::DependencyProperty& TitleProperty();
    
    /// WindowState 属性：窗口状态
    static const binding::DependencyProperty& WindowStateProperty();
    
    /// Left 属性：窗口左边距（相对于屏幕）
    static const binding::DependencyProperty& LeftProperty();
    
    /// Top 属性：窗口顶边距（相对于屏幕）
    static const binding::DependencyProperty& TopProperty();
    
    /// ShowInTaskbar 属性：是否在任务栏显示
    static const binding::DependencyProperty& ShowInTaskbarProperty();
    
    /// Topmost 属性：是否置顶显示
    static const binding::DependencyProperty& TopmostProperty();
    
    /// ResizeMode 属性：调整大小模式（暂未实现枚举）
    // static const binding::DependencyProperty& ResizeModeProperty();

    // ========== 窗口标题 ==========
    
    std::string GetTitle() const { return GetValue<std::string>(TitleProperty()); }
    void SetTitle(const std::string& value) { SetValue(TitleProperty(), value); }
    
    Window* Title(const std::string& value) {
        SetTitle(value);
        return this;
    }
    Window* Title(binding::Binding binding) {
        SetBinding(TitleProperty(), std::move(binding));
        return this;
    }
    std::string Title() const { return GetTitle(); }

    // ========== 窗口状态 ==========
    
    WindowState GetWindowState() const { return GetValue<ui::WindowState>(WindowStateProperty()); }
    void SetWindowState(WindowState value);
    
    // 流式 API：使用 State 避免与枚举类型名称冲突
    Window* State(ui::WindowState value) {
        SetWindowState(value);
        return this;
    }

    // ========== 窗口位置 ==========
    
    float GetLeft() const { return GetValue<float>(LeftProperty()); }
    void SetLeft(float value) { SetValue(LeftProperty(), value); }
    
    Window* Left(float value) {
        SetLeft(value);
        return this;
    }
    Window* Left(binding::Binding binding) {
        SetBinding(LeftProperty(), std::move(binding));
        return this;
    }
    float Left() const { return GetLeft(); }
    
    float GetTop() const { return GetValue<float>(TopProperty()); }
    void SetTop(float value) { SetValue(TopProperty(), value); }
    
    Window* Top(float value) {
        SetTop(value);
        return this;
    }
    Window* Top(binding::Binding binding) {
        SetBinding(TopProperty(), std::move(binding));
        return this;
    }
    float Top() const { return GetTop(); }

    // ========== 窗口行为 ==========
    
    bool GetShowInTaskbar() const { return GetValue<bool>(ShowInTaskbarProperty()); }
    void SetShowInTaskbar(bool value) { SetValue(ShowInTaskbarProperty(), value); }
    
    Window* ShowInTaskbar(bool value) {
        SetShowInTaskbar(value);
        return this;
    }
    bool ShowInTaskbar() const { return GetShowInTaskbar(); }
    
    bool GetTopmost() const { return GetValue<bool>(TopmostProperty()); }
    void SetTopmost(bool value) { SetValue(TopmostProperty(), value); }
    
    Window* Topmost(bool value) {
        SetTopmost(value);
        return this;
    }
    bool Topmost() const { return GetTopmost(); }

    // ========== 窗口操作 ==========
    
    /**
     * @brief 显示窗口
     */
    void Show();
    
    /**
     * @brief 显示模态窗口
     * @return 对话框结果（true/false）
     */
    bool ShowDialog();
    
    /**
     * @brief 关闭窗口
     */
    void Close();
    
    /**
     * @brief 激活窗口（置于前台）
     */
    void Activate();
    
    /**
     * @brief 隐藏窗口
     */
    void Hide();
    
    /**
     * @brief 检查窗口是否可见
     */
    bool IsVisible() const { return isVisible_; }
    
    /**
     * @brief 处理窗口事件
     * @return 如果窗口继续运行返回 true，关闭返回 false
     */
    bool ProcessEvents();
    
    /**
     * @brief 渲染窗口帧
     */
    void RenderFrame();
    
    /**
     * @brief 在窗口的内容树中查找指定名称的元素
     * 
     * @param name 要查找的元素名称
     * @return 找到的元素指针，未找到返回 nullptr
     * 
     * 这是一个便捷方法，等价于调用窗口内容的FindName()。
     * 类似于WPF中的Window.FindName()方法。
     * 
     * 示例：
     * @code
     * auto window = app->CreateWindow();
     * auto panel = new StackPanel();
     * auto button = new Button();
     * button->Name("submitButton");
     * panel->AddChild(button);
     * window->Content(panel);
     * 
     * // 稍后从窗口中查找
     * UIElement* found = window->FindName("submitButton");
     * if (found) {
     *     auto* btn = dynamic_cast<Button*>(found);
     *     // 使用按钮...
     * }
     * @endcode
     */
    UIElement* FindName(const std::string& name);

    // ========== 窗口事件 ==========
    
    core::Event<> Closing;      // 窗口即将关闭
    core::Event<> Closed;       // 窗口已关闭
    core::Event<> Activated;    // 窗口激活
    core::Event<> Deactivated;  // 窗口失活

protected:
    /**
     * @brief 窗口状态变更钩子
     */
    virtual void OnWindowStateChanged(ui::WindowState oldState, ui::WindowState newState);
    
    /**
     * @brief 窗口关闭前钩子（可取消）
     */
    virtual bool OnClosing() { return true; }
    
    /**
     * @brief 窗口关闭后钩子
     */
    virtual void OnClosed() {}

private:
    void* nativeHandle_{nullptr};  // 平台原生窗口句柄
    bool isModal_{false};           // 是否为模态窗口
    bool isClosing_{false};         // 正在关闭标志
    bool isVisible_{false};         // 窗口可见性标志
    
    // 渲染系统
    std::unique_ptr<render::GlRenderer> renderer_;     // OpenGL 渲染器
    std::unique_ptr<render::RenderList> renderList_;   // 渲染命令列表
    
    // 输入管理系统
    std::unique_ptr<class InputManager> inputManager_; // 输入管理器
    
    // 视口大小缓存（用于优化，避免每帧都调用 Resize）
    int lastViewportWidth_{0};
    int lastViewportHeight_{0};
    
    // 属性变更回调（用于同步到原生窗口）
    static void OnTitleChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnWidthChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnHeightChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnLeftChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnTopChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void OnTopmostChanged(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    // 辅助方法：应用Topmost属性到原生窗口
    void ApplyTopmostToNativeWindow();
};

// 窗口智能指针类型（用于 Application 管理）
using WindowPtr = std::shared_ptr<Window>;

} // namespace fk::ui
