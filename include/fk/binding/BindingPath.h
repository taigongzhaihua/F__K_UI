#pragma once

#include <any>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fk::binding {

class PropertyAccessorRegistry {
public:
    struct Accessor {
        using Getter = std::function<bool(const std::any&, std::any&)>;
        using Setter = std::function<bool(std::any&, const std::any&)>;

        Getter getter{};
        Setter setter{};
    };

    static void RegisterAccessor(std::type_index ownerType, std::string name, const Accessor& accessor);
    static const Accessor* FindAccessor(std::type_index ownerType, std::string_view name);

    template<typename Owner, typename Getter>
    static void RegisterPropertyGetter(std::string name, Getter getter);

    template<typename Owner, typename Setter>
    static void RegisterPropertySetter(std::string name, Setter setter);

    template<typename Owner, typename Getter, typename Setter>
    static void RegisterProperty(std::string name, Getter getter, Setter setter);

    template<typename Owner, typename Getter>
    static void RegisterProperty(std::string name, Getter getter);

private:
    template<typename Owner>
    static void RegisterVariants(const std::string& name, const Accessor& accessor);

    template<typename Owner, typename Func>
    static bool VisitConstInstance(const std::any& instance, Func&& func);

    template<typename Owner, typename Func>
    static bool VisitInstance(std::any& instance, Func&& func);

    template<typename Owner>
    static constexpr std::type_index TypeIndex() {
        return std::type_index(typeid(Owner));
    }
};

class BindingPath {
public:
    struct Segment {
        enum class Kind { Property, Index };

        Kind kind{Kind::Property};
        std::string name{};
        std::size_t index{0};

        static Segment Property(std::string segmentName) {
            Segment segment;
            segment.kind = Kind::Property;
            segment.name = std::move(segmentName);
            return segment;
        }

        static Segment Index(std::size_t value) {
            Segment segment;
            segment.kind = Kind::Index;
            segment.index = value;
            return segment;
        }
    };

    BindingPath() = default;
    explicit BindingPath(std::string path);

    [[nodiscard]] bool IsEmpty() const noexcept { return segments_.empty(); }
    [[nodiscard]] const std::vector<Segment>& Segments() const noexcept { return segments_; }
    [[nodiscard]] const std::string& Raw() const noexcept { return raw_; }

    bool Resolve(const std::any& source, std::any& result) const;
    bool SetValue(std::any& source, const std::any& value) const;

private:
    static std::vector<Segment> Parse(const std::string& path);
    static Segment MakePropertySegment(const std::string& token);
    static Segment MakeIndexSegment(std::string_view token);

    std::string raw_;
    std::vector<Segment> segments_;
};

// Template implementations

namespace detail {

template<typename Setter>
struct MemberFunctionTraits;

template<typename Owner, typename Return, typename... Args>
struct MemberFunctionTraits<Return (Owner::*)(Args...)> {
    using OwnerType = Owner;
    using ReturnType = Return;
    using ArgumentTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
};

template<typename Owner, typename Return, typename... Args>
struct MemberFunctionTraits<Return (Owner::*)(Args...) const> {
    using OwnerType = Owner;
    using ReturnType = Return;
    using ArgumentTuple = std::tuple<Args...>;
    static constexpr std::size_t Arity = sizeof...(Args);
};

template<typename Owner, typename Return, typename... Args>
struct MemberFunctionTraits<Return (Owner::*)(Args...) noexcept> : MemberFunctionTraits<Return (Owner::*)(Args...)> {};

template<typename Owner, typename Return, typename... Args>
struct MemberFunctionTraits<Return (Owner::*)(Args...) const noexcept> : MemberFunctionTraits<Return (Owner::*)(Args...) const> {};

} // namespace detail

template<typename Owner, typename Getter>
void PropertyAccessorRegistry::RegisterPropertyGetter(std::string name, Getter getter) {
    static_assert(!std::is_void_v<std::invoke_result_t<Getter, const Owner&>>, "Getter must return a value");

    Accessor accessor;
    accessor.getter = [getter = std::move(getter)](const std::any& instance, std::any& outValue) -> bool {
        bool resolved = VisitConstInstance<Owner>(instance, [&](const Owner& owner) {
            outValue = std::any(std::invoke(getter, owner));
        });
        return resolved;
    };

    RegisterVariants<Owner>(name, accessor);
}

template<typename Owner, typename Setter>
void PropertyAccessorRegistry::RegisterPropertySetter(std::string name, Setter setter) {
    static_assert(std::is_member_function_pointer_v<Setter>, "Setter must be a pointer to member function");
    using Traits = detail::MemberFunctionTraits<Setter>;
    static_assert(Traits::Arity == 1, "Setter must accept exactly one argument");

    using ArgumentRaw = std::tuple_element_t<0, typename Traits::ArgumentTuple>;
    using ArgumentType = std::remove_cv_t<std::remove_reference_t<ArgumentRaw>>;

    Accessor accessor;
    accessor.setter = [setter](std::any& instance, const std::any& value) -> bool {
        bool invoked = false;
        auto visitor = [&](Owner& owner) {
            ArgumentType argument = std::any_cast<ArgumentType>(value);
            std::invoke(setter, owner, argument);
            invoked = true;
        };
        return VisitInstance<Owner>(instance, visitor) && invoked;
    };

    RegisterVariants<Owner>(name, accessor);
}

template<typename Owner, typename Getter, typename Setter>
void PropertyAccessorRegistry::RegisterProperty(std::string name, Getter getter, Setter setter) {
    RegisterPropertyGetter<Owner>(name, std::move(getter));
    RegisterPropertySetter<Owner>(std::move(name), std::move(setter));
}

template<typename Owner, typename Getter>
void PropertyAccessorRegistry::RegisterProperty(std::string name, Getter getter) {
    RegisterPropertyGetter<Owner>(std::move(name), std::move(getter));
}

template<typename Owner>
void PropertyAccessorRegistry::RegisterVariants(const std::string& name, const Accessor& accessor) {
    RegisterAccessor(TypeIndex<Owner>(), name, accessor);
    RegisterAccessor(TypeIndex<const Owner>(), name, accessor);
    RegisterAccessor(TypeIndex<Owner*>(), name, accessor);
    RegisterAccessor(TypeIndex<const Owner*>(), name, accessor);
    RegisterAccessor(TypeIndex<std::shared_ptr<Owner>>(), name, accessor);
    RegisterAccessor(TypeIndex<std::shared_ptr<const Owner>>(), name, accessor);
    RegisterAccessor(TypeIndex<std::weak_ptr<Owner>>(), name, accessor);
    RegisterAccessor(TypeIndex<std::weak_ptr<const Owner>>(), name, accessor);
    RegisterAccessor(TypeIndex<std::reference_wrapper<Owner>>(), name, accessor);
    RegisterAccessor(TypeIndex<std::reference_wrapper<const Owner>>(), name, accessor);
}

template<typename Owner, typename Func>
bool PropertyAccessorRegistry::VisitConstInstance(const std::any& instance, Func&& func) {
    if (const auto* value = std::any_cast<Owner>(&instance)) {
        func(*value);
        return true;
    }
    if (const auto* value = std::any_cast<const Owner>(&instance)) {
        func(*value);
        return true;
    }
    if (const auto* ref = std::any_cast<std::reference_wrapper<Owner>>(&instance)) {
        func(ref->get());
        return true;
    }
    if (const auto* ref = std::any_cast<std::reference_wrapper<const Owner>>(&instance)) {
        func(ref->get());
        return true;
    }
    if (const auto* pointerPtr = std::any_cast<Owner*>(&instance)) {
        if (*pointerPtr) {
            func(**pointerPtr);
            return true;
        }
    }
    if (const auto* pointerPtr = std::any_cast<const Owner*>(&instance)) {
        if (*pointerPtr) {
            func(**pointerPtr);
            return true;
        }
    }
    if (const auto* sharedPtr = std::any_cast<std::shared_ptr<Owner>>(&instance)) {
        if (sharedPtr->get()) {
            func(**sharedPtr);
            return true;
        }
    }
    if (const auto* sharedPtr = std::any_cast<std::shared_ptr<const Owner>>(&instance)) {
        if (sharedPtr->get()) {
            func(**sharedPtr);
            return true;
        }
    }
    if (const auto* weakPtr = std::any_cast<std::weak_ptr<Owner>>(&instance)) {
        if (auto locked = weakPtr->lock()) {
            func(*locked);
            return true;
        }
    }
    if (const auto* weakPtr = std::any_cast<std::weak_ptr<const Owner>>(&instance)) {
        if (auto locked = weakPtr->lock()) {
            func(*locked);
            return true;
        }
    }
    return false;
}

template<typename Owner, typename Func>
bool PropertyAccessorRegistry::VisitInstance(std::any& instance, Func&& func) {
    if (auto* value = std::any_cast<Owner>(&instance)) {
        func(*value);
        return true;
    }
    if (auto* ref = std::any_cast<std::reference_wrapper<Owner>>(&instance)) {
        func(ref->get());
        return true;
    }
    if (auto* pointerPtr = std::any_cast<Owner*>(&instance)) {
        if (*pointerPtr) {
            func(**pointerPtr);
            return true;
        }
    }
    if (auto* sharedPtr = std::any_cast<std::shared_ptr<Owner>>(&instance)) {
        if (sharedPtr->get()) {
            func(**sharedPtr);
            return true;
        }
    }
    if (auto* weakPtr = std::any_cast<std::weak_ptr<Owner>>(&instance)) {
        if (auto locked = weakPtr->lock()) {
            func(*locked);
            return true;
        }
    }
    return false;
}

} // namespace fk::binding

namespace fk {
    using binding::BindingPath;
    using binding::PropertyAccessorRegistry;
}
