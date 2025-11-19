#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/TextRenderer.h"
#include <algorithm>
#include <cmath>

namespace fk::ui {

// 全局 TextRenderer 实例（用于 Measure 阶段的文本度量）
static render::TextRenderer* g_textRenderer = nullptr;

void TextBlock::SetGlobalTextRenderer(render::TextRenderer* renderer) {
    g_textRenderer = renderer;
}

render::TextRenderer* TextBlock::GetGlobalTextRenderer() {
    return g_textRenderer;
}

// ========== 构造函数 ==========

TextBlock::TextBlock() {
    // TextBlock 默认不拉伸，使用内容的自然尺寸
    SetHorizontalAlignment(HorizontalAlignment::Left);
    SetVerticalAlignment(VerticalAlignment::Top);
}

// ========== 依赖属性注册 ==========

const binding::DependencyProperty& TextBlock::TextProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Text",
        typeid(std::string),
        typeid(TextBlock),
        {std::string("")}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::FontFamilyProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontFamily",
        typeid(std::string),
        typeid(TextBlock),
        {std::string("Segoe UI")}  // Windows 默认字体，支持中英文
    );
    return property;
}

const binding::DependencyProperty& TextBlock::FontSizeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontSize",
        typeid(float),
        typeid(TextBlock),
        {12.0f}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::FontWeightProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontWeight",
        typeid(ui::FontWeight),
        typeid(TextBlock),
        {ui::FontWeight::Normal}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::FontStyleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FontStyle",
        typeid(ui::FontStyle),
        typeid(TextBlock),
        {ui::FontStyle::Normal}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::TextAlignmentProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "TextAlignment",
        typeid(ui::TextAlignment),
        typeid(TextBlock),
        {ui::TextAlignment::Left}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::ForegroundProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Foreground",
        typeid(Brush*),
        typeid(TextBlock),
        {static_cast<Brush*>(nullptr)}
    );
    return property;
}

const binding::DependencyProperty& TextBlock::TextWrappingProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "TextWrapping",
        typeid(ui::TextWrapping),
        typeid(TextBlock),
        {ui::TextWrapping::NoWrap}
    );
    return property;
}

Size TextBlock::MeasureOverride(const Size& availableSize) {
    auto text = GetText();
    if (text.empty()) {
        return Size(0, GetFontSize() * 1.2f); // 空文本返回一行高度
    }
    
    auto fontSize = GetFontSize();
    auto fontFamily = GetFontFamily();
    auto textWrapping = GetTextWrapping();
    
    // 获取全局 TextRenderer 进行精确测量
    render::TextRenderer* textRenderer = GetGlobalTextRenderer();
    
    if (textRenderer) {
        // 使用与渲染相同的字体查找逻辑
        // 尝试加载跨平台字体路径列表中的第一个可用字体
        int fontId = -1;
        std::vector<std::string> fontPaths;
        
        #ifdef _WIN32
            // Windows 字体路径
            fontPaths = {
                "C:/Windows/Fonts/msyh.ttc",      // 微软雅黑
                "C:/Windows/Fonts/simhei.ttf",    // 黑体
                "C:/Windows/Fonts/arial.ttf",     // Arial
                "C:/Windows/Fonts/times.ttf"      // Times
            };
        #elif __APPLE__
            // macOS 字体路径
            fontPaths = {
                "/System/Library/Fonts/PingFang.ttc",
                "/System/Library/Fonts/Helvetica.ttc",
                "/Library/Fonts/Arial Unicode.ttf"
            };
        #else
            // Linux 字体路径
            fontPaths = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc"
            };
        #endif
        
        // 尝试加载字体
        unsigned int fontSizeKey = static_cast<unsigned int>(fontSize);
        for (const auto& path : fontPaths) {
            fontId = textRenderer->LoadFont(path, fontSizeKey);
            if (fontId >= 0) {
                break;
            }
        }
        
        // 如果都加载失败，尝试使用默认字体
        if (fontId < 0) {
            fontId = textRenderer->GetDefaultFont();
        }
        
        // 检查字体是否加载成功（fontId >= 0 表示成功）
        if (fontId >= 0) {
            if (textWrapping == TextWrapping::Wrap && availableSize.width > 0) {
                // 自动换行：使用 MeasureTextMultiline
                int measuredWidth = 0;
                int measuredHeight = 0;
                textRenderer->MeasureTextMultiline(
                    text,
                    fontId,
                    availableSize.width,
                    measuredWidth,
                    measuredHeight
                );
                
                // 确保测量结果有效
                if (measuredHeight > 0) {
                    return Size(static_cast<float>(measuredWidth), static_cast<float>(measuredHeight));
                }
            } else {
                // 不换行：使用 MeasureText 单行测量
                int measuredWidth = 0;
                int measuredHeight = 0;
                textRenderer->MeasureText(
                    text,
                    fontId,
                    measuredWidth,
                    measuredHeight
                );
                
                // 确保测量结果有效
                if (measuredHeight > 0) {
                    // 如果有宽度限制且文本超宽，裁剪到可用宽度
                    if (availableSize.width > 0 && measuredWidth > availableSize.width) {
                        return Size(availableSize.width, static_cast<float>(measuredHeight));
                    }
                    
                    return Size(static_cast<float>(measuredWidth), static_cast<float>(measuredHeight));
                }
            }
        }
        // 如果字体加载失败或测量返回无效结果，继续执行下面的 fallback
    }
    
    // Fallback：使用简单估算
    // 这个分支仅为向后兼容，正常情况下不应该进入
    if (textWrapping == TextWrapping::Wrap && availableSize.width > 0) {
            // 自动换行：估算行数
            float charWidth = fontSize * 0.6f;  // 平均字符宽度估算
            float charsPerLine = availableSize.width / charWidth;
            if (charsPerLine < 1.0f) charsPerLine = 1.0f;
            
            int lineCount = static_cast<int>(std::ceil(text.length() / charsPerLine));
            if (lineCount < 1) lineCount = 1;
            
            float lineHeight = fontSize * 1.2f;
            return Size(availableSize.width, lineCount * lineHeight);
        } else {
            // 不换行：单行文本
            float estimatedWidth = text.length() * fontSize * 0.6f;
            float estimatedHeight = fontSize * 1.2f;
            
            return Size(
                std::min(estimatedWidth, availableSize.width),
                estimatedHeight
            );
        }
}

Size TextBlock::ArrangeOverride(const Size& finalSize) {
    // 返回 finalSize，这是由 FrameworkElement::ArrangeCore 
    // 根据对齐方式计算出的实际尺寸
    return finalSize;
}

void TextBlock::OnRender(render::RenderContext& context) {
    auto text = GetText();
    if (text.empty()) {
        return; // 空文本不绘制
    }

    auto fontSize = GetFontSize();
    auto fontFamily = GetFontFamily();
    auto textAlignment = GetTextAlignment();
    auto textWrapping = GetTextWrapping();
    auto foreground = GetForeground();
    
    std::array<float, 4> textColor{{0.0f, 0.0f, 0.0f, 1.0f}}; // 默认黑色
    if (foreground) {
        auto solidBrush = dynamic_cast<SolidColorBrush*>(foreground);
        if (solidBrush) {
            auto color = solidBrush->GetColor();
            textColor[0] = color.r;
            textColor[1] = color.g;
            textColor[2] = color.b;
            textColor[3] = color.a;
        }
    }

    auto renderSize = GetRenderSize();
    
    // 使用renderSize作为绘制边界,防止文本超出范围
    ui::Rect bounds(0.0f, 0.0f, renderSize.width, renderSize.height);
    float maxWidth = (textWrapping == TextWrapping::Wrap) ? renderSize.width : renderSize.width;
    (void)textAlignment; // 对齐支持将在 RenderContext 中实现
    
    context.DrawText(
        bounds,
        text,
        textColor,
        fontSize,
        fontFamily,
        maxWidth,
        textWrapping == TextWrapping::Wrap
    );
}

} // namespace fk::ui
