#include "fk/binding/BindingContext.h"

#include "fk/binding/DependencyObject.h"

#include <string>

namespace fk::binding {

namespace {

const std::any& EmptyAny() {
    static const std::any empty;
    return empty;
}

} // namespace

BindingContext::BindingContext(DependencyObject& owner)
    : owner_(owner) {}

BindingContext::~BindingContext() {
    parentConnection_.Disconnect();
}

const std::any& BindingContext::GetDataContext() const noexcept {
    return hasEffective_ ? effectiveValue_ : EmptyAny();
}

void BindingContext::SetLocalDataContext(std::any value) {
    hasLocal_ = true;
    localValue_ = std::move(value);
    UpdateEffective();
}

void BindingContext::ClearLocalDataContext() {
    if (!hasLocal_) {
        return;
    }
    hasLocal_ = false;
    localValue_.reset();
    UpdateEffective();
}

void BindingContext::SetParent(BindingContext* parent) {
    if (parent == parent_) {
        return;
    }

    parentConnection_.Disconnect();
    parent_ = parent;

    if (parent_) {
        hasInherited_ = parent_->hasEffective_;
        inheritedValue_ = hasInherited_ ? parent_->effectiveValue_ : std::any{};
        parentConnection_ = parent_->DataContextChanged.Connect(
            [this](const std::any& oldValue, const std::any& newValue) {
                OnParentDataContextChanged(oldValue, newValue);
            });
    } else {
        hasInherited_ = false;
        inheritedValue_.reset();
    }

    UpdateEffective();
}

void BindingContext::OnParentDataContextChanged(const std::any&, const std::any& newValue) {
    if (!parent_) {
        return;
    }
    hasInherited_ = parent_->hasEffective_;
    inheritedValue_ = hasInherited_ ? newValue : std::any{};
    UpdateEffective();
}

void BindingContext::UpdateEffective() {
    std::any oldValue = hasEffective_ ? effectiveValue_ : std::any{};
    const bool hadOldValue = hasEffective_;

    std::any candidate;
    bool hasCandidate = false;
    if (hasLocal_) {
        candidate = localValue_;
        hasCandidate = true;
    } else if (hasInherited_) {
        candidate = inheritedValue_;
        hasCandidate = true;
    }

    if (hasCandidate) {
        effectiveValue_ = candidate;
    } else {
        effectiveValue_.reset();
    }

    hasEffective_ = hasCandidate;
    std::any newValue = hasEffective_ ? effectiveValue_ : std::any{};

    if (!AreEquivalent(oldValue, newValue) || hadOldValue != hasEffective_) {
        DataContextChanged(oldValue, newValue);
    }
}

bool BindingContext::AreEquivalent(const std::any& lhs, const std::any& rhs) {
    if (!lhs.has_value() && !rhs.has_value()) {
        return true;
    }
    if (lhs.type() != rhs.type()) {
        return false;
    }
    if (!lhs.has_value()) {
        return false;
    }
    if (lhs.type() == typeid(int)) {
        return std::any_cast<int>(lhs) == std::any_cast<int>(rhs);
    }
    if (lhs.type() == typeid(bool)) {
        return std::any_cast<bool>(lhs) == std::any_cast<bool>(rhs);
    }
    if (lhs.type() == typeid(double)) {
        return std::any_cast<double>(lhs) == std::any_cast<double>(rhs);
    }
    if (lhs.type() == typeid(float)) {
        return std::any_cast<float>(lhs) == std::any_cast<float>(rhs);
    }
    if (lhs.type() == typeid(std::string)) {
        return std::any_cast<std::string>(lhs) == std::any_cast<std::string>(rhs);
    }
    return false;
}

} // namespace fk::binding
