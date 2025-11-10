#pragma once

#include "fk/ui/DrawCommand.h"
#include "fk/ui/Primitives.h"
#include <string>

namespace fk::ui {

/**
 * @brief 渲染后端接口
 * 
 * 职责：
 * - 定义平台无关的渲染接口
 * - 由具体后端实现（OpenGL, DirectX, Vulkan等）
 */
class RenderBackend {
public:
    virtual ~RenderBackend() = default;

    /**
     * @brief 初始化后端
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief 清理后端资源
     */
    virtual void Shutdown() = 0;

    /**
     * @brief 开始新一帧渲染
     */
    virtual void BeginFrame() = 0;
    
    /**
     * @brief 结束当前帧渲染
     */
    virtual void EndFrame() = 0;

    /**
     * @brief 清空画布
     */
    virtual void Clear(const Color& color) = 0;
    
    /**
     * @brief 设置视口
     */
    virtual void SetViewport(int x, int y, int width, int height) = 0;

    // ========== 基础绘制 ==========
    
    /**
     * @brief 绘制矩形
     */
    virtual void DrawRectangle(const Rect& rect, const Color& fill, const Color& stroke, float strokeWidth) = 0;
    
    /**
     * @brief 绘制圆角矩形
     */
    virtual void DrawRoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke) = 0;
    
    /**
     * @brief 绘制圆形
     */
    virtual void DrawCircle(const Point& center, float radius, const Color& fill, const Color& stroke) = 0;
    
    /**
     * @brief 绘制线条
     */
    virtual void DrawLine(const Point& start, const Point& end, const Color& color, float width) = 0;
    
    /**
     * @brief 绘制文本
     */
    virtual void DrawText(const Point& position, const std::string& text, const Color& color, float fontSize) = 0;
    
    /**
     * @brief 绘制图像
     */
    virtual void DrawImage(const Rect& bounds, const std::string& imagePath) = 0;

    /**
     * @brief 应用变换矩阵
     */
    virtual void PushTransform(const Matrix3x2& transform) = 0;
    
    /**
     * @brief 弹出变换矩阵
     */
    virtual void PopTransform() = 0;
};

/**
 * @brief OpenGL 渲染后端（示例实现）
 */
class OpenGLRenderBackend : public RenderBackend {
public:
    OpenGLRenderBackend() = default;
    ~OpenGLRenderBackend() override = default;

    bool Initialize() override;
    void Shutdown() override;
    
    void BeginFrame() override;
    void EndFrame() override;
    
    void Clear(const Color& color) override;
    void SetViewport(int x, int y, int width, int height) override;
    
    void DrawRectangle(const Rect& rect, const Color& fill, const Color& stroke, float strokeWidth) override;
    void DrawRoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke) override;
    void DrawCircle(const Point& center, float radius, const Color& fill, const Color& stroke) override;
    void DrawLine(const Point& start, const Point& end, const Color& color, float width) override;
    void DrawText(const Point& position, const std::string& text, const Color& color, float fontSize) override;
    void DrawImage(const Rect& bounds, const std::string& imagePath) override;
    
    void PushTransform(const Matrix3x2& transform) override;
    void PopTransform() override;

private:
    // OpenGL 特定状态
    bool initialized_{false};
};

} // namespace fk::ui
