#pragma once

#include "fk/ui/Control.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"
#include "fk/core/Event.h"

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

namespace fk::ui {

namespace detail {

class TextBoxBase : public ControlBase {
public:
    using ControlBase::ControlBase;

    TextBoxBase();
    ~TextBoxBase() override;

    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(PlaceholderText, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(IsReadOnly, bool)
    FK_DEPENDENCY_PROPERTY_DECLARE(CaretIndex, int)
    FK_DEPENDENCY_PROPERTY_DECLARE(SelectionStart, int)
    FK_DEPENDENCY_PROPERTY_DECLARE(SelectionLength, int)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Foreground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(BorderBrush, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(BorderThickness, float)

public:
    core::Event<TextBoxBase&, const std::string&, const std::string&> TextChanged;

    [[nodiscard]] bool ShouldShowCaret() const;
    [[nodiscard]] Rect GetCaretRect() const;
    [[nodiscard]] bool HasSelection() const;
    [[nodiscard]] Rect GetSelectionRect() const;
    void PrepareForRender(double frameTime);

protected:
    void OnAttachedToLogicalTree() override;
    bool HasRenderContent() const override;
    bool OnMouseButtonDown(int button, double x, double y) override;
    bool OnMouseButtonUp(int button, double x, double y) override;
    bool OnMouseMove(double x, double y) override;
    bool OnKeyDown(int key, int scancode, int mods) override;
    bool OnTextInput(unsigned int codepoint) override;
    void OnFocusGained() override;
    void OnFocusLost() override;

private:
    static bool ValidateNonNegativeInt(const std::any& value);
    static bool ValidateColor(const std::any& value);
    static bool ValidateBorderThickness(const std::any& value);

    bool RemoveSelection(std::string& text, int& caretIndex);
    void CommitTextChange(std::string newText, int caretIndex);
    bool HandleBackspace();
    bool HandleDelete();
    bool MoveCaretLeft();
    bool MoveCaretRight();
    bool MoveCaretToStart();
    bool MoveCaretToEnd();
    float ComputeOffsetForIndex(int index) const;
    float ComputeCaretOffset() const;
    int HitTestCaretIndex(float pointX) const;
    void ResetCaretBlink();
    void UpdateCaretAnimation(double frameTime);

    void EnsureContent();
    void UpdateTextPresenter();
    void ApplyForeground();
    void UpdateSelectionVisual();
    void ClampCaretAndSelection();
    void EnsureCaretVisible();  // 🎯 确保光标可见(自动滚动)

    std::shared_ptr<TextBlockView> textPresenter_{};
    std::shared_ptr<UIElement> presenterElement_{};
    bool placeholderVisible_{false};
    std::string placeholderColor_{"#808080"};
    bool isFocused_{false};
    bool isSelecting_{false};
    int selectionAnchor_{0};
    bool updatingFromBinding_{false};
    bool caretBlinkVisible_{false};
    double caretBlinkPeriod_{0.8};
    double caretBlinkStartTime_{0.0};
    double lastFrameTime_{0.0};
    float horizontalScrollOffset_{0.0f};  // 🎯 水平滚动偏移
};

} // namespace detail

template <typename Derived = void>
class TextBox : public View<std::conditional_t<std::is_void_v<Derived>, TextBox<>, Derived>, detail::TextBoxBase> {
public:
    using Base = View<std::conditional_t<std::is_void_v<Derived>, TextBox<>, Derived>, detail::TextBoxBase>;
    using Ptr = typename Base::Ptr;
    using TextBoxBase = detail::TextBoxBase;

    using Base::Base;

    FK_BINDING_PROPERTY_BASE(Text, std::string, TextBoxBase)
    FK_BINDING_PROPERTY_BASE(PlaceholderText, std::string, TextBoxBase)
    FK_BINDING_PROPERTY_VALUE_BASE(IsReadOnly, bool, TextBoxBase)
    FK_BINDING_PROPERTY_VALUE_BASE(CaretIndex, int, TextBoxBase)
    FK_BINDING_PROPERTY_VALUE_BASE(SelectionStart, int, TextBoxBase)
    FK_BINDING_PROPERTY_VALUE_BASE(SelectionLength, int, TextBoxBase)
    FK_BINDING_PROPERTY_BASE(Foreground, std::string, TextBoxBase)
    FK_BINDING_PROPERTY_BASE(Background, std::string, TextBoxBase)
    FK_BINDING_PROPERTY_BASE(BorderBrush, std::string, TextBoxBase)
    FK_BINDING_PROPERTY_VALUE_BASE(BorderThickness, float, TextBoxBase)

    Ptr OnTextChanged(std::function<void(TextBoxBase&, const std::string&, const std::string&)> handler) {
        this->TextChanged.Add(std::move(handler));
        return this->Self();
    }

    static Ptr Create() {
        return std::make_shared<std::conditional_t<std::is_void_v<Derived>, TextBox<>, Derived>>();
    }
};

using TextBoxPtr = TextBox<>::Ptr;

inline TextBoxPtr textBox() {
    return TextBox<>::Create();
}

} // namespace fk::ui
