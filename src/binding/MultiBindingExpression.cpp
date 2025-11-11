#include "fk/binding/MultiBindingExpression.h"
#include "fk/binding/ValueConverters.h"
#include <stdexcept>
#include <utility>

namespace fk::binding {

namespace {

class ScopedFlag {
public:
    explicit ScopedFlag(bool& flag) : flag_(flag) {
        flag_ = true;
    }
    ~ScopedFlag() {
        flag_ = false;
    }
    ScopedFlag(const ScopedFlag&) = delete;
    ScopedFlag& operator=(const ScopedFlag&) = delete;

private:
    bool& flag_;
};

} // namespace

MultiBindingExpression::MultiBindingExpression(
    MultiBinding definition,
    DependencyObject* target,
    const DependencyProperty& property)
    : definition_(std::move(definition))
    , target_(target)
    , property_(&property) {
    if (target_ == nullptr) {
        throw std::invalid_argument("MultiBindingExpression requires a non-null target");
    }
    InitializeEffectiveSettings();
}

void MultiBindingExpression::Activate() {
    if (isActive_) {
        return;
    }

    EnsureTargetAlive();
    isActive_ = true;

    // Create child binding expressions
    childExpressions_.clear();
    childExpressions_.reserve(definition_.GetBindings().size());

    for (const auto& binding : definition_.GetBindings()) {
        auto childExpr = binding.CreateExpression(target_, *property_);
        if (childExpr) {
            childExpressions_.push_back(childExpr);
        }
    }

    SubscribeToChildren();
    UpdateTarget();
}

void MultiBindingExpression::Detach() {
    if (!isActive_) {
        return;
    }

    UnsubscribeFromChildren();

    // Detach all child expressions
    for (auto& child : childExpressions_) {
        if (child) {
            child->Detach();
        }
    }
    childExpressions_.clear();

    isActive_ = false;
}

void MultiBindingExpression::UpdateTarget() {
    if (!isActive_ || effectiveMode_ == BindingMode::OneWayToSource) {
        return;
    }

    EnsureTargetAlive();

    if (isUpdatingTarget_) {
        return;
    }

    // Collect values from all child bindings
    std::vector<std::any> sourceValues = CollectSourceValues();

    // Apply converter
    std::any convertedValue;
    if (definition_.GetConverter()) {
        const std::any* parameter = definition_.HasConverterParameter() 
            ? &definition_.GetConverterParameter() 
            : nullptr;
        convertedValue = definition_.GetConverter()->Convert(
            sourceValues,
            property_->PropertyType(),
            parameter);
    } else {
        // No converter: if single value, use it directly; otherwise use first value
        if (sourceValues.size() == 1) {
            convertedValue = sourceValues[0];
        } else if (!sourceValues.empty()) {
            convertedValue = sourceValues[0];
        }
    }

    ApplyTargetValue(std::move(convertedValue));
}

void MultiBindingExpression::EnsureTargetAlive() const {
    if (target_ == nullptr) {
        throw std::runtime_error("MultiBindingExpression target has been detached");
    }
}

void MultiBindingExpression::InitializeEffectiveSettings() {
    const auto& metadata = property_->Metadata();
    const auto& options = metadata.bindingOptions;

    if (definition_.HasExplicitMode()) {
        effectiveMode_ = definition_.GetMode();
    } else {
        effectiveMode_ = options.defaultMode;
    }

    UpdateSourceTrigger resolvedTrigger;
    if (definition_.HasExplicitUpdateSourceTrigger()) {
        resolvedTrigger = definition_.GetUpdateSourceTrigger();
        if (resolvedTrigger == UpdateSourceTrigger::Default) {
            resolvedTrigger = options.updateSourceTrigger;
        }
    } else {
        resolvedTrigger = options.updateSourceTrigger;
    }

    if (resolvedTrigger == UpdateSourceTrigger::Default) {
        resolvedTrigger = UpdateSourceTrigger::PropertyChanged;
    }

    effectiveUpdateSourceTrigger_ = resolvedTrigger;
}

void MultiBindingExpression::SubscribeToChildren() {
    auto weakSelf = weak_from_this();

    childConnections_.clear();
    childConnections_.reserve(childExpressions_.size());

    for (size_t i = 0; i < childExpressions_.size(); ++i) {
        auto& child = childExpressions_[i];
        if (!child) {
            childConnections_.emplace_back();
            continue;
        }

        // Activate child binding
        child->Activate();

        // Subscribe to SOURCE property changes, not target
        const auto& childDef = child->Definition();
        std::any sourceRoot;
        
        if (childDef.HasExplicitSource()) {
            sourceRoot = childDef.GetSource();
        } else {
            sourceRoot = target_->GetDataContext();
        }

        // Try to get INotifyPropertyChanged from source
        std::shared_ptr<INotifyPropertyChanged> notifier;
        if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&sourceRoot)) {
            notifier = *ptr;
        }

        if (notifier) {
            const std::string& propertyName = childDef.GetPath();
            auto connection = notifier->PropertyChanged().Connect(
                [weakSelf, propertyName](std::string_view changedProp) {
                    if (auto self = weakSelf.lock()) {
                        if (changedProp == propertyName) {
                            self->OnChildBindingUpdated();
                        }
                    }
                });
            childConnections_.push_back(std::move(connection));
        } else {
            childConnections_.emplace_back();
        }
    }
}

void MultiBindingExpression::UnsubscribeFromChildren() {
    for (auto& conn : childConnections_) {
        if (conn.IsConnected()) {
            conn.Disconnect();
        }
    }
    childConnections_.clear();
}

void MultiBindingExpression::OnChildBindingUpdated() {
    if (!isActive_ || isUpdatingTarget_) {
        return;
    }
    UpdateTarget();
}

void MultiBindingExpression::ApplyTargetValue(std::any value) {
    if (!isActive_) {
        return;
    }
    EnsureTargetAlive();

    if (isUpdatingTarget_) {
        return;
    }

    ScopedFlag guard(isUpdatingTarget_);
    target_->ApplyBindingValue(*property_, std::move(value));
}

std::vector<std::any> MultiBindingExpression::CollectSourceValues() const {
    std::vector<std::any> values;
    values.reserve(childExpressions_.size());

    for (const auto& child : childExpressions_) {
        if (child && child->IsActive()) {
            // Get the child's source and resolve its path
            const auto& childDef = child->Definition();
            std::any sourceRoot;
            
            if (childDef.HasExplicitSource()) {
                sourceRoot = childDef.GetSource();
            } else {
                sourceRoot = target_->GetDataContext();
            }
            
            // Resolve the path on the source
            const auto& pathStr = childDef.GetPath();
            if (!pathStr.empty()) {
                // Find the property accessor
                const auto* accessor = PropertyAccessorRegistry::FindAccessor(
                    sourceRoot.type(), pathStr);
                
                if (accessor && accessor->getter) {
                    std::any result;
                    if (accessor->getter(sourceRoot, result)) {
                        values.push_back(result);
                    } else {
                        values.push_back(std::any{});
                    }
                } else {
                    values.push_back(std::any{});
                }
            } else {
                // No path, use source root directly
                values.push_back(sourceRoot);
            }
        } else {
            values.push_back(std::any{});
        }
    }

    return values;
}

} // namespace fk::binding
