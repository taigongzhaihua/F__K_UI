#include "fk/binding/MultiBinding.h"
#include "fk/binding/MultiBindingExpression.h"
#include <utility>

namespace fk::binding {

MultiBinding& MultiBinding::AddBinding(Binding binding) {
    bindings_.push_back(std::move(binding));
    return *this;
}

MultiBinding& MultiBinding::Converter(std::shared_ptr<IMultiValueConverter> converter) {
    converter_ = std::move(converter);
    return *this;
}

MultiBinding& MultiBinding::ConverterParameter(std::any parameter) {
    converterParameter_ = std::move(parameter);
    return *this;
}

MultiBinding& MultiBinding::Mode(BindingMode mode) {
    hasExplicitMode_ = true;
    mode_ = mode;
    return *this;
}

MultiBinding& MultiBinding::SetUpdateSourceTrigger(UpdateSourceTrigger trigger) {
    hasExplicitUpdateSourceTrigger_ = true;
    updateSourceTrigger_ = trigger;
    return *this;
}

std::shared_ptr<MultiBindingExpression> MultiBinding::CreateExpression(
    DependencyObject* target,
    const DependencyProperty& targetProperty) const {
    return std::make_shared<MultiBindingExpression>(*this, target, targetProperty);
}

} // namespace fk::binding
