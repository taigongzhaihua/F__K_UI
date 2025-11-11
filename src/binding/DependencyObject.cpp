#include "fk/binding/DependencyObject.h"

#include "fk/binding/BindingExpression.h"
#include "fk/binding/MultiBinding.h"
#include "fk/binding/MultiBindingExpression.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <typeindex>
#include <utility>
#include <iostream>

namespace fk::binding {

DependencyObject::DependencyObject()
    : bindingContext_(*this)
    , propertyStore_([this](const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource) {
          HandleStoreValueChanged(property, oldValue, newValue, oldSource, newSource);
      }) {
    dataContextChangedConnection_ = bindingContext_.DataContextChanged.Connect(
        [this](const std::any& oldValue, const std::any& newValue) {
            OnDataContextChanged(oldValue, newValue);
        });
}

DependencyObject::~DependencyObject() = default;

const std::any& DependencyObject::GetValue(const DependencyProperty& property) const {
    return propertyStore_.GetValue(property);
}

void DependencyObject::SetValue(const DependencyProperty& property, std::any value) {
    ValidateValue(property, value);
    propertyStore_.SetValue(property, std::move(value), ValueSource::Local);
}

void DependencyObject::SetValue(const DependencyProperty& property, Binding binding) {
    SetBinding(property, std::move(binding));
}

void DependencyObject::ClearValue(const DependencyProperty& property) {
    propertyStore_.ClearValue(property, ValueSource::Local);
}

ValueSource DependencyObject::GetValueSource(const DependencyProperty& property) const {
    return propertyStore_.GetValueSource(property);
}

void DependencyObject::SetBinding(const DependencyProperty& property, Binding binding) {
    auto expression = binding.CreateExpression(this, property);
    auto current = propertyStore_.GetBinding(property);

    if (current == expression) {
        return;
    }

    if (current) {
        current->Detach();
    }

    propertyStore_.SetBinding(property, expression);

    if (expression) {
        expression->Activate();
    }

    OnBindingChanged(property, current, expression);
}

void DependencyObject::SetBinding(const DependencyProperty& property, MultiBinding binding) {
    auto current = propertyStore_.GetBinding(property);
    if (current) {
        current->Detach();
    }

    // Detach any existing MultiBinding
    if (activeMultiBinding_) {
        activeMultiBinding_->Detach();
        activeMultiBinding_.reset();
    }

    propertyStore_.ClearBinding(property);

    // Create and activate new MultiBindingExpression
    activeMultiBinding_ = binding.CreateExpression(this, property);
    
    if (activeMultiBinding_) {
        activeMultiBinding_->Activate();
    }

    OnBindingChanged(property, current, nullptr);
}

void DependencyObject::ClearBinding(const DependencyProperty& property) {
    auto current = propertyStore_.GetBinding(property);
    if (!current) {
        propertyStore_.ClearBinding(property);
        return;
    }

    propertyStore_.ClearBinding(property);
    current->Detach();

    OnBindingChanged(property, current, nullptr);
}

std::shared_ptr<BindingExpression> DependencyObject::GetBinding(const DependencyProperty& property) const {
    return propertyStore_.GetBinding(property);
}

void DependencyObject::UpdateSource(const DependencyProperty& property) {
    auto binding = GetBinding(property);
    if (binding) {
        binding->UpdateSourceExplicitly();
    }
}

void DependencyObject::SetDataContext(std::any value) {
    bindingContext_.SetLocalDataContext(std::move(value));
}

void DependencyObject::ClearDataContext() {
    bindingContext_.ClearLocalDataContext();
}

const std::any& DependencyObject::GetDataContext() const noexcept {
    return bindingContext_.GetDataContext();
}

bool DependencyObject::HasDataContext() const noexcept {
    return bindingContext_.HasDataContext();
}

void DependencyObject::SetDataContextParent(DependencyObject* parent) {
    bindingContext_.SetParent(parent ? &parent->bindingContext_ : nullptr);
}

void DependencyObject::SetElementName(std::string name) {
    elementName_ = std::move(name);
}

void DependencyObject::SetLogicalParent(DependencyObject* parent) {
    if (logicalParent_ == parent) {
        SetDataContextParent(parent);
        return;
    }

    if (logicalParent_) {
        auto& siblings = logicalParent_->logicalChildren_;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    logicalParent_ = parent;

    if (logicalParent_) {
        auto& siblings = logicalParent_->logicalChildren_;
        if (std::find(siblings.begin(), siblings.end(), this) == siblings.end()) {
            siblings.push_back(this);
        }
    }

    SetDataContextParent(parent);
}

void DependencyObject::AddLogicalChild(DependencyObject* child) {
    if (!child || child == this) {
        return;
    }
    child->SetLogicalParent(this);
}

void DependencyObject::RemoveLogicalChild(DependencyObject* child) {
    if (!child) {
        return;
    }
    if (child->logicalParent_ == this) {
        child->SetLogicalParent(nullptr);
    }
}

DependencyObject* DependencyObject::FindElementByName(std::string_view name) {
    if (!name.empty() && elementName_ == name) {
        return this;
    }
    for (auto* child : logicalChildren_) {
        if (!child) {
            continue;
        }
        if (auto* found = child->FindElementByName(name)) {
            return found;
        }
    }
    return nullptr;
}

const DependencyObject* DependencyObject::FindElementByName(std::string_view name) const {
    if (!name.empty() && elementName_ == name) {
        return this;
    }
    for (const auto* child : logicalChildren_) {
        if (!child) {
            continue;
        }
        const auto* constChild = static_cast<const DependencyObject*>(child);
        if (const auto* found = constChild->FindElementByName(name)) {
            return found;
        }
    }
    return nullptr;
}

void DependencyObject::OnPropertyChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource) {
    PropertyChanged(property, oldValue, newValue, oldSource, newSource);
}

void DependencyObject::OnBindingChanged(const DependencyProperty& property, const std::shared_ptr<BindingExpression>& oldBinding, const std::shared_ptr<BindingExpression>& newBinding) {
    BindingChanged(property, oldBinding, newBinding);
}

void DependencyObject::OnDataContextChanged(const std::any& oldValue, const std::any& newValue) {
    DataContextChanged(oldValue, newValue);
}

void DependencyObject::ApplyBindingValue(const DependencyProperty& property, std::any value) {
    ValidateValue(property, value);
    propertyStore_.ApplyBindingValue(property, std::move(value));
}

void DependencyObject::HandleStoreValueChanged(const DependencyProperty& property, const std::any& oldValue, const std::any& newValue, ValueSource oldSource, ValueSource newSource) {
    if (const auto& metadataCallback = property.Metadata().propertyChangedCallback) {
        metadataCallback(*this, property, oldValue, newValue);
    }
    OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);
}

void DependencyObject::ValidateValue(const DependencyProperty& property, const std::any& value) {
    if (!value.has_value()) {
        return;
    }

    const auto expectedType = property.PropertyType();
    const auto actualType = std::type_index(value.type());

    if (expectedType != actualType && expectedType != std::type_index(typeid(std::any))) {
        std::ostringstream oss;
        oss << "Value type mismatch for property '" << property.Name() << "'";
        throw std::invalid_argument(oss.str());
    }

    if (const auto& validate = property.Metadata().validateCallback) {
        if (!validate(value)) {
            std::ostringstream oss;
            oss << "Validation failed for property '" << property.Name() << "'";
            throw std::invalid_argument(oss.str());
        }
    }
}

} // namespace fk::binding
