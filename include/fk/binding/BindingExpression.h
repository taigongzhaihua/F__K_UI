#pragma once

#include "fk/binding/Binding.h"
#include "fk/binding/BindingPath.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/core/Event.h"

#include <any>
#include <memory>
#include <string>
#include <vector>

namespace fk::binding {

class BindingExpression : public std::enable_shared_from_this<BindingExpression> {
public:
    BindingExpression(const Binding& definition, DependencyObject* target, const DependencyProperty& property);

    void Activate();
    void Detach();
    void UpdateTarget();
    void UpdateSource();
    void UpdateSourceExplicitly();  // For UpdateSourceTrigger::Explicit mode
    void UpdateTargetAsync();       // For async binding support
    void UpdateSourceAsync();       // For async binding support

    [[nodiscard]] bool IsActive() const noexcept { return isActive_; }
    [[nodiscard]] DependencyObject* Target() const noexcept { return target_; }
    [[nodiscard]] const DependencyProperty& Property() const noexcept { return *property_; }
    [[nodiscard]] const Binding& Definition() const noexcept { return definition_; }
    [[nodiscard]] bool HasValidationErrors() const noexcept { return !validationErrors_.empty(); }
    [[nodiscard]] const std::vector<ValidationResult>& GetValidationErrors() const noexcept { return validationErrors_; }
    [[nodiscard]] UpdateSourceTrigger GetEffectiveUpdateSourceTrigger() const noexcept { return effectiveUpdateSourceTrigger_; }

    void ApplyTargetValue(std::any value);

    using ValidationErrorsChangedEvent = core::Event<const std::vector<ValidationResult>&>;
    ValidationErrorsChangedEvent ValidationErrorsChanged;

private:
    void EnsureTargetAlive() const;
    void InitializeEffectiveSettings();
    void Subscribe();
    void Unsubscribe();
    std::any ResolveSourceRoot() const;
    bool TryResolveSourceValue(std::any& value) const;
    bool TrySetSourceValue(const std::any& value);
    bool ShouldListenToSource() const;
    bool ShouldSubscribeToTargetChanges() const;
    void RefreshSourceSubscription();
    INotifyPropertyChanged* TryGetNotifier(std::any& holder);
    bool ValidateBeforeSet(const std::any& candidate);
    void SetValidationErrors(std::vector<ValidationResult> errors);
    void ClearValidationErrors();
    std::any ResolveRelativeSource() const;
    std::any ResolveElementSource() const;

    Binding definition_;
    BindingPath path_;
    DependencyObject* target_{nullptr};
    const DependencyProperty* property_{nullptr};
    bool isActive_{false};
    // TemplateBinding 特殊支持（因为对象切片，需要在构造时保存）
    bool isTemplateBinding_{false};
    const DependencyProperty* templateBindingSourceProperty_{nullptr};
    bool isUpdatingTarget_{false};
    bool isUpdatingSource_{false};
    BindingMode effectiveMode_{BindingMode::OneWay};
    UpdateSourceTrigger effectiveUpdateSourceTrigger_{UpdateSourceTrigger::PropertyChanged};
    BindingContext::DataContextChangedEvent::Connection dataContextConnection_{};
    DependencyObject::PropertyChangedEvent::Connection targetPropertyConnection_{};
    INotifyPropertyChanged::PropertyChangedEvent::Connection sourcePropertyConnection_{};
    DependencyObject::PropertyChangedEvent::Connection sourceDependencyObjectConnection_{};
    std::any currentSource_;
    std::shared_ptr<INotifyPropertyChanged> sharedNotifierHolder_{};
    INotifyPropertyChanged* rawNotifier_{nullptr};
    std::vector<ValidationResult> validationErrors_{};
    bool hasPendingTargetUpdate_{false};
    bool hasPendingSourceUpdate_{false};
};

} // namespace fk::binding
