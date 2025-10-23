#include "fk/binding/DependencyProperty.h"

#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fk::binding {

class DependencyPropertyRegistry {
public:
    static DependencyPropertyRegistry& Instance() {
        static DependencyPropertyRegistry registry;
        return registry;
    }

    const DependencyProperty& Register(std::string name,
        std::type_index propertyType,
        std::type_index ownerType,
        PropertyMetadata metadata,
        bool attached) {
        std::lock_guard lock(mutex_);

        auto& ownerMap = propertiesByOwner_[ownerType];
        if (ownerMap.find(name) != ownerMap.end()) {
            throw std::logic_error("DependencyProperty already registered for owner: " + name);
        }

        if (metadata.validateCallback && metadata.defaultValue.has_value()) {
            if (!metadata.validateCallback(metadata.defaultValue)) {
                throw std::invalid_argument("DependencyProperty default value failed validation");
            }
        }

        auto property = std::unique_ptr<DependencyProperty>(new DependencyProperty(
            std::move(name), propertyType, ownerType, std::move(metadata), attached));
        auto* propertyPtr = property.get();
        properties_.push_back(std::move(property));
        ownerMap.emplace(propertyPtr->Name(), propertyPtr);
        return *propertyPtr;
    }

private:
    std::mutex mutex_;
    std::vector<std::unique_ptr<DependencyProperty>> properties_;
    std::unordered_map<std::type_index, std::unordered_map<std::string, const DependencyProperty*>> propertiesByOwner_;
};

namespace {

std::atomic<std::size_t> g_dependencyPropertyIdCounter{1};

} // namespace

DependencyProperty::DependencyProperty(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata,
    bool attached)
    : name_(std::move(name))
    , propertyType_(propertyType)
    , ownerType_(ownerType)
    , metadata_(std::move(metadata))
    , id_(g_dependencyPropertyIdCounter.fetch_add(1, std::memory_order_relaxed))
    , isAttached_(attached) {}

const DependencyProperty& DependencyProperty::Register(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata) {
    return RegisterInternal(std::move(name), propertyType, ownerType, std::move(metadata), false);
}

const DependencyProperty& DependencyProperty::RegisterAttached(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata) {
    return RegisterInternal(std::move(name), propertyType, ownerType, std::move(metadata), true);
}

const DependencyProperty& DependencyProperty::RegisterInternal(std::string name,
    std::type_index propertyType,
    std::type_index ownerType,
    PropertyMetadata metadata,
    bool attached) {
    if (name.empty()) {
        throw std::invalid_argument("DependencyProperty name cannot be empty");
    }

    return DependencyPropertyRegistry::Instance().Register(std::move(name), propertyType, ownerType, std::move(metadata), attached);
}

} // namespace fk::binding
