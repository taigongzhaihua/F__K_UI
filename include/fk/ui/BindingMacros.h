#pragma once

/**
 * @file BindingMacros.h
 * @brief 用于简化依赖属性绑定支持的宏
 *
 * 这些宏可以快速为依赖属性添加 Binding 重载，支持链式绑定语法。
 */

#include "fk/binding/Binding.h"

namespace fk::ui {

/**
 * @brief 通用属性绑定宏（适用于所有类型：string, float, int, bool, enum 等）
 * 
 * 适用于：直接继承 enable_shared_from_this 的类（如 TextBlock, ScrollBar, ScrollViewer）
 * 
 * 使用示例：
 * ```cpp
 * // 在 TextBlock 模板类中
 * FK_BINDING_PROPERTY(Text, std::string)          // ✅ 引用类型
 * FK_BINDING_PROPERTY(FontSize, float)            // ✅ 值类型
 * FK_BINDING_PROPERTY(TextWrapping, ui::TextWrapping)  // ✅ 枚举类型
 * FK_BINDING_PROPERTY(IsEnabled, bool)            // ✅ 布尔类型
 * ```
 * 
 * 展开后生成：
 * ```cpp
 * // Getter
 * [[nodiscard]] const ValueType& PropertyName() const {
 *     return GetPropertyName();
 * }
 * 
 * // Setter (普通值)
 * Ptr PropertyName(const ValueType& value) {
 *     SetPropertyName(value);
 *     return Self();
 * }
 * 
 * // Setter (绑定)
 * Ptr PropertyName(binding::Binding binding) {
 *     SetBinding(PropertyNameProperty(), std::move(binding));
 *     return Self();
 * }
 * ```
 */
#define FK_BINDING_PROPERTY(PropertyName, ValueType)                                     \
    [[nodiscard]] const ValueType& PropertyName() const {                               \
        return Get##PropertyName();                                                      \
    }                                                                                    \
                                                                                         \
    Ptr PropertyName(const ValueType& value) {                                          \
        Set##PropertyName(value);                                                        \
        return Self();                                                                   \
    }                                                                                    \
                                                                                         \
    Ptr PropertyName(binding::Binding binding) {                                        \
        SetBinding(PropertyName##Property(), std::move(binding));                       \
        return Self();                                                                   \
    }



/**
 * @brief 为属性添加绑定支持（使用 this-> 访问基类方法）
 * 
 * 适用于 View 派生类，通过 this-> 访问基类的 Get/Set 方法
 * 适用于所有类型：string, float, int, bool, enum 等
 * 需要指定 BaseClass 来访问 Property() 静态方法
 * 
 * 使用示例：
 * ```cpp
 * FK_BINDING_PROPERTY_THIS(Background, std::string, ButtonBase)  // 引用类型
 * FK_BINDING_PROPERTY_THIS(IsPressed, bool, ButtonBase)          // bool
 * FK_BINDING_PROPERTY_THIS(ClickMode, ClickMode, ButtonBase)     // enum
 * ```
 */
#define FK_BINDING_PROPERTY_THIS(PropertyName, ValueType, BaseClass)               \
    [[nodiscard]] const ValueType& PropertyName() const {                          \
        return this->Get##PropertyName();                                          \
    }                                                                              \
    Ptr PropertyName(const ValueType& value) {                                     \
        this->Set##PropertyName(value);                                            \
        return this->Self();                                                       \
    }                                                                              \
    Ptr PropertyName(binding::Binding binding) {                                   \
        this->SetBinding(BaseClass::PropertyName##Property(), std::move(binding)); \
        return this->Self();                                                       \
    }

/**
 * @brief 为属性添加绑定支持（使用 Base:: 访问基类方法）
 * 
 * 适用于 View<Derived, Base> 模板类，使用 Base:: 访问继承的方法
 * 适用于所有类型：string, float, int, bool, enum 等
 * 需要指定 PropertyOwner 来访问 Property() 静态方法
 * 
 * 使用示例：
 * ```cpp
 * // 在 View 模板类中
 * FK_BINDING_PROPERTY_BASE(Width, float, FrameworkElement)       // 值类型
 * FK_BINDING_PROPERTY_BASE(Visibility, Visibility, UIElement)    // enum
 * FK_BINDING_PROPERTY_BASE(IsEnabled, bool, UIElement)           // bool
 * FK_BINDING_PROPERTY_BASE(Background, Brush, Control)           // 引用类型
 * ```
 */
#define FK_BINDING_PROPERTY_BASE(PropertyName, ValueType, PropertyOwner)               \
    [[nodiscard]] const ValueType& PropertyName() const {                              \
        return Base::Get##PropertyName();                                              \
    }                                                                                  \
    Ptr PropertyName(const ValueType& value) {                                         \
        Base::Set##PropertyName(value);                                                \
        return this->Self();                                                           \
    }                                                                                  \
    Ptr PropertyName(binding::Binding binding) {                                       \
        Base::SetBinding(PropertyOwner::PropertyName##Property(), std::move(binding)); \
        return this->Self();                                                           \
    }

// ============================================================================
// 向后兼容别名 (Backward Compatibility Aliases)
// ============================================================================

// FK_BINDING_PROPERTY 的别名
#define FK_BINDING_PROPERTY_VALUE FK_BINDING_PROPERTY
#define FK_BINDING_PROPERTY_ENUM FK_BINDING_PROPERTY

// FK_BINDING_PROPERTY_THIS 的别名
#define FK_BINDING_PROPERTY_VALUE_BASE FK_BINDING_PROPERTY_THIS

// FK_BINDING_PROPERTY_BASE 的别名
#define FK_BINDING_PROPERTY_VIEW FK_BINDING_PROPERTY_BASE
#define FK_BINDING_PROPERTY_VIEW_VALUE FK_BINDING_PROPERTY_BASE
#define FK_BINDING_PROPERTY_VIEW_ENUM FK_BINDING_PROPERTY_BASE

} // namespace fk::ui
