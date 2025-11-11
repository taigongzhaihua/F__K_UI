#pragma once

#include "fk/binding/Binding.h"
#include "fk/binding/DependencyProperty.h"
#include <any>
#include <memory>
#include <string>
#include <typeindex>
#include <vector>

namespace fk::binding {

class DependencyObject;
class MultiBindingExpression;

/**
 * @brief Converter interface for MultiBinding that combines multiple values
 */
class IMultiValueConverter {
public:
    virtual ~IMultiValueConverter() = default;

    /**
     * @brief Convert multiple source values to a single target value
     * @param values Vector of source values
     * @param targetType Target property type
     * @param parameter Optional converter parameter
     * @return Converted value
     */
    virtual std::any Convert(
        const std::vector<std::any>& values,
        std::type_index targetType,
        const std::any* parameter) const = 0;

    /**
     * @brief Convert target value back to multiple source values (optional)
     * @param value Target value
     * @param sourceTypes Types of source properties
     * @param parameter Optional converter parameter
     * @return Vector of converted source values
     */
    virtual std::vector<std::any> ConvertBack(
        const std::any& value,
        const std::vector<std::type_index>& sourceTypes,
        const std::any* parameter) const {
        // Default implementation: not supported
        return {};
    }
};

/**
 * @brief Binding that combines multiple source properties into one target
 * 
 * Example:
 *   MultiBinding multi;
 *   multi.AddBinding(Binding().Path("FirstName").Source(person));
 *   multi.AddBinding(Binding().Path("LastName").Source(person));
 *   multi.Converter(std::make_shared<FullNameConverter>());
 *   textBox->SetBinding(TextBox::TextProperty(), multi);
 */
class MultiBinding {
public:
    MultiBinding() = default;

    MultiBinding& AddBinding(Binding binding);
    MultiBinding& Converter(std::shared_ptr<IMultiValueConverter> converter);
    MultiBinding& ConverterParameter(std::any parameter);
    MultiBinding& Mode(BindingMode mode);
    MultiBinding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);

    [[nodiscard]] const std::vector<Binding>& GetBindings() const noexcept { return bindings_; }
    [[nodiscard]] const std::shared_ptr<IMultiValueConverter>& GetConverter() const noexcept { return converter_; }
    [[nodiscard]] const std::any& GetConverterParameter() const noexcept { return converterParameter_; }
    [[nodiscard]] bool HasConverterParameter() const noexcept { return converterParameter_.has_value(); }
    [[nodiscard]] BindingMode GetMode() const noexcept { return mode_; }
    [[nodiscard]] UpdateSourceTrigger GetUpdateSourceTrigger() const noexcept { return updateSourceTrigger_; }
    [[nodiscard]] bool HasExplicitMode() const noexcept { return hasExplicitMode_; }
    [[nodiscard]] bool HasExplicitUpdateSourceTrigger() const noexcept { return hasExplicitUpdateSourceTrigger_; }

    std::shared_ptr<MultiBindingExpression> CreateExpression(
        DependencyObject* target,
        const DependencyProperty& targetProperty) const;

private:
    std::vector<Binding> bindings_;
    std::shared_ptr<IMultiValueConverter> converter_;
    std::any converterParameter_;
    BindingMode mode_{BindingMode::OneWay};
    bool hasExplicitMode_{false};
    UpdateSourceTrigger updateSourceTrigger_{UpdateSourceTrigger::Default};
    bool hasExplicitUpdateSourceTrigger_{false};
};

} // namespace fk::binding

namespace fk {
    using binding::IMultiValueConverter;
    using binding::MultiBinding;
}
