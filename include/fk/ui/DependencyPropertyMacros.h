#pragma once

/**
 * @file DependencyPropertyMacros.h
 * @brief 用于简化依赖属性声明和实现的宏
 * 
 * 这些宏可以大幅减少依赖属性的样板代码，统一代码风格。
 */

#include "fk/binding/DependencyProperty.h"

namespace fk::ui {

// ============================================================================
// 头文件中的声明宏（用于 .h 文件）
// ============================================================================

/**
 * @brief 在头文件中声明依赖属性的所有必要方法
 * 
 * 适用于值类型（float, int, bool, enum 等）
 * 
 * 生成：
 * - static const DependencyProperty& PropertyNameProperty();
 * - void SetPropertyName(ValueType value);
 * - ValueType GetPropertyName() const;
 * - virtual void OnPropertyNameChanged(ValueType oldValue, ValueType newValue);
 * - static void PropertyNamePropertyChanged(...);
 * - static PropertyMetadata BuildPropertyNameMetadata();
 * 
 * 使用示例：
 * ```cpp
 * class MyControl {
 * public:
 *     FK_DEPENDENCY_PROPERTY_DECLARE(CornerRadius, float)
 *     FK_DEPENDENCY_PROPERTY_DECLARE(IsEnabled, bool)
 * };
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_DECLARE(PropertyName, ValueType)                                     \
public:                                                                                             \
    static const binding::DependencyProperty& PropertyName##Property();                            \
    void Set##PropertyName(ValueType value);                                                        \
    [[nodiscard]] ValueType Get##PropertyName() const;                                             \
                                                                                                    \
protected:                                                                                          \
    virtual void On##PropertyName##Changed(ValueType oldValue, ValueType newValue);                \
                                                                                                    \
private:                                                                                            \
    static binding::PropertyMetadata Build##PropertyName##Metadata();                              \
    static void PropertyName##PropertyChanged(binding::DependencyObject& sender,                   \
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);

/**
 * @brief 在头文件中声明依赖属性（引用类型版本）
 * 
 * 适用于引用类型（std::string, Brush 等需要返回 const& 的类型）
 * 
 * 使用示例：
 * ```cpp
 * class MyControl {
 * public:
 *     FK_DEPENDENCY_PROPERTY_DECLARE_REF(Background, std::string)
 *     FK_DEPENDENCY_PROPERTY_DECLARE_REF(Text, std::string)
 * };
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_DECLARE_REF(PropertyName, ValueType)                                 \
public:                                                                                             \
    static const binding::DependencyProperty& PropertyName##Property();                            \
    void Set##PropertyName(ValueType value);                                                        \
    [[nodiscard]] const ValueType& Get##PropertyName() const;                                      \
                                                                                                    \
protected:                                                                                          \
    virtual void On##PropertyName##Changed(const ValueType& oldValue, const ValueType& newValue);  \
                                                                                                    \
private:                                                                                            \
    static binding::PropertyMetadata Build##PropertyName##Metadata();                              \
    static void PropertyName##PropertyChanged(binding::DependencyObject& sender,                   \
        const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue);

// ============================================================================
// 实现文件中的定义宏（用于 .cpp 文件）
// ============================================================================

/**
 * @brief 实现依赖属性的注册方法（Property() 静态函数）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_REGISTER(ButtonBase, CornerRadius, float)
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_REGISTER(OwnerClass, PropertyName, ValueType)                       \
const binding::DependencyProperty& OwnerClass::PropertyName##Property() {                          \
    static const auto& property = binding::DependencyProperty::Register(                           \
        #PropertyName,                                                                              \
        typeid(ValueType),                                                                          \
        typeid(OwnerClass),                                                                         \
        Build##PropertyName##Metadata()                                                            \
    );                                                                                              \
    return property;                                                                                \
}

/**
 * @brief 实现依赖属性的 Getter/Setter（值类型版本）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_IMPL(ButtonBase, CornerRadius, float, 0.0f)
 * FK_DEPENDENCY_PROPERTY_IMPL(ButtonBase, IsEnabled, bool, false)
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_IMPL(OwnerClass, PropertyName, ValueType, DefaultValue)             \
void OwnerClass::Set##PropertyName(ValueType value) {                                              \
    binding::DependencyObject::SetValue(PropertyName##Property(), value);                          \
}                                                                                                   \
                                                                                                    \
ValueType OwnerClass::Get##PropertyName() const {                                                  \
    const auto& value = binding::DependencyObject::GetValue(PropertyName##Property());             \
    if (!value.has_value()) return DefaultValue;                                                   \
    try {                                                                                           \
        return std::any_cast<ValueType>(value);                                                    \
    } catch (...) {                                                                                 \
        return DefaultValue;                                                                       \
    }                                                                                               \
}

/**
 * @brief 实现依赖属性的 Getter/Setter（引用类型版本）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_IMPL_REF(ButtonBase, Background, std::string)
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_IMPL_REF(OwnerClass, PropertyName, ValueType)                       \
void OwnerClass::Set##PropertyName(ValueType value) {                                              \
    binding::DependencyObject::SetValue(PropertyName##Property(), std::move(value));               \
}                                                                                                   \
                                                                                                    \
const ValueType& OwnerClass::Get##PropertyName() const {                                           \
    const auto& value = binding::DependencyObject::GetValue(PropertyName##Property());             \
    static const ValueType empty{};                                                                \
    if (!value.has_value()) return empty;                                                          \
    try {                                                                                           \
        return std::any_cast<const ValueType&>(value);                                             \
    } catch (...) {                                                                                 \
        return empty;                                                                              \
    }                                                                                               \
}

/**
 * @brief 实现依赖属性的静态回调函数（值类型版本）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_CALLBACK(ButtonBase, CornerRadius, float, 0.0f)
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_CALLBACK(OwnerClass, PropertyName, ValueType, DefaultValue)         \
void OwnerClass::PropertyName##PropertyChanged(binding::DependencyObject& sender,                  \
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) { \
    auto* obj = dynamic_cast<OwnerClass*>(&sender);                                                \
    if (!obj) return;                                                                              \
                                                                                                    \
    try {                                                                                           \
        ValueType oldVal = oldValue.has_value() ? std::any_cast<ValueType>(oldValue) : DefaultValue; \
        ValueType newVal = std::any_cast<ValueType>(newValue);                                     \
        obj->On##PropertyName##Changed(oldVal, newVal);                                            \
    } catch (...) {}                                                                                \
}

/**
 * @brief 实现依赖属性的静态回调函数（引用类型版本）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_CALLBACK_REF(ButtonBase, Background, std::string)
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_CALLBACK_REF(OwnerClass, PropertyName, ValueType)                   \
void OwnerClass::PropertyName##PropertyChanged(binding::DependencyObject& sender,                  \
    const binding::DependencyProperty& property, const std::any& oldValue, const std::any& newValue) { \
    auto* obj = dynamic_cast<OwnerClass*>(&sender);                                                \
    if (!obj) return;                                                                              \
                                                                                                    \
    try {                                                                                           \
        const auto& oldVal = oldValue.has_value() ? std::any_cast<const ValueType&>(oldValue) : ValueType{}; \
        const auto& newVal = std::any_cast<const ValueType&>(newValue);                            \
        obj->On##PropertyName##Changed(oldVal, newVal);                                            \
    } catch (...) {}                                                                                \
}

// ============================================================================
// 组合宏：一次性完成实现文件中的所有定义
// ============================================================================

/**
 * @brief 在 .cpp 文件中一次性实现依赖属性的所有方法（值类型）
 * 
 * 包含：Register + Getter/Setter + Callback
 * 不包含：Metadata 构建（因为通常需要自定义）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_DEFINE(ButtonBase, CornerRadius, float, 0.0f)
 * 
 * // 然后手动实现 Metadata 和 OnChanged：
 * binding::PropertyMetadata ButtonBase::BuildCornerRadiusMetadata() {
 *     binding::PropertyMetadata metadata(4.0f);
 *     metadata.propertyChangedCallback = CornerRadiusPropertyChanged;
 *     return metadata;
 * }
 * 
 * void ButtonBase::OnCornerRadiusChanged(float oldValue, float newValue) {
 *     InvalidateVisual();
 * }
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_DEFINE(OwnerClass, PropertyName, ValueType, DefaultValue)           \
    FK_DEPENDENCY_PROPERTY_REGISTER(OwnerClass, PropertyName, ValueType)                           \
    FK_DEPENDENCY_PROPERTY_IMPL(OwnerClass, PropertyName, ValueType, DefaultValue)                 \
    FK_DEPENDENCY_PROPERTY_CALLBACK(OwnerClass, PropertyName, ValueType, DefaultValue)

/**
 * @brief 在 .cpp 文件中一次性实现依赖属性的所有方法（引用类型）
 * 
 * 使用示例：
 * ```cpp
 * FK_DEPENDENCY_PROPERTY_DEFINE_REF(ButtonBase, Background, std::string)
 * 
 * // 然后手动实现 Metadata 和 OnChanged
 * ```
 */
#define FK_DEPENDENCY_PROPERTY_DEFINE_REF(OwnerClass, PropertyName, ValueType)                     \
    FK_DEPENDENCY_PROPERTY_REGISTER(OwnerClass, PropertyName, ValueType)                           \
    FK_DEPENDENCY_PROPERTY_IMPL_REF(OwnerClass, PropertyName, ValueType)                           \
    FK_DEPENDENCY_PROPERTY_CALLBACK_REF(OwnerClass, PropertyName, ValueType)

} // namespace fk::ui
