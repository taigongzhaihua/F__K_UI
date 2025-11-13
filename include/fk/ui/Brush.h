/**
 * @file Brush.h
 * @brief Brush 画刷系统
 * 
 * 职责：
 * - 提供图形填充和描边的画刷抽象
 * - 支持纯色画刷（SolidColorBrush）
 * 
 * WPF 对应：Brush, SolidColorBrush
 */

#pragma once

#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/ui/DrawCommand.h"  // 使用 DrawCommand.h 中的 Color 定义
#include <cstdint>

namespace fk::ui {

// Color 在 DrawCommand.h 中已定义，这里不需要重复定义

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
    SolidColorBrush() : color_(Color::Black()) {}
    
    explicit SolidColorBrush(Color color) : color_(color) {}
    
    SolidColorBrush(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : color_(Color::FromRGB(r, g, b, a)) {}
    
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
