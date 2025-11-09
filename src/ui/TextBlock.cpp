#include "fk/ui/TextBlock.h"
#include "fk/binding/DependencyProperty.h"

#include <regex>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace fk::ui::detail {

// ============================================================================
// TextBlockBase 实现
// ============================================================================

TextBlockBase::TextBlockBase() = default;
TextBlockBase::~TextBlockBase() = default;

// ============================================================================
// 依赖属性定义（使用宏）
// ============================================================================

FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBlockBase, Text, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBlockBase, Foreground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBlockBase, FontSize, float, 14.0f)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBlockBase, FontFamily, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBlockBase, TextWrapping, TextWrapping, TextWrapping::NoWrap)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBlockBase, TextTrimming, TextTrimming, TextTrimming::None)

// Getter/Setter 由宏自动生成

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
    
    // 🎯 首先按硬换行符 \n 分割文本
    std::vector<std::string> hardLines;
    {
        std::string currentHardLine;
        for (char c : text) {
            if (c == '\n') {
                hardLines.push_back(currentHardLine);
                currentHardLine.clear();
            } else {
                currentHardLine += c;
            }
        }
        hardLines.push_back(currentHardLine);
    }
    
    // 然后对每个硬换行的行应用软换行（根据宽度）
    wrappedLines_.clear();
    float maxWidth = 0.0f;
    
    for (const auto& hardLine : hardLines) {
        if (hardLine.empty()) {
            wrappedLines_.push_back("");  // 保留空行
            continue;
        }
        
        std::string currentLine;
        float currentLineWidth = 0.0f;
        
        size_t i = 0;
        while (i < hardLine.length()) {
            unsigned char c = hardLine[i];
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
            
            // 检查是否需要软换行
            if (currentLineWidth + charWidth > maxLineWidth && !currentLine.empty()) {
                wrappedLines_.push_back(currentLine);
                maxWidth = std::max(maxWidth, currentLineWidth);
                currentLine.clear();
                currentLineWidth = 0.0f;
            }
            
            // 添加字符到当前行
            currentLine.append(hardLine, i, charSize);
            currentLineWidth += charWidth;
            i += charSize;
        }
        
        // 添加当前硬行的最后一个软行
        if (!currentLine.empty()) {
            wrappedLines_.push_back(currentLine);
            maxWidth = std::max(maxWidth, currentLineWidth);
        }
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
    // 清除缓存的换行文本，强制重新计算
    wrappedLines_.clear();
    
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

// 静态回调由宏自动生成

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
