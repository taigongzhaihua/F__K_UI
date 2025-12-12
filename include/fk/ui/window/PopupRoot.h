#pragma once

#include "fk/ui/graphics/Primitives.h"
#include <memory>

// 前向声明
namespace fk::render {
    class GlRenderer;
    class RenderList;
    class TextRenderer;
}

namespace fk::ui {

class UIElement;
class InputManager;

/**
 * @brief PopupRoot - Popup 的原生窗口容器
 * 
 * 职责：
 * - 创建和管理无边框原生窗口
 * - 提供独立的 OpenGL 渲染上下文
 * - 处理窗口位置、大小和显示状态
 * - 管理 Popup 内容的渲染
 * 
 * 设计：
 * - 每个 Popup 有一个独立的 PopupRoot
 * - 使用 GLFW 无边框窗口（GLFW_DECORATED = false）
 * - 独立的 GlRenderer 和 RenderList
 * - 支持透明背景和置顶显示
 */
class PopupRoot {
public:
    PopupRoot();
    ~PopupRoot();

    // ========== 生命周期管理 ==========
    
    /**
     * @brief 初始化 PopupRoot
     */
    void Initialize();
    
    /**
     * @brief 关闭并清理资源
     */
    void Shutdown();
    
    /**
     * @brief 检查是否已初始化
     */
    bool IsInitialized() const { return initialized_; }

    // ========== 窗口管理 ==========
    
    /**
     * @brief 创建原生窗口
     * @param width 窗口宽度
     * @param height 窗口高度
     */
    void Create(int width, int height);
    
    /**
     * @brief 销毁窗口
     */
    void Destroy();
    
    /**
     * @brief 检查窗口是否已创建
     */
    bool IsCreated() const { return nativeHandle_ != nullptr; }
    
    /**
     * @brief 显示窗口
     * @param screenPos 屏幕绝对坐标
     */
    void Show(Point screenPos);
    
    /**
     * @brief 隐藏窗口
     */
    void Hide();
    
    /**
     * @brief 检查窗口是否可见
     */
    bool IsVisible() const { return isVisible_; }
    
    /**
     * @brief 设置窗口大小
     * @param width 宽度
     * @param height 高度
     */
    void SetSize(int width, int height);
    
    /**
     * @brief 获取窗口大小
     */
    Size GetSize() const;

    // ========== 内容管理 ==========
    
    /**
     * @brief 设置 Popup 内容
     * @param content 内容元素
     */
    void SetContent(UIElement* content);
    
    /**
     * @brief 获取 Popup 内容
     */
    UIElement* GetContent() const { return content_; }

    // ========== 渲染 ==========
    
    /**
     * @brief 渲染一帧
     */
    void RenderFrame();
    
    /**
     * @brief 调整渲染器大小
     * @param width 宽度
     * @param height 高度
     */
    void Resize(int width, int height);

    // ========== 属性设置 ==========
    
    /**
     * @brief 设置是否允许透明
     * @param allow 是否允许
     */
    void SetAllowsTransparency(bool allow);
    
    /**
     * @brief 获取是否允许透明
     */
    bool GetAllowsTransparency() const { return allowsTransparency_; }
    
    /**
     * @brief 设置是否置顶显示
     * @param topmost 是否置顶
     */
    void SetTopmost(bool topmost);
    
    /**
     * @brief 获取是否置顶
     */
    bool GetTopmost() const { return isTopmost_; }
    
    /**
     * @brief 获取原生窗口句柄
     */
    void* GetNativeHandle() const { return nativeHandle_; }
    
    /**
     * @brief 设置主窗口句柄（用于焦点管理）
     * @param mainWindow 主窗口的 GLFW 句柄
     */
    void SetOwnerWindow(void* mainWindow) { ownerWindow_ = mainWindow; }
    
    // ========== 事件处理 ==========
    
    /**
     * @brief 处理窗口事件（输入等）
     * @return true 表示窗口仍然活跃
     */
    bool ProcessEvents();
    
    /**
     * @brief 获取输入管理器
     */
    InputManager* GetInputManager() const { return inputManager_.get(); }
    
    /**
     * @brief 检查屏幕坐标点是否在窗口范围内
     * @param screenX 屏幕 X 坐标
     * @param screenY 屏幕 Y 坐标
     * @return true 如果点在窗口内
     */
    bool ContainsScreenPoint(int screenX, int screenY) const;

private:
    // 初始化渲染器
    void InitializeRenderer();
    
    // 清理渲染资源
    void CleanupRenderer();
    
private:
    void* nativeHandle_{nullptr};                      // GLFW 窗口句柄
    void* ownerWindow_{nullptr};                       // 主窗口句柄（用于焦点管理）
    UIElement* content_{nullptr};                      // Popup 内容
    
    // 渲染系统
    std::unique_ptr<render::GlRenderer> renderer_;     // 独立渲染器
    std::unique_ptr<render::RenderList> renderList_;   // 渲染命令列表
    std::unique_ptr<InputManager> inputManager_;       // 输入管理器
    
    // 状态标志
    bool initialized_{false};                          // 是否已初始化
    bool isVisible_{false};                            // 是否可见
    bool allowsTransparency_{false};                   // 是否允许透明
    bool isTopmost_{true};                             // 是否置顶（默认置顶）
    
    // 缓存的窗口大小
    int width_{0};
    int height_{0};
    int lastViewportWidth_{0};
    int lastViewportHeight_{0};
};

} // namespace fk::ui
