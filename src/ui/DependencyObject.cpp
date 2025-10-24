#include "fk/ui/DependencyObject.h"

#include <stdexcept>
#include <utility>

namespace fk::ui {

DependencyObject::DependencyObject() = default;

DependencyObject::DependencyObject(std::shared_ptr<core::Dispatcher> dispatcher)
    : DispatcherObject(std::move(dispatcher)) {}

DependencyObject::~DependencyObject() = default;

void DependencyObject::AttachToLogicalTree(std::shared_ptr<core::Dispatcher> dispatcher) {
    VerifyAccess();
    if (dispatcher) {
        SetDispatcher(std::move(dispatcher));
    }
    if (isAttached_) {
        return;
    }
    InvokeAttach();
}

void DependencyObject::AttachToLogicalParent(DependencyObject* parent) {
    if (!parent) {
        AttachToLogicalTree();
        return;
    }
    if (parent == this) {
        throw std::invalid_argument("DependencyObject cannot attach to itself as logical parent");
    }

    parent->AddLogicalChild(this);
}

void DependencyObject::DetachFromLogicalTree() {
    VerifyAccess();
    if (auto* parent = dynamic_cast<DependencyObject*>(GetLogicalParent())) {
        parent->RemoveLogicalChild(this);
        return;
    }

    InvokeDetach();
}

void DependencyObject::AddLogicalChild(DependencyObject* child) {
    if (!child || child == this) {
        return;
    }

    VerifyAccess();

    auto* existingParent = dynamic_cast<DependencyObject*>(child->GetLogicalParent());
    if (existingParent == this) {
        if (!child->isAttached_) {
            child->InheritDispatcherFrom(this);
            child->InvokeAttach();
        }
        return;
    }

    if (existingParent) {
        existingParent->RemoveLogicalChild(child);
    }

    BindingDependencyObject::AddLogicalChild(child);

    child->InheritDispatcherFrom(this);
    child->InvokeAttach();
}

void DependencyObject::RemoveLogicalChild(DependencyObject* child) {
    if (!child) {
        return;
    }

    VerifyAccess();

    if (child->GetLogicalParent() != this) {
        return;
    }

    BindingDependencyObject::RemoveLogicalChild(child);
    child->InvokeDetach();
}

void DependencyObject::InheritDispatcherFrom(DependencyObject* parent) {
    if (!parent) {
        return;
    }
    if (GetDispatcher()) {
        return;
    }
    SetDispatcher(parent->GetDispatcher());
}

void DependencyObject::InvokeAttach() {
    if (isAttached_) {
        return;
    }

    isAttached_ = true;
    OnAttachedToLogicalTree();
    AttachedToLogicalTree(*this);
}

void DependencyObject::InvokeDetach() {
    if (!isAttached_) {
        return;
    }

    isAttached_ = false;
    OnDetachedFromLogicalTree();
    DetachedFromLogicalTree(*this);
}

void DependencyObject::OnAttachedToLogicalTree() {}

void DependencyObject::OnDetachedFromLogicalTree() {}

} // namespace fk::ui
