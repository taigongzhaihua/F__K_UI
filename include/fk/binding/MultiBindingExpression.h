#pragma once

#include "fk/binding/MultiBinding.h"
#include "fk/binding/BindingExpression.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/core/Event.h"
#include <any>
#include <memory>
#include <vector>

namespace fk::binding {

/**
 * @brief Expression that manages multiple child bindings and combines their values
 */
class MultiBindingExpression : public std::enable_shared_from_this<MultiBindingExpression> {
public:
    MultiBindingExpression(
        MultiBinding definition,
        DependencyObject* target,
        const DependencyProperty& property);

    void Activate();
    void Detach();
    void UpdateTarget();

    [[nodiscard]] bool IsActive() const noexcept { return isActive_; }
    [[nodiscard]] DependencyObject* Target() const noexcept { return target_; }
    [[nodiscard]] const DependencyProperty& Property() const noexcept { return *property_; }
    [[nodiscard]] const MultiBinding& Definition() const noexcept { return definition_; }

private:
    void EnsureTargetAlive() const;
    void InitializeEffectiveSettings();
    void SubscribeToChildren();
    void UnsubscribeFromChildren();
    void OnChildBindingUpdated();
    void ApplyTargetValue(std::any value);
    std::vector<std::any> CollectSourceValues() const;

    MultiBinding definition_;
    DependencyObject* target_{nullptr};
    const DependencyProperty* property_{nullptr};
    bool isActive_{false};
    bool isUpdatingTarget_{false};
    BindingMode effectiveMode_{BindingMode::OneWay};
    UpdateSourceTrigger effectiveUpdateSourceTrigger_{UpdateSourceTrigger::PropertyChanged};

    // Child binding expressions
    std::vector<std::shared_ptr<BindingExpression>> childExpressions_;
    std::vector<INotifyPropertyChanged::PropertyChangedEvent::Connection> childConnections_;
};

} // namespace fk::binding

namespace fk {
    using binding::MultiBindingExpression;
}
