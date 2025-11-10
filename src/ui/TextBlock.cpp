#include "fk/ui/TextBlock.h"

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
    // TODO: 实际测量文本尺寸（需要字体度量）
    // 临时实现：简单估算
    auto text = GetText();
    auto fontSize = GetFontSize();
    float estimatedWidth = text.length() * fontSize * 0.6f;
    float estimatedHeight = fontSize * 1.2f;
    
    return Size(
        std::min(estimatedWidth, availableSize.width),
        estimatedHeight
    );
}

Size TextBlock::ArrangeOverride(const Size& finalSize) {
    return finalSize;
}

void TextBlock::CollectDrawCommands(RenderContext& context) {
    // TODO: 添加文本绘制命令
    // auto text = GetText();
    // auto foreground = GetForeground();
    // auto fontSize = GetFontSize();
    // context.DrawText(text, position, foreground, fontSize);
}

} // namespace fk::ui
