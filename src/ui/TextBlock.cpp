#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include <algorithm>
#include <cmath>

namespace fk::ui {

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
        {std::string("Arial")}
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
    // Phase 5.0.5: 使用 RenderContext 进行精确文本度量
    auto text = GetText();
    if (text.empty()) {
        return Size(0, GetFontSize() * 1.2f); // 空文本返回一行高度
    }
    
    auto fontSize = GetFontSize();
    auto fontFamily = GetFontFamily();
    auto textWrapping = GetTextWrapping();
    
    // 使用 RenderContext 的 MeasureText 进行精确度量
    // 注意：这里需要一个临时的 RenderContext，但在 Measure 阶段我们还没有
    // 因此使用简单的字符计数估算，实际渲染时会使用精确度量
    
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
    ui::Point position(0.0f, 0.0f);
    float maxWidth = (textWrapping == TextWrapping::Wrap) ? renderSize.width : 0.0f;
    (void)textAlignment; // 对齐支持将在 RenderContext 中实现
    
    context.DrawText(
        position,
        text,
        textColor,
        fontSize,
        fontFamily,
        maxWidth,
        textWrapping == TextWrapping::Wrap
    );
}

} // namespace fk::ui
