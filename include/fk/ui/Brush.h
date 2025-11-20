/**
 * @file Brush.h
 * @brief Brush 画刷系统
 * 
 * 职责：
 * - 提供图形填充和描边的画刷抽象
 * - 支持纯色画刷（SolidColorBrush）
 * - 支持线性渐变画刷（LinearGradientBrush）
 * - 支持径向渐变画刷（RadialGradientBrush）
 * - 支持图像画刷（ImageBrush）
 * 
 * WPF 对应：Brush, SolidColorBrush, LinearGradientBrush, RadialGradientBrush, ImageBrush
 */

#pragma once

#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/render/DrawCommand.h"  // 使用 DrawCommand.h 中的 Color 定义
#include "fk/ui/Primitives.h"   // Point
#include <cstdint>
#include <vector>
#include <string>

namespace fk::ui {

// Color 现在在 render 模块中定义
using Color = fk::render::Color;

/**
 * @brief 渐变停止点
 * 
 * 定义渐变中某个位置的颜色。
 */
struct GradientStop {
    Color color;      ///< 该停止点的颜色
    float offset;     ///< 位置偏移（0.0到1.0）
    
    GradientStop() : color(Color::Black()), offset(0.0f) {}
    GradientStop(Color c, float o) : color(c), offset(o) {}
};

/**
 * @brief Brush 抽象基类
 * 
 * 所有画刷的基类，提供颜色和渲染接口。
 */
class Brush : public binding::DependencyObject {
public:
    virtual ~Brush() = default;
    
    /**
     * @brief 获取画刷的主要颜色
     * 
     * 对于纯色画刷返回该颜色，对于渐变画刷返回第一个停止点的颜色。
     */
    virtual Color GetColor() const = 0;
    
    /**
     * @brief 应用画刷到渲染上下文
     * 
     * 派生类实现具体的渲染设置。
     */
    virtual void Apply(class RenderContext* context) = 0;
    
    /**
     * @brief 克隆画刷
     */
    virtual Brush* Clone() const = 0;
    
protected:
    Brush() = default;
};

/**
 * @brief 纯色画刷
 * 
 * 使用单一颜色填充或描边图形。
 */
class SolidColorBrush : public Brush {
public:
    /**
     * @brief 构造函数
     */
    SolidColorBrush() : color_(Color::Black()) {
        SetValue(ColorProperty(), std::any(color_));
    }
    
    explicit SolidColorBrush(Color color) : color_(color) {
        SetValue(ColorProperty(), std::any(color_));
    }
    
    SolidColorBrush(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : color_(Color::FromRGB(r, g, b, a)) {
        SetValue(ColorProperty(), std::any(color_));
    }

    
    
    /**
     * @brief 从 ARGB 整数创建
     */
    static SolidColorBrush* FromArgb(uint32_t argb) {
        uint8_t a = (argb >> 24) & 0xFF;
        uint8_t r = (argb >> 16) & 0xFF;
        uint8_t g = (argb >> 8) & 0xFF;
        uint8_t b = argb & 0xFF;
        return new SolidColorBrush(Color::FromRGB(r, g, b, a));
    }
    
    // ========== 依赖属性 ==========
    
    /// Color 属性
    static const binding::DependencyProperty& ColorProperty();
    
    // ========== 属性访问 ==========
    
    Color GetColor() const override;
    void SetColor(Color value);
    
    SolidColorBrush* SetBrushColor(fk::ui::Color color) {
        SetColor(color);
        return this;
    }
    
    // ========== Brush 接口实现 ==========
    
    void Apply(class RenderContext* context) override;
    Brush* Clone() const override;
    
private:
    fk::ui::Color color_;
};

/**
 * @brief 线性渐变画刷
 * 
 * 在起点和终点之间创建线性颜色渐变。
 */
class LinearGradientBrush : public Brush {
public:
    /**
     * @brief 构造函数
     */
    LinearGradientBrush();
    
    /**
     * @brief 使用两个渐变点创建
     */
    LinearGradientBrush(Color startColor, Color endColor);
    
    /**
     * @brief 使用自定义渐变停止点创建
     */
    explicit LinearGradientBrush(const std::vector<GradientStop>& stops);
    
    // ========== 依赖属性 ==========
    
    /// StartPoint 属性（相对坐标，0-1）
    static const binding::DependencyProperty& StartPointProperty();
    
    /// EndPoint 属性（相对坐标，0-1）
    static const binding::DependencyProperty& EndPointProperty();
    
    // ========== 属性访问 ==========
    
    Point GetStartPoint() const { return startPoint_; }
    void SetStartPoint(Point value) { startPoint_ = value; }
    
    Point GetEndPoint() const { return endPoint_; }
    void SetEndPoint(Point value) { endPoint_ = value; }
    
    /// 添加渐变停止点
    void AddGradientStop(Color color, float offset);
    
    /// 获取渐变停止点列表
    const std::vector<GradientStop>& GetGradientStops() const { return gradientStops_; }
    
    /// 清空渐变停止点
    void ClearGradientStops() { gradientStops_.clear(); }
    
    // ========== Brush 接口实现 ==========
    
    Color GetColor() const override;
    void Apply(class RenderContext* context) override;
    Brush* Clone() const override;
    
private:
    Point startPoint_{0.0f, 0.0f};  ///< 起点（相对坐标）
    Point endPoint_{1.0f, 1.0f};    ///< 终点（相对坐标）
    std::vector<GradientStop> gradientStops_;
};

/**
 * @brief 径向渐变画刷
 * 
 * 从中心点向外创建放射状颜色渐变。
 */
class RadialGradientBrush : public Brush {
public:
    /**
     * @brief 构造函数
     */
    RadialGradientBrush();
    
    /**
     * @brief 使用两个渐变点创建
     */
    RadialGradientBrush(Color centerColor, Color edgeColor);
    
    /**
     * @brief 使用自定义渐变停止点创建
     */
    explicit RadialGradientBrush(const std::vector<GradientStop>& stops);
    
    // ========== 依赖属性 ==========
    
    /// Center 属性（相对坐标，0-1）
    static const binding::DependencyProperty& CenterProperty();
    
    /// RadiusX 属性（相对半径）
    static const binding::DependencyProperty& RadiusXProperty();
    
    /// RadiusY 属性（相对半径）
    static const binding::DependencyProperty& RadiusYProperty();
    
    /// GradientOrigin 属性（渐变焦点，相对坐标）
    static const binding::DependencyProperty& GradientOriginProperty();
    
    // ========== 属性访问 ==========
    
    Point GetCenter() const { return center_; }
    void SetCenter(Point value) { center_ = value; }
    
    float GetRadiusX() const { return radiusX_; }
    void SetRadiusX(float value) { radiusX_ = value; }
    
    float GetRadiusY() const { return radiusY_; }
    void SetRadiusY(float value) { radiusY_ = value; }
    
    Point GetGradientOrigin() const { return gradientOrigin_; }
    void SetGradientOrigin(Point value) { gradientOrigin_ = value; }
    
    /// 添加渐变停止点
    void AddGradientStop(Color color, float offset);
    
    /// 获取渐变停止点列表
    const std::vector<GradientStop>& GetGradientStops() const { return gradientStops_; }
    
    /// 清空渐变停止点
    void ClearGradientStops() { gradientStops_.clear(); }
    
    // ========== Brush 接口实现 ==========
    
    Color GetColor() const override;
    void Apply(class RenderContext* context) override;
    Brush* Clone() const override;
    
private:
    Point center_{0.5f, 0.5f};           ///< 中心点（相对坐标）
    float radiusX_{0.5f};                ///< X方向半径
    float radiusY_{0.5f};                ///< Y方向半径
    Point gradientOrigin_{0.5f, 0.5f};   ///< 渐变焦点（相对坐标）
    std::vector<GradientStop> gradientStops_;
};

/**
 * @brief 图像画刷平铺模式
 */
enum class TileMode {
    None,       ///< 不平铺，拉伸图像
    Tile,       ///< 平铺
    FlipX,      ///< X轴翻转平铺
    FlipY,      ///< Y轴翻转平铺
    FlipXY      ///< XY轴翻转平铺
};

/**
 * @brief 图像画刷
 * 
 * 使用图像作为填充或描边的画刷。
 */
class ImageBrush : public Brush {
public:
    /**
     * @brief 构造函数
     */
    ImageBrush();
    
    /**
     * @brief 从文件路径创建
     */
    explicit ImageBrush(const std::string& imagePath);
    
    /**
     * @brief 从纹理ID创建
     */
    explicit ImageBrush(unsigned int textureId);
    
    // ========== 依赖属性 ==========
    
    /// ImageSource 属性（图像路径）
    static const binding::DependencyProperty& ImageSourceProperty();
    
    /// TileMode 属性（平铺模式）
    static const binding::DependencyProperty& TileModeProperty();
    
    // ========== 属性访问 ==========
    
    std::string GetImageSource() const { return imageSource_; }
    void SetImageSource(const std::string& value);
    
    TileMode GetTileMode() const { return tileMode_; }
    void SetTileMode(TileMode value) { tileMode_ = value; }
    
    unsigned int GetTextureId() const { return textureId_; }
    
    // ========== Brush 接口实现 ==========
    
    Color GetColor() const override;
    void Apply(class RenderContext* context) override;
    Brush* Clone() const override;
    
private:
    std::string imageSource_;           ///< 图像源路径
    unsigned int textureId_{0};         ///< OpenGL纹理ID
    TileMode tileMode_{TileMode::None}; ///< 平铺模式
    
    /// 加载图像到纹理
    void LoadTexture();
};

/**
 * @brief 预定义画刷（类似 WPF Brushes 类）
 */
class Brushes {
public:
    static Brush* Black() { return new SolidColorBrush(Color::Black()); }
    static Brush* White() { return new SolidColorBrush(Color::White()); }
    static Brush* Red() { return new SolidColorBrush(Color::Red()); }
    static Brush* Green() { return new SolidColorBrush(Color::Green()); }
    static Brush* Blue() { return new SolidColorBrush(Color::Blue()); }
    static Brush* Yellow() { return new SolidColorBrush(Color::Yellow()); }
    static Brush* Cyan() { return new SolidColorBrush(Color::Cyan()); }
    static Brush* Magenta() { return new SolidColorBrush(Color::Magenta()); }
    static Brush* Transparent() { return new SolidColorBrush(Color::Transparent()); }
    static Brush* Gray() { return new SolidColorBrush(Color::Gray()); }
    static Brush* LightGray() { return new SolidColorBrush(Color::LightGray()); }
    static Brush* DarkGray() { return new SolidColorBrush(Color::DarkGray()); }
};

} // namespace fk::ui
