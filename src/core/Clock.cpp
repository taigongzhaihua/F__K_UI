#include "fk/core/Clock.h"

namespace fk::core {

Clock& Clock::Instance() {
    static Clock instance;
    return instance;
}

Clock::Clock()
    : lastTime_(std::chrono::steady_clock::now()) {}

Clock::TimePoint Clock::Now() const {
    return std::chrono::steady_clock::now();
}

double Clock::GetDeltaSeconds() {
    const auto current = Now();
    std::lock_guard lock(mutex_);
    const auto delta = current - lastTime_;
    lastTime_ = current;
    return std::chrono::duration<double>(delta).count();
}

void Clock::Reset() {
    std::lock_guard lock(mutex_);
    lastTime_ = std::chrono::steady_clock::now();
}

} // namespace fk::core
