#pragma once

/**
 * @file PropertyMacros.h
 * @brief 用于简化依赖属性声明的宏定义
 * 
 * 这些宏可以大幅减少为依赖属性编写 getter、setter 和链式绑定接口的重复代码。
 * 虽然现代 C++ 一般不推荐宏，但对于这种高度重复的模式代码，
 * 宏提供了最简洁和可维护的解决方案。
 */

#include "fk/binding/Binding.h"

// ============================================================================
// 基础宏：为简单类型的依赖属性生成完整接口
// ============================================================================

/**
 * @brief 为简单值类型的依赖属性生成完整接口（Get/Set + 链式API + 链式绑定）
 * @param PropertyName 属性名称（如 Width）
 * @param Type 属性类型（如 float）
 * @param Derived 派生类类型（CRTP 模式）
 * 
 * 生成的方法：
 * - Type GetPropertyName() const
 * - void SetPropertyName(Type value)
 * - Derived* PropertyName(Type value)        // 链式设置
 * - Derived* PropertyName(Binding binding)   // 链式绑定
 * - Type PropertyName() const                // 获取值
 * 
 * 使用示例：
 * FK_PROPERTY_SIMPLE(Width, float, Derived, InvalidateMeasure())
 */
#define FK_PROPERTY_SIMPLE(PropertyName, Type, Derived, OnChangeAction) \
    Type Get##PropertyName() const { \
        return this->template GetValue<Type>(PropertyName##Property()); \
    } \
    void Set##PropertyName(Type value) { \
        this->SetValue(PropertyName##Property(), value); \
        OnChangeAction; \
    } \
    Derived* PropertyName(Type value) { \
        Set##PropertyName(value); \
        return static_cast<Derived*>(this); \
    } \
    Derived* PropertyName(const binding::Binding& binding) { \
        this->SetBinding(PropertyName##Property(), binding); \
        return static_cast<Derived*>(this); \
    } \
    Type PropertyName() const { return Get##PropertyName(); }

/**
 * @brief 为指针类型的依赖属性生成完整接口
 * @param PropertyName 属性名称（如 Foreground）
 * @param Type 属性类型（如 Brush*）
 * @param Derived 派生类类型（CRTP 模式）
 * 
 * 与 FK_PROPERTY_SIMPLE 类似，但针对指针类型优化
 */
#define FK_PROPERTY_POINTER(PropertyName, Type, Derived, OnChangeAction) \
    Type Get##PropertyName() const { \
        return this->template GetValue<Type>(PropertyName##Property()); \
    } \
    void Set##PropertyName(Type value) { \
        this->SetValue(PropertyName##Property(), value); \
        OnChangeAction; \
    } \
    Derived* PropertyName(Type value) { \
        Set##PropertyName(value); \
        return static_cast<Derived*>(this); \
    } \
    Derived* PropertyName(const binding::Binding& binding) { \
        this->SetBinding(PropertyName##Property(), binding); \
        return static_cast<Derived*>(this); \
    } \
    Type PropertyName() const { return Get##PropertyName(); }

/**
 * @brief 为复杂类型（如结构体）的依赖属性生成完整接口
 * @param PropertyName 属性名称（如 Margin）
 * @param Type 属性类型（如 Thickness）
 * @param Derived 派生类类型（CRTP 模式）
 * 
 * 与简单类型类似，但使用 const 引用传参
 */
#define FK_PROPERTY_COMPLEX(PropertyName, Type, Derived, OnChangeAction) \
    Type Get##PropertyName() const { \
        return this->template GetValue<Type>(PropertyName##Property()); \
    } \
    void Set##PropertyName(const Type& value) { \
        this->SetValue(PropertyName##Property(), value); \
        OnChangeAction; \
    } \
    Derived* PropertyName(const Type& value) { \
        Set##PropertyName(value); \
        return static_cast<Derived*>(this); \
    } \
    Derived* PropertyName(const binding::Binding& binding) { \
        this->SetBinding(PropertyName##Property(), binding); \
        return static_cast<Derived*>(this); \
    } \
    Type PropertyName() const { return Get##PropertyName(); }

// ============================================================================
// 特殊宏：为带有多重载的属性生成接口
// ============================================================================

/**
 * @brief 为支持多种参数形式的属性生成基础接口
 * @param PropertyName 属性名称
 * @param Type 属性类型
 * @param Derived 派生类类型
 * 
 * 仅生成 Get/Set 和基本链式接口，不包括额外的便捷重载
 * 适用于需要手动添加特殊重载的情况（如 Margin 的 uniform 重载）
 */
#define FK_PROPERTY_BASE(PropertyName, Type, Derived, OnChangeAction) \
    Type Get##PropertyName() const { \
        return this->template GetValue<Type>(PropertyName##Property()); \
    } \
    void Set##PropertyName(const Type& value) { \
        this->SetValue(PropertyName##Property(), value); \
        OnChangeAction; \
    } \
    Derived* PropertyName(const Type& value) { \
        Set##PropertyName(value); \
        return static_cast<Derived*>(this); \
    } \
    Derived* PropertyName(binding::Binding binding) { \
        this->SetBinding(PropertyName##Property(), std::move(binding)); \
        return static_cast<Derived*>(this); \
    } \
    Type PropertyName() const { return Get##PropertyName(); }

// ============================================================================
// 便捷宏：常见操作的简写
// ============================================================================

/**
 * @brief 无额外操作的简单属性（不触发任何变更通知）
 */
#define FK_PROPERTY_SIMPLE_NO_ACTION(PropertyName, Type, Derived) \
    FK_PROPERTY_SIMPLE(PropertyName, Type, Derived, (void)0)

/**
 * @brief 触发测量失效的属性
 */
#define FK_PROPERTY_MEASURE(PropertyName, Type, Derived) \
    FK_PROPERTY_SIMPLE(PropertyName, Type, Derived, this->InvalidateMeasure())

/**
 * @brief 触发排列失效的属性
 */
#define FK_PROPERTY_ARRANGE(PropertyName, Type, Derived) \
    FK_PROPERTY_SIMPLE(PropertyName, Type, Derived, this->InvalidateArrange())

/**
 * @brief 触发视觉失效的属性
 */
#define FK_PROPERTY_VISUAL(PropertyName, Type, Derived) \
    FK_PROPERTY_POINTER(PropertyName, Type, Derived, this->InvalidateVisual())

/**
 * @brief 触发测量失效的复杂类型属性
 */
#define FK_PROPERTY_COMPLEX_MEASURE(PropertyName, Type, Derived) \
    FK_PROPERTY_COMPLEX(PropertyName, Type, Derived, this->InvalidateMeasure())

// ============================================================================
// 使用示例（注释形式）
// ============================================================================

/*
// 在 FrameworkElement 中使用：
class FrameworkElement : public UIElement {
    // 原来需要写 20+ 行代码
    void SetWidth(float value) { ... }
    float GetWidth() const { ... }
    Derived* Width(float value) { ... }
    Derived* Width(binding::Binding binding) { ... }
    float Width() const { ... }
    
    // 现在只需要 1 行：
    FK_PROPERTY_MEASURE(Width, float, Derived)
    FK_PROPERTY_MEASURE(Height, float, Derived)
    FK_PROPERTY_MEASURE(MinWidth, float, Derived)
    FK_PROPERTY_MEASURE(MaxWidth, float, Derived)
    FK_PROPERTY_MEASURE(MinHeight, float, Derived)
    FK_PROPERTY_MEASURE(MaxHeight, float, Derived)
};

// 在 Control 中使用：
class Control : public FrameworkElement<Derived> {
    // 原来每个属性需要 10+ 行
    // 现在只需要 1 行：
    FK_PROPERTY_VISUAL(Foreground, Brush*, Derived)
    FK_PROPERTY_VISUAL(Background, Brush*, Derived)
    FK_PROPERTY_VISUAL(BorderBrush, Brush*, Derived)
};

// 在 TextBlock 中使用：
class TextBlock : public FrameworkElement<TextBlock> {
    FK_PROPERTY_VISUAL(Text, std::string, TextBlock)
    FK_PROPERTY_VISUAL(FontFamily, std::string, TextBlock)
    FK_PROPERTY_VISUAL(FontSize, float, TextBlock)
};
*/
