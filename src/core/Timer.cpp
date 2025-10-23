#include "fk/core/Timer.h"

#include <stdexcept>
#include <utility>

namespace fk::core {

namespace {
inline std::shared_ptr<Dispatcher> RequireDispatcher(const std::weak_ptr<Dispatcher>& weak) {
    auto dispatcher = weak.lock();
    if (!dispatcher) {
        throw std::runtime_error("Timer requires a valid Dispatcher");
    }
    return dispatcher;
}
} // namespace

Timer::Timer(std::shared_ptr<Dispatcher> dispatcher)
    : dispatcher_(std::move(dispatcher)) {}

Timer::~Timer() {
    try {
        Stop();
    } catch (...) {
        // Destructors must not throw.
    }
}

void Timer::Start(std::chrono::milliseconds interval, bool repeat) {
    if (interval.count() < 0) {
        interval = std::chrono::milliseconds{0};
    }

    auto dispatcher = RequireDispatcher(dispatcher_);
    auto self = shared_from_this();

    DispatcherOperation scheduledOperation;
    {
        std::lock_guard lock(mutex_);
        if (running_) {
            operation_.Cancel();
        }
        interval_ = interval;
        repeat_ = repeat;
        running_ = true;
    }

    scheduledOperation = dispatcher->PostDelayed([self]() {
        self->OnTick();
    }, interval);

    {
        std::lock_guard lock(mutex_);
        if (!running_) {
            scheduledOperation.Cancel();
        } else {
            operation_ = std::move(scheduledOperation);
        }
    }
}

void Timer::Stop() {
    DispatcherOperation op;
    {
        std::lock_guard lock(mutex_);
        if (!running_) {
            return;
        }
        running_ = false;
        op = operation_;
        operation_ = DispatcherOperation{};
    }
    op.Cancel();
}

bool Timer::IsRunning() const {
    std::lock_guard lock(mutex_);
    return running_;
}

void Timer::OnTick() {
    std::shared_ptr<Dispatcher> dispatcher;
    std::chrono::milliseconds interval;
    bool repeat;

    {
        std::lock_guard lock(mutex_);
        dispatcher = dispatcher_.lock();
        interval = interval_;
        repeat = repeat_;
        operation_ = DispatcherOperation{};
        if (!running_) {
            return;
        }
        if (!dispatcher) {
            running_ = false;
            return;
        }
    }

    Tick();

    if (!repeat) {
        std::lock_guard lock(mutex_);
        running_ = false;
        return;
    }

    auto self = shared_from_this();
    auto op = dispatcher->PostDelayed([self]() {
        self->OnTick();
    }, interval);

    {
        std::lock_guard lock(mutex_);
        if (!running_) {
            op.Cancel();
        } else {
            operation_ = std::move(op);
        }
    }
}

} // namespace fk::core
