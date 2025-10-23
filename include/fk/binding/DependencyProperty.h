#pragma once

#include <any>
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <typeindex>

namespace fk::binding {

class DependencyObject;

enum class BindingMode {
    OneTime,
    OneWay,
    TwoWay,
    OneWayToSource
};

enum class UpdateSourceTrigger {
    Default,
    PropertyChanged,
    LostFocus,
    Explicit
};

struct BindingOptions {
    BindingMode defaultMode{BindingMode::OneWay};
    UpdateSourceTrigger updateSourceTrigger{UpdateSourceTrigger::PropertyChanged};
    bool inheritsDataContext{false};
};

class DependencyProperty;
class DependencyPropertyRegistry;

using PropertyChangedCallback = std::function<void(DependencyObject&, const DependencyProperty&, const std::any& oldValue, const std::any& newValue)>;
using ValidateValueCallback = std::function<bool(const std::any& value)>;

struct PropertyMetadata {
    std::any defaultValue{};
    PropertyChangedCallback propertyChangedCallback{};
    ValidateValueCallback validateCallback{};
    BindingOptions bindingOptions{};
};

class DependencyProperty {
public:
    DependencyProperty(const DependencyProperty&) = delete;
    DependencyProperty& operator=(const DependencyProperty&) = delete;
    DependencyProperty(DependencyProperty&&) = delete;
    DependencyProperty& operator=(DependencyProperty&&) = delete;

    static const DependencyProperty& Register(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {});

    static const DependencyProperty& RegisterAttached(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata = {});

    [[nodiscard]] const std::string& Name() const noexcept { return name_; }
    [[nodiscard]] std::type_index PropertyType() const noexcept { return propertyType_; }
    [[nodiscard]] std::type_index OwnerType() const noexcept { return ownerType_; }
    [[nodiscard]] const PropertyMetadata& Metadata() const noexcept { return metadata_; }
    [[nodiscard]] std::size_t Id() const noexcept { return id_; }
    [[nodiscard]] bool IsAttached() const noexcept { return isAttached_; }

private:
    DependencyProperty(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata,
        bool attached);

    static const DependencyProperty& RegisterInternal(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata,
        bool attached);

    std::string name_;
    std::type_index propertyType_;
    std::type_index ownerType_;
    PropertyMetadata metadata_;
    std::size_t id_{0};
    bool isAttached_{false};

    friend class DependencyPropertyRegistry;
};

} // namespace fk::binding

namespace fk {
    using binding::DependencyProperty;
    using binding::PropertyMetadata;
    using binding::BindingMode;
    using binding::UpdateSourceTrigger;
    using binding::BindingOptions;
    using binding::PropertyChangedCallback;
    using binding::ValidateValueCallback;
}
