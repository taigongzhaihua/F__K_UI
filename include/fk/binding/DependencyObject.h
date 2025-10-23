#pragma once

#include "fk/binding/Binding.h"
#include "fk/binding/BindingContext.h"
#include "fk/binding/PropertyStore.h"
#include "fk/core/Event.h"

#include <any>
#include <memory>
#include <type_traits>
#include <utility>
#include <string>
#include <string_view>
#include <vector>

namespace fk::binding {

class DependencyObject {
public:
    using PropertyChangedEvent = core::Event<const DependencyProperty&, const std::any&, const std::any&, ValueSource, ValueSource>;
    using BindingChangedEvent = core::Event<const DependencyProperty&, const std::shared_ptr<BindingExpression>&, const std::shared_ptr<BindingExpression>&>;
    using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;

    virtual ~DependencyObject();

    const std::any& GetValue(const DependencyProperty& property) const;

    template<typename T>
    T GetValue(const DependencyProperty& property) const {
        return std::any_cast<T>(GetValue(property));
    }

    void SetValue(const DependencyProperty& property, std::any value);

    template<typename T>
    std::enable_if_t<!std::is_same_v<std::decay_t<T>, Binding>, void>
    SetValue(const DependencyProperty& property, T&& value) {
        SetValue(property, std::any(std::forward<T>(value)));
    }

    void SetValue(const DependencyProperty& property, Binding binding);

    void ClearValue(const DependencyProperty& property);
    ValueSource GetValueSource(const DependencyProperty& property) const;

    void SetBinding(const DependencyProperty& property, Binding binding);
    void ClearBinding(const DependencyProperty& property);
    std::shared_ptr<BindingExpression> GetBinding(const DependencyProperty& property) const;

    void SetDataContext(std::any value);

    template<typename T>
    void SetDataContext(T&& value) {
        SetDataContext(std::any(std::forward<T>(value)));
    }

    void ClearDataContext();
    const std::any& GetDataContext() const noexcept;
    bool HasDataContext() const noexcept;

    void SetDataContextParent(DependencyObject* parent);

    void SetElementName(std::string name);
    const std::string& GetElementName() const noexcept { return elementName_; }

    void SetLogicalParent(DependencyObject* parent);
    DependencyObject* GetLogicalParent() const noexcept { return logicalParent_; }
    void AddLogicalChild(DependencyObject* child);
    void RemoveLogicalChild(DependencyObject* child);
    const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept { return logicalChildren_; }

    DependencyObject* FindElementByName(std::string_view name);
    const DependencyObject* FindElementByName(std::string_view name) const;

    BindingContext& GetBindingContext() noexcept { return bindingContext_; }
    const BindingContext& GetBindingContext() const noexcept { return bindingContext_; }

    PropertyChangedEvent PropertyChanged;
    BindingChangedEvent BindingChanged;
    DataContextChangedEvent DataContextChanged;

protected:
    DependencyObject();

    virtual void OnPropertyChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource);
    virtual void OnBindingChanged(const DependencyProperty& property, const std::shared_ptr<BindingExpression>& oldBinding, const std::shared_ptr<BindingExpression>& newBinding);
    virtual void OnDataContextChanged(const std::any& oldValue, const std::any& newValue);

    PropertyStore& MutablePropertyStore() noexcept { return propertyStore_; }
    const PropertyStore& GetPropertyStore() const noexcept { return propertyStore_; }

private:
    void ApplyBindingValue(const DependencyProperty& property, std::any value);
    void HandleStoreValueChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource);

    static void ValidateValue(const DependencyProperty& property, const std::any& value);

    BindingContext bindingContext_;
    DataContextChangedEvent::Connection dataContextChangedConnection_{};
    PropertyStore propertyStore_;
    DependencyObject* logicalParent_{nullptr};
    std::vector<DependencyObject*> logicalChildren_{};
    std::string elementName_{};

    friend class BindingExpression;
};

} // namespace fk::binding

namespace fk {
    using binding::DependencyObject;
}
