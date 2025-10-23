#pragma once

#include "fk/binding/DependencyProperty.h"

#include <any>
#include <functional>
#include <memory>
#include <unordered_map>

namespace fk::binding {

class BindingExpression;

enum class ValueSource {
    Default,
    Inherited,
    Style,
    Binding,
    Local
};

class PropertyStore {
public:
    using ValueChangedCallback = std::function<void(const DependencyProperty&, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource)>;

    PropertyStore();
    explicit PropertyStore(ValueChangedCallback callback);

    const std::any& GetValue(const DependencyProperty& property) const;
    ValueSource GetValueSource(const DependencyProperty& property) const;

    void SetValue(const DependencyProperty& property, std::any value, ValueSource source);
    void ClearValue(const DependencyProperty& property, ValueSource source);

    void SetBinding(const DependencyProperty& property, std::shared_ptr<BindingExpression> binding);
    std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;
    void ClearBinding(const DependencyProperty& property);
    void ApplyBindingValue(const DependencyProperty& property, std::any value);

    bool HasValue(const DependencyProperty& property) const;
    void ClearAll();

    void SetValueChangedCallback(ValueChangedCallback callback);

private:
    struct Layer {
        bool hasValue{false};
        std::any value;
    };

    struct PropertyEntry {
        Layer local;
        Layer binding;
        Layer style;
        Layer inherited;
        std::shared_ptr<BindingExpression> bindingExpression;
        std::any effectiveValue;
        bool hasEffective{false};
        ValueSource effectiveSource{ValueSource::Default};
    };

    PropertyEntry& EnsureEntry(const DependencyProperty& property);
    PropertyEntry* FindEntry(const DependencyProperty& property);
    const PropertyEntry* FindEntry(const DependencyProperty& property) const;

    void UpdateEffectiveValue(const DependencyProperty& property, PropertyEntry& entry);
    static Layer& SelectLayer(PropertyEntry& entry, ValueSource source);
    static const Layer& SelectLayer(const PropertyEntry& entry, ValueSource source);
    static ValueSource DetermineEffectiveSource(const PropertyEntry& entry);
    static const std::any* TryGetLayerValue(const PropertyEntry& entry, ValueSource source);
    static bool AreEquivalent(const std::any& lhs, const std::any& rhs);
    static bool EntryHasAnyValue(const PropertyEntry& entry);

    ValueChangedCallback valueChangedCallback_{};
    std::unordered_map<std::size_t, PropertyEntry> entries_;
};

} // namespace fk::binding

namespace fk {
    using binding::PropertyStore;
    using binding::ValueSource;
}
