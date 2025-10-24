#include "fk/ui/DispatcherObject.h"

#include <stdexcept>

namespace fk::ui {

DispatcherObject::DispatcherObject() = default;

DispatcherObject::DispatcherObject(std::shared_ptr<core::Dispatcher> dispatcher)
    : dispatcher_(std::move(dispatcher)) {}

DispatcherObject::~DispatcherObject() = default;

void DispatcherObject::SetDispatcher(std::shared_ptr<core::Dispatcher> dispatcher) {
    std::lock_guard lock(dispatcherMutex_);
    dispatcher_ = std::move(dispatcher);
}

std::shared_ptr<core::Dispatcher> DispatcherObject::GetDispatcherLocked() const {
    std::lock_guard lock(dispatcherMutex_);
    return dispatcher_;
}

std::shared_ptr<core::Dispatcher> DispatcherObject::GetDispatcher() const {
    return GetDispatcherLocked();
}

core::Dispatcher* DispatcherObject::Dispatcher() const noexcept {
    auto dispatcher = GetDispatcherLocked();
    return dispatcher.get();
}

bool DispatcherObject::HasThreadAccess() const {
    auto dispatcher = GetDispatcherLocked();
    if (!dispatcher) {
        return true;
    }
    return dispatcher->HasThreadAccess();
}

void DispatcherObject::VerifyAccess() const {
    auto dispatcher = GetDispatcherLocked();
    if (!dispatcher) {
        return;
    }
    dispatcher->VerifyAccess();
}

void DispatcherObject::EnsureDispatcher() const {
    if (!Dispatcher()) {
        throw std::runtime_error("DispatcherObject requires an associated Dispatcher");
    }
}

} // namespace fk::ui
