#include "fk/ui/TextBox.h"

#include "fk/binding/BindingExpression.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/render/RenderHost.h"
#include "fk/render/TextRenderer.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <any>
#include <cmath>
#include <limits>
#include <regex>
#include <utility>
#include <iostream>
#include <unordered_map>

namespace {

struct GlyphMetrics {
    float width{0.0f};
    int advanceBytes{1};
};

// 精确测量单个字符的宽度(通过 TextRenderer)
float MeasureCharacterWidth(const std::string& text, int byteIndex, int byteLength, float fontSize, 
                            fk::ui::TextBlockView* textPresenter, fk::render::TextRenderer* textRenderer, int fontId) {
    if (!textRenderer || byteIndex < 0 || byteIndex + byteLength > static_cast<int>(text.size())) {
        // 回退到估算
        unsigned char lead = static_cast<unsigned char>(text[byteIndex]);
        if (lead < 0x80) {
            return fontSize * 0.5f;  // ASCII
        } else if (lead < 0xE0) {
            return fontSize * 0.9f;  // 2字节UTF-8
        } else {
            return fontSize;  // 3-4字节UTF-8
        }
    }

    // 提取单个字符的子串
    std::string charStr = text.substr(byteIndex, byteLength);
    
    // 🎯 使用 TextRenderer 精确测量字符宽度(基于实际字形纹理)
    int width = 0;
    int height = 0;
    textRenderer->MeasureText(charStr, fontId, width, height);
    
    return static_cast<float>(width);
}

GlyphMetrics MeasureGlyph(unsigned char lead, float fontSize) {
    GlyphMetrics metrics;
    if (lead < 0x80) {
        metrics.width = fontSize * 0.5f;
        metrics.advanceBytes = 1;
    } else if (lead < 0xE0) {
        metrics.width = fontSize * 0.9f;
        metrics.advanceBytes = 2;
    } else if (lead < 0xF0) {
        metrics.width = fontSize;
        metrics.advanceBytes = 3;
    } else if (lead < 0xF8) {
        metrics.width = fontSize;
        metrics.advanceBytes = 4;
    } else {
        metrics.width = fontSize * 0.5f;
        metrics.advanceBytes = 1;
    }
    return metrics;
}

std::string EncodeCodepointToUtf8(unsigned int codepoint) {
    std::string result;
    if (codepoint <= 0x7F) {
        result.push_back(static_cast<char>(codepoint));
    } else if (codepoint <= 0x7FF) {
        result.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0xFFFF) {
        result.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    } else if (codepoint <= 0x10FFFF) {
        result.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
    }
    return result;
}

int Utf8NextIndex(const std::string& text, int index) {
    const int size = static_cast<int>(text.size());
    if (index < 0) {
        index = 0;
    }
    if (index >= size) {
        return size;
    }

    unsigned char lead = static_cast<unsigned char>(text[index]);
    int advance = 1;
    if ((lead & 0x80) == 0x00) {
        advance = 1;
    } else if ((lead & 0xE0) == 0xC0) {
        advance = 2;
    } else if ((lead & 0xF0) == 0xE0) {
        advance = 3;
    } else if ((lead & 0xF8) == 0xF0) {
        advance = 4;
    }

    index += advance;
    return std::min(index, size);
}

int Utf8PrevIndex(const std::string& text, int index) {
    if (index <= 0) {
        return 0;
    }
    index = std::min(index, static_cast<int>(text.size()));
    --index;
    while (index > 0) {
        unsigned char ch = static_cast<unsigned char>(text[index]);
        if ((ch & 0xC0) != 0x80) {
            break;
        }
        --index;
    }
    return index;
}

bool IsPrintableCodepoint(unsigned int codepoint) {
    if (codepoint == 0u || codepoint == 13u || codepoint == 10u || codepoint == 9u) {
        return false;
    }
    if (codepoint < 0x20u || codepoint == 0x7Fu) {
        return false;
    }
    return codepoint <= 0x10FFFFu;
}

} // namespace

namespace fk::ui::detail {

TextBoxBase::TextBoxBase() {
    textPresenter_ = ui::textBlock();
    if (textPresenter_) {
        textPresenter_->Text("");
        textPresenter_->FontSize(14.0f);
        textPresenter_->Foreground("#000000");
        textPresenter_->SetHorizontalAlignment(HorizontalAlignment::Stretch);
        textPresenter_->SetVerticalAlignment(VerticalAlignment::Center);
    }
    presenterElement_ = std::static_pointer_cast<UIElement>(textPresenter_);

    SetPadding(fk::Thickness{6.0f, 4.0f, 6.0f, 4.0f});
    SetClipToBounds(true);  // 🎯 启用裁剪,防止文本超出边界
    EnsureContent();
    UpdateTextPresenter();
}

TextBoxBase::~TextBoxBase() = default;

FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBoxBase, Text, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBoxBase, PlaceholderText, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBoxBase, IsReadOnly, bool, false)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBoxBase, CaretIndex, int, 0)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBoxBase, SelectionStart, int, 0)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBoxBase, SelectionLength, int, 0)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBoxBase, Foreground, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBoxBase, Background, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE_REF(TextBoxBase, BorderBrush, std::string)
FK_DEPENDENCY_PROPERTY_DEFINE(TextBoxBase, BorderThickness, float, 1.0f)

void TextBoxBase::OnAttachedToLogicalTree() {
    ControlBase::OnAttachedToLogicalTree();
    EnsureContent();
    UpdateTextPresenter();
}

bool TextBoxBase::HasRenderContent() const {
    return true;
}

void TextBoxBase::OnTextChanged(const std::string& oldValue, const std::string& newValue) {
    ClampCaretAndSelection();
    UpdateTextPresenter();
    EnsureCaretVisible();  // 🎯 文本变化时确保光标可见
    TextChanged(*this, oldValue, newValue);
    InvalidateMeasure();
    InvalidateArrange();
    InvalidateVisual();
}

void TextBoxBase::OnPlaceholderTextChanged(const std::string&, const std::string&) {
    UpdateTextPresenter();
}

void TextBoxBase::OnIsReadOnlyChanged(bool, bool) {
    InvalidateVisual();
}

void TextBoxBase::OnCaretIndexChanged(int, int newValue) {
    const int length = static_cast<int>(GetText().size());
    const int clamped = std::clamp(newValue, 0, length);
    if (clamped != newValue) {
        SetCaretIndex(clamped);
        return;
    }
    EnsureCaretVisible();  // 🎯 光标移动时自动滚动
    UpdateSelectionVisual();
}

void TextBoxBase::OnSelectionStartChanged(int, int newValue) {
    const int length = static_cast<int>(GetText().size());
    const int clampedStart = std::clamp(newValue, 0, length);
    if (clampedStart != newValue) {
        SetSelectionStart(clampedStart);
        return;
    }

    const int maxSelection = std::max(0, length - clampedStart);
    const int selection = std::clamp(GetSelectionLength(), 0, maxSelection);
    if (selection != GetSelectionLength()) {
        SetSelectionLength(selection);
        return;
    }
    UpdateSelectionVisual();
}

void TextBoxBase::OnSelectionLengthChanged(int, int newValue) {
    const int length = static_cast<int>(GetText().size());
    const int start = std::clamp(GetSelectionStart(), 0, length);
    if (start != GetSelectionStart()) {
        SetSelectionStart(start);
        return;
    }

    const int maxSelection = std::max(0, length - start);
    const int clampedLength = std::clamp(newValue, 0, maxSelection);
    if (clampedLength != newValue) {
        SetSelectionLength(clampedLength);
        return;
    }
    UpdateSelectionVisual();
}

void TextBoxBase::OnForegroundChanged(const std::string&, const std::string&) {
    ApplyForeground();
    InvalidateVisual();
}

void TextBoxBase::OnBackgroundChanged(const std::string&, const std::string&) {
    InvalidateVisual();
}

void TextBoxBase::OnBorderBrushChanged(const std::string&, const std::string&) {
    InvalidateVisual();
}

void TextBoxBase::OnBorderThicknessChanged(float, float newValue) {
    if (newValue < 0.0f) {
        SetBorderThickness(0.0f);
        return;
    }
    InvalidateVisual();
}

bool TextBoxBase::OnMouseButtonDown(int button, double x, double y) {
    bool handled = ControlBase::OnMouseButtonDown(button, x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        Focus();
        const int textLength = static_cast<int>(GetText().size());
        int caretIndex = HitTestCaretIndex(static_cast<float>(x));
        caretIndex = std::clamp(caretIndex, 0, textLength);

        selectionAnchor_ = caretIndex;
        isSelecting_ = true;

        SetSelectionStart(caretIndex);
        SetSelectionLength(0);
        SetCaretIndex(caretIndex);
        ResetCaretBlink();
        return true;
    }

    return handled;
}

bool TextBoxBase::OnMouseButtonUp(int button, double x, double y) {
    bool handled = ControlBase::OnMouseButtonUp(button, x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (isSelecting_) {
            isSelecting_ = false;
            return true;
        }
        return handled;
    }

    return handled;
}

bool TextBoxBase::OnMouseMove(double x, double y) {
    bool handled = ControlBase::OnMouseMove(x, y);

    if (!isSelecting_) {
        return handled;
    }

    const int textLength = static_cast<int>(GetText().size());
    int caretIndex = HitTestCaretIndex(static_cast<float>(x));
    caretIndex = std::clamp(caretIndex, 0, textLength);

    int anchor = std::clamp(selectionAnchor_, 0, textLength);
    int selectionStart = std::min(anchor, caretIndex);
    int selectionEnd = std::max(anchor, caretIndex);
    int selectionLength = selectionEnd - selectionStart;

    SetSelectionStart(selectionStart);
    SetSelectionLength(selectionLength);
    SetCaretIndex(caretIndex);
    ResetCaretBlink();
    return true;
}

bool TextBoxBase::OnKeyDown(int key, int scancode, int mods) {
    if (ControlBase::OnKeyDown(key, scancode, mods)) {
        return true;
    }

    bool handled = false;
    switch (key) {
        case GLFW_KEY_BACKSPACE:
            handled = HandleBackspace();
            break;
        case GLFW_KEY_DELETE:
            handled = HandleDelete();
            break;
        case GLFW_KEY_LEFT:
            handled = MoveCaretLeft();
            break;
        case GLFW_KEY_RIGHT:
            handled = MoveCaretRight();
            break;
        case GLFW_KEY_HOME:
            handled = MoveCaretToStart();
            break;
        case GLFW_KEY_END:
            handled = MoveCaretToEnd();
            break;
        default:
            break;
    }

    if (handled) {
        ResetCaretBlink();
    }

    return handled;
}

bool TextBoxBase::OnTextInput(unsigned int codepoint) {
    if (GetIsReadOnly() || !GetIsEnabled()) {
        return false;
    }

    if (!IsPrintableCodepoint(codepoint)) {
        return false;
    }

    std::string insertion = EncodeCodepointToUtf8(codepoint);
    if (insertion.empty()) {
        return false;
    }

    std::string text = GetText();
    int caret = std::clamp(GetCaretIndex(), 0, static_cast<int>(text.size()));

    if (RemoveSelection(text, caret)) {
        // Selection removed and caret already set to selection start.
    }

    text.insert(static_cast<std::size_t>(caret), insertion);
    caret += static_cast<int>(insertion.size());
    CommitTextChange(std::move(text), caret);
    ResetCaretBlink();
    return true;
}

void TextBoxBase::OnFocusGained() {
    isFocused_ = true;
    selectionAnchor_ = std::clamp(GetCaretIndex(), 0, static_cast<int>(GetText().size()));
    ResetCaretBlink();
    UpdateSelectionVisual();
    InvalidateVisual();
}

void TextBoxBase::OnFocusLost() {
    isFocused_ = false;
    isSelecting_ = false;
    caretBlinkVisible_ = false;
    SetSelectionLength(0);
    UpdateSelectionVisual();
    InvalidateVisual();
    selectionAnchor_ = std::clamp(GetCaretIndex(), 0, static_cast<int>(GetText().size()));
}

binding::PropertyMetadata TextBoxBase::BuildTextMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{};
    metadata.propertyChangedCallback = &TextBoxBase::TextPropertyChanged;
    metadata.bindingOptions.defaultMode = binding::BindingMode::TwoWay;
    metadata.bindingOptions.updateSourceTrigger = binding::UpdateSourceTrigger::PropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildPlaceholderTextMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{};
    metadata.propertyChangedCallback = &TextBoxBase::PlaceholderTextPropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildIsReadOnlyMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = false;
    metadata.propertyChangedCallback = &TextBoxBase::IsReadOnlyPropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildCaretIndexMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &TextBoxBase::CaretIndexPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateNonNegativeInt;
    metadata.bindingOptions.defaultMode = binding::BindingMode::TwoWay;
    metadata.bindingOptions.updateSourceTrigger = binding::UpdateSourceTrigger::PropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildSelectionStartMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &TextBoxBase::SelectionStartPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateNonNegativeInt;
    metadata.bindingOptions.defaultMode = binding::BindingMode::TwoWay;
    metadata.bindingOptions.updateSourceTrigger = binding::UpdateSourceTrigger::PropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildSelectionLengthMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 0;
    metadata.propertyChangedCallback = &TextBoxBase::SelectionLengthPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateNonNegativeInt;
    metadata.bindingOptions.defaultMode = binding::BindingMode::TwoWay;
    metadata.bindingOptions.updateSourceTrigger = binding::UpdateSourceTrigger::PropertyChanged;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildForegroundMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{"#000000"};
    metadata.propertyChangedCallback = &TextBoxBase::ForegroundPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateColor;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildBackgroundMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{"#FFFFFF"};
    metadata.propertyChangedCallback = &TextBoxBase::BackgroundPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateColor;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildBorderBrushMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = std::string{"#D0D0D0"};
    metadata.propertyChangedCallback = &TextBoxBase::BorderBrushPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateColor;
    return metadata;
}

binding::PropertyMetadata TextBoxBase::BuildBorderThicknessMetadata() {
    binding::PropertyMetadata metadata;
    metadata.defaultValue = 1.0f;
    metadata.propertyChangedCallback = &TextBoxBase::BorderThicknessPropertyChanged;
    metadata.validateCallback = &TextBoxBase::ValidateBorderThickness;
    return metadata;
}

bool TextBoxBase::ValidateNonNegativeInt(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(int)) {
        return false;
    }
    return std::any_cast<int>(value) >= 0;
}

bool TextBoxBase::ValidateColor(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    try {
        const auto& color = std::any_cast<const std::string&>(value);
        static const std::regex colorRegex(R"(^#([0-9A-Fa-f]{3}|[0-9A-Fa-f]{4}|[0-9A-Fa-f]{6}|[0-9A-Fa-f]{8})$)");
        return std::regex_match(color, colorRegex);
    } catch (...) {
        return false;
    }
}

bool TextBoxBase::ValidateBorderThickness(const std::any& value) {
    if (!value.has_value()) {
        return true;
    }
    if (value.type() != typeid(float)) {
        return false;
    }
    return std::any_cast<float>(value) >= 0.0f;
}

bool TextBoxBase::ShouldShowCaret() const {
    return isFocused_ && GetIsEnabled() && !GetIsReadOnly() && caretBlinkVisible_;
}

Rect TextBoxBase::GetCaretRect() const {
    Rect presenterBounds{};
    if (presenterElement_) {
        presenterBounds = presenterElement_->GetRenderBounds();
    }

    const float fontSize = textPresenter_ ? textPresenter_->GetFontSize() : 14.0f;
    float caretHeight = presenterBounds.height;
    if (caretHeight <= 0.0f) {
        caretHeight = fontSize * 1.2f;
    }

    float caretX = presenterBounds.x + ComputeCaretOffset();
    const float maxX = presenterBounds.x + presenterBounds.width;
    caretX = std::clamp(caretX, presenterBounds.x, maxX);

    float caretY = presenterBounds.y;
    const float caretWidth = std::max(1.0f, fontSize * 0.08f);
    return Rect{caretX, caretY, caretWidth, std::max(caretHeight, fontSize)};
}

bool TextBoxBase::HasSelection() const {
    const int selectionLength = GetSelectionLength();
    return selectionLength > 0 && selectionLength <= static_cast<int>(GetText().size());
}

Rect TextBoxBase::GetSelectionRect() const {
    if (!HasSelection()) {
        return Rect{};
    }

    Rect presenterBounds{};
    if (presenterElement_) {
        presenterBounds = presenterElement_->GetRenderBounds();
    }

    const float fontSize = textPresenter_ ? textPresenter_->GetFontSize() : 14.0f;
    float selectionHeight = presenterBounds.height;
    if (selectionHeight <= 0.0f) {
        selectionHeight = fontSize * 1.2f;
    }

    const int textLength = static_cast<int>(GetText().size());
    int start = std::clamp(GetSelectionStart(), 0, textLength);
    int end = std::clamp(start + GetSelectionLength(), start, textLength);

    const float startOffset = ComputeOffsetForIndex(start);
    const float endOffset = ComputeOffsetForIndex(end);
    float selectionWidth = std::max(0.0f, endOffset - startOffset);

    float selectionX = presenterBounds.x + startOffset;
    const float availableWidth = std::max(0.0f, presenterBounds.width - (selectionX - presenterBounds.x));
    selectionWidth = std::min(selectionWidth, availableWidth);

    return Rect{selectionX, presenterBounds.y, selectionWidth, std::max(selectionHeight, fontSize)};
}

float TextBoxBase::ComputeOffsetForIndex(int targetIndex) const {
    if (!textPresenter_) {
        return 0.0f;
    }

    const std::string& text = GetText();
    const int length = static_cast<int>(text.size());
    const int clampedIndex = std::clamp(targetIndex, 0, length);
    const float fontSize = textPresenter_->GetFontSize();

    // 🎯 获取 TextRenderer 用于精确测量
    auto* renderHost = GetRenderHost();
    auto* textRenderer = renderHost ? renderHost->GetTextRenderer() : nullptr;
    
    // 获取对应字体大小的字体 ID (使用静态缓存避免重复加载)
    static std::unordered_map<unsigned int, int> fontCache;
    int fontId = -1;
    if (textRenderer) {
        unsigned int fontSizeKey = static_cast<unsigned int>(fontSize);
        auto it = fontCache.find(fontSizeKey);
        if (it != fontCache.end()) {
            fontId = it->second;
        } else {
            // 尝试加载字体
            fontId = textRenderer->LoadFont("C:/Windows/Fonts/msyh.ttc", fontSizeKey);
            if (fontId < 0) {
                fontId = textRenderer->LoadFont("C:/Windows/Fonts/simhei.ttf", fontSizeKey);
            }
            if (fontId >= 0) {
                fontCache[fontSizeKey] = fontId;
            }
        }
    }

    float offset = 0.0f;
    int index = 0;
    while (index < clampedIndex && index < length) {
        unsigned char lead = static_cast<unsigned char>(text[index]);
        
        // 获取字符的字节长度
        int byteLength = 1;
        if (lead >= 0xF0) {
            byteLength = 4;
        } else if (lead >= 0xE0) {
            byteLength = 3;
        } else if (lead >= 0xC0) {
            byteLength = 2;
        }
        byteLength = std::min(byteLength, length - index);

        if (index + byteLength > clampedIndex) {
            // 如果目标位置在字符中间,按比例分配宽度
            const float charWidth = MeasureCharacterWidth(text, index, byteLength, fontSize, 
                                                         textPresenter_.get(), textRenderer, fontId);
            const float fraction = static_cast<float>(clampedIndex - index) / static_cast<float>(byteLength);
            offset += charWidth * fraction;
            break;
        }

        // 🎯 使用 TextRenderer 精确测量的字符宽度(基于实际字形纹理)
        const float charWidth = MeasureCharacterWidth(text, index, byteLength, fontSize, 
                                                     textPresenter_.get(), textRenderer, fontId);
        offset += charWidth;
        index += byteLength;
    }

    const Rect presenterBounds = presenterElement_ ? presenterElement_->GetRenderBounds() : Rect{};
    const float maxOffset = std::max(0.0f, presenterBounds.width);
    return std::min(offset, maxOffset);
}

void TextBoxBase::PrepareForRender(double frameTime) {
    UpdateCaretAnimation(frameTime);
    if (isFocused_ && GetIsEnabled() && !GetIsReadOnly()) {
        InvalidateVisual();
    }
}

float TextBoxBase::ComputeCaretOffset() const {
    return ComputeOffsetForIndex(GetCaretIndex());
}

int TextBoxBase::HitTestCaretIndex(float pointX) const {
    const std::string& text = GetText();
    const int textLength = static_cast<int>(text.size());
    if (textLength == 0) {
        return 0;
    }

    Rect presenterBounds{};
    if (presenterElement_) {
        presenterBounds = presenterElement_->GetRenderBounds();
    }

    float localX = pointX - presenterBounds.x;
    if (presenterBounds.width > 0.0f) {
        localX = std::clamp(localX, 0.0f, presenterBounds.width);
    } else {
        localX = std::max(localX, 0.0f);
    }
    int bestIndex = 0;
    float bestDistance = std::numeric_limits<float>::max();
    int index = 0;
    while (index <= textLength) {
        float offset = ComputeOffsetForIndex(index);
        float distance = std::fabs(localX - offset);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = index;
        }

        if (index == textLength) {
            break;
        }

        index = Utf8NextIndex(text, index);
    }

    return bestIndex;
}

void TextBoxBase::ResetCaretBlink() {
    caretBlinkStartTime_ = lastFrameTime_;
    caretBlinkVisible_ = true;
    InvalidateVisual();
    if (textPresenter_) {
        textPresenter_->InvalidateVisual();
    }
}

void TextBoxBase::UpdateCaretAnimation(double frameTime) {
    lastFrameTime_ = frameTime;

    if (!isFocused_ || !GetIsEnabled() || GetIsReadOnly()) {
        caretBlinkVisible_ = false;
        return;
    }

    const double elapsed = std::max(0.0, frameTime - caretBlinkStartTime_);
    const double period = std::max(0.1, caretBlinkPeriod_);
    const double phase = std::fmod(elapsed, period);
    caretBlinkVisible_ = phase < (period * 0.5);
}

bool TextBoxBase::RemoveSelection(std::string& text, int& caretIndex) {
    int selectionLength = GetSelectionLength();
    if (selectionLength <= 0) {
        return false;
    }

    const int length = static_cast<int>(text.size());
    int selectionStart = std::clamp(GetSelectionStart(), 0, length);
    selectionLength = std::min(selectionLength, length - selectionStart);

    text.erase(static_cast<std::size_t>(selectionStart), static_cast<std::size_t>(selectionLength));
    caretIndex = selectionStart;
    return true;
}

void TextBoxBase::CommitTextChange(std::string newText, int caretIndex) {
    caretIndex = std::clamp(caretIndex, 0, static_cast<int>(newText.size()));

    auto bindingExpression = GetBinding(TextProperty());
    if (bindingExpression && bindingExpression->IsActive()) {
        if (!updatingFromBinding_) {
            updatingFromBinding_ = true;

            SetText(newText);
std::cout << "----" << std::endl;
std::cout << "Committing text change: " << newText << std::endl;
std::cout << "Caret index: " << caretIndex << std::endl;

            bindingExpression->UpdateSource();
            bindingExpression->ApplyTargetValue(std::any(GetText()));
            ClearValue(TextProperty());

            updatingFromBinding_ = false;
        }
    } else {
        SetText(std::move(newText));
    }

    SetSelectionLength(0);
    SetSelectionStart(caretIndex);
    SetCaretIndex(caretIndex);
    selectionAnchor_ = caretIndex;
}

bool TextBoxBase::HandleBackspace() {
    if (GetIsReadOnly() || !GetIsEnabled()) {
        return false;
    }

    std::string text = GetText();
    if (text.empty()) {
        return false;
    }

    int caret = std::clamp(GetCaretIndex(), 0, static_cast<int>(text.size()));
    if (RemoveSelection(text, caret)) {
        CommitTextChange(std::move(text), caret);
        return true;
    }

    if (caret == 0) {
        return false;
    }

    int prev = Utf8PrevIndex(text, caret);
    text.erase(static_cast<std::size_t>(prev), static_cast<std::size_t>(caret - prev));
    caret = prev;
    CommitTextChange(std::move(text), caret);
    return true;
}

bool TextBoxBase::HandleDelete() {
    if (GetIsReadOnly() || !GetIsEnabled()) {
        return false;
    }

    std::string text = GetText();
    if (text.empty()) {
        return false;
    }

    int caret = std::clamp(GetCaretIndex(), 0, static_cast<int>(text.size()));
    if (RemoveSelection(text, caret)) {
        CommitTextChange(std::move(text), caret);
        return true;
    }

    if (caret >= static_cast<int>(text.size())) {
        return false;
    }

    int next = Utf8NextIndex(text, caret);
    text.erase(static_cast<std::size_t>(caret), static_cast<std::size_t>(next - caret));
    CommitTextChange(std::move(text), caret);
    return true;
}

bool TextBoxBase::MoveCaretLeft() {
    std::string text = GetText();
    if (text.empty()) {
        return false;
    }

    int caret = std::clamp(GetCaretIndex(), 0, static_cast<int>(text.size()));
    int newCaret = Utf8PrevIndex(text, caret);
    if (newCaret == caret) {
        return false;
    }

    SetSelectionLength(0);
    SetSelectionStart(newCaret);
    SetCaretIndex(newCaret);
    selectionAnchor_ = newCaret;
    return true;
}

bool TextBoxBase::MoveCaretRight() {
    std::string text = GetText();
    if (text.empty()) {
        return false;
    }

    int caret = std::clamp(GetCaretIndex(), 0, static_cast<int>(text.size()));
    int newCaret = Utf8NextIndex(text, caret);
    if (newCaret == caret) {
        return false;
    }

    SetSelectionLength(0);
    SetSelectionStart(newCaret);
    SetCaretIndex(newCaret);
    selectionAnchor_ = newCaret;
    return true;
}

bool TextBoxBase::MoveCaretToStart() {
    const int caret = GetCaretIndex();
    if (caret == 0) {
        return false;
    }
    SetSelectionLength(0);
    SetSelectionStart(0);
    SetCaretIndex(0);
    selectionAnchor_ = 0;
    return true;
}

bool TextBoxBase::MoveCaretToEnd() {
    const int length = static_cast<int>(GetText().size());
    if (GetCaretIndex() == length) {
        return false;
    }
    SetSelectionLength(0);
    SetSelectionStart(length);
    SetCaretIndex(length);
    selectionAnchor_ = length;
    return true;
}

void TextBoxBase::EnsureContent() {
    if (!presenterElement_) {
        presenterElement_ = std::static_pointer_cast<UIElement>(textPresenter_);
    }
    const auto content = GetContent();
    if (!content || content.get() != presenterElement_.get()) {
        SetContent(presenterElement_);
    }
}

void TextBoxBase::UpdateTextPresenter() {
    EnsureContent();
    if (!textPresenter_) {
        return;
    }

    const auto& text = GetText();
    if (!text.empty()) {
        placeholderVisible_ = false;
        textPresenter_->Text(text);
    } else {
        const auto& placeholder = GetPlaceholderText();
        if (!placeholder.empty()) {
            placeholderVisible_ = true;
            textPresenter_->Text(placeholder);
        } else {
            placeholderVisible_ = false;
            textPresenter_->Text("");
        }
    }

    ApplyForeground();
    textPresenter_->InvalidateMeasure();
    textPresenter_->InvalidateArrange();
    textPresenter_->InvalidateVisual();
}

void TextBoxBase::ApplyForeground() {
    if (!textPresenter_) {
        return;
    }

    if (placeholderVisible_) {
        textPresenter_->Foreground(placeholderColor_);
    } else {
        textPresenter_->Foreground(GetForeground());
    }
}

void TextBoxBase::UpdateSelectionVisual() {
    InvalidateVisual();
    if (textPresenter_) {
        textPresenter_->InvalidateVisual();
    }
}

void TextBoxBase::ClampCaretAndSelection() {
    const int length = static_cast<int>(GetText().size());

    const int caret = std::clamp(GetCaretIndex(), 0, length);
    if (caret != GetCaretIndex()) {
        SetCaretIndex(caret);
    }

    const int start = std::clamp(GetSelectionStart(), 0, length);
    if (start != GetSelectionStart()) {
        SetSelectionStart(start);
        return;
    }

    const int maxSelection = std::max(0, length - start);
    const int selection = std::clamp(GetSelectionLength(), 0, maxSelection);
    if (selection != GetSelectionLength()) {
        SetSelectionLength(selection);
    }
}

void TextBoxBase::EnsureCaretVisible() {
    if (!presenterElement_) {
        return;
    }

    // 获取可视区域宽度
    const Rect bounds = GetRenderBounds();
    const Thickness padding = GetPadding();
    const float leftPadding = padding.left;
    const float rightPadding = padding.right;
    const float viewportWidth = bounds.width - leftPadding - rightPadding;
    
    if (viewportWidth <= 0.0f) {
        return;
    }

    // 计算光标位置(相对于文本起点)
    const float caretOffset = ComputeCaretOffset();
    
    // 计算光标在视口中的位置
    const float caretInViewport = caretOffset - horizontalScrollOffset_;

    const float margin = leftPadding + 1.0f;  // 🎯 Padding + 1px 额外缓冲区
    

    // 如果光标在视口左侧外面,滚动到光标位置
    if (caretInViewport < margin) {
        horizontalScrollOffset_ = std::max(0.0f, caretOffset - margin);
    }
    // 如果光标在视口右侧外面,滚动到光标可见
    else if (caretInViewport > viewportWidth - margin) {
        horizontalScrollOffset_ = caretOffset - viewportWidth + margin;
    }
    
    // 应用滚动偏移到 TextPresenter
    if (textPresenter_) {
        // 通过设置 Margin 来实现滚动效果
        textPresenter_->SetMargin(fk::Thickness{-horizontalScrollOffset_, 0.0f, 0.0f, 0.0f});
        InvalidateVisual();
    }
}

} // namespace fk::ui::detail
