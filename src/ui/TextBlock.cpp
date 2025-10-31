#include "fk/ui/TextBlock.h"
#include "fk/binding/DependencyProperty.h"

#include <regex>
#include <cmath>
#include <algorithm>

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

const binding::DependencyProperty& TextBlockBase::TextWrappingProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "TextWrapping",
        typeid(TextWrapping),
        typeid(TextBlockBase),
        BuildTextWrappingMetadata()
    );
    return property;
}

const binding::DependencyProperty& TextBlockBase::TextTrimmingProperty() {
    static const auto& property = binding::DependencyProperty::Register(
        "TextTrimming",
        typeid(TextTrimming),
        typeid(TextBlockBase),
        BuildTextTrimmingMetadata()
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

void TextBlockBase::SetTextWrapping(TextWrapping wrapping) {
    SetValue(TextWrappingProperty(), wrapping);
}

TextWrapping TextBlockBase::GetTextWrapping() const {
    const auto& value = GetValue(TextWrappingProperty());
    if (!value.has_value()) return TextWrapping::NoWrap;
    return std::any_cast<TextWrapping>(value);
}

void TextBlockBase::SetTextTrimming(TextTrimming trimming) {
    SetValue(TextTrimmingProperty(), trimming);
}

TextTrimming TextBlockBase::GetTextTrimming() const {
    const auto& value = GetValue(TextTrimmingProperty());
    if (!value.has_value()) return TextTrimming::None;
    return std::any_cast<TextTrimming>(value);
}

// ============================================================================
// 布局重写
// ============================================================================

Size TextBlockBase::MeasureOverride(const Size& availableSize) {
    const auto& text = GetText();
    float fontSize = GetFontSize();
    float lineHeight = fontSize * 1.2f;
    
    if (text.empty()) {
        return Size(0.0f, lineHeight);
    }
    
    TextWrapping wrapping = GetTextWrapping();
    TextTrimming trimming = GetTextTrimming();
    
    // 不换行模式:单行计算
    if (wrapping == TextWrapping::NoWrap) {
        float estimatedWidth = 0.0f;
        size_t i = 0;
        while (i < text.length()) {
            unsigned char c = text[i];
            
            if (c < 0x80) {
                estimatedWidth += fontSize * 0.5f;
                i++;
            } else if (c < 0xE0) {
                estimatedWidth += fontSize * 0.9f;
                i += 2;
            } else if (c < 0xF0) {
                estimatedWidth += fontSize;
                i += 3;
            } else {
                estimatedWidth += fontSize;
                i += 4;
            }
        }
        
        // 如果需要截断且文本超出宽度
        if (trimming != TextTrimming::None && estimatedWidth > availableSize.width && availableSize.width > 0.0f) {
            // 计算省略号宽度 "..."
            float ellipsisWidth = fontSize * 0.5f * 3;  // 3个点的宽度
            float maxTextWidth = availableSize.width - ellipsisWidth;
            
            // 重新计算可以显示的字符
            wrappedLines_.clear();
            std::string trimmedText;
            float currentWidth = 0.0f;
            size_t j = 0;
            
            while (j < text.length() && currentWidth < maxTextWidth) {
                unsigned char c = text[j];
                float charWidth;
                size_t charSize;
                
                if (c < 0x80) {
                    charWidth = fontSize * 0.5f;
                    charSize = 1;
                } else if (c < 0xE0) {
                    charWidth = fontSize * 0.9f;
                    charSize = 2;
                } else if (c < 0xF0) {
                    charWidth = fontSize;
                    charSize = 3;
                } else {
                    charWidth = fontSize;
                    charSize = 4;
                }
                
                if (currentWidth + charWidth > maxTextWidth) {
                    break;
                }
                
                trimmedText.append(text, j, charSize);
                currentWidth += charWidth;
                j += charSize;
            }
            
            // 添加省略号
            trimmedText += "...";
            wrappedLines_.push_back(trimmedText);
            
            return Size(availableSize.width, std::min(lineHeight, availableSize.height));
        } else {
            // 不需要截断,存储完整文本
            wrappedLines_.clear();
            wrappedLines_.push_back(text);
        }
        
        return Size(
            std::min(estimatedWidth, availableSize.width),
            std::min(lineHeight, availableSize.height)
        );
    }
    
    // 自动换行模式:多行计算
    float maxLineWidth = availableSize.width;
    if (maxLineWidth <= 0.0f || std::isinf(maxLineWidth)) {
        maxLineWidth = 1000.0f;  // 默认最大宽度
    }
    
    // 按字符分割并计算每行
    wrappedLines_.clear();
    std::string currentLine;
    float currentLineWidth = 0.0f;
    float maxWidth = 0.0f;
    
    size_t i = 0;
    while (i < text.length()) {
        unsigned char c = text[i];
        float charWidth;
        size_t charSize;
        
        // 计算当前字符宽度和字节数
        if (c < 0x80) {
            charWidth = fontSize * 0.5f;
            charSize = 1;
        } else if (c < 0xE0) {
            charWidth = fontSize * 0.9f;
            charSize = 2;
        } else if (c < 0xF0) {
            charWidth = fontSize;
            charSize = 3;
        } else {
            charWidth = fontSize;
            charSize = 4;
        }
        
        // 检查是否需要换行
        if (currentLineWidth + charWidth > maxLineWidth && !currentLine.empty()) {
            wrappedLines_.push_back(currentLine);
            maxWidth = std::max(maxWidth, currentLineWidth);
            currentLine.clear();
            currentLineWidth = 0.0f;
        }
        
        // 添加字符到当前行
        currentLine.append(text, i, charSize);
        currentLineWidth += charWidth;
        i += charSize;
    }
    
    // 添加最后一行
    if (!currentLine.empty()) {
        wrappedLines_.push_back(currentLine);
        maxWidth = std::max(maxWidth, currentLineWidth);
    }
    
    // 如果没有行,添加一个空行以保持高度
    if (wrappedLines_.empty()) {
        wrappedLines_.push_back("");
    }
    
    float totalHeight = lineHeight * wrappedLines_.size();
    
    return Size(
        std::min(maxWidth, availableSize.width),
        std::min(totalHeight, availableSize.height)
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

void TextBlockBase::OnTextWrappingChanged(TextWrapping oldValue, TextWrapping newValue) {
    InvalidateMeasure();  // 换行模式变化需要重新测量
    InvalidateVisual();
}

void TextBlockBase::OnTextTrimmingChanged(TextTrimming oldValue, TextTrimming newValue) {
    InvalidateMeasure();  // 截断模式变化需要重新测量
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

binding::PropertyMetadata TextBlockBase::BuildTextWrappingMetadata() {
    binding::PropertyMetadata metadata(TextWrapping::NoWrap);  // 默认不换行
    metadata.propertyChangedCallback = TextWrappingPropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBlockBase::BuildTextTrimmingMetadata() {
    binding::PropertyMetadata metadata(TextTrimming::None);  // 默认不截断
    metadata.propertyChangedCallback = TextTrimmingPropertyChanged;
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

void TextBlockBase::TextWrappingPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    TextWrapping oldWrapping = oldValue.has_value() ? std::any_cast<TextWrapping>(oldValue) : TextWrapping::NoWrap;
    TextWrapping newWrapping = std::any_cast<TextWrapping>(newValue);
    textBlock->OnTextWrappingChanged(oldWrapping, newWrapping);
}

void TextBlockBase::TextTrimmingPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* textBlock = dynamic_cast<TextBlockBase*>(&sender);
    if (!textBlock) return;

    TextTrimming oldTrimming = oldValue.has_value() ? std::any_cast<TextTrimming>(oldValue) : TextTrimming::None;
    TextTrimming newTrimming = std::any_cast<TextTrimming>(newValue);
    textBlock->OnTextTrimmingChanged(oldTrimming, newTrimming);
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
