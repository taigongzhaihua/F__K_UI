#pragma once

#include "fk/ui/DrawCommand.h"
#include "fk/ui/Primitives.h"
#include <vector>
#include <memory>

namespace fk::ui {

// 前向声明
class RenderBackend;

/**
 * @brief 渲染器
 * 
 * 职责：
 * - 管理绘制命令队列
 * - 协调后端渲染
 * - 提供高级绘制API
 */
class Renderer {
public:
    Renderer();
    ~Renderer();

    /**
     * @brief 设置渲染后端
     */
    void SetBackend(std::unique_ptr<RenderBackend> backend);
    
    /**
     * @brief 获取渲染后端
     */
    RenderBackend* GetBackend() const { return backend_.get(); }

    /**
     * @brief 提交绘制命令
     */
    void Submit(const DrawCommand& command);
    
    /**
     * @brief 批量提交命令
     */
    void Submit(const std::vector<DrawCommand>& commands);
    
    /**
     * @brief 清空命令队列
     */
    void Clear();

    /**
     * @brief 渲染一帧
     */
    void RenderFrame();
    
    /**
     * @brief 设置视口尺寸
     */
    void SetViewport(const Size& size);
    
    /**
     * @brief 获取视口尺寸
     */
    const Size& GetViewport() const { return viewport_; }

    // ========== 便捷绘制方法 ==========
    
    void DrawRectangle(const Rect& rect, const Color& fill, const Color& stroke = Color::Transparent(), float strokeWidth = 1.0f);
    void DrawRoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke = Color::Transparent());
    void DrawCircle(const Point& center, float radius, const Color& fill, const Color& stroke = Color::Transparent());
    void DrawLine(const Point& start, const Point& end, const Color& color, float width = 1.0f);
    void DrawText(const Point& position, const std::string& text, const Color& color, float fontSize = 14.0f);
    void DrawImage(const Rect& bounds, const std::string& path);

private:
    std::unique_ptr<RenderBackend> backend_;
    std::vector<DrawCommand> commandQueue_;
    Size viewport_{800, 600};
};

} // namespace fk::ui
