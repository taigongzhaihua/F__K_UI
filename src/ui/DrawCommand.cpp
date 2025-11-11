#include "fk/ui/DrawCommand.h"

namespace fk::ui {

DrawCommand DrawCommand::Rectangle(const Rect& rect, const Color& fill, const Color& stroke, float strokeWidth) {
    DrawCommand cmd(DrawCommandType::Rectangle);
    cmd.bounds = rect;
    cmd.fillColor = fill;
    cmd.strokeColor = stroke;
    cmd.strokeWidth = strokeWidth;
    return cmd;
}

DrawCommand DrawCommand::RoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke) {
    DrawCommand cmd(DrawCommandType::RoundedRect);
    cmd.bounds = rect;
    cmd.radius = radius;
    cmd.fillColor = fill;
    cmd.strokeColor = stroke;
    return cmd;
}

DrawCommand DrawCommand::Circle(const Point& center, float radius, const Color& fill, const Color& stroke) {
    DrawCommand cmd(DrawCommandType::Circle);
    cmd.position = center;
    cmd.radius = radius;
    cmd.fillColor = fill;
    cmd.strokeColor = stroke;
    return cmd;
}

DrawCommand DrawCommand::Line(const Point& start, const Point& end, const Color& color, float width) {
    DrawCommand cmd(DrawCommandType::Line);
    cmd.position = start;
    cmd.bounds = Rect(end.x, end.y, 0, 0); // 使用 bounds 存储终点
    cmd.strokeColor = color;
    cmd.strokeWidth = width;
    return cmd;
}

DrawCommand DrawCommand::Text(const Point& position, const std::string& text, const Color& color, float fontSize) {
    DrawCommand cmd(DrawCommandType::Text);
    cmd.position = position;
    cmd.text = text;
    cmd.fillColor = color;
    cmd.fontSize = fontSize;
    return cmd;
}

DrawCommand DrawCommand::Image(const Rect& bounds, const std::string& path) {
    DrawCommand cmd(DrawCommandType::Image);
    cmd.bounds = bounds;
    cmd.imagePath = path;
    return cmd;
}

DrawCommand DrawCommand::Image(const Rect& bounds, unsigned int textureId, const Color& tint) {
    DrawCommand cmd(DrawCommandType::Image);
    cmd.bounds = bounds;
    cmd.textureId = reinterpret_cast<void*>(static_cast<uintptr_t>(textureId));
    cmd.fillColor = tint;
    return cmd;
}

} // namespace fk::ui
