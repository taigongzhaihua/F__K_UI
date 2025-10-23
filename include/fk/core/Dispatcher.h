#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <functional>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>

namespace fk::core {

class DispatcherOperation;

namespace detail {
struct DispatcherOperationState;
} // namespace detail

class IDispatcherBackend {
public:
    virtual ~IDispatcherBackend() = default;

    virtual void NotifyWorkPending() = 0;
    virtual void WaitForEvents(std::chrono::milliseconds timeout) = 0;
};

class Dispatcher : public std::enable_shared_from_this<Dispatcher> {
public:
    using Task = std::function<void()>;

    enum class Priority {
        High = 0,
        Normal = 1,
        Low = 2
    };

    explicit Dispatcher(std::string name = {}, std::unique_ptr<IDispatcherBackend> backend = {});
    ~Dispatcher();

    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

    void Post(Task task, Priority priority = Priority::Normal);
    DispatcherOperation BeginInvoke(Task task, Priority priority = Priority::Normal);
    DispatcherOperation PostDelayed(Task task, std::chrono::milliseconds delay, Priority priority = Priority::Normal);

    void Send(Task task);

    void Run();
    void Shutdown();

    bool HasThreadAccess() const;
    void VerifyAccess() const;

    const std::string& Name() const { return name_; }

private:
    using OperationState = detail::DispatcherOperationState;

    struct QueuedTask;
    struct ScheduledTask {
        std::chrono::steady_clock::time_point due;
        Task task;
        std::shared_ptr<OperationState> state;
        Priority priority{Priority::Normal};
        std::size_t sequence{0};
    };

    struct ScheduledTaskComparer {
        bool operator()(const ScheduledTask& lhs, const ScheduledTask& rhs) const noexcept {
            if (lhs.due == rhs.due) {
                return lhs.sequence > rhs.sequence;
            }
            return lhs.due > rhs.due;
        }
    };

    void EnqueueTask(Task task, std::shared_ptr<OperationState> state, Priority priority);
    DispatcherOperation EnqueueDelayedTask(Task task, std::chrono::steady_clock::time_point due,
        std::shared_ptr<OperationState> state, Priority priority);

    bool TryDequeue(QueuedTask& outTask);
    void MoveDueTasksLocked(std::chrono::steady_clock::time_point now);
    void WaitForWork();
    void ExecuteTask(QueuedTask& task);
    void WakeUp();
    void CancelPendingTasks();

    std::string name_;
    std::thread::id threadId_{};
    std::atomic<bool> running_{false};

    mutable std::mutex queueMutex_;
    std::condition_variable cv_;

    std::deque<QueuedTask> immediateTasks_;
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, ScheduledTaskComparer> delayedTasks_;

    std::unique_ptr<IDispatcherBackend> backend_;
    std::size_t sequenceCounter_{0};
};

class DispatcherOperation {
public:
    enum class Status {
        Pending,
        Running,
        Completed,
        Canceled,
        Faulted
    };

    DispatcherOperation();

    bool IsValid() const;

    bool Cancel();

    void Wait() const;

    Status GetStatus() const;
    std::exception_ptr GetException() const;

private:
    friend class Dispatcher;
    explicit DispatcherOperation(std::shared_ptr<detail::DispatcherOperationState> state) noexcept;

    std::shared_ptr<detail::DispatcherOperationState> state_;
};
} // namespace fk::core
