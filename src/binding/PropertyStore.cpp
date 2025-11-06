#include "fk/binding/PropertyStore.h"

#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

namespace fk::binding {

PropertyStore::PropertyStore() = default;

PropertyStore::PropertyStore(ValueChangedCallback callback)
    : valueChangedCallback_(std::move(callback)) {}

const std::any& PropertyStore::GetValue(const DependencyProperty& property) const {
    const auto* entry = FindEntry(property);
    if (!entry || !entry->hasEffective) {
        return property.Metadata().defaultValue;
    }
    return entry->effectiveValue;
}

ValueSource PropertyStore::GetValueSource(const DependencyProperty& property) const {
    const auto* entry = FindEntry(property);
    return entry ? entry->effectiveSource : ValueSource::Default;
}

void PropertyStore::SetValue(const DependencyProperty& property, std::any value, ValueSource source) {
    if (source == ValueSource::Binding) {
        throw std::invalid_argument("Use SetBinding to assign binding values");
    }
    auto& entry = EnsureEntry(property);
    auto& layer = SelectLayer(entry, source);
    layer.hasValue = true;
    layer.value = std::move(value);
    UpdateEffectiveValue(property, entry);
}

void PropertyStore::ClearValue(const DependencyProperty& property, ValueSource source) {
    auto* entry = FindEntry(property);
    if (!entry) {
        return;
    }

    auto& layer = SelectLayer(*entry, source);
    const bool hadValue = layer.hasValue;
    layer.hasValue = false;
    layer.value.reset();

    if (!hadValue) {
        return;
    }

    UpdateEffectiveValue(property, *entry);

    if (!EntryHasAnyValue(*entry) && !entry->bindingExpression) {
        entries_.erase(property.Id());
    }
}

void PropertyStore::SetBinding(const DependencyProperty& property, std::shared_ptr<BindingExpression> binding) {
    auto& entry = EnsureEntry(property);
    entry.bindingExpression = std::move(binding);
    auto& layer = entry.binding;
    layer.hasValue = false;
    layer.value.reset();
    UpdateEffectiveValue(property, entry);
}

std::shared_ptr<BindingExpression> PropertyStore::GetBinding(const DependencyProperty& property) const {
    const auto* entry = FindEntry(property);
    if (!entry) {
        return nullptr;
    }
    return entry->bindingExpression;
}

void PropertyStore::ClearBinding(const DependencyProperty& property) {
    auto* entry = FindEntry(property);
    if (!entry) {
        return;
    }
    entry->bindingExpression.reset();
    entry->binding.hasValue = false;
    entry->binding.value.reset();
    UpdateEffectiveValue(property, *entry);
    if (!EntryHasAnyValue(*entry) && !entry->bindingExpression) {
        entries_.erase(property.Id());
    }
}

void PropertyStore::ApplyBindingValue(const DependencyProperty& property, std::any value) {
    auto& entry = EnsureEntry(property);
    entry.binding.hasValue = true;
    entry.binding.value = std::move(value);
    UpdateEffectiveValue(property, entry);
}

bool PropertyStore::HasValue(const DependencyProperty& property) const {
    const auto* entry = FindEntry(property);
    return entry && (entry->hasEffective || EntryHasAnyValue(*entry));
}

void PropertyStore::ClearAll() {
    entries_.clear();
}

void PropertyStore::SetValueChangedCallback(ValueChangedCallback callback) {
    valueChangedCallback_ = std::move(callback);
}

PropertyStore::PropertyEntry& PropertyStore::EnsureEntry(const DependencyProperty& property) {
    auto [it, inserted] = entries_.try_emplace(property.Id());
    if (inserted) {
        it->second.effectiveSource = ValueSource::Default;
        it->second.hasEffective = false;
    }
    return it->second;
}

PropertyStore::PropertyEntry* PropertyStore::FindEntry(const DependencyProperty& property) {
    auto it = entries_.find(property.Id());
    if (it == entries_.end()) {
        return nullptr;
    }
    return &it->second;
}

const PropertyStore::PropertyEntry* PropertyStore::FindEntry(const DependencyProperty& property) const {
    auto it = entries_.find(property.Id());
    if (it == entries_.end()) {
        return nullptr;
    }
    return &it->second;
}

void PropertyStore::UpdateEffectiveValue(const DependencyProperty& property, PropertyEntry& entry) {
    const auto oldSource = entry.effectiveSource;
    const std::any oldValue = entry.hasEffective ? entry.effectiveValue : property.Metadata().defaultValue;

    const auto newSource = DetermineEffectiveSource(entry);
    const std::any* layerValue = nullptr;
    if (newSource == ValueSource::Default) {
        layerValue = &property.Metadata().defaultValue;
    } else {
        layerValue = TryGetLayerValue(entry, newSource);
    }

    const bool hasNewValue = layerValue && layerValue->has_value();

    if (hasNewValue) {
        entry.effectiveValue = *layerValue;
    } else {
        entry.effectiveValue.reset();
    }

    entry.hasEffective = hasNewValue;
    entry.effectiveSource = newSource;

    const std::any& newValueRef = entry.hasEffective ? entry.effectiveValue : property.Metadata().defaultValue;

    if (!AreEquivalent(oldValue, newValueRef) || oldSource != newSource) {
        if (valueChangedCallback_) {
            valueChangedCallback_(property, oldValue, newValueRef, oldSource, newSource);
        }
    }
}

PropertyStore::Layer& PropertyStore::SelectLayer(PropertyEntry& entry, ValueSource source) {
    switch (source) {
    case ValueSource::Local:
        return entry.local;
    case ValueSource::Binding:
        return entry.binding;
    case ValueSource::Style:
        return entry.style;
    case ValueSource::Inherited:
        return entry.inherited;
    default:
        throw std::invalid_argument("ValueSource::Default cannot be written to");
    }
}

const PropertyStore::Layer& PropertyStore::SelectLayer(const PropertyEntry& entry, ValueSource source) {
    switch (source) {
    case ValueSource::Local:
        return entry.local;
    case ValueSource::Binding:
        return entry.binding;
    case ValueSource::Style:
        return entry.style;
    case ValueSource::Inherited:
        return entry.inherited;
    default:
        throw std::invalid_argument("ValueSource::Default cannot be selected for reading as a layer");
    }
}

bool PropertyStore::AreEquivalent(const std::any& lhs, const std::any& rhs) {
    if (!lhs.has_value() && !rhs.has_value()) {
        return true;
    }
    if (lhs.type() != rhs.type()) {
        return false;
    }
    if (lhs.has_value()) {
        if (lhs.type() == typeid(int)) {
            return std::any_cast<int>(lhs) == std::any_cast<int>(rhs);
        }
        if (lhs.type() == typeid(bool)) {
            return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
        }
        if (lhs.type() == typeid(double)) {
            return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
        }
        if (lhs.type() == typeid(std::string)) {
            return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
        }
        if (lhs.type() == typeid(float)) {
            return std::any_cast<float>(lhs) == std::any_cast<float>(rhs);
        }
    }
    return false;
}

bool PropertyStore::EntryHasAnyValue(const PropertyEntry& entry) {
    return entry.local.hasValue || entry.binding.hasValue || entry.style.hasValue || entry.inherited.hasValue;
}

ValueSource PropertyStore::DetermineEffectiveSource(const PropertyEntry& entry) {
    if (entry.local.hasValue) {
        return ValueSource::Local;
    }
    if (entry.binding.hasValue) {
        return ValueSource::Binding;
    }
    if (entry.style.hasValue) {
        return ValueSource::Style;
    }
    if (entry.inherited.hasValue) {
        return ValueSource::Inherited;
    }
    return ValueSource::Default;
}

const std::any* PropertyStore::TryGetLayerValue(const PropertyEntry& entry, ValueSource source) {
    switch (source) {
    case ValueSource::Local:
        return entry.local.hasValue ? &entry.local.value : nullptr;
    case ValueSource::Binding:
        return entry.binding.hasValue ? &entry.binding.value : nullptr;
    case ValueSource::Style:
        return entry.style.hasValue ? &entry.style.value : nullptr;
    case ValueSource::Inherited:
        return entry.inherited.hasValue ? &entry.inherited.value : nullptr;
    default:
        return nullptr;
    }
}

} // namespace fk::binding
