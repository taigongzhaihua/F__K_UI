#pragma once

#include "fk/binding/ObservableObject.h"
#include "fk/binding/PropertyAccessors.h"

/**
 * @file ViewModelMacros.h
 * @brief ViewModel 自动注册宏
 *
 * 使用方法：
 * 1. 在 ViewModel 类定义后使用 FK_VIEWMODEL_BEGIN/END 包裹属性声明
 * 2. 使用 FK_VIEWMODEL_PROPERTY 声明每个属性
 *
 * 示例：
 * ```cpp
 * class MyViewModel : public binding::ObservableObject {
 * public:
 *     std::string GetName() const { return name_; }
 *     void SetName(const std::string& value) {
 *         name_ = value;
 *         RaisePropertyChanged("Name");
 *     }
 * private:
 *     std::string name_;
 * };
 *
 * FK_VIEWMODEL_BEGIN(MyViewModel)
 *     FK_VIEWMODEL_PROPERTY(Name, &MyViewModel::GetName)
 * FK_VIEWMODEL_END()
 * ```
 */

// 辅助宏：生成唯一名称
#define FK_VM_UNIQUE_NAME(base) FK_VM_UNIQUE_NAME_IMPL(base, __LINE__)
#define FK_VM_UNIQUE_NAME_IMPL(base, line) FK_VM_UNIQUE_NAME_IMPL2(base, line)
#define FK_VM_UNIQUE_NAME_IMPL2(base, line) base##line

/**
 * 开始 ViewModel 属性注册块
 * @param ViewModelType ViewModel 类名
 */
#define FK_VIEWMODEL_BEGIN(ViewModelType)         \
    namespace                                     \
    {                                             \
        struct ViewModelRegistrar_##ViewModelType \
        {                                         \
            ViewModelRegistrar_##ViewModelType()  \
            {                                     \
                using VM = ViewModelType;         \
                using Base = ::fk::binding::INotifyPropertyChanged;

/**
 * 注册 ViewModel 属性（只读）
 * @param PropertyName 属性名称（字符串）
 * @param Getter getter 方法指针
 */
#define FK_VIEWMODEL_PROPERTY(PropertyName, Getter)                        \
    ::fk::binding::PropertyAccessorRegistry::RegisterPropertyGetter<Base>( \
        PropertyName,                                                      \
        [](const Base &obj) -> decltype((std::declval<VM>().*Getter)()) { return (static_cast<const VM &>(obj).*Getter)(); });

/**
 * 注册 ViewModel 属性（可读写）
 * @param PropertyName 属性名称（字符串）
 * @param Getter getter 方法指针
 * @param Setter setter 方法指针
 */
#define FK_VIEWMODEL_PROPERTY_RW(PropertyName, Getter, Setter)                                                                \
    ::fk::binding::PropertyAccessorRegistry::RegisterProperty<Base>(                                                          \
        PropertyName,                                                                                                         \
        [](const Base &obj) -> decltype((std::declval<VM>().*Getter)()) { return (static_cast<const VM &>(obj).*Getter)(); }, \
        [](Base &obj, const std::any& value) {                                                                               \
            using ValueType = decltype((std::declval<VM>().*Getter)());                                                      \
            (static_cast<VM &>(obj).*Setter)(std::any_cast<ValueType>(value));                                               \
        });

/**
 * 结束 ViewModel 属性注册块
 * @param ViewModelType ViewModel 类名（必须与 BEGIN 一致）
 */
#define FK_VIEWMODEL_END(ViewModelType)              \
    }                                                \
    }                                                \
    ;                                                \
    static ViewModelRegistrar_##ViewModelType        \
        gViewModelRegistrarInstance_##ViewModelType; \
    }

// ============================================================================
// 简化版宏（适合快速原型）
// ============================================================================

/**
 * 一次性注册多个可读写属性
 * 自动推断 getter 方法名为 Get##PropertyName，setter 方法名为 Set##PropertyName
 *
 * 示例：
 * FK_VIEWMODEL_AUTO(MyViewModel, Name, Age, Email)
 * 将注册：
 * - "Name" -> GetName() / SetName()
 * - "Age" -> GetAge() / SetAge()
 * - "Email" -> GetEmail() / SetEmail()
 */
#define FK_VIEWMODEL_AUTO_HELPER_1(VM, P1) \
    FK_VIEWMODEL_PROPERTY_RW(#P1, &VM::Get##P1, &VM::Set##P1)

#define FK_VIEWMODEL_AUTO_HELPER_2(VM, P1, P2) \
    FK_VIEWMODEL_AUTO_HELPER_1(VM, P1)         \
    FK_VIEWMODEL_PROPERTY_RW(#P2, &VM::Get##P2, &VM::Set##P2)

#define FK_VIEWMODEL_AUTO_HELPER_3(VM, P1, P2, P3) \
    FK_VIEWMODEL_AUTO_HELPER_2(VM, P1, P2)         \
    FK_VIEWMODEL_PROPERTY_RW(#P3, &VM::Get##P3, &VM::Set##P3)

#define FK_VIEWMODEL_AUTO_HELPER_4(VM, P1, P2, P3, P4) \
    FK_VIEWMODEL_AUTO_HELPER_3(VM, P1, P2, P3)         \
    FK_VIEWMODEL_PROPERTY_RW(#P4, &VM::Get##P4, &VM::Set##P4)

#define FK_VIEWMODEL_AUTO_HELPER_5(VM, P1, P2, P3, P4, P5) \
    FK_VIEWMODEL_AUTO_HELPER_4(VM, P1, P2, P3, P4)         \
    FK_VIEWMODEL_PROPERTY_RW(#P5, &VM::Get##P5, &VM::Set##P5)

#define FK_VIEWMODEL_AUTO_HELPER_6(VM, P1, P2, P3, P4, P5, P6) \
    FK_VIEWMODEL_AUTO_HELPER_5(VM, P1, P2, P3, P4, P5)         \
    FK_VIEWMODEL_PROPERTY_RW(#P6, &VM::Get##P6, &VM::Set##P6)

#define FK_VIEWMODEL_AUTO_HELPER_7(VM, P1, P2, P3, P4, P5, P6, P7) \
    FK_VIEWMODEL_AUTO_HELPER_6(VM, P1, P2, P3, P4, P5, P6)         \
    FK_VIEWMODEL_PROPERTY_RW(#P7, &VM::Get##P7, &VM::Set##P7)

#define FK_VIEWMODEL_AUTO_HELPER_8(VM, P1, P2, P3, P4, P5, P6, P7, P8) \
    FK_VIEWMODEL_AUTO_HELPER_7(VM, P1, P2, P3, P4, P5, P6, P7)         \
    FK_VIEWMODEL_PROPERTY_RW(#P8, &VM::Get##P8, &VM::Set##P8)

// 宏重载辅助
#define FK_VIEWMODEL_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME

/**
 * 自动注册 ViewModel 属性（假设 getter 为 Get##PropertyName，setter 为 Set##PropertyName）
 */
#define FK_VIEWMODEL_AUTO(ViewModelType, ...)                                      \
    FK_VIEWMODEL_BEGIN(ViewModelType)                                              \
    FK_VIEWMODEL_GET_MACRO(__VA_ARGS__,                                            \
                           FK_VIEWMODEL_AUTO_HELPER_8,                             \
                           FK_VIEWMODEL_AUTO_HELPER_7,                             \
                           FK_VIEWMODEL_AUTO_HELPER_6,                             \
                           FK_VIEWMODEL_AUTO_HELPER_5,                             \
                           FK_VIEWMODEL_AUTO_HELPER_4,                             \
                           FK_VIEWMODEL_AUTO_HELPER_3,                             \
                           FK_VIEWMODEL_AUTO_HELPER_2,                             \
                           FK_VIEWMODEL_AUTO_HELPER_1)(ViewModelType, __VA_ARGS__) \
        FK_VIEWMODEL_END(ViewModelType)

// ============================================================================
// 属性声明宏（自动生成成员变量、getter、setter 和注册代码）
// ============================================================================

/**
 * 在类定义中声明一个只读属性
 * 自动生成：
 * - private 成员变量 propertyName_
 * - public const getter GetPropertyName()
 *
 * 用法：
 * class MyViewModel : public binding::ObservableObject {
 * public:
 *     FK_PROPERTY_READONLY(std::string, Name)
 * };
 */
#define FK_PROPERTY_READONLY(Type, PropertyName) \
private:                                         \
    Type propertyName_##PropertyName{};          \
                                                 \
public:                                          \
    const Type &Get##PropertyName() const        \
    {                                            \
        return propertyName_##PropertyName;      \
    }                                            \
                                                 \
private:

/**
 * 在类定义中声明一个可读写属性
 * 自动生成：
 * - private 成员变量 propertyName_
 * - public const getter GetPropertyName()
 * - public setter SetPropertyName() (自动调用 RaisePropertyChanged)
 *
 * 用法：
 * class MyViewModel : public binding::ObservableObject {
 * public:
 *     FK_PROPERTY(std::string, Name)
 * };
 */
#define FK_PROPERTY(Type, PropertyName)                            \
private:                                                           \
    Type propertyName_##PropertyName{};                            \
                                                                   \
public:                                                            \
    const Type &Get##PropertyName() const                          \
    {                                                              \
        return propertyName_##PropertyName;                        \
    }                                                              \
    void Set##PropertyName(const Type &value)                      \
    {                                                              \
        if (propertyName_##PropertyName != value)                  \
        {                                                          \
            propertyName_##PropertyName = value;                   \
            RaisePropertyChanged(std::string_view(#PropertyName)); \
        }                                                          \
    }                                                              \
                                                                   \
private:

/**
 * 在类定义中声明一个可读写属性（自定义 setter 体）
 * 这允许用户在 setter 中添加自定义逻辑，如通知依赖属性
 *
 * 用法：
 * class MyViewModel : public binding::ObservableObject {
 * public:
 *     FK_PROPERTY_CUSTOM(std::string, FirstName, {
 *         RaisePropertyChanged(std::string_view("FullName"));
 *         RaisePropertyChanged(std::string_view("DisplayText"));
 *     })
 * };
 */
#define FK_PROPERTY_CUSTOM(Type, PropertyName, CustomCode)         \
private:                                                           \
    Type propertyName_##PropertyName{};                            \
                                                                   \
public:                                                            \
    const Type &Get##PropertyName() const                          \
    {                                                              \
        return propertyName_##PropertyName;                        \
    }                                                              \
    void Set##PropertyName(const Type &value)                      \
    {                                                              \
        if (propertyName_##PropertyName != value)                  \
        {                                                          \
            propertyName_##PropertyName = value;                   \
            RaisePropertyChanged(std::string_view(#PropertyName)); \
            CustomCode                                             \
        }                                                          \
    }                                                              \
                                                                   \
private:
