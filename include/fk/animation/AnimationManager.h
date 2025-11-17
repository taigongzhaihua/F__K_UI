#pragma once

#include "fk/animation/Timeline.h"
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>

namespace fk::animation {

/**
 * @brief 全局动画管理器（单例）
 * 
 * 负责驱动所有活动的动画，在每帧更新动画状态
 */
class AnimationManager {
public:
    static AnimationManager& Instance();
    
    // 注册/取消注册动画
    void RegisterAnimation(Timeline* timeline);
    void UnregisterAnimation(Timeline* timeline);
    
    // 每帧更新所有动画
    void Update(std::chrono::milliseconds deltaTime);
    
    // 清除所有动画
    void Clear();

private:
    AnimationManager() = default;
    ~AnimationManager() = default;
    
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
    
    std::vector<Timeline*> activeTimelines_;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point lastUpdateTime_;
    bool initialized_{false};
};

} // namespace fk::animation
