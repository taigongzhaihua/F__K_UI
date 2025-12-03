#include "fk/ui/text/TextBoxBase.h"
#include "fk/render/RenderContext.h"
#include <algorithm>

namespace fk {
namespace ui {

// 静态依赖属性定�?
DependencyProperty* TextBoxBase::TextProperty = nullptr;
DependencyProperty* TextBoxBase::MaxLengthProperty = nullptr;
DependencyProperty* TextBoxBase::IsReadOnlyProperty = nullptr;
DependencyProperty* TextBoxBase::CaretIndexProperty = nullptr;
DependencyProperty* TextBoxBase::SelectionStartProperty = nullptr;
DependencyProperty* TextBoxBase::SelectionLengthProperty = nullptr;
DependencyProperty* TextBoxBase::CaretBrushProperty = nullptr;
DependencyProperty* TextBoxBase::SelectionBrushProperty = nullptr;

// 静态路由事件定�?
RoutedEvent* TextBoxBase::TextChangedEvent = nullptr;
RoutedEvent* TextBoxBase::SelectionChangedEvent = nullptr;

TextBoxBase::TextBoxBase()
    : text_("")
    , caretIndex_(0)
    , selectionStart_(0)
    , selectionLength_(0)
    , maxLength_(0)
    , isReadOnly_(false)
    , caretVisible_(false)
    , isSelecting_(false)
    , caretBlinkTimer_(nullptr)
{
    // 初始化依赖属性（如果尚未初始化）
    if (!TextProperty) {
        TextProperty = DependencyProperty::Register("Text", this);
        MaxLengthProperty = DependencyProperty::Register("MaxLength", this);
        IsReadOnlyProperty = DependencyProperty::Register("IsReadOnly", this);
        CaretIndexProperty = DependencyProperty::Register("CaretIndex", this);
        SelectionStartProperty = DependencyProperty::Register("SelectionStart", this);
        SelectionLengthProperty = DependencyProperty::Register("SelectionLength", this);
        CaretBrushProperty = DependencyProperty::Register("CaretBrush", this);
        SelectionBrushProperty = DependencyProperty::Register("SelectionBrush", this);
    }
    
    // 初始化路由事件（如果尚未初始化）
    if (!TextChangedEvent) {
        TextChangedEvent = new RoutedEvent("TextChanged");
        SelectionChangedEvent = new RoutedEvent("SelectionChanged");
    }
}

TextBoxBase::~TextBoxBase() {
    if (caretBlinkTimer_) {
        StopCaretBlink();
    }
}

// ========== 属性访问器 ==========

std::string TextBoxBase::GetText() const {
    return text_;
}

void TextBoxBase::SetText(const std::string& value) {
    if (text_ != value) {
        text_ = value;
        caretIndex_ = std::min(caretIndex_, static_cast<int>(text_.length()));
        ClearSelection();
        OnTextChanged();
        InvalidateVisual();
    }
}

int TextBoxBase::GetMaxLength() const {
    return maxLength_;
}

void TextBoxBase::SetMaxLength(int value) {
    maxLength_ = value;
}

bool TextBoxBase::GetIsReadOnly() const {
    return isReadOnly_;
}

void TextBoxBase::SetIsReadOnly(bool value) {
    isReadOnly_ = value;
}

int TextBoxBase::GetCaretIndex() const {
    return caretIndex_;
}

void TextBoxBase::SetCaretIndex(int value) {
    caretIndex_ = std::max(0, std::min(value, static_cast<int>(text_.length())));
    InvalidateVisual();
}

int TextBoxBase::GetSelectionStart() const {
    return selectionStart_;
}

int TextBoxBase::GetSelectionLength() const {
    return selectionLength_;
}

// ========== 公共方法 ==========

std::string TextBoxBase::GetSelectedText() const {
    if (selectionLength_ <= 0) return "";
    return text_.substr(selectionStart_, selectionLength_);
}

void TextBoxBase::SelectAll() {
    selectionStart_ = 0;
    selectionLength_ = static_cast<int>(text_.length());
    OnSelectionChanged();
    InvalidateVisual();
}

void TextBoxBase::ClearSelection() {
    if (selectionLength_ != 0) {
        selectionStart_ = 0;
        selectionLength_ = 0;
        OnSelectionChanged();
        InvalidateVisual();
    }
}

void TextBoxBase::Cut() {
    if (isReadOnly_ || selectionLength_ <= 0) return;
    Copy();
    DeleteSelection();
}

void TextBoxBase::Copy() {
    if (selectionLength_ <= 0) return;
    // TODO: 实现剪贴板复�?
    // Clipboard::SetText(GetSelectedText());
}

void TextBoxBase::Paste() {
    if (isReadOnly_) return;
    // TODO: 实现剪贴板粘�?
    // std::string clipboardText = Clipboard::GetText();
    // InsertText(clipboardText);
}

void TextBoxBase::Undo() {
    // TODO: 实现撤销
}

void TextBoxBase::Redo() {
    // TODO: 实现重做
}

bool TextBoxBase::CanUndo() const {
    // TODO: 实现撤销检�?
    return false;
}

bool TextBoxBase::CanRedo() const {
    // TODO: 实现重做检�?
    return false;
}

// ========== 受保护方�?==========

void TextBoxBase::InsertText(const std::string& text) {
    if (isReadOnly_) return;
    
    // 删除选中文本
    if (selectionLength_ > 0) {
        DeleteSelection();
    }
    
    // 检查最大长度限�?
    if (maxLength_ > 0 && text_.length() + text.length() > static_cast<size_t>(maxLength_)) {
        return;
    }
    
    // 插入文本
    text_.insert(caretIndex_, text);
    caretIndex_ += text.length();
    
    OnTextChanged();
    InvalidateVisual();
}

void TextBoxBase::DeleteSelection() {
    if (selectionLength_ <= 0) return;
    
    text_.erase(selectionStart_, selectionLength_);
    caretIndex_ = selectionStart_;
    ClearSelection();
    
    OnTextChanged();
    InvalidateVisual();
}

void TextBoxBase::DeleteForward() {
    if (isReadOnly_) return;
    
    if (selectionLength_ > 0) {
        DeleteSelection();
    } else if (caretIndex_ < static_cast<int>(text_.length())) {
        text_.erase(caretIndex_, 1);
        OnTextChanged();
        InvalidateVisual();
    }
}

void TextBoxBase::DeleteBackward() {
    if (isReadOnly_) return;
    
    if (selectionLength_ > 0) {
        DeleteSelection();
    } else if (caretIndex_ > 0) {
        text_.erase(caretIndex_ - 1, 1);
        caretIndex_--;
        OnTextChanged();
        InvalidateVisual();
    }
}

void TextBoxBase::MoveCaretLeft(bool extendSelection) {
    if (extendSelection) {
        // TODO: 扩展选择
    } else {
        if (selectionLength_ > 0) {
            caretIndex_ = selectionStart_;
            ClearSelection();
        } else if (caretIndex_ > 0) {
            caretIndex_--;
        }
    }
    InvalidateVisual();
}

void TextBoxBase::MoveCaretRight(bool extendSelection) {
    if (extendSelection) {
        // TODO: 扩展选择
    } else {
        if (selectionLength_ > 0) {
            caretIndex_ = selectionStart_ + selectionLength_;
            ClearSelection();
        } else if (caretIndex_ < static_cast<int>(text_.length())) {
            caretIndex_++;
        }
    }
    InvalidateVisual();
}

void TextBoxBase::MoveCaretToStart(bool extendSelection) {
    if (extendSelection) {
        // TODO: 扩展选择
    } else {
        caretIndex_ = 0;
        ClearSelection();
    }
    InvalidateVisual();
}

void TextBoxBase::MoveCaretToEnd(bool extendSelection) {
    if (extendSelection) {
        // TODO: 扩展选择
    } else {
        caretIndex_ = static_cast<int>(text_.length());
        ClearSelection();
    }
    InvalidateVisual();
}

void TextBoxBase::StartCaretBlink() {
    caretVisible_ = true;
    // TODO: 启动光标闪烁定时�?
    InvalidateVisual();
}

void TextBoxBase::StopCaretBlink() {
    caretVisible_ = false;
    // TODO: 停止光标闪烁定时�?
    InvalidateVisual();
}

// ========== 虚方法（子类可重写）==========

std::string TextBoxBase::GetDisplayText() const {
    return text_;  // 默认直接返回文本
}

bool TextBoxBase::ValidateInput(const std::string& /*input*/) {
    return true;  // 默认接受所有输�?
}

void TextBoxBase::OnTextChanged() {
    // 触发 TextChanged 事件
    // RaiseEvent(TextChangedEvent, EventArgs());
}

void TextBoxBase::OnSelectionChanged() {
    // 触发 SelectionChanged 事件
    // RaiseEvent(SelectionChangedEvent, EventArgs());
}

// ========== 渲染方法 ==========

void TextBoxBase::OnRender(render::RenderContext& context) {
    // 渲染文本内容
    RenderText(context);
    
    // 渲染选择背景
    RenderSelection(context);
    
    // 渲染光标
    if (caretVisible_) {
        RenderCaret(context);
    }
}

void TextBoxBase::RenderText(render::RenderContext& /*context*/) {
    // 由子类实现具体的文本渲染
}

void TextBoxBase::RenderSelection(render::RenderContext& context) {
    if (selectionLength_ <= 0) return;
    
    // TODO: 计算选择区域并渲染背�?
    // Rect selectionRect = CalculateSelectionRect();
    // context.DrawRectangle(selectionRect, selectionBrush_, ...);
}

void TextBoxBase::RenderCaret(render::RenderContext& context) {
    // TODO: 计算光标位置并渲�?
    // Point caretPos = CalculateCaretPosition();
    // context.DrawLine(caretPos, Point{caretPos.x, caretPos.y + fontSize_}, caretBrush_, 1.0f);
}

} // namespace ui
} // namespace fk
