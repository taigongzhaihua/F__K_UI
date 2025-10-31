#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/View.h"

#include <string>
#include <memory>

namespace fk::ui {

/**
 * @brief 文本换行模式
 */
enum class TextWrapping {
    NoWrap,  // 不换行
    Wrap     // 自动换行
};

/**
 * @brief 文本截断模式
 */
enum class TextTrimming {
    None,              // 不截断
    CharacterEllipsis, // 字符级别截断,添加省略号(...)
    WordEllipsis       // 单词级别截断,添加省略号(...)
};

namespace detail {

/**
 * @brief TextBlock 基类实现
 * 
 * 提供文本显示功能：
 * - Text 文本内容
 * - Foreground 前景色(文字颜色)
 * - FontSize 字体大小
 * - FontFamily 字体族
 */
class TextBlockBase : public FrameworkElement {
public:
    using FrameworkElement::FrameworkElement;

    TextBlockBase();
    ~TextBlockBase() override;

    // 依赖属性
    static const binding::DependencyProperty& TextProperty();
    static const binding::DependencyProperty& ForegroundProperty();
    static const binding::DependencyProperty& FontSizeProperty();
    static const binding::DependencyProperty& FontFamilyProperty();
    static const binding::DependencyProperty& TextWrappingProperty();
    static const binding::DependencyProperty& TextTrimmingProperty();

    // Text 属性
    void SetText(const std::string& text);
    [[nodiscard]] const std::string& GetText() const;

    // Foreground 属性 (RGBA hex string: "#RRGGBBAA" or "#RRGGBB")
    void SetForeground(const std::string& color);
    [[nodiscard]] const std::string& GetForeground() const;

    // FontSize 属性
    void SetFontSize(float size);
    [[nodiscard]] float GetFontSize() const;

    // FontFamily 属性
    void SetFontFamily(const std::string& family);
    [[nodiscard]] const std::string& GetFontFamily() const;

    // TextWrapping 属性
    void SetTextWrapping(TextWrapping wrapping);
    [[nodiscard]] TextWrapping GetTextWrapping() const;

    // TextTrimming 属性
    void SetTextTrimming(TextTrimming trimming);
    [[nodiscard]] TextTrimming GetTextTrimming() const;

    // 辅助方法:获取分行后的文本(用于渲染)
    [[nodiscard]] const std::vector<std::string>& GetWrappedLines() const { return wrappedLines_; }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    bool HasRenderContent() const override;

    // 属性变更回调
    virtual void OnTextChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnForegroundChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnFontSizeChanged(float oldValue, float newValue);
    virtual void OnFontFamilyChanged(const std::string& oldValue, const std::string& newValue);
    virtual void OnTextWrappingChanged(TextWrapping oldValue, TextWrapping newValue);
    virtual void OnTextTrimmingChanged(TextTrimming oldValue, TextTrimming newValue);

private:
    // 元数据构建
    static binding::PropertyMetadata BuildTextMetadata();
    static binding::PropertyMetadata BuildForegroundMetadata();
    static binding::PropertyMetadata BuildFontSizeMetadata();
    static binding::PropertyMetadata BuildFontFamilyMetadata();
    static binding::PropertyMetadata BuildTextWrappingMetadata();
    static binding::PropertyMetadata BuildTextTrimmingMetadata();

    // 属性变更回调 (静态)
    static void TextPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void ForegroundPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void FontSizePropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void FontFamilyPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void TextWrappingPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    static void TextTrimmingPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );

    // 验证回调
    static bool ValidateColor(const std::any& value);
    static bool ValidateFontSize(const std::any& value);

private:
    mutable std::vector<std::string> wrappedLines_;  // 缓存的分行文本
};

} // namespace detail

// TextBlock 模板类 (支持链式调用)
template <typename Derived>
class TextBlock : public detail::TextBlockBase, public std::enable_shared_from_this<Derived> {
public:
    using BaseType = detail::TextBlockBase;
    using Ptr = std::shared_ptr<Derived>;

    using detail::TextBlockBase::TextBlockBase;

    // Fluent API: Width/Height (从 FrameworkElement 继承)
    [[nodiscard]] float Width() const { return GetWidth(); }
    Ptr Width(float value) {
        SetWidth(value);
        return Self();
    }

    [[nodiscard]] float Height() const { return GetHeight(); }
    Ptr Height(float value) {
        SetHeight(value);
        return Self();
    }

    // Fluent API: Text
    [[nodiscard]] const std::string& Text() const {
        return GetText();
    }

    Ptr Text(const std::string& text) {
        SetText(text);
        return Self();
    }

    // Fluent API: Foreground
    [[nodiscard]] const std::string& Foreground() const {
        return GetForeground();
    }

    Ptr Foreground(const std::string& color) {
        SetForeground(color);
        return Self();
    }

    // Fluent API: FontSize
    [[nodiscard]] float FontSize() const {
        return GetFontSize();
    }

    Ptr FontSize(float size) {
        SetFontSize(size);
        return Self();
    }

    // Fluent API: FontFamily
    [[nodiscard]] const std::string& FontFamily() const {
        return GetFontFamily();
    }

    Ptr FontFamily(const std::string& family) {
        SetFontFamily(family);
        return Self();
    }

    // Fluent API: TextWrapping
    [[nodiscard]] ui::TextWrapping TextWrapping() const {
        return GetTextWrapping();
    }

    Ptr TextWrapping(ui::TextWrapping wrapping) {
        SetTextWrapping(wrapping);
        return Self();
    }

    // Fluent API: TextTrimming
    [[nodiscard]] ui::TextTrimming TextTrimming() const {
        return GetTextTrimming();
    }

    Ptr TextTrimming(ui::TextTrimming trimming) {
        SetTextTrimming(trimming);
        return Self();
    }

protected:
    Ptr Self() {
        auto* derivedThis = static_cast<Derived*>(this);
        return std::static_pointer_cast<Derived>(derivedThis->shared_from_this());
    }
};

// 标准 TextBlock 类
class TextBlockView : public TextBlock<TextBlockView> {
public:
    using TextBlock<TextBlockView>::TextBlock;
};

// 工厂函数
inline std::shared_ptr<TextBlockView> textBlock() {
    return std::make_shared<TextBlockView>();
}

} // namespace fk::ui
