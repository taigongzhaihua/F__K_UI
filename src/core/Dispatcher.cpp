#include "fk/core/Dispatcher.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <utility>

namespace fk::core {

namespace {
using Clock = std::chrono::steady_clock;
} // namespace

namespace detail {
struct DispatcherOperationState {
    std::atomic<DispatcherOperation::Status> status{DispatcherOperation::Status::Pending};
    std::atomic<bool> canceled{false};
    std::mutex mutex;
    std::condition_variable cv;
    std::exception_ptr error{};

    bool TryStart() {
        if (canceled.load(std::memory_order_acquire)) {
            status.store(DispatcherOperation::Status::Canceled, std::memory_order_release);
            cv.notify_all();
            return false;
        }

        auto expected = DispatcherOperation::Status::Pending;
        if (!status.compare_exchange_strong(expected, DispatcherOperation::Status::Running, std::memory_order_acq_rel)) {
            return false;
        }
        return true;
    }

    void MarkCompleted() {
        status.store(DispatcherOperation::Status::Completed, std::memory_order_release);
        cv.notify_all();
    }

    void MarkCanceled() {
        status.store(DispatcherOperation::Status::Canceled, std::memory_order_release);
        cv.notify_all();
    }

    void MarkFaulted(std::exception_ptr ex) {
        error = std::move(ex);
        status.store(DispatcherOperation::Status::Faulted, std::memory_order_release);
        cv.notify_all();
    }
};
} // namespace detail

struct Dispatcher::QueuedTask {
    Task task;
    std::shared_ptr<OperationState> state;
    Priority priority{Priority::Normal};
    std::size_t sequence{0};
};

Dispatcher::Dispatcher(std::string name, std::unique_ptr<IDispatcherBackend> backend)
    : name_(std::move(name)), backend_(std::move(backend)) {}

Dispatcher::~Dispatcher() {
    Shutdown();
}

void Dispatcher::Post(Task task, Priority priority) {
    if (!task) {
        return;
    }
    EnqueueTask(std::move(task), nullptr, priority);
}

DispatcherOperation Dispatcher::BeginInvoke(Task task, Priority priority) {
    if (!task) {
        return DispatcherOperation{};
    }
    auto state = std::make_shared<OperationState>();
    EnqueueTask(std::move(task), state, priority);
    return DispatcherOperation(state);
}

DispatcherOperation Dispatcher::PostDelayed(Task task, std::chrono::milliseconds delay, Priority priority) {
    if (!task) {
        return DispatcherOperation{};
    }
    auto state = std::make_shared<OperationState>();
    const auto due = Clock::now() + delay;
    return EnqueueDelayedTask(std::move(task), due, state, priority);
}

void Dispatcher::Send(Task task) {
    if (!task) {
        return;
    }

    if (HasThreadAccess()) {
        task();
        return;
    }

    auto op = BeginInvoke(std::move(task), Priority::High);
    op.Wait();

    if (op.GetStatus() == DispatcherOperation::Status::Faulted) {
        if (auto ex = op.GetException()) {
            std::rethrow_exception(ex);
        }
    }
}

void Dispatcher::Run() {
    if (running_.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    threadId_ = std::this_thread::get_id();

    while (running_.load(std::memory_order_acquire)) {
        QueuedTask task;
        if (TryDequeue(task)) {
            ExecuteTask(task);
            continue;
        }
        WaitForWork();
    }

    CancelPendingTasks();
}

void Dispatcher::Shutdown() {
    bool expected = true;
    if (!running_.compare_exchange_strong(expected, false, std::memory_order_acq_rel)) {
        running_.store(false, std::memory_order_release);
    }
    WakeUp();
}

bool Dispatcher::HasThreadAccess() const {
    return std::this_thread::get_id() == threadId_;
}

void Dispatcher::VerifyAccess() const {
    if (!HasThreadAccess()) {
        throw std::runtime_error("Dispatcher access from invalid thread");
    }
}

void Dispatcher::EnqueueTask(Task task, std::shared_ptr<OperationState> state, Priority priority) {
    {
        std::lock_guard lock(queueMutex_);
        immediateTasks_.push_back(QueuedTask{std::move(task), std::move(state), priority, sequenceCounter_++});
    }
    WakeUp();
}

DispatcherOperation Dispatcher::EnqueueDelayedTask(Task task, std::chrono::steady_clock::time_point due,
    std::shared_ptr<OperationState> state, Priority priority) {
    auto opState = state;
    {
        std::lock_guard lock(queueMutex_);
        delayedTasks_.push(ScheduledTask{due, std::move(task), std::move(state), priority, sequenceCounter_++});
    }
    WakeUp();
    return DispatcherOperation(std::move(opState));
}

bool Dispatcher::TryDequeue(QueuedTask& outTask) {
    std::lock_guard lock(queueMutex_);
    MoveDueTasksLocked(Clock::now());
    if (immediateTasks_.empty()) {
        return false;
    }

    outTask = std::move(immediateTasks_.front());
    immediateTasks_.pop_front();
    return true;
}

void Dispatcher::MoveDueTasksLocked(std::chrono::steady_clock::time_point now) {
    while (!delayedTasks_.empty()) {
        const auto& top = delayedTasks_.top();
        if (top.due > now) {
            break;
        }
        auto task = delayedTasks_.top();
        delayedTasks_.pop();
        immediateTasks_.push_back(QueuedTask{std::move(task.task), std::move(task.state), task.priority, task.sequence});
    }
}

void Dispatcher::WaitForWork() {
    std::unique_lock lock(queueMutex_);
    while (running_.load(std::memory_order_acquire)) {
        MoveDueTasksLocked(Clock::now());
        if (!immediateTasks_.empty()) {
            return;
        }
        if (!running_.load(std::memory_order_acquire)) {
            return;
        }
        if (!delayedTasks_.empty()) {
            auto nextDue = delayedTasks_.top().due;
            cv_.wait_until(lock, nextDue, [this]() {
                return !running_.load(std::memory_order_acquire) || !immediateTasks_.empty() || delayedTasks_.empty();
            });
        } else {
            cv_.wait(lock, [this]() {
                return !running_.load(std::memory_order_acquire) || !immediateTasks_.empty() || !delayedTasks_.empty();
            });
        }
    }
}

void Dispatcher::ExecuteTask(QueuedTask& task) {
    auto state = task.state;
    if (state) {
        if (!state->TryStart()) {
            return;
        }
    }

    try {
        task.task();
        if (state) {
            state->MarkCompleted();
        }
    } catch (...) {
        if (state) {
            state->MarkFaulted(std::current_exception());
        } else {
            std::cerr << "Unhandled exception in Dispatcher::ExecuteTask" << std::endl;
        }
    }
}

void Dispatcher::WakeUp() {
    cv_.notify_one();
    if (backend_) {
        backend_->NotifyWorkPending();
    }
}

void Dispatcher::CancelPendingTasks() {
    std::lock_guard lock(queueMutex_);

    for (auto& task : immediateTasks_) {
        if (task.state) {
            task.state->MarkCanceled();
        }
    }
    immediateTasks_.clear();

    while (!delayedTasks_.empty()) {
        auto task = delayedTasks_.top();
        delayedTasks_.pop();
        if (task.state) {
            task.state->MarkCanceled();
        }
    }
}

DispatcherOperation::DispatcherOperation() = default;

DispatcherOperation::DispatcherOperation(std::shared_ptr<detail::DispatcherOperationState> state) noexcept
    : state_(std::move(state)) {}

bool DispatcherOperation::IsValid() const {
    return static_cast<bool>(state_);
}

bool DispatcherOperation::Cancel() {
    if (!state_) {
        return false;
    }

    auto expected = DispatcherOperation::Status::Pending;
    if (state_->status.compare_exchange_strong(expected, DispatcherOperation::Status::Canceled, std::memory_order_acq_rel)) {
        state_->canceled.store(true, std::memory_order_release);
        state_->cv.notify_all();
        return true;
    }

    state_->canceled.store(true, std::memory_order_release);
    return false;
}

void DispatcherOperation::Wait() const {
    if (!state_) {
        return;
    }
    std::unique_lock lock(state_->mutex);
    state_->cv.wait(lock, [this]() {
        auto status = state_->status.load(std::memory_order_acquire);
        return status != Status::Pending && status != Status::Running;
    });
}

DispatcherOperation::Status DispatcherOperation::GetStatus() const {
    if (!state_) {
        return Status::Completed;
    }
    return state_->status.load(std::memory_order_acquire);
}

std::exception_ptr DispatcherOperation::GetException() const {
    if (!state_) {
        return nullptr;
    }
    return state_->error;
}

} // namespace fk::core
