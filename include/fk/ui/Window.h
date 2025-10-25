#pragma once

#include <memory>
#include <string>
#include "fk/core/Event.h"
#include "fk/ui/ContentControl.h"

struct GLFWwindow;

namespace fk::render {
class RenderHost;
}

namespace fk::ui {

class WindowInteropHelper;

/**
 * @brief Window - 顶级窗口类
 * 
 * 继承自 ContentControl，提供顶级窗口功能
 * 通过 WindowInteropHelper 管理原生窗口
 */
class Window : public ContentControl {
public:
    using Ptr = std::shared_ptr<Window>;

    Window();
    virtual ~Window();

    // 依赖属性声明
    static const binding::DependencyProperty& TitleProperty();
    static const binding::DependencyProperty& WidthProperty();
    static const binding::DependencyProperty& HeightProperty();

    // 流式 API (无参getter/有参setter重载)
    Ptr Title(const std::string& title);
    std::string Title() const;
    
    // 窗口大小 (隐藏父类的 float 版本,提供 int 版本控制窗口大小)
    Ptr Width(int w);
    int Width() const;
    
    Ptr Height(int h);
    int Height() const;
    
    bool IsVisible() const { return visible_; }


    // 窗口操作
    void Show();
    void Hide();
    void Close();
    
    /**
     * @brief 处理一次消息（非阻塞）
     * @return 如果窗口应该关闭返回 false
     */
    bool ProcessEvents();
    
    /**
     * @brief 执行一帧渲染
     */
    void RenderFrame();
    
    // 获取本地窗口句柄（尽量避免使用，仅供底层需要）
    GLFWwindow* GetNativeHandle() const;

    // 渲染集成
    void SetRenderHost(std::shared_ptr<render::RenderHost> renderHost);
    std::shared_ptr<render::RenderHost> GetRenderHost() const { return renderHost_; }

    // 事件
    core::Event<> Closed;
    core::Event<> Opened;
    core::Event<int, int> Resized;
    core::Event<> Activated;
    core::Event<> Deactivated;

    // 由 WindowInteropHelper 调用的回调
    friend class WindowInteropHelper;
    void OnNativeWindowClose();
    void OnNativeWindowResize(int width, int height);
    void OnNativeWindowFocus(bool focused);
    void OnNativeWindowMove(int x, int y);

protected:
    // 重写 ContentControl 的内容变更通知
    void OnContentChanged(UIElement* oldContent, UIElement* newContent) override;

private:
    /**
     * @brief 执行布局
     */
    void PerformLayout();

    /**
     * @brief 请求重绘
     */
    void RequestRender();

    // 依赖属性元数据
    static binding::PropertyMetadata BuildTitleMetadata();
    static binding::PropertyMetadata BuildWidthMetadata();
    static binding::PropertyMetadata BuildHeightMetadata();
    
    // 依赖属性变更回调
    static void TitlePropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );
    
    static void SizePropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    // int width_;   // 移除,改用依赖属性
    // int height_;  // 移除,改用依赖属性
    bool visible_;
    int frameCount_{0};
    
    std::unique_ptr<WindowInteropHelper> interopHelper_;
    std::shared_ptr<render::RenderHost> renderHost_;
};

using WindowPtr = Window::Ptr;

inline WindowPtr window() {
    return std::make_shared<Window>();
}

} // namespace fk::ui
