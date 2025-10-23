#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>

#include "fk/core/Dispatcher.h"
#include "fk/core/Event.h"

namespace fk::core {

class Timer : public std::enable_shared_from_this<Timer> {
public:
    using Ptr = std::shared_ptr<Timer>;

    explicit Timer(std::shared_ptr<Dispatcher> dispatcher);
    ~Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void Start(std::chrono::milliseconds interval, bool repeat = true);
    void Stop();

    bool IsRunning() const;

    core::Event<> Tick;

private:
    void OnTick();

    std::weak_ptr<Dispatcher> dispatcher_;

    mutable std::mutex mutex_;
    std::chrono::milliseconds interval_{0};
    bool repeat_{false};
    bool running_{false};
    DispatcherOperation operation_;
};

} // namespace fk::core
