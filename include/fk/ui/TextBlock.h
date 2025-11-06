#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/View.h"
#include "fk/ui/BindingMacros.h"
#include "fk/ui/DependencyPropertyMacros.h"

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

    // 依赖属性（使用宏声明）
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(Foreground, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(FontSize, float)
    FK_DEPENDENCY_PROPERTY_DECLARE_REF(FontFamily, std::string)
    FK_DEPENDENCY_PROPERTY_DECLARE(TextWrapping, TextWrapping)
    FK_DEPENDENCY_PROPERTY_DECLARE(TextTrimming, TextTrimming)

public:

    // 辅助方法:获取分行后的文本(用于渲染)
    [[nodiscard]] const std::vector<std::string>& GetWrappedLines() const { return wrappedLines_; }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    bool HasRenderContent() const override;

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

    // Fluent API with Binding Support
    // 使用宏自动生成 getter, setter 和 binding 重载
    FK_BINDING_PROPERTY(Text, std::string)
    FK_BINDING_PROPERTY(Foreground, std::string)
    FK_BINDING_PROPERTY_VALUE(FontSize, float)
    FK_BINDING_PROPERTY(FontFamily, std::string)
    FK_BINDING_PROPERTY_ENUM(TextWrapping, ui::TextWrapping)
    FK_BINDING_PROPERTY_ENUM(TextTrimming, ui::TextTrimming)

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
