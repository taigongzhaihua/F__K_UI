#include "fk/ui/Button.h"
#include "fk/binding/DependencyProperty.h"

#include <regex>
#include <iostream>

namespace fk::ui::detail {

// ============================================================================
// ButtonBase 实现
// ============================================================================

ButtonBase::ButtonBase() {
    // Fluent Design 默认样式
    SetPadding(fk::Thickness{12.0f, 8.f, 12.f, 9.f});  // WinUI 3 标准: 左12, 上8, 右12, 下9
    SetCornerRadius(4.0f);  // Fluent 圆角
}

ButtonBase::~ButtonBase() = default;

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)

FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, Background, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, Foreground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, HoveredBackground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, PressedBackground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, BorderBrush, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, BorderThickness, float, 0.0f)
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, IsMouseOver, bool, false)
FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, IsPressed, bool, false)

// Getter/Setter 由宏自动生成

std::string ButtonBase::GetActualBackground() const {
    // 优先使用显式设置的状态颜色
    if (IsPressed()) {
        const std::string& pressedBg = GetPressedBackground();
        if (!pressedBg.empty()) {
            return pressedBg;
        }
    }
    
    if (IsMouseOver()) {
        const std::string& hoveredBg = GetHoveredBackground();
        if (!hoveredBg.empty()) {
            return hoveredBg;
        }
    }
    
    const std::string& baseColor = GetBackground();
    
    // 如果没有状态变化或没有显式状态颜色,直接返回基础颜色
    if (!IsPressed() && !IsMouseOver()) {
        return baseColor;
    }
    
    // 解析基础颜色(用于自动计算状态颜色)
    static const std::regex colorRegex(R"(#([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})?)");
    std::smatch match;
    
    if (!std::regex_match(baseColor, match, colorRegex)) {
        return baseColor; // 格式不正确,返回原值
    }
    
    // 提取 RGB 分量
    int r = std::stoi(match[1].str(), nullptr, 16);
    int g = std::stoi(match[2].str(), nullptr, 16);
    int b = std::stoi(match[3].str(), nullptr, 16);
    int a = match[4].matched ? std::stoi(match[4].str(), nullptr, 16) : 255;
    
    // 根据状态调整颜色 (Fluent Design 风格)
    if (IsPressed()) {
        // 按下状态: 变暗 (乘以 0.8,更温和)
        r = static_cast<int>(r * 0.8f);
        g = static_cast<int>(g * 0.8f);
        b = static_cast<int>(b * 0.8f);
    } else if (IsMouseOver()) {
        // 悬停状态: 微妙变亮 (乘以 1.1)
        r = std::min(255, static_cast<int>(r * 1.1f));
        g = std::min(255, static_cast<int>(g * 1.1f));
        b = std::min(255, static_cast<int>(b * 1.1f));
    }
    
    // 格式化为十六进制字符串
    char buffer[16];
    if (match[4].matched) {
        std::snprintf(buffer, sizeof(buffer), "#%02X%02X%02X%02X", r, g, b, a);
    } else {
        std::snprintf(buffer, sizeof(buffer), "#%02X%02X%02X", r, g, b);
    }
    
    return std::string(buffer);
}

// ============================================================================
// 事件处理
// ============================================================================

bool ButtonBase::OnMouseButtonDown(int button, double x, double y) {
    if (button == 0) { // 左键
        // 检查鼠标是否在按钮边界内（x, y 是局部坐标）
        auto bounds = GetRenderBounds();
        bool isInside = (x >= 0 && x < bounds.width && y >= 0 && y < bounds.height);
        
        if (isInside) {
            // 设置 MouseOver 状态
            SetValue(IsMouseOverProperty(), true);
            OnMouseDown();
            return true; // 按钮处理了事件
        }
        
        return false; // 鼠标不在按钮上，不处理
    }
    return false;
}

bool ButtonBase::OnMouseButtonUp(int button, double x, double y) {
    if (button == 0) { // 左键
        // 检查鼠标是否在按钮边界内（x, y 是局部坐标）
        auto bounds = GetRenderBounds();
        bool isInside = (x >= 0 && x < bounds.width && y >= 0 && y < bounds.height);
        
        // 只有在捕获了鼠标时才处理 MouseUp
        if (isMouseCaptured_) {
            OnMouseUp();
            
            // 根据鼠标位置更新 MouseOver 状态
            SetValue(IsMouseOverProperty(), isInside);
            return true;
        }
        
        // 如果没有捕获鼠标，但鼠标在按钮上，也更新状态
        if (isInside) {
            SetValue(IsMouseOverProperty(), true);
            return true;
        }
        
        return false; // 鼠标不在按钮上，不处理
    }
    return false;
}

bool ButtonBase::OnMouseMove(double x, double y) {
    // 处理鼠标移动事件
    if (!IsMouseOver()) {
        SetValue(IsMouseOverProperty(), true);
        OnMouseEnter();
    }
    return true; // 按钮处理了鼠标移动
}

void ButtonBase::OnClick() {
    Click(*this);  // 使用 operator() 触发事件
}

void ButtonBase::OnMouseEnter() {
    SetValue(IsMouseOverProperty(), true);
}

void ButtonBase::OnMouseLeave() {
    SetValue(IsMouseOverProperty(), false);
    if (isMouseCaptured_) {
        SetValue(IsPressedProperty(), false);
        isMouseCaptured_ = false;
    }
}

void ButtonBase::OnMouseDown() {
    SetValue(IsPressedProperty(), true);
    isMouseCaptured_ = true;
}

void ButtonBase::OnMouseUp() {
    if (isMouseCaptured_) {
        SetValue(IsPressedProperty(), false);
        isMouseCaptured_ = false;

        // 仅在鼠标仍在按钮上时触发点击
        if (IsMouseOver()) {
            OnClick();
        }
    }
}

// ============================================================================
// 属性变更回调
// ============================================================================

void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
    InvalidateVisual();  // 触发重绘
}

void ButtonBase::OnBackgroundChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

void ButtonBase::OnForegroundChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

void ButtonBase::OnHoveredBackgroundChanged(const std::string& oldValue, const std::string& newValue) {
    if (IsMouseOver()) {
        InvalidateVisual();
    }
}

void ButtonBase::OnPressedBackgroundChanged(const std::string& oldValue, const std::string& newValue) {
    if (IsPressed()) {
        InvalidateVisual();
    }
}

void ButtonBase::OnBorderBrushChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

void ButtonBase::OnBorderThicknessChanged(float oldValue, float newValue) {
    InvalidateVisual();
}

void ButtonBase::OnIsMouseOverChanged(bool oldValue, bool newValue) {
    InvalidateVisual();  // 悬停状态变化时重绘
}

void ButtonBase::OnIsPressedChanged(bool oldValue, bool newValue) {
    InvalidateVisual();  // 按下状态变化时重绘
}

// ============================================================================
// 元数据构建
// ============================================================================

binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
    binding::PropertyMetadata metadata(4.0f);  // Fluent 默认圆角
    metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
    metadata.validateCallback = ValidateCornerRadius;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildBackgroundMetadata() {
    binding::PropertyMetadata metadata(std::string("#0078D4"));  // Fluent Accent Blue
    metadata.propertyChangedCallback = BackgroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildForegroundMetadata() {
    binding::PropertyMetadata metadata(std::string("#FFFFFF"));  // 白色文字
    metadata.propertyChangedCallback = ForegroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildHoveredBackgroundMetadata() {
    binding::PropertyMetadata metadata(std::string(""));  // 空字符串表示使用自动计算
    metadata.propertyChangedCallback = HoveredBackgroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildPressedBackgroundMetadata() {
    binding::PropertyMetadata metadata(std::string(""));  // 空字符串表示使用自动计算
    metadata.propertyChangedCallback = PressedBackgroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildBorderBrushMetadata() {
    binding::PropertyMetadata metadata(std::string("#0078D400"));  // 透明边框
    metadata.propertyChangedCallback = BorderBrushPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildBorderThicknessMetadata() {
    binding::PropertyMetadata metadata(1.0f);
    metadata.propertyChangedCallback = BorderThicknessPropertyChanged;
    metadata.validateCallback = ValidateBorderThickness;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildIsMouseOverMetadata() {
    binding::PropertyMetadata metadata(false);
    metadata.propertyChangedCallback = IsMouseOverPropertyChanged;
    return metadata;
}

binding::PropertyMetadata ButtonBase::BuildIsPressedMetadata() {
    binding::PropertyMetadata metadata(false);
    metadata.propertyChangedCallback = IsPressedPropertyChanged;
    return metadata;
}

// 静态回调由宏自动生成

// ============================================================================
// 验证函数
// ============================================================================

bool ButtonBase::ValidateCornerRadius(const std::any& value) {
    if (!value.has_value()) return false;
    try {
        float radius = std::any_cast<float>(value);
        return radius >= 0.0f;  // 圆角半径必须非负
    } catch (...) {
        return false;
    }
}

bool ButtonBase::ValidateColor(const std::any& value) {
    if (!value.has_value()) return false;
    try {
        const auto& colorStr = std::any_cast<const std::string&>(value);
        if (colorStr.empty()) return true;  // 空字符串表示透明
        
        // 验证颜色格式: #RGB, #RRGGBB, #RRGGBBAA
        std::regex colorPattern(R"(^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8})$)");
        return std::regex_match(colorStr, colorPattern);
    } catch (...) {
        return false;
    }
}

bool ButtonBase::ValidateBorderThickness(const std::any& value) {
    if (!value.has_value()) return false;
    try {
        float thickness = std::any_cast<float>(value);
        return thickness >= 0.0f;  // 边框粗细必须非负
    } catch (...) {
        return false;
    }
}

} // namespace fk::ui::detail
