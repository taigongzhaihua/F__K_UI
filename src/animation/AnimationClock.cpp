#include "fk/animation/AnimationClock.h"
#include <algorithm>

namespace fk::animation {

AnimationClock& AnimationClock::Instance() {
    static AnimationClock instance;
    return instance;
}

AnimationClock::AnimationClock() 
    : lastUpdateTime_(core::Clock::Instance().Now()) {
}

void AnimationClock::RegisterTimeline(std::shared_ptr<Timeline> timeline) {
    if (!timeline) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 检查是否已经注册
    auto it = std::find_if(timelines_.begin(), timelines_.end(),
        [&timeline](const std::shared_ptr<Timeline>& t) {
            return t.get() == timeline.get();
        });
    
    if (it == timelines_.end()) {
        timelines_.push_back(timeline);
    }
    
    // 如果时钟没有运行，自动启动
    if (!isRunning_) {
        Start();
    }
}

void AnimationClock::UnregisterTimeline(Timeline* timeline) {
    if (!timeline) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    timelines_.erase(
        std::remove_if(timelines_.begin(), timelines_.end(),
            [timeline](const std::shared_ptr<Timeline>& t) {
                return t.get() == timeline;
            }),
        timelines_.end()
    );
}

void AnimationClock::Start() {
    if (isRunning_) return;
    
    isRunning_ = true;
    lastUpdateTime_ = core::Clock::Instance().Now();
    
    // 创建更新定时器
    // 注意：这里需要 Dispatcher，暂时先用简单的实现
    // updateTimer_ = std::make_shared<core::Timer>(dispatcher);
    // updateTimer_->Start(std::chrono::milliseconds(1000 / frameRate_), true);
    // updateTimer_->Tick += [this]() { Update(); };
}

void AnimationClock::Stop() {
    if (!isRunning_) return;
    
    isRunning_ = false;
    
    if (updateTimer_) {
        updateTimer_->Stop();
        updateTimer_.reset();
    }
}

void AnimationClock::SetFrameRate(int fps) {
    if (fps <= 0 || fps > 240) return;
    
    frameRate_ = fps;
    
    // 如果正在运行，重新启动定时器
    if (isRunning_ && updateTimer_) {
        Stop();
        Start();
    }
}

void AnimationClock::Update() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 计算时间增量
    auto currentTime = core::Clock::Instance().Now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - lastUpdateTime_
    );
    lastUpdateTime_ = currentTime;
    
    // 更新所有活动的时间线
    std::vector<Timeline*> completedTimelines;
    
    for (auto& timeline : timelines_) {
        if (timeline && timeline->IsActive()) {
            timeline->Update(deltaTime);
            
            // 收集已完成的动画
            if (!timeline->IsActive()) {
                completedTimelines.push_back(timeline.get());
            }
        }
    }
    
    // 移除已完成的非重复动画
    for (auto* completed : completedTimelines) {
        auto repeat = completed->GetRepeatBehavior();
        if (!repeat.forever && repeat.count <= 1.0) {
            UnregisterTimeline(completed);
        }
    }
    
    // 如果没有活动动画，停止时钟
    if (timelines_.empty()) {
        Stop();
    }
}

} // namespace fk::animation
