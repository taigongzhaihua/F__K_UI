#pragma once

#include "fk/binding/BindingPath.h"

#define FK_BINDING_UNIQUE_NAME(base) FK_BINDING_UNIQUE_NAME_IMPL(base, __COUNTER__)
#define FK_BINDING_UNIQUE_NAME_IMPL(base, count) base##count

#define FK_BINDING_REGISTER_GETTER(OwnerType, PropertyName, Getter)                                         \
    namespace {                                                                                            \
    const int FK_BINDING_UNIQUE_NAME(kFkBindingAccessorGetter) = []() {                                     \
        ::fk::binding::PropertyAccessorRegistry::RegisterPropertyGetter<OwnerType>(PropertyName, Getter);  \
        return 0;                                                                                          \
    }();                                                                                                    \
    }

#define FK_BINDING_REGISTER_SETTER(OwnerType, PropertyName, Setter)                                         \
    namespace {                                                                                            \
    const int FK_BINDING_UNIQUE_NAME(kFkBindingAccessorSetter) = []() {                                     \
        ::fk::binding::PropertyAccessorRegistry::RegisterPropertySetter<OwnerType>(PropertyName, Setter);  \
        return 0;                                                                                          \
    }();                                                                                                    \
    }

#define FK_BINDING_REGISTER_PROPERTY(OwnerType, PropertyName, Getter, Setter)                               \
    namespace {                                                                                            \
    const int FK_BINDING_UNIQUE_NAME(kFkBindingAccessorProperty) = []() {                                   \
        ::fk::binding::PropertyAccessorRegistry::RegisterProperty<OwnerType>(PropertyName, Getter, Setter); \
        return 0;                                                                                          \
    }();                                                                                                    \
    }

#undef FK_BINDING_UNIQUE_NAME
#undef FK_BINDING_UNIQUE_NAME_IMPL
