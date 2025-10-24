#pragma once

#include "fk/ui/DispatcherObject.h"
#include "fk/binding/DependencyObject.h"
#include "fk/core/Event.h"

#include <memory>

namespace fk::ui {

class DependencyObject : public DispatcherObject, public fk::binding::DependencyObject {
public:
    using BindingDependencyObject = fk::binding::DependencyObject;

    DependencyObject();
    explicit DependencyObject(std::shared_ptr<core::Dispatcher> dispatcher);
    ~DependencyObject() override;

    DependencyObject(const DependencyObject&) = delete;
    DependencyObject& operator=(const DependencyObject&) = delete;
    DependencyObject(DependencyObject&&) noexcept = delete;
    DependencyObject& operator=(DependencyObject&&) noexcept = delete;

    void AttachToLogicalTree(std::shared_ptr<core::Dispatcher> dispatcher = {});
    void AttachToLogicalParent(DependencyObject* parent);
    void DetachFromLogicalTree();

    void AddLogicalChild(DependencyObject* child);
    void RemoveLogicalChild(DependencyObject* child);

    [[nodiscard]] bool IsAttachedToLogicalTree() const noexcept { return isAttached_; }

    using BindingDependencyObject::FindElementByName;
    using BindingDependencyObject::GetDataContext;
    using BindingDependencyObject::GetLogicalChildren;
    using BindingDependencyObject::GetLogicalParent;

    core::Event<DependencyObject&> AttachedToLogicalTree;
    core::Event<DependencyObject&> DetachedFromLogicalTree;

protected:
    virtual void OnAttachedToLogicalTree();
    virtual void OnDetachedFromLogicalTree();

private:
    void InheritDispatcherFrom(DependencyObject* parent);
    void InvokeAttach();
    void InvokeDetach();

    bool isAttached_{false};
};

} // namespace fk::ui

