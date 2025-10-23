#pragma once

#include "fk/core/Event.h"

#include <any>
#include <memory>
#include <utility>

namespace fk::binding {

class DependencyObject;

class BindingContext {
public:
    using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;

    explicit BindingContext(DependencyObject& owner);
    ~BindingContext();

    const std::any& GetDataContext() const noexcept;
    bool HasDataContext() const noexcept { return hasEffective_; }
    bool HasLocalDataContext() const noexcept { return hasLocal_; }

    void SetLocalDataContext(std::any value);
    void ClearLocalDataContext();

    void SetParent(BindingContext* parent);
    BindingContext* Parent() const noexcept { return parent_; }

    DataContextChangedEvent DataContextChanged;

private:
    void UpdateEffective();
    void OnParentDataContextChanged(const std::any& oldValue, const std::any& newValue);

    static bool AreEquivalent(const std::any& lhs, const std::any& rhs);

    DependencyObject& owner_;
    BindingContext* parent_{nullptr};
    DataContextChangedEvent::Connection parentConnection_{};

    std::any localValue_;
    bool hasLocal_{false};

    std::any inheritedValue_;
    bool hasInherited_{false};

    std::any effectiveValue_;
    bool hasEffective_{false};
};

} // namespace fk::binding

namespace fk {
    using binding::BindingContext;
}
