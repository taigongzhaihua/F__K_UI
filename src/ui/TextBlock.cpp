#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include <algorithm>
#include <cmath>

namespace fk::ui {

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
    return finalSize;
}

void TextBlock::CollectDrawCommands(render::RenderContext& context) {
    // Phase 5.0.5: 完整的文本绘制命令生成
    auto text = GetText();
    if (text.empty()) {
        return; // 空文本不绘制
    }
    
    // 获取所有文本属性
    auto fontSize = GetFontSize();
    auto fontFamily = GetFontFamily();
    auto textAlignment = GetTextAlignment();
    auto textWrapping = GetTextWrapping();
    auto foreground = GetForeground();
    
    // 转换 Brush 到颜色
    std::array<float, 4> textColor{{0.0f, 0.0f, 0.0f, 1.0f}}; // 默认黑色
    if (foreground) {
        // 如果有画刷，使用画刷的颜色
        // 这里假设 SolidColorBrush，实际需要类型检查
        auto solidBrush = dynamic_cast<SolidColorBrush*>(foreground);
        if (solidBrush) {
            auto color = solidBrush->GetColor();
            textColor[0] = color.r;
            textColor[1] = color.g;
            textColor[2] = color.b;
            textColor[3] = color.a;
        }
    }
    
    // 获取渲染位置（左上角）
    auto renderSize = GetRenderSize();
    ui::Point position(0.0f, 0.0f); // 相对于自身的位置
    
    // 计算最大宽度（用于换行）
    float maxWidth = 0.0f;
    if (textWrapping == TextWrapping::Wrap) {
        maxWidth = renderSize.width;
    }
    
    // 调用 RenderContext 的 DrawText
    // 注意：TextAlignment 支持将在 RenderContext 中实现
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
