#pragma once

#include "fk/animation/Timeline.h"
#include "fk/core/Clock.h"
#include "fk/core/Timer.h"
#include <vector>
#include <memory>
#include <mutex>

namespace fk::animation {

// 动画时钟 - 管理所有活动动画的更新
class AnimationClock {
public:
    static AnimationClock& Instance();

    // 注册和注销动画
    void RegisterTimeline(std::shared_ptr<Timeline> timeline);
    void UnregisterTimeline(Timeline* timeline);

    // 开始/停止动画时钟
    void Start();
    void Stop();
    
    bool IsRunning() const { return isRunning_; }

    // 设置更新频率（FPS）
    void SetFrameRate(int fps);
    int GetFrameRate() const { return frameRate_; }

private:
    AnimationClock();
    ~AnimationClock() = default;

    AnimationClock(const AnimationClock&) = delete;
    AnimationClock& operator=(const AnimationClock&) = delete;

    // 更新所有活动动画
    void Update();

    mutable std::mutex mutex_;
    std::vector<std::shared_ptr<Timeline>> timelines_;
    
    core::Clock::TimePoint lastUpdateTime_;
    bool isRunning_{false};
    int frameRate_{60};  // 默认60 FPS
    
    std::shared_ptr<core::Timer> updateTimer_;
};

} // namespace fk::animation
