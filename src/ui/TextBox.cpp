#include "fk/ui/TextBox.h"
#include "fk/render/RenderContext.h"

namespace fk {
namespace ui {

TextBox::TextBox()
    : TextBoxBase()
    , scrollOffset_(0.0f)
{
}

TextBox::~TextBox() {
}

// ========== 重写基类方法 ==========

std::string TextBox::GetDisplayText() const {
    return GetText();  // 单行文本直接返回
}

Size TextBox::MeasureOverride(const Size& availableSize) {
    // 测量文本尺寸
    // TODO: 使用 TextRenderer 进行准确度量
    float fontSize = 14.0f;  // 默认字体大小
    float textWidth = GetText().length() * fontSize * 0.6f;
    float textHeight = fontSize * 1.5f;
    
    return Size{
        std::min(textWidth, availableSize.width),
        std::min(textHeight, availableSize.height)
    };
}

Size TextBox::ArrangeOverride(const Size& finalSize) {
    // 更新滚动偏移确保光标可见
    UpdateScrollOffset();
    return finalSize;
}

void TextBox::RenderText(render::RenderContext& context) {
    std::string displayText = GetDisplayText();
    if (displayText.empty()) return;
    
    // 计算可见文本区域
    float startOffset = 0.0f;
    std::string visibleText = GetVisibleText(startOffset);
    
    // TODO: 渲染可见文本
    // Point textPosition{-scrollOffset_, 0};
    // context.DrawText(textPosition, visibleText, textColor_, fontSize_, fontFamily_);
}

// ========== 私有方法 ==========

void TextBox::UpdateScrollOffset() {
    // TODO: 根据光标位置更新滚动偏移
    // 确保光标始终在可见区域内
}

std::string TextBox::GetVisibleText(float& startOffset) const {
    // TODO: 计算并返回可见的文本部分
    startOffset = scrollOffset_;
    return GetText();
}

} // namespace ui
} // namespace fk
