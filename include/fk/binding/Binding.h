#pragma once

#include "fk/binding/DependencyProperty.h"
#include "fk/binding/ValidationRule.h"

#include <any>
#include <memory>
#include <string>
#include <typeindex>
#include <optional>
#include <vector>

namespace fk::binding {

class BindingExpression;
class DependencyObject;

enum class RelativeSourceMode {
    Self,
    FindAncestor
};

class RelativeSource {
public:
    RelativeSource() = default;
    explicit RelativeSource(RelativeSourceMode mode)
        : mode_(mode) {}

    static RelativeSource Self() { return RelativeSource(RelativeSourceMode::Self); }
    static RelativeSource FindAncestor(std::type_index type, int level = 1) {
        RelativeSource source(RelativeSourceMode::FindAncestor);
        source.SetAncestorType(type);
        source.SetAncestorLevel(level);
        return source;
    }

    RelativeSourceMode Mode() const noexcept { return mode_; }
    bool HasAncestorType() const noexcept { return hasAncestorType_; }
    std::type_index AncestorType() const noexcept { return ancestorType_; }
    int AncestorLevel() const noexcept { return ancestorLevel_; }

    void SetAncestorType(std::type_index type) {
        ancestorType_ = type;
        hasAncestorType_ = true;
    }

    void SetAncestorLevel(int level) {
        ancestorLevel_ = level < 1 ? 1 : level;
    }

private:
    RelativeSourceMode mode_{RelativeSourceMode::Self};
    std::type_index ancestorType_{typeid(void)};
    int ancestorLevel_{1};
    bool hasAncestorType_{false};
};

class IValueConverter {
public:
    virtual ~IValueConverter() = default;
    virtual std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const = 0;
    virtual std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const = 0;
};

class Binding {
public:
    Binding() = default;
    virtual ~Binding() = default;

    Binding& Path(std::string path);
    Binding& Source(std::any source);
    Binding& Mode(BindingMode mode);
    Binding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
    Binding& Converter(std::shared_ptr<IValueConverter> converter);
    Binding& ConverterParameter(std::any parameter);
    Binding& ValidatesOnDataErrors(bool enable);
    Binding& AddValidationRule(std::shared_ptr<ValidationRule> rule);
    Binding& ElementName(std::string name);
    Binding& SetRelativeSource(RelativeSource relativeSource);
    Binding& IsAsync(bool async);

    [[nodiscard]] const std::string& GetPath() const noexcept { return path_; }
    [[nodiscard]] const std::any& GetSource() const noexcept { return source_; }
    [[nodiscard]] bool HasExplicitSource() const noexcept { return hasExplicitSource_; }
    [[nodiscard]] BindingMode GetMode() const noexcept { return mode_; }
    [[nodiscard]] UpdateSourceTrigger GetUpdateSourceTrigger() const noexcept { return updateSourceTrigger_; }
    [[nodiscard]] const std::shared_ptr<IValueConverter>& GetConverter() const noexcept { return converter_; }
    [[nodiscard]] const std::any& GetConverterParameter() const noexcept { return converterParameter_; }
    [[nodiscard]] bool HasConverterParameter() const noexcept { return converterParameter_.has_value(); }
    [[nodiscard]] bool ShouldValidateOnDataErrors() const noexcept { return validatesOnDataErrors_; }
    [[nodiscard]] const std::vector<std::shared_ptr<ValidationRule>>& GetValidationRules() const noexcept { return validationRules_; }
    [[nodiscard]] bool HasExplicitMode() const noexcept { return hasExplicitMode_; }
    [[nodiscard]] bool HasExplicitUpdateSourceTrigger() const noexcept { return hasExplicitUpdateSourceTrigger_; }
    [[nodiscard]] bool HasElementName() const noexcept { return hasElementName_; }
    [[nodiscard]] const std::string& GetElementName() const noexcept { return elementName_; }
    [[nodiscard]] bool HasRelativeSource() const noexcept { return relativeSource_.has_value(); }
    [[nodiscard]] const RelativeSource& GetRelativeSource() const { return relativeSource_.value(); }
    [[nodiscard]] bool GetIsAsync() const noexcept { return isAsync_; }
    
    /**
     * @brief 检查是否为 TemplateBinding
     * @return 如果是 TemplateBinding 返回 true，否则返回 false
     */
    [[nodiscard]] virtual bool IsTemplateBinding() const noexcept { return false; }
    
    /**
     * @brief 获取 TemplateBinding 的源属性（仅用于 TemplateBinding）
     * @return DependencyProperty 指针，如果不是 TemplateBinding 返回 nullptr
     */
    [[nodiscard]] virtual const DependencyProperty* GetTemplateBindingSourceProperty() const noexcept { return nullptr; }
    
    /**
     * @brief 克隆绑定对象（保留派生类类型）
     * @return 克隆的绑定对象指针
     */
    [[nodiscard]] virtual std::unique_ptr<Binding> Clone() const { return std::make_unique<Binding>(*this); }

    std::shared_ptr<BindingExpression> CreateExpression(DependencyObject* target, const DependencyProperty& targetProperty) const;

private:
    std::string path_{};
    std::any source_{};
    bool hasExplicitSource_{false};
    BindingMode mode_{BindingMode::OneWay};
    bool hasExplicitMode_{false};
    UpdateSourceTrigger updateSourceTrigger_{UpdateSourceTrigger::Default};
    bool hasExplicitUpdateSourceTrigger_{false};
    std::shared_ptr<IValueConverter> converter_{};
    std::any converterParameter_{};
    bool validatesOnDataErrors_{false};
    std::vector<std::shared_ptr<ValidationRule>> validationRules_{};
    bool hasElementName_{false};
    std::string elementName_{};
    std::optional<RelativeSource> relativeSource_{};
    bool isAsync_{false};
};

} // namespace fk::binding

namespace fk {
    using binding::Binding;
    using binding::RelativeSource;
    using binding::RelativeSourceMode;
    using binding::IValueConverter;
    using binding::ValidationResult;
    using binding::ValidationRule;

    // Helper function for convenient binding creation
    inline Binding bind(std::string path) {
        Binding b;
        b.Path(std::move(path));
        return b;
    }
}
