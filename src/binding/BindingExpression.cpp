#include "fk/binding/BindingExpression.h"
#include "fk/binding/ValueConverters.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/core/Dispatcher.h"
#include "fk/ui/UIElement.h"

#include <functional>
#include <stdexcept>
#include <string_view>
#include <typeindex>
#include <utility>
#include <iostream>

namespace fk::binding {

namespace {

class ScopedFlag {
public:
    explicit ScopedFlag(bool& flag)
        : flag_(flag) {
        flag_ = true;
    }

    ScopedFlag(const ScopedFlag&) = delete;
    ScopedFlag& operator=(const ScopedFlag&) = delete;

    ~ScopedFlag() {
        flag_ = false;
    }

private:
    bool& flag_;
};

} // namespace

BindingExpression::BindingExpression(Binding definition, DependencyObject* target, const DependencyProperty& property)
    : definition_(std::move(definition))
    , path_(definition_.GetPath())
    , target_(target)
    , property_(&property) {
    if (target_ == nullptr) {
        throw std::invalid_argument("BindingExpression requires a non-null target");
    }
    InitializeEffectiveSettings();
}

void BindingExpression::Activate() {
    if (isActive_) {
        return;
    }
    EnsureTargetAlive();
    isActive_ = true;
    Subscribe();
    UpdateTarget();
}

void BindingExpression::Detach() {
    if (!isActive_) {
        return;
    }
    Unsubscribe();
    isActive_ = false;
}

void BindingExpression::UpdateTarget() {
    if (!isActive_ || effectiveMode_ == BindingMode::OneWayToSource) {
        return;
    }

    EnsureTargetAlive();

    if (isUpdatingTarget_) {
        return;
    }

    std::any resolvedValue;
    bool resolved = false;

    // 对于 TemplateBinding，总是重新解析源，因为 TemplatedParent 可能在设置绑定后才被设置
    std::any* sourceRef = currentSource_.has_value() && !definition_.IsTemplateBinding() ? &currentSource_ : nullptr;
    std::any temporary;
    if (sourceRef == nullptr) {
        temporary = ResolveSourceRoot();
        sourceRef = &temporary;
    }

    if (path_.IsEmpty()) {
        resolvedValue = *sourceRef;
        resolved = sourceRef->has_value();
    } else {
        resolved = path_.Resolve(*sourceRef, resolvedValue);
    }

    if (!resolved) {
        ApplyTargetValue(std::any{});
        return;
    }

    std::any value = resolvedValue;
    if (const auto& converter = definition_.GetConverter()) {
        const std::any* parameter = definition_.HasConverterParameter() ? &definition_.GetConverterParameter() : nullptr;
        value = converter->Convert(value, property_->PropertyType(), parameter);
    } else {
        std::any converted;
        if (TryDefaultConvert(resolvedValue, property_->PropertyType(), converted)) {
            value = std::move(converted);
        }
    }

    ApplyTargetValue(std::move(value));
}

void BindingExpression::UpdateSource() {
    if (!isActive_) {
        return;
    }

    const auto mode = effectiveMode_;
    if (mode == BindingMode::OneWay || mode == BindingMode::OneTime) {
        return;
    }

    EnsureTargetAlive();

    if (isUpdatingSource_) {
        return;
    }

    if (path_.IsEmpty()) {
        // Writing directly to the root object is not supported.
        return;
    }

    std::any currentSourceValue;
    const bool hasCurrentValue = TryResolveSourceValue(currentSourceValue);
    std::type_index sourceType = hasCurrentValue && currentSourceValue.has_value()
        ? std::type_index(currentSourceValue.type())
        : std::type_index(typeid(std::any));

    std::any targetValue = target_->GetValue(*property_);
    std::any valueToAssign = targetValue;

    if (const auto& converter = definition_.GetConverter()) {
        const std::any* parameter = definition_.HasConverterParameter() ? &definition_.GetConverterParameter() : nullptr;
        valueToAssign = converter->ConvertBack(targetValue, sourceType, parameter);
    } else {
        std::any converted;
        if (TryDefaultConvert(targetValue, sourceType, converted)) {
            valueToAssign = std::move(converted);
        }
    }

    if (!ValidateBeforeSet(valueToAssign)) {
        return;
    }

    ScopedFlag guard(isUpdatingSource_);
    if (!TrySetSourceValue(valueToAssign)) {
        return;
    }

    if (HasValidationErrors()) {
        ClearValidationErrors();
    }
}

void BindingExpression::UpdateSourceExplicitly() {
    // For Explicit mode, allow manual trigger of source update
    if (!isActive_) {
        return;
    }
    if (effectiveUpdateSourceTrigger_ != UpdateSourceTrigger::Explicit) {
        return;
    }
    UpdateSource();
}

void BindingExpression::UpdateTargetAsync() {
    if (!isActive_ || !definition_.GetIsAsync()) {
        UpdateTarget();  // Fall back to synchronous update
        return;
    }

    // Avoid duplicate pending updates
    if (hasPendingTargetUpdate_) {
        return;
    }

    hasPendingTargetUpdate_ = true;
    auto weakSelf = weak_from_this();

    // Note: Using a simple approach here. In production, this should integrate
    // with the UI framework's Dispatcher/main thread message loop.
    // For now, we'll execute immediately but with the async flag for demonstration.
    // A real implementation would use: Dispatcher::Instance().Post([weakSelf]() { ... });
    
    // Immediate execution for now (to avoid threading complexity in demo)
    if (auto self = weakSelf.lock()) {
        self->hasPendingTargetUpdate_ = false;
        self->UpdateTarget();
    }
}

void BindingExpression::UpdateSourceAsync() {
    if (!isActive_ || !definition_.GetIsAsync()) {
        UpdateSource();  // Fall back to synchronous update
        return;
    }

    // Avoid duplicate pending updates
    if (hasPendingSourceUpdate_) {
        return;
    }

    hasPendingSourceUpdate_ = true;
    auto weakSelf = weak_from_this();

    // Immediate execution for now (to avoid threading complexity in demo)
    if (auto self = weakSelf.lock()) {
        self->hasPendingSourceUpdate_ = false;
        self->UpdateSource();
    }
}

void BindingExpression::ApplyTargetValue(std::any value) {
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

void BindingExpression::EnsureTargetAlive() const {
    if (target_ == nullptr) {
        throw std::runtime_error("BindingExpression target has been detached");
    }
}

void BindingExpression::Subscribe() {
    auto weakSelf = weak_from_this();

    RefreshSourceSubscription();

    if (ShouldListenToSource() && !definition_.HasExplicitSource()) {
        dataContextConnection_ = target_->DataContextChanged.Connect(
            [weakSelf](const std::any&, const std::any&) {
                if (auto self = weakSelf.lock()) {
                    self->RefreshSourceSubscription();
                    if (self->definition_.GetIsAsync()) {
                        self->UpdateTargetAsync();
                    } else {
                        self->UpdateTarget();
                    }
                }
            });
    } else {
        dataContextConnection_.Disconnect();
    }

    if (ShouldSubscribeToTargetChanges()) {
        targetPropertyConnection_ = target_->PropertyChanged.Connect(
            [weakSelf](const DependencyProperty& property, const std::any&, const std::any&, ValueSource, ValueSource newSource) {
                if (auto self = weakSelf.lock()) {
                    if (&property != self->property_) {
                        return;
                    }
                    if (!self->isActive_ || self->isUpdatingTarget_) {
                        return;
                    }
                    if (newSource != ValueSource::Local) {
                        return;
                    }
                    if (self->effectiveUpdateSourceTrigger_ == UpdateSourceTrigger::PropertyChanged) {
                        if (self->definition_.GetIsAsync()) {
                            self->UpdateSourceAsync();
                        } else {
                            self->UpdateSource();
                        }
                    }
                }
            });
    } else {
        targetPropertyConnection_.Disconnect();
    }
}

void BindingExpression::Unsubscribe() {
    dataContextConnection_.Disconnect();
    targetPropertyConnection_.Disconnect();
    sourcePropertyConnection_.Disconnect();
    sharedNotifierHolder_.reset();
    rawNotifier_ = nullptr;
    currentSource_.reset();
}

std::any BindingExpression::ResolveSourceRoot() const {
    // 检测是否为 TemplateBinding
    if (definition_.IsTemplateBinding()) {
        // TemplateBinding 应该绑定到 TemplatedParent
        if (target_) {
            // 尝试将 target 转换为 UIElement 以获取 TemplatedParent
            if (auto* uiElement = dynamic_cast<ui::UIElement*>(target_)) {
                auto* templatedParent = uiElement->GetTemplatedParent();
                if (templatedParent) {
                    return std::any(templatedParent);
                }
            }
        }
        // 如果无法获取 TemplatedParent，返回空
        return std::any{};
    }
    
    if (definition_.HasExplicitSource()) {
        return definition_.GetSource();
    }
    if (definition_.HasRelativeSource()) {
        return ResolveRelativeSource();
    }
    if (definition_.HasElementName()) {
        return ResolveElementSource();
    }
    if (!target_) {
        return std::any{};
    }
    return target_->GetDataContext();
}

bool BindingExpression::TryResolveSourceValue(std::any& value) const {
    if (currentSource_.has_value()) {
        if (path_.IsEmpty()) {
            value = currentSource_;
            return currentSource_.has_value();
        }
        return path_.Resolve(currentSource_, value);
    }

    std::any sourceRoot = ResolveSourceRoot();
    if (path_.IsEmpty()) {
        value = sourceRoot;
        return sourceRoot.has_value();
    }
    return path_.Resolve(sourceRoot, value);
}

bool BindingExpression::TrySetSourceValue(const std::any& value) {
    if (path_.IsEmpty()) {
        return false;
    }
    std::any* sourceRef = currentSource_.has_value() ? &currentSource_ : nullptr;
    std::any temporary;
    if (sourceRef == nullptr) {
        temporary = ResolveSourceRoot();
        sourceRef = &temporary;
    }
    if (!sourceRef->has_value()) {
        return false;
    }
    return path_.SetValue(*sourceRef, value);
}

bool BindingExpression::ShouldListenToSource() const {
    return effectiveMode_ == BindingMode::OneWay || effectiveMode_ == BindingMode::TwoWay;
}

bool BindingExpression::ShouldSubscribeToTargetChanges() const {
    if (effectiveMode_ != BindingMode::TwoWay && effectiveMode_ != BindingMode::OneWayToSource) {
        return false;
    }
    return effectiveUpdateSourceTrigger_ == UpdateSourceTrigger::PropertyChanged;
}

void BindingExpression::RefreshSourceSubscription() {
    sourcePropertyConnection_.Disconnect();
    sharedNotifierHolder_.reset();
    rawNotifier_ = nullptr;
    currentSource_.reset();

    if (!ShouldListenToSource()) {
        return;
    }

    currentSource_ = ResolveSourceRoot();
    auto notifier = TryGetNotifier(currentSource_);
    
    if (notifier == nullptr) {
        return;
    }

    auto weakSelf = weak_from_this();
    rawNotifier_ = notifier;
    sourcePropertyConnection_ = notifier->PropertyChanged().Connect(
        [weakSelf](std::string_view propertyName) {
            if (auto self = weakSelf.lock()) {
                if (!self->isActive_ || self->isUpdatingTarget_) {
                    return;
                }
                if (self->definition_.GetIsAsync()) {
                    self->UpdateTargetAsync();
                } else {
                    self->UpdateTarget();
                }
            }
        });
}

INotifyPropertyChanged* BindingExpression::TryGetNotifier(std::any& holder) {
    if (!holder.has_value()) {
        return nullptr;
    }

    if (auto* ptr = std::any_cast<INotifyPropertyChanged>(&holder)) {
        return ptr;
    }

    if (auto* ptr = std::any_cast<INotifyPropertyChanged*>(&holder)) {
        return *ptr;
    }

    if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&holder)) {
        sharedNotifierHolder_ = *ptr;
        return sharedNotifierHolder_.get();
    }

    if (auto* ptr = std::any_cast<std::weak_ptr<INotifyPropertyChanged>>(&holder)) {
        sharedNotifierHolder_ = ptr->lock();
        return sharedNotifierHolder_.get();
    }

    if (auto* ptr = std::any_cast<std::reference_wrapper<INotifyPropertyChanged>>(&holder)) {
        return &ptr->get();
    }

    if (auto* ptr = std::any_cast<std::reference_wrapper<const INotifyPropertyChanged>>(&holder)) {
        return const_cast<INotifyPropertyChanged*>(&ptr->get());
    }

    return nullptr;
}

void BindingExpression::InitializeEffectiveSettings() {
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

std::any BindingExpression::ResolveRelativeSource() const {
    if (!target_ || !definition_.HasRelativeSource()) {
        return std::any{};
    }

    const auto& relative = definition_.GetRelativeSource();
    switch (relative.Mode()) {
    case RelativeSourceMode::Self:
        return std::any(target_);
    case RelativeSourceMode::FindAncestor: {
        DependencyObject* current = target_->GetLogicalParent();
        int remaining = relative.AncestorLevel();
        while (current) {
            if (!relative.HasAncestorType() || std::type_index(typeid(*current)) == relative.AncestorType()) {
                --remaining;
                if (remaining <= 0) {
                    return std::any(current);
                }
            }
            current = current->GetLogicalParent();
        }
        break;
    }
    default:
        break;
    }
    return std::any{};
}

std::any BindingExpression::ResolveElementSource() const {
    if (!target_ || !definition_.HasElementName()) {
        return std::any{};
    }

    const std::string& name = definition_.GetElementName();
    for (DependencyObject* scope = target_; scope != nullptr; scope = scope->GetLogicalParent()) {
        if (auto* found = scope->FindElementByName(name)) {
            return std::any(found);
        }
    }
    return std::any{};
}

bool BindingExpression::ValidateBeforeSet(const std::any& candidate) {
    std::vector<ValidationResult> errors;
    errors.reserve(definition_.GetValidationRules().size());

    for (const auto& rule : definition_.GetValidationRules()) {
        if (!rule) {
            continue;
        }
        auto result = rule->Validate(candidate);
        if (!result.isValid) {
            errors.push_back(std::move(result));
        }
    }

    if (!errors.empty()) {
        SetValidationErrors(std::move(errors));
        return false;
    }

    if (HasValidationErrors()) {
        ClearValidationErrors();
    }

    return true;
}

void BindingExpression::SetValidationErrors(std::vector<ValidationResult> errors) {
    validationErrors_ = std::move(errors);
    ValidationErrorsChanged(validationErrors_);
}

void BindingExpression::ClearValidationErrors() {
    if (validationErrors_.empty()) {
        return;
    }
    validationErrors_.clear();
    ValidationErrorsChanged(validationErrors_);
}

} // namespace fk::binding
