#pragma once

#include "fk/core/Dispatcher.h"

#include <memory>
#include <mutex>

namespace fk::ui {

class DispatcherObject {
public:
    DispatcherObject();
    explicit DispatcherObject(std::shared_ptr<core::Dispatcher> dispatcher);
    virtual ~DispatcherObject();

    DispatcherObject(const DispatcherObject&) = delete;
    DispatcherObject& operator=(const DispatcherObject&) = delete;

    DispatcherObject(DispatcherObject&&) noexcept = delete;
    DispatcherObject& operator=(DispatcherObject&&) noexcept = delete;

    void SetDispatcher(std::shared_ptr<core::Dispatcher> dispatcher);

    [[nodiscard]] std::shared_ptr<core::Dispatcher> GetDispatcher() const;
    [[nodiscard]] core::Dispatcher* Dispatcher() const noexcept;

    [[nodiscard]] bool HasThreadAccess() const;
    void VerifyAccess() const;

protected:
    void EnsureDispatcher() const;

private:
    std::shared_ptr<core::Dispatcher> GetDispatcherLocked() const;

    mutable std::mutex dispatcherMutex_;
    std::shared_ptr<core::Dispatcher> dispatcher_{};
};

} // namespace fk::ui

namespace fk {
    using ui::DispatcherObject;
}
