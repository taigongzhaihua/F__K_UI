/**
 * @file FrameworkElement_refactored_example.h
 * @brief 使用 PropertyMacros.h 重构后的 FrameworkElement 示例
 * 
 * 这个文件展示了如何使用宏将 FrameworkElement 的尺寸属性从
 * 约 100 行代码减少到 6 行代码，同时保持所有功能不变。
 * 
 * 这不是实际的重构，只是一个概念验证示例。
 */

#pragma once

#include "fk/ui/UIElement.h"
#include "fk/ui/PropertyMacros.h"
#include "fk/binding/DependencyProperty.h"

namespace fk::ui {

// 这是一个示例类，展示重构后的代码
template<typename Derived = void>
class FrameworkElementRefactoredExample : public UIElement {
public:
    // ========== 依赖属性声明 ==========
    
    static const binding::DependencyProperty& WidthProperty();
    static const binding::DependencyProperty& HeightProperty();
    static const binding::DependencyProperty& MinWidthProperty();
    static const binding::DependencyProperty& MaxWidthProperty();
    static const binding::DependencyProperty& MinHeightProperty();
    static const binding::DependencyProperty& MaxHeightProperty();

    // ========== 尺寸属性（重构后）==========
    
    // 原来每个属性需要约 15 行代码，现在只需 1 行！
    // 以下 6 行代码替代了原来的 90+ 行代码
    
    FK_PROPERTY_MEASURE(Width, float, Derived)
    FK_PROPERTY_MEASURE(Height, float, Derived)
    FK_PROPERTY_MEASURE(MinWidth, float, Derived)
    FK_PROPERTY_MEASURE(MaxWidth, float, Derived)
    FK_PROPERTY_MEASURE(MinHeight, float, Derived)
    FK_PROPERTY_MEASURE(MaxHeight, float, Derived)
    
    // 就这么简单！所有功能都自动生成：
    // - GetWidth(), SetWidth()
    // - Width(float), Width(Binding), Width() const
    // - 以及其他 5 个属性的所有方法
    
    // ========== 对比说明 ==========
    
    /*
    原来的代码（以 Width 为例）：
    
    void SetWidth(float value) { 
        SetValue(WidthProperty(), value); 
        InvalidateMeasure(); 
    }
    float GetWidth() const { 
        return GetValue<float>(WidthProperty()); 
    }
    Derived* Width(float value) { 
        SetWidth(value); 
        return static_cast<Derived*>(this); 
    }
    Derived* Width(binding::Binding binding) { 
        this->SetBinding(WidthProperty(), std::move(binding)); 
        return static_cast<Derived*>(this); 
    }
    float Width() const { 
        return GetWidth(); 
    }
    
    重复 5 次（Height, MinWidth, MaxWidth, MinHeight, MaxHeight）
    总计：约 90 行代码
    
    现在的代码：
    
    FK_PROPERTY_MEASURE(Width, float, Derived)
    FK_PROPERTY_MEASURE(Height, float, Derived)
    FK_PROPERTY_MEASURE(MinWidth, float, Derived)
    FK_PROPERTY_MEASURE(MaxWidth, float, Derived)
    FK_PROPERTY_MEASURE(MinHeight, float, Derived)
    FK_PROPERTY_MEASURE(MaxHeight, float, Derived)
    
    总计：6 行代码
    代码减少：93%！
    */
};

// ============================================================================
// Control 类的重构示例
// ============================================================================

template<typename Derived = void>
class ControlRefactoredExample : public FrameworkElementRefactoredExample<Derived> {
public:
    // ========== 依赖属性声明 ==========
    
    static const binding::DependencyProperty& ForegroundProperty();
    static const binding::DependencyProperty& BackgroundProperty();
    static const binding::DependencyProperty& BorderBrushProperty();

    // ========== 外观属性（重构后）==========
    
    // 原来每个属性需要约 12 行代码，现在只需 1 行！
    
    FK_PROPERTY_VISUAL(Foreground, Brush*, Derived)
    FK_PROPERTY_VISUAL(Background, Brush*, Derived)
    FK_PROPERTY_VISUAL(BorderBrush, Brush*, Derived)
    
    // 3 行代码替代了原来的 36+ 行代码
    // 代码减少：92%！
};

// ============================================================================
// TextBlock 类的重构示例
// ============================================================================

class TextBlockRefactoredExample : public FrameworkElementRefactoredExample<TextBlockRefactoredExample> {
public:
    // ========== 依赖属性声明 ==========
    
    static const binding::DependencyProperty& TextProperty();
    static const binding::DependencyProperty& FontFamilyProperty();
    static const binding::DependencyProperty& FontSizeProperty();

    // ========== 文本属性（重构后）==========
    
    // 对于 string 类型，使用 COMPLEX 版本（因为传递 const&）
    FK_PROPERTY_COMPLEX(Text, std::string, TextBlockRefactoredExample, this->InvalidateVisual())
    FK_PROPERTY_COMPLEX(FontFamily, std::string, TextBlockRefactoredExample, this->InvalidateVisual())
    FK_PROPERTY_VISUAL(FontSize, float, TextBlockRefactoredExample)
    
    // 3 行代码替代了原来的 36+ 行代码
};

// ============================================================================
// 使用示例
// ============================================================================

/*
// 重构后的类使用方式完全相同：

auto* element = new FrameworkElementRefactoredExample<MyControl>();

// 直接设置值
element->Width(100.0f)
       ->Height(200.0f);

// 使用绑定
element->Width(bind("ElementWidth"))
       ->Height(bind("ElementHeight"));

// 混合使用
element->Width(bind("Width"))
       ->Height(100.0f)
       ->MinWidth(50.0f);

// 获取值
float width = element->Width();
*/

} // namespace fk::ui
