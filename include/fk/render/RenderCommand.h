#pragma once

#include <cstdint>
#include <variant>
#include <array>
#include <vector>
#include "fk/ui/base/UIElement.h"

namespace fk::render {

/**
 * @brief 渲染命令类型
 */
enum class CommandType : std::uint8_t {
    SetClip,           // 设置裁剪区域
    SetTransform,      // 设置变换矩阵
    DrawRectangle,     // 绘制矩形
    DrawText,          // 绘制文本
    DrawImage,         // 绘制图像
    DrawPolygon,       // 绘制多边�?
    DrawPath,          // 绘制路径
    PushLayer,         // 推入图层（用于透明�?混合�?
    PopLayer,          // 弹出图层
};

/**
 * @brief 描边对齐方式
 */
enum class StrokeAlignment : std::uint8_t {
    Center = 0,
    Inside = 1,
    Outside = 2,
};

/**
 * @brief 裁剪命令载荷
 */
struct ClipPayload {
    ui::Rect clipRect;
    bool enabled{true};  // true=启用裁切, false=禁用裁切
};

/**
 * @brief 变换命令载荷（简化版，先�?D偏移�?
 */
struct TransformPayload {
    float offsetX{0.0f};
    float offsetY{0.0f};
    // TODO: 后续扩展为完整的变换矩阵
};

/**
 * @brief 矩形绘制载荷
 */
struct RectanglePayload {
    ui::Rect rect;
    std::array<float, 4> fillColor; // RGBA 填充�?
    std::array<float, 4> strokeColor; // RGBA 描边�?
    float strokeThickness{0.0f};
    // 圆角半径（四个角可以独立设置 - 用于 Border�?
    float cornerRadiusTopLeft{0.0f};
    float cornerRadiusTopRight{0.0f};
    float cornerRadiusBottomRight{0.0f};
    float cornerRadiusBottomLeft{0.0f};
    // 椭圆圆角半径（用�?Rectangle Shape�?
    float radiusX{0.0f};
    float radiusY{0.0f};
    StrokeAlignment strokeAlignment{StrokeAlignment::Center};
    float aaWidth{0.75f};
};

/**
 * @brief 文本绘制载荷
 */
struct TextPayload {
    ui::Rect bounds;
    std::array<float, 4> color;
    std::string text;           // 文本内容
    int fontId{0};              // 字体 ID
    float fontSize{14.0f};      // 字体大小
    std::string fontFamily;     // 字体�?可�?
    bool textWrapping{false};   // 是否自动换行
    float maxWidth{0.0f};       // 换行时的最大宽�?
};

/**
 * @brief 图像绘制载荷（占位）
 */
struct ImagePayload {
    ui::Rect destRect;
    std::uint32_t textureId{0};
};

/**
 * @brief 图层载荷
 */
struct LayerPayload {
    float opacity{1.0f};
};

/**
 * @brief 多边形绘制载�?
 */
struct PolygonPayload {
    std::vector<ui::Point> points;
    std::array<float, 4> fillColor;   // RGBA
    std::array<float, 4> strokeColor; // RGBA
    float strokeThickness{0.0f};
    bool filled{true};
};

/**
 * @brief 路径段类�?
 */
enum class PathSegmentType : std::uint8_t {
    MoveTo,
    LineTo,
    QuadraticBezierTo,
    CubicBezierTo,
    ArcTo,
    Close
};

/**
 * @brief 路径段数�?
 */
struct PathSegment {
    PathSegmentType type;
    std::vector<ui::Point> points; // 控制�?端点
    std::array<float, 4> strokeColor; // 分段描边颜色(RGBA,可�?
    bool hasStrokeColor{false};       // 是否设置了分段颜�?
    std::array<float, 4> fillColor;   // 子路径填充颜�?RGBA,可�?仅MoveTo有效)
    bool hasFillColor{false};         // 是否设置了子路径填充颜色
    std::array<float, 4> subPathStrokeColor; // 子路径描边颜�?RGBA,可�?仅MoveTo有效)
    float subPathStrokeThickness{0.0f};      // 子路径描边粗�?可�?仅MoveTo有效)
    bool hasSubPathStroke{false};            // 是否设置了子路径描边
    // 用于圆弧的额外参�?
    float radiusX{0.0f};
    float radiusY{0.0f};
    float angle{0.0f};
    bool largeArc{false};
    bool sweep{false};
};

/**
 * @brief 路径绘制载荷
 */
struct PathPayload {
    std::vector<PathSegment> segments;
    std::array<float, 4> fillColor;   // RGBA
    std::array<float, 4> strokeColor; // RGBA
    float strokeThickness{0.0f};
    bool filled{true};
};

/**
 * @brief 命令载荷联合�?
 */
using CommandPayload = std::variant<
    std::monostate,
    ClipPayload,
    TransformPayload,
    RectanglePayload,
    TextPayload,
    ImagePayload,
    LayerPayload,
    PolygonPayload,
    PathPayload
>;

/**
 * @brief 渲染命令
 */
struct RenderCommand {
    CommandType type;
    CommandPayload payload;

    RenderCommand(CommandType t, CommandPayload p = {})
        : type(t), payload(std::move(p)) {}
};

} // namespace fk::render
