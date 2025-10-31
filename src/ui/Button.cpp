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
    SetPadding(fk::Thickness{11.0f, 5.0f});  // 水平 11px, 垂直 5px
    SetCornerRadius(4.0f);  // Fluent 圆角
}

ButtonBase::~ButtonBase() = default;

// ============================================================================
// 依赖属性注册
// ============================================================================

const binding::DependencyProperty& ButtonBase::CornerRadiusProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "CornerRadius",
        typeid(float),
        typeid(ButtonBase),
        BuildCornerRadiusMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::BackgroundProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Background",
        typeid(std::string),
        typeid(ButtonBase),
        BuildBackgroundMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::ForegroundProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Foreground",
        typeid(std::string),
        typeid(ButtonBase),
        BuildForegroundMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::HoveredBackgroundProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "HoveredBackground",
        typeid(std::string),
        typeid(ButtonBase),
        BuildHoveredBackgroundMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::PressedBackgroundProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "PressedBackground",
        typeid(std::string),
        typeid(ButtonBase),
        BuildPressedBackgroundMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::BorderBrushProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "BorderBrush",
        typeid(std::string),
        typeid(ButtonBase),
        BuildBorderBrushMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::BorderThicknessProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "BorderThickness",
        typeid(float),
        typeid(ButtonBase),
        BuildBorderThicknessMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::IsMouseOverProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "IsMouseOver",
        typeid(bool),
        typeid(ButtonBase),
        BuildIsMouseOverMetadata()
    );
    return property;
}

const binding::DependencyProperty& ButtonBase::IsPressedProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "IsPressed",
        typeid(bool),
        typeid(ButtonBase),
        BuildIsPressedMetadata()
    );
    return property;
}

// ============================================================================
// 属性访问器
// ============================================================================

void ButtonBase::SetCornerRadius(float radius) {
    SetValue(CornerRadiusProperty(), radius);
}

float ButtonBase::GetCornerRadius() const {
    const auto& value = GetValue(CornerRadiusProperty());
    if (!value.has_value()) return 0.0f;
    try {
        return std::any_cast<float>(value);
    } catch (...) {
        return 0.0f;
    }
}

void ButtonBase::SetBackground(std::string color) {
    SetValue(BackgroundProperty(), std::move(color));
}

const std::string& ButtonBase::GetBackground() const {
    const auto& value = GetValue(BackgroundProperty());
    static const std::string empty;
    if (!value.has_value()) return empty;
    try {
        return std::any_cast<const std::string&>(value);
    } catch (...) {
        return empty;
    }
}

void ButtonBase::SetForeground(std::string color) {
    SetValue(ForegroundProperty(), std::move(color));
}

const std::string& ButtonBase::GetForeground() const {
    const auto& value = GetValue(ForegroundProperty());
    static const std::string empty;
    if (!value.has_value()) return empty;
    try {
        return std::any_cast<const std::string&>(value);
    } catch (...) {
        return empty;
    }
}

void ButtonBase::SetHoveredBackground(std::string color) {
    SetValue(HoveredBackgroundProperty(), std::move(color));
}

const std::string& ButtonBase::GetHoveredBackground() const {
    const auto& value = GetValue(HoveredBackgroundProperty());
    static const std::string empty;
    if (!value.has_value()) return empty;
    try {
        return std::any_cast<const std::string&>(value);
    } catch (...) {
        return empty;
    }
}

void ButtonBase::SetPressedBackground(std::string color) {
    SetValue(PressedBackgroundProperty(), std::move(color));
}

const std::string& ButtonBase::GetPressedBackground() const {
    const auto& value = GetValue(PressedBackgroundProperty());
    static const std::string empty;
    if (!value.has_value()) return empty;
    try {
        return std::any_cast<const std::string&>(value);
    } catch (...) {
        return empty;
    }
}

void ButtonBase::SetBorderBrush(std::string color) {
    SetValue(BorderBrushProperty(), std::move(color));
}

const std::string& ButtonBase::GetBorderBrush() const {
    const auto& value = GetValue(BorderBrushProperty());
    static const std::string empty;
    if (!value.has_value()) return empty;
    try {
        return std::any_cast<const std::string&>(value);
    } catch (...) {
        return empty;
    }
}

void ButtonBase::SetBorderThickness(float thickness) {
    SetValue(BorderThicknessProperty(), thickness);
}

float ButtonBase::GetBorderThickness() const {
    const auto& value = GetValue(BorderThicknessProperty());
    if (!value.has_value()) return 0.0f;
    try {
        return std::any_cast<float>(value);
    } catch (...) {
        return 0.0f;
    }
}

bool ButtonBase::IsMouseOver() const {
    const auto& value = GetValue(IsMouseOverProperty());
    if (!value.has_value()) return false;
    try {
        return std::any_cast<bool>(value);
    } catch (...) {
        return false;
    }
}

bool ButtonBase::IsPressed() const {
    const auto& value = GetValue(IsPressedProperty());
    if (!value.has_value()) return false;
    try {
        return std::any_cast<bool>(value);
    } catch (...) {
        return false;
    }
}

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

void ButtonBase::OnMouseButtonDown(int button, double x, double y) {
    if (button == 0) { // 左键
        // 设置 MouseOver 状态
        SetValue(IsMouseOverProperty(), true);
        OnMouseDown();
    }
}

void ButtonBase::OnMouseButtonUp(int button, double x, double y) {
    if (button == 0) { // 左键
        OnMouseUp();
    }
}

void ButtonBase::OnMouseMove(double x, double y) {
    // 处理鼠标移动事件
    if (!IsMouseOver()) {
        SetValue(IsMouseOverProperty(), true);
        OnMouseEnter();
    }
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

// ============================================================================
// 静态回调
// ============================================================================

void ButtonBase::CornerRadiusPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        float oldRadius = oldValue.has_value() ? std::any_cast<float>(oldValue) : 0.0f;
        float newRadius = std::any_cast<float>(newValue);
        button->OnCornerRadiusChanged(oldRadius, newRadius);
    } catch (...) {}
}

void ButtonBase::BackgroundPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string();
        const auto& newColor = std::any_cast<const std::string&>(newValue);
        button->OnBackgroundChanged(oldColor, newColor);
    } catch (...) {}
}

void ButtonBase::ForegroundPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string();
        const auto& newColor = std::any_cast<const std::string&>(newValue);
        button->OnForegroundChanged(oldColor, newColor);
    } catch (...) {}
}

void ButtonBase::HoveredBackgroundPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string();
        const auto& newColor = std::any_cast<const std::string&>(newValue);
        button->OnHoveredBackgroundChanged(oldColor, newColor);
    } catch (...) {}
}

void ButtonBase::PressedBackgroundPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string();
        const auto& newColor = std::any_cast<const std::string&>(newValue);
        button->OnPressedBackgroundChanged(oldColor, newColor);
    } catch (...) {}
}

void ButtonBase::BorderBrushPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string();
        const auto& newColor = std::any_cast<const std::string&>(newValue);
        button->OnBorderBrushChanged(oldColor, newColor);
    } catch (...) {}
}

void ButtonBase::BorderThicknessPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        float oldThickness = oldValue.has_value() ? std::any_cast<float>(oldValue) : 0.0f;
        float newThickness = std::any_cast<float>(newValue);
        button->OnBorderThicknessChanged(oldThickness, newThickness);
    } catch (...) {}
}

void ButtonBase::IsMouseOverPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        bool oldState = oldValue.has_value() ? std::any_cast<bool>(oldValue) : false;
        bool newState = std::any_cast<bool>(newValue);
        button->OnIsMouseOverChanged(oldState, newState);
    } catch (...) {}
}

void ButtonBase::IsPressedPropertyChanged(binding::DependencyObject& sender,
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) {
    auto* button = dynamic_cast<ButtonBase*>(&sender);
    if (!button) return;

    try {
        bool oldState = oldValue.has_value() ? std::any_cast<bool>(oldValue) : false;
        bool newState = std::any_cast<bool>(newValue);
        button->OnIsPressedChanged(oldState, newState);
    } catch (...) {}
}

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
