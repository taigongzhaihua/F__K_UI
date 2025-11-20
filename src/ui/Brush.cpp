/**
 * @file Brush.cpp
 * @brief Brush 画刷系统实现
 */

#include "fk/ui/Brush.h"
#include <any>
#include <algorithm>
#include <cmath>

// stb_image 用于图像加载（头文件已在Image.cpp中实现）
#include "stb_image.h"

namespace fk::ui {

// ========== SolidColorBrush 依赖属性 ==========

const binding::DependencyProperty& SolidColorBrush::ColorProperty() {
    static auto& property = 
        DependencyProperty::Register(
            "Color",
            typeid(Color),
            typeid(SolidColorBrush),
            binding::PropertyMetadata{std::any(Color::Black())}
        );
    return property;
}

// ========== SolidColorBrush 实现 ==========

Color SolidColorBrush::GetColor() const {
    // 优先从依赖属性读取（确保动画能正确更新颜色）
    // 如果依赖属性未设置，则使用成员变量
    try {
        auto value = GetValue(ColorProperty());
        if (value.has_value()) {
            auto color = std::any_cast<Color>(value);
            // 检查是否是默认值（黑色），如果是且成员变量不是黑色，则使用成员变量
            if (color == Color::Black() && color_ != Color::Black()) {
                return color_;
            }
            return color;
        }
    } catch (...) {
        // Fallback to member variable
    }
    return color_;
}

void SolidColorBrush::SetColor(Color value) {
    auto oldColor = color_;
    color_ = value;
    SetValue(ColorProperty(), std::any(value));
    
    // 触发属性变化事件，让使用此画刷的UI元素知道需要重绘
    // 注意：SetValue 内部会触发 PropertyChanged 事件
}

void SolidColorBrush::Apply(RenderContext* context) {
    if (!context) return;
    
    // 将颜色应用到渲染上下文
    // 这里调用渲染后端设置颜色
    // context->SetColor(color_);
    
    // 注意：实际的渲染设置由 Shape::OnRender() 中处理
    // 这个方法主要用于未来扩展（如渐变画刷）
}

Brush* SolidColorBrush::Clone() const {
    return new SolidColorBrush(color_);
}

// ========== LinearGradientBrush 实现 ==========

LinearGradientBrush::LinearGradientBrush() {
    // 默认黑到白渐变
    gradientStops_.push_back(GradientStop(Color::Black(), 0.0f));
    gradientStops_.push_back(GradientStop(Color::White(), 1.0f));
}

LinearGradientBrush::LinearGradientBrush(Color startColor, Color endColor) {
    gradientStops_.push_back(GradientStop(startColor, 0.0f));
    gradientStops_.push_back(GradientStop(endColor, 1.0f));
}

LinearGradientBrush::LinearGradientBrush(const std::vector<GradientStop>& stops)
    : gradientStops_(stops) {
}

const binding::DependencyProperty& LinearGradientBrush::StartPointProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "StartPoint",
            typeid(Point),
            typeid(LinearGradientBrush),
            binding::PropertyMetadata{std::any(Point(0.0f, 0.0f))}
        );
    return property;
}

const binding::DependencyProperty& LinearGradientBrush::EndPointProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "EndPoint",
            typeid(Point),
            typeid(LinearGradientBrush),
            binding::PropertyMetadata{std::any(Point(1.0f, 1.0f))}
        );
    return property;
}

void LinearGradientBrush::AddGradientStop(Color color, float offset) {
    gradientStops_.push_back(GradientStop(color, offset));
    // 按offset排序
    std::sort(gradientStops_.begin(), gradientStops_.end(), 
              [](const GradientStop& a, const GradientStop& b) {
                  return a.offset < b.offset;
              });
}

Color LinearGradientBrush::GetColor() const {
    if (gradientStops_.empty()) {
        return Color::Black();
    }
    return gradientStops_[0].color;
}

void LinearGradientBrush::Apply(RenderContext* context) {
    // 线性渐变的应用由渲染器处理
    // 这里可以设置渐变参数到上下文
}

Brush* LinearGradientBrush::Clone() const {
    auto* clone = new LinearGradientBrush(gradientStops_);
    clone->SetStartPoint(startPoint_);
    clone->SetEndPoint(endPoint_);
    return clone;
}

// ========== RadialGradientBrush 实现 ==========

RadialGradientBrush::RadialGradientBrush() {
    // 默认中心白色到边缘黑色
    gradientStops_.push_back(GradientStop(Color::White(), 0.0f));
    gradientStops_.push_back(GradientStop(Color::Black(), 1.0f));
}

RadialGradientBrush::RadialGradientBrush(Color centerColor, Color edgeColor) {
    gradientStops_.push_back(GradientStop(centerColor, 0.0f));
    gradientStops_.push_back(GradientStop(edgeColor, 1.0f));
}

RadialGradientBrush::RadialGradientBrush(const std::vector<GradientStop>& stops)
    : gradientStops_(stops) {
}

const binding::DependencyProperty& RadialGradientBrush::CenterProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "Center",
            typeid(Point),
            typeid(RadialGradientBrush),
            binding::PropertyMetadata{std::any(Point(0.5f, 0.5f))}
        );
    return property;
}

const binding::DependencyProperty& RadialGradientBrush::RadiusXProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "RadiusX",
            typeid(float),
            typeid(RadialGradientBrush),
            binding::PropertyMetadata{std::any(0.5f)}
        );
    return property;
}

const binding::DependencyProperty& RadialGradientBrush::RadiusYProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "RadiusY",
            typeid(float),
            typeid(RadialGradientBrush),
            binding::PropertyMetadata{std::any(0.5f)}
        );
    return property;
}

const binding::DependencyProperty& RadialGradientBrush::GradientOriginProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "GradientOrigin",
            typeid(Point),
            typeid(RadialGradientBrush),
            binding::PropertyMetadata{std::any(Point(0.5f, 0.5f))}
        );
    return property;
}

void RadialGradientBrush::AddGradientStop(Color color, float offset) {
    gradientStops_.push_back(GradientStop(color, offset));
    // 按offset排序
    std::sort(gradientStops_.begin(), gradientStops_.end(), 
              [](const GradientStop& a, const GradientStop& b) {
                  return a.offset < b.offset;
              });
}

Color RadialGradientBrush::GetColor() const {
    if (gradientStops_.empty()) {
        return Color::Black();
    }
    return gradientStops_[0].color;
}

void RadialGradientBrush::Apply(RenderContext* context) {
    // 径向渐变的应用由渲染器处理
}

Brush* RadialGradientBrush::Clone() const {
    auto* clone = new RadialGradientBrush(gradientStops_);
    clone->SetCenter(center_);
    clone->SetRadiusX(radiusX_);
    clone->SetRadiusY(radiusY_);
    clone->SetGradientOrigin(gradientOrigin_);
    return clone;
}

// ========== ImageBrush 实现 ==========

ImageBrush::ImageBrush() {
}

ImageBrush::ImageBrush(const std::string& imagePath)
    : imageSource_(imagePath) {
    LoadTexture();
}

ImageBrush::ImageBrush(unsigned int textureId)
    : textureId_(textureId) {
}

const binding::DependencyProperty& ImageBrush::ImageSourceProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "ImageSource",
            typeid(std::string),
            typeid(ImageBrush),
            binding::PropertyMetadata{std::any(std::string())}
        );
    return property;
}

const binding::DependencyProperty& ImageBrush::TileModeProperty() {
    static auto& property = 
        binding::DependencyProperty::Register(
            "TileMode",
            typeid(TileMode),
            typeid(ImageBrush),
            binding::PropertyMetadata{std::any(TileMode::None)}
        );
    return property;
}

void ImageBrush::SetImageSource(const std::string& value) {
    imageSource_ = value;
    LoadTexture();
}

void ImageBrush::LoadTexture() {
    if (imageSource_.empty()) {
        return;
    }
    
    // 使用stb_image加载图像
    int width, height, channels;
    unsigned char* data = stbi_load(imageSource_.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        // 加载失败
        return;
    }
    
    // 创建OpenGL纹理（注意：这需要OpenGL上下文）
    // 实际实现中应该通过渲染器创建纹理
    // 这里简化处理
    
    // 清理
    stbi_image_free(data);
}

Color ImageBrush::GetColor() const {
    // 图像画刷返回白色作为主色
    return Color::White();
}

void ImageBrush::Apply(RenderContext* context) {
    // 图像画刷的应用由渲染器处理
}

Brush* ImageBrush::Clone() const {
    auto* clone = new ImageBrush(imageSource_);
    clone->SetTileMode(tileMode_);
    return clone;
}

} // namespace fk::ui
