#include "fk/binding/Binding.h"

#include "fk/binding/BindingExpression.h"

#include <stdexcept>
#include <utility>

namespace fk::binding {

Binding& Binding::Path(std::string path) {
    path_ = std::move(path);
    return *this;
}

Binding& Binding::Source(std::any source) {
    hasExplicitSource_ = source.has_value();
    source_ = std::move(source);
    return *this;
}

Binding& Binding::Mode(BindingMode mode) {
    hasExplicitMode_ = true;
    mode_ = mode;
    return *this;
}

Binding& Binding::SetUpdateSourceTrigger(UpdateSourceTrigger trigger) {
    hasExplicitUpdateSourceTrigger_ = true;
    updateSourceTrigger_ = trigger;
    return *this;
}

Binding& Binding::Converter(std::shared_ptr<IValueConverter> converter) {
    converter_ = std::move(converter);
    return *this;
}

Binding& Binding::ConverterParameter(std::any parameter) {
    converterParameter_ = std::move(parameter);
    return *this;
}

Binding& Binding::ValidatesOnDataErrors(bool enable) {
    validatesOnDataErrors_ = enable;
    return *this;
}

Binding& Binding::AddValidationRule(std::shared_ptr<ValidationRule> rule) {
    if (rule) {
        validationRules_.push_back(std::move(rule));
    }
    return *this;
}

Binding& Binding::ElementName(std::string name) {
    hasElementName_ = true;
    elementName_ = std::move(name);
    return *this;
}

Binding& Binding::SetRelativeSource(RelativeSource relativeSource) {
    relativeSource_ = std::move(relativeSource);
    return *this;
}

std::shared_ptr<BindingExpression> Binding::CreateExpression(DependencyObject* target, const DependencyProperty& targetProperty) const {
    if (target == nullptr) {
        throw std::invalid_argument("Binding requires a non-null target");
    }
    return std::make_shared<BindingExpression>(*this, target, targetProperty);
}

} // namespace fk::binding
