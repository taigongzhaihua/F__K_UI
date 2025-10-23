#pragma once

#include <chrono>
#include <mutex>

namespace fk::core {

class Clock {
public:
    using TimePoint = std::chrono::steady_clock::time_point;
    using Duration = std::chrono::steady_clock::duration;

    static Clock& Instance();

    TimePoint Now() const;
    double GetDeltaSeconds();
    void Reset();

private:
    Clock();

    mutable std::mutex mutex_;
    TimePoint lastTime_;
};

} // namespace fk::core
