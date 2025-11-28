#include "fk/animation/AnimationManager.h"
#include <algorithm>

namespace fk::animation {

AnimationManager& AnimationManager::Instance() {
    static AnimationManager instance;
    return instance;
}

void AnimationManager::RegisterAnimation(Timeline* timeline) {
    if (!timeline) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 避免重复注册
    auto it = std::find(activeTimelines_.begin(), activeTimelines_.end(), timeline);
    if (it == activeTimelines_.end()) {
        activeTimelines_.push_back(timeline);
    }
    
    // 初始化时间戳
    if (!initialized_) {
        lastUpdateTime_ = std::chrono::steady_clock::now();
        initialized_ = true;
    }
}

void AnimationManager::UnregisterAnimation(Timeline* timeline) {
    if (!timeline) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find(activeTimelines_.begin(), activeTimelines_.end(), timeline);
    if (it != activeTimelines_.end()) {
        activeTimelines_.erase(it);
    }
}

void AnimationManager::Update(std::chrono::milliseconds deltaTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (activeTimelines_.empty()) {
        return;
    }
    
    // 更新所有活动动画
    std::vector<Timeline*> toRemove;
    
    for (auto* timeline : activeTimelines_) {
        if (!timeline) {
            toRemove.push_back(timeline);
            continue;
        }
        
        if (timeline->IsActive() && !timeline->IsPaused()) {
            timeline->Update(deltaTime);
        }
        
        // 如果动画已完成，标记为待移除
        if (!timeline->IsActive()) {
            toRemove.push_back(timeline);
        }
    }
    
    // 移除已完成的动画
    for (auto* timeline : toRemove) {
        auto it = std::find(activeTimelines_.begin(), activeTimelines_.end(), timeline);
        if (it != activeTimelines_.end()) {
            activeTimelines_.erase(it);
        }
    }
}

void AnimationManager::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    activeTimelines_.clear();
    initialized_ = false;
}

} // namespace fk::animation
