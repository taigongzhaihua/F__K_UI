#include "fk/ui/TextBlock.h"
#include "fk/binding/DependencyProperty.h"

#include <regex>

namespace fk::ui::detail {

// ============================================================================
// TextBlockBase 实现
// ============================================================================

TextBlockBase::TextBlockBase() = default;
TextBlockBase::~TextBlockBase() = default;

// ============================================================================
// 依赖属性注册
// ============================================================================

const binding::DependencyProperty& TextBlockBase::TextProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Text",
        typeid(std::string),
        typeid(TextBlockBase),
        BuildTextMetadata()
    );
    return property;
}

const binding::DependencyProperty& TextBlockBase::ForegroundProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "Foreground",
        typeid(std::string),
        typeid(TextBlockBase),
        BuildForegroundMetadata()
    );
    return property;
}

const binding::DependencyProperty& TextBlockBase::FontSizeProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "FontSize",
        typeid(float),
        typeid(TextBlockBase),
        BuildFontSizeMetadata()
    );
    return property;
}

const binding::DependencyProperty& TextBlockBase::FontFamilyProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "FontFamily",
        typeid(std::string),
        typeid(TextBlockBase),
        BuildFontFamilyMetadata()
    );
    return property;
}

// ============================================================================
// 属性访问器
// ============================================================================

void TextBlockBase::SetText(const std::string& text) {
    SetValue(TextProperty(), text);
}

const std::string& TextBlockBase::GetText() const {
    const auto& value = GetValue(TextProperty());
    if (!value.has_value()) {
        static const std::string empty;
        return empty;
    }
    return std::any_cast<const std::string&>(value);
}

void TextBlockBase::SetForeground(const std::string& color) {
    SetValue(ForegroundProperty(), color);
}

const std::string& TextBlockBase::GetForeground() const {
    const auto& value = GetValue(ForegroundProperty());
    if (!value.has_value()) {
        static const std::string defaultColor = "#000000";
        return defaultColor;
    }
    return std::any_cast<const std::string&>(value);
}

void TextBlockBase::SetFontSize(float size) {
    SetValue(FontSizeProperty(), size);
}

float TextBlockBase::GetFontSize() const {
    const auto& value = GetValue(FontSizeProperty());
    if (!value.has_value()) return 14.0f;
    return std::any_cast<float>(value);
}

void TextBlockBase::SetFontFamily(const std::string& family) {
    SetValue(FontFamilyProperty(), family);
}

const std::string& TextBlockBase::GetFontFamily() const {
    const auto& value = GetValue(FontFamilyProperty());
    if (!value.has_value()) {
        static const std::string defaultFamily = "Arial";
        return defaultFamily;
    }
    return std::any_cast<const std::string&>(value);
}

// ============================================================================
// 布局重写
// ============================================================================

Size TextBlockBase::MeasureOverride(const Size& availableSize) {
    const auto& text = GetText();
    float fontSize = GetFontSize();
    
    if (text.empty()) {
        return Size(0.0f, fontSize * 1.2f);
    }
    
    // 改进的文本宽度估算
    // 遍历 UTF-8 字符串,区分中文和 ASCII
    float estimatedWidth = 0.0f;
    size_t i = 0;
    while (i < text.length()) {
        unsigned char c = text[i];
        
        if (c < 0x80) {
            // ASCII 字符: 约 0.5 倍字体大小
            estimatedWidth += fontSize * 0.5f;
            i++;
        } else if (c < 0xE0) {
            // 2 字节 UTF-8
            estimatedWidth += fontSize * 0.9f;
            i += 2;
        } else if (c < 0xF0) {
            // 3 字节 UTF-8 (中文等): 接近字体大小
            estimatedWidth += fontSize;
            i += 3;
        } else {
            // 4 字节 UTF-8
            estimatedWidth += fontSize;
            i += 4;
        }
    }
    
    float estimatedHeight = fontSize * 1.2f; // 行高
    
    return Size(
        std::min(estimatedWidth, availableSize.width),
        std::min(estimatedHeight, availableSize.height)
    );
}

Size TextBlockBase::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

bool TextBlockBase::HasRenderContent() const {
    // TextBlock 始终有渲染内容(即使文本为空)
    return true;
}

// ============================================================================
// 属性变更回调
// ============================================================================

void TextBlockBase::OnTextChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateMeasure();  // 文本变化需要重新测量
    InvalidateVisual();
}

void TextBlockBase::OnForegroundChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateVisual();
}

void TextBlockBase::OnFontSizeChanged(float oldValue, float newValue) {
    InvalidateMeasure();  // 字体大小变化需要重新测量
    InvalidateVisual();
}

void TextBlockBase::OnFontFamilyChanged(const std::string& oldValue, const std::string& newValue) {
    InvalidateMeasure();  // 字体变化需要重新测量
    InvalidateVisual();
}

// ============================================================================
// 元数据构建
// ============================================================================

binding::PropertyMetadata TextBlockBase::BuildTextMetadata() {
    binding::PropertyMetadata metadata(std::string(""));  // 默认空文本
    metadata.propertyChangedCallback = TextPropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBlockBase::BuildForegroundMetadata() {
    binding::PropertyMetadata metadata(std::string("#000000"));  // 默认黑色
    metadata.propertyChangedCallback = ForegroundPropertyChanged;
    metadata.validateCallback = ValidateColor;
    return metadata;
}

binding::PropertyMetadata TextBlockBase::BuildFontSizeMetadata() {
    binding::PropertyMetadata metadata(14.0f);  // 默认 14px
    metadata.propertyChangedCallback = FontSizePropertyChanged;
    metadata.validateCallback = ValidateFontSize;
    return metadata;
}

binding::PropertyMetadata TextBlockBase::BuildFontFamilyMetadata() {
    binding::PropertyMetadata metadata(std::string("Arial"));  // 默认 Arial
    metadata.propertyChangedCallback = FontFamilyPropertyChanged;
    return metadata;
}

// ============================================================================
// 静态属性变更回调
// ============================================================================

void TextBlockBase::TextPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    const auto& oldText = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string("");
    const auto& newText = std::any_cast<const std::string&>(newValue);
    textBlock->OnTextChanged(oldText, newText);
}

void TextBlockBase::ForegroundPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    const auto& oldColor = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string("#000000");
    const auto& newColor = std::any_cast<const std::string&>(newValue);
    textBlock->OnForegroundChanged(oldColor, newColor);
}

void TextBlockBase::FontSizePropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    float oldSize = oldValue.has_value() ? std::any_cast<float>(oldValue) : 14.0f;
    float newSize = std::any_cast<float>(newValue);
    textBlock->OnFontSizeChanged(oldSize, newSize);
}

void TextBlockBase::FontFamilyPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    const auto& oldFamily = oldValue.has_value() ? std::any_cast<const std::string&>(oldValue) : std::string("Arial");
    const auto& newFamily = std::any_cast<const std::string&>(newValue);
    textBlock->OnFontFamilyChanged(oldFamily, newFamily);
}

// ============================================================================
// 验证回调
// ============================================================================

bool TextBlockBase::ValidateColor(const std::any& value) {
    if (!value.has_value()) return false;
    
    try {
        const auto& color = std::any_cast<const std::string&>(value);
        
        // 验证颜色格式: #RGB, #RGBA, #RRGGBB, #RRGGBBAA
        std::regex colorRegex(R"(^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{4}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8})$)");
        return std::regex_match(color, colorRegex);
    } catch (...) {
        return false;
    }
}

bool TextBlockBase::ValidateFontSize(const std::any& value) {
    if (!value.has_value()) return false;
    
    try {
        float size = std::any_cast<float>(value);
        return size > 0.0f && size <= 1000.0f;  // 合理的字体大小范围
    } catch (...) {
        return false;
    }
}

} // namespace fk::ui::detail
