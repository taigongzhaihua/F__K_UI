#pragma once

#include "fk/ui/graphics/Primitives.h"
#include "fk/ui/text/TextEnums.h"
#include "fk/render/RenderCommand.h"
#include <vector>
#include <stack>
#include <memory>

namespace fk::render {

// 前向声明
class RenderList;
class TextRenderer;

/**
 * @brief 变换状�?
 */
struct TransformState {
    float offsetX{0.0f};
    float offsetY{0.0f};
    // TODO: 未来扩展为完整的 Matrix3x2
};

/**
 * @brief 裁剪状�?
 */
struct ClipState {
    ui::Rect clipRect;
    bool enabled{true};
};

/**
 * @brief 图层状�?
 */
struct LayerState {
    float opacity{1.0f};
};

/**
 * @brief 渲染上下�?
 * 
 * 职责�?
 * - 提供高层绘制 API
 * - 管理变换栈、裁剪栈、图层栈
 * - 自动处理坐标变换和裁�?
 * - 生成 RenderCommand �?RenderList
 * 
 * Phase 5.0 核心�?
 */
class RenderContext {
public:
    /**
     * @brief 构造渲染上下文
     * @param renderList 渲染命令列表（必须有效）
     * @param textRenderer 文本渲染器（可选，用于文本度量�?
     */
    RenderContext(RenderList* renderList, TextRenderer* textRenderer = nullptr);
    
    ~RenderContext() = default;

    // ========== 变换管理 ==========
    
    /**
     * @brief 推入变换
     * @param offsetX X 偏移
     * @param offsetY Y 偏移
     */
    void PushTransform(float offsetX, float offsetY);
    
    /**
     * @brief 弹出变换
     */
    void PopTransform();
    
    /**
     * @brief 获取当前累积变换
     */
    TransformState GetCurrentTransform() const;
    
    /**
     * @brief 变换点坐�?
     */
    ui::Point TransformPoint(const ui::Point& point) const;
    
    /**
     * @brief 变换矩形
     */
    ui::Rect TransformRect(const ui::Rect& rect) const;

    // ========== 裁剪管理 ==========
    
    /**
     * @brief 推入裁剪区域
     * @param clipRect 裁剪矩形（局部坐标）
     */
    void PushClip(const ui::Rect& clipRect);
    
    /**
     * @brief 弹出裁剪区域
     */
    void PopClip();
    
    /**
     * @brief 获取当前裁剪区域（全局坐标�?
     */
    ui::Rect GetCurrentClip() const;
    
    /**
     * @brief 判断矩形是否被完全裁�?
     * @param rect 矩形（局部坐标）
     * @return true=完全被裁剪，false=部分或完全可�?
     */
    bool IsClipped(const ui::Rect& rect) const;
    
    /**
     * @brief 判断矩形是否被完全裁剪（IsClipped的别名，更清晰的命名�?
     * @param rect 矩形（局部坐标）
     * @return true=完全被裁剪，false=部分或完全可�?
     */
    bool IsCompletelyClipped(const ui::Rect& rect) const {
        return IsClipped(rect);
    }

    // ========== 图层管理 ==========
    
    /**
     * @brief 推入透明度图�?
     * @param opacity 透明�?(0.0-1.0)
     */
    void PushLayer(float opacity);
    
    /**
     * @brief 弹出透明度图�?
     */
    void PopLayer();
    
    /**
     * @brief 获取当前累积透明�?
     */
    float GetCurrentOpacity() const;

    // ========== 绘制 API ==========
    
    /**
     * @brief 绘制边框（Border 控件专用�?
     * @param rect 矩形区域（局部坐标）
     * @param fillColor 填充颜色
     * @param strokeColor 描边颜色
     * @param strokeWidth 描边宽度
     * @param cornerRadiusTopLeft 左上圆角半径
     * @param cornerRadiusTopRight 右上圆角半径
     * @param cornerRadiusBottomRight 右下圆角半径
     * @param cornerRadiusBottomLeft 左下圆角半径
     * @note 四个角可以有不同的圆角半�?
     */
    void DrawBorder(
        const ui::Rect& rect,
        const std::array<float, 4>& fillColor,
        const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
        float strokeWidth = 0.0f,
        float cornerRadiusTopLeft = 0.0f,
        float cornerRadiusTopRight = 0.0f,
        float cornerRadiusBottomRight = 0.0f,
        float cornerRadiusBottomLeft = 0.0f,
        StrokeAlignment strokeAlignment = StrokeAlignment::Center,
        float aaWidth = 0.75f
    );
    
    /**
     * @brief 绘制矩形（Rectangle Shape 专用�?
     * @param rect 矩形区域（局部坐标）
     * @param fillColor 填充颜色
     * @param strokeColor 描边颜色
     * @param strokeWidth 描边宽度
     * @param radiusX 圆角水平半径（应用到所有四个角�?
     * @param radiusY 圆角垂直半径（应用到所有四个角�?
     * @note radiusX �?radiusY 用于创建椭圆形圆角，四个角共享相同的半径�?
     */
    void DrawRectangle(
        const ui::Rect& rect,
        const std::array<float, 4>& fillColor,
        const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
        float strokeWidth = 0.0f,
        float radiusX = 0.0f,
        float radiusY = 0.0f,
        StrokeAlignment strokeAlignment = StrokeAlignment::Center,
        float aaWidth = 0.75f
    );
    
    /**
     * @brief 绘制文本
     * @param bounds 文本边界矩形（局部坐标），用于裁�?
     * @param text 文本内容
     * @param color 文本颜色
     * @param fontSize 字体大小
     * @param fontFamily 字体�?
     * @param maxWidth 最大宽度（用于自动换行�? 表示不限制）
     * @param textWrapping 是否自动换行
     */
    void DrawText(
        const ui::Rect& bounds,
        const std::string& text,
        const std::array<float, 4>& color,
        float fontSize,
        const std::string& fontFamily = "Arial",
        float maxWidth = 0.0f,
        bool textWrapping = false
    );
    
    /**
     * @brief 绘制椭圆
     * @param bounds 边界矩形（局部坐标）
     * @param fillColor 填充颜色
     * @param strokeColor 描边颜色
     * @param strokeWidth 描边宽度
     */
    void DrawEllipse(
        const ui::Rect& bounds,
        const std::array<float, 4>& fillColor,
        const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
        float strokeWidth = 0.0f
    );
    
    /**
     * @brief 绘制线条
     * @param start 起点（局部坐标）
     * @param end 终点（局部坐标）
     * @param color 线条颜色
     * @param width 线宽
     */
    void DrawLine(
        const ui::Point& start,
        const ui::Point& end,
        const std::array<float, 4>& color,
        float width = 1.0f
    );
    
    /**
     * @brief 绘制多边�?
     * @param points 顶点列表（局部坐标）
     * @param fillColor 填充颜色
     * @param strokeColor 描边颜色
     * @param strokeWidth 描边宽度
     */
    void DrawPolygon(
        const std::vector<ui::Point>& points,
        const std::array<float, 4>& fillColor,
        const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
        float strokeWidth = 0.0f
    );
    
    /**
     * @brief 绘制路径
     * @param segments 路径段列�?
     * @param fillColor 填充颜色
     * @param strokeColor 描边颜色
     * @param strokeWidth 描边宽度
     */
    void DrawPath(
        const std::vector<PathSegment>& segments,
        const std::array<float, 4>& fillColor,
        const std::array<float, 4>& strokeColor = {0, 0, 0, 0},
        float strokeWidth = 0.0f
    );
    
    /**
     * @brief 绘制图像
     * @param bounds 目标矩形（局部坐标）
     * @param textureId 纹理 ID
     * @param tint 着色（默认白色�?
     */
    void DrawImage(
        const ui::Rect& bounds,
        uint32_t textureId,
        const std::array<float, 4>& tint = {1, 1, 1, 1}
    );

    // ========== 文本度量 ==========
    
    /**
     * @brief 测量文本尺寸
     * @param text 文本内容
     * @param fontSize 字体大小
     * @param fontFamily 字体�?
     * @return 文本尺寸（宽度和高度�?
     */
    ui::Size MeasureText(
        const std::string& text,
        float fontSize,
        const std::string& fontFamily = "Arial"
    );
    
    /**
     * @brief 获取文本渲染器（用于高级度量�?
     */
    TextRenderer* GetTextRenderer() const { return textRenderer_; }

private:
    /**
     * @brief 应用当前变换�?RenderCommand
     */
    void ApplyCurrentTransform();
    
    /**
     * @brief 应用当前裁剪�?RenderCommand
     */
    void ApplyCurrentClip();
    
    /**
     * @brief 应用当前透明度到颜色
     */
    std::array<float, 4> ApplyOpacity(const std::array<float, 4>& color) const;

private:
    RenderList* renderList_{nullptr};       // 渲染命令列表
    TextRenderer* textRenderer_{nullptr};   // 文本渲染�?
    
    // 状态栈
    std::stack<TransformState> transformStack_;
    std::stack<ClipState> clipStack_;
    std::stack<LayerState> layerStack_;
    
    // 当前累积状态（缓存，避免重复计算）
    TransformState currentTransform_;
    ClipState currentClip_;
    float currentOpacity_{1.0f};
};

} // namespace fk::render
