#pragma once

#include "fk/ui/Primitives.h"
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief 绘制命令类型
 */
enum class DrawCommandType {
    Rectangle,      // 矩形
    RoundedRect,    // 圆角矩形
    Circle,         // 圆形
    Line,           // 线条
    Text,           // 文本
    Image,          // 图像
    Custom          // 自定义
};

/**
 * @brief 颜色（RGBA）
 */
struct Color {
    float r{0}, g{0}, b{0}, a{1};
    
    Color() = default;
    Color(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}
    
    static Color FromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
    }
    
    static Color White() { return Color(1, 1, 1, 1); }
    static Color Black() { return Color(0, 0, 0, 1); }
    static Color Red() { return Color(1, 0, 0, 1); }
    static Color Green() { return Color(0, 1, 0, 1); }
    static Color Blue() { return Color(0, 0, 1, 1); }
    static Color Transparent() { return Color(0, 0, 0, 0); }
};

/**
 * @brief 绘制命令
 * 
 * 职责：
 * - 封装单个绘制操作的所有参数
 * - 支持多种绘制类型
 */
class DrawCommand {
public:
    DrawCommandType type;
    
    // 几何属性
    Rect bounds;
    Point position;
    Size size;
    float radius{0};          // 圆角半径或圆形半径
    float strokeWidth{1.0f};  // 线宽
    
    // 颜色属性
    Color fillColor{Color::Transparent()};
    Color strokeColor{Color::Black()};
    
    // 文本属性
    std::string text;
    float fontSize{14.0f};
    
    // 图像属性
    std::string imagePath;
    void* textureId{nullptr};
    
    // 自定义绘制
    std::function<void()> customDraw;
    
    // 变换矩阵
    Matrix3x2 transform{Matrix3x2::Identity()};
    
    DrawCommand(DrawCommandType t) : type(t) {}
    
    // 工厂方法
    static DrawCommand Rectangle(const Rect& rect, const Color& fill, const Color& stroke = Color::Transparent(), float strokeWidth = 1.0f);
    static DrawCommand RoundedRectangle(const Rect& rect, float radius, const Color& fill, const Color& stroke = Color::Transparent());
    static DrawCommand Circle(const Point& center, float radius, const Color& fill, const Color& stroke = Color::Transparent());
    static DrawCommand Line(const Point& start, const Point& end, const Color& color, float width = 1.0f);
    static DrawCommand Text(const Point& position, const std::string& text, const Color& color, float fontSize = 14.0f);
    static DrawCommand Image(const Rect& bounds, const std::string& path);
};

} // namespace fk::ui
