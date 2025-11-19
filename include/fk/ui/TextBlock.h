#pragma once

#include "fk/ui/FrameworkElement.h"
#include "fk/ui/TextEnums.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/binding/DependencyProperty.h"
#include <string>

namespace fk {
namespace render { 
    class RenderContext; 
    class TextRenderer;
}
}

namespace fk::ui {

// 前向声明
class Brush;

/**
 * @brief 文本块控件
 * 
 * 职责：
 * - 显示只读文本
 * - 文本样式（字体、颜色等）
 * 
 * WPF 对应：TextBlock
 */
class TextBlock : public FrameworkElement<TextBlock> {
public:
    // ========== 依赖属性 ==========
    
    /// Text 属性：文本内容
    static const binding::DependencyProperty& TextProperty();
    
    /// FontFamily 属性：字体系列
    static const binding::DependencyProperty& FontFamilyProperty();
    
    /// FontSize 属性：字体大小
    static const binding::DependencyProperty& FontSizeProperty();
    
    /// FontWeight 属性：字体粗细
    static const binding::DependencyProperty& FontWeightProperty();
    
    /// FontStyle 属性：字体样式
    static const binding::DependencyProperty& FontStyleProperty();
    
    /// TextAlignment 属性：文本对齐方式
    static const binding::DependencyProperty& TextAlignmentProperty();
    
    /// Foreground 属性：前景色（文本颜色）
    static const binding::DependencyProperty& ForegroundProperty();
    
    /// TextWrapping 属性：文本换行方式
    static const binding::DependencyProperty& TextWrappingProperty();
    
    // ========== 静态方法 ==========
    
    /**
     * @brief 设置全局 TextRenderer（由 Window 调用）
     * @param renderer TextRenderer 实例指针
     */
    static void SetGlobalTextRenderer(render::TextRenderer* renderer);
    
    /**
     * @brief 获取全局 TextRenderer
     * @return TextRenderer 实例指针，可能为 nullptr
     */
    static render::TextRenderer* GetGlobalTextRenderer();

public:
    TextBlock();
    virtual ~TextBlock() = default;

    // ========== 文本内容 ==========
    
    // 使用 PropertyMacros 简化属性声明（从 36 行减少到 3 行）
    FK_PROPERTY_COMPLEX(Text, std::string, TextBlock, this->InvalidateVisual())

    // ========== 字体属性 ==========
    
    FK_PROPERTY_COMPLEX(FontFamily, std::string, TextBlock, this->InvalidateVisual())
    FK_PROPERTY_VISUAL(FontSize, float, TextBlock)
    
    ui::FontWeight GetFontWeight() const { return GetValue<ui::FontWeight>(FontWeightProperty()); }
    void SetFontWeight(ui::FontWeight value) { SetValue(FontWeightProperty(), value); }
    
    TextBlock* FontWeight(ui::FontWeight value) {
        SetFontWeight(value);
        return this;
    }
    ui::FontWeight FontWeight() const { return GetFontWeight(); }
    
    ui::FontStyle GetFontStyle() const { return GetValue<ui::FontStyle>(FontStyleProperty()); }
    void SetFontStyle(ui::FontStyle value) { SetValue(FontStyleProperty(), value); }
    
    TextBlock* FontStyle(ui::FontStyle value) {
        SetFontStyle(value);
        return this;
    }
    ui::FontStyle FontStyle() const { return GetFontStyle(); }
    
    // ========== 文本格式 ==========
    
    ui::TextAlignment GetTextAlignment() const { return GetValue<ui::TextAlignment>(TextAlignmentProperty()); }
    void SetTextAlignment(ui::TextAlignment value) { SetValue(TextAlignmentProperty(), value); }
    
    TextBlock* TextAlignment(ui::TextAlignment value) {
        SetTextAlignment(value);
        return this;
    }
    ui::TextAlignment TextAlignment() const { return GetTextAlignment(); }
    
    ui::TextWrapping GetTextWrapping() const { return GetValue<ui::TextWrapping>(TextWrappingProperty()); }
    void SetTextWrapping(ui::TextWrapping value) { SetValue(TextWrappingProperty(), value); }
    
    TextBlock* TextWrapping(ui::TextWrapping value) {
        SetTextWrapping(value);
        return this;
    }
    ui::TextWrapping TextWrapping() const { return GetTextWrapping(); }
    
    // ========== 外观 ==========
    
    Brush* GetForeground() const { return GetValue<Brush*>(ForegroundProperty()); }
    void SetForeground(Brush* value) { SetValue(ForegroundProperty(), value); }
    
    TextBlock* Foreground(Brush* brush) {
        SetForeground(brush);
        return this;
    }
    Brush* Foreground() const { return GetForeground(); }

protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(render::RenderContext& context) override;
};

} // namespace fk::ui
