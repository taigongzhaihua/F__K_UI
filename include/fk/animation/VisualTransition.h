#pragma once

#include "fk/animation/Timeline.h"
#include "fk/animation/EasingFunction.h"
#include <string>
#include <memory>

namespace fk::animation {

// VisualTransition - 状态转换
class VisualTransition {
public:
    VisualTransition() = default;
    
    // 转换源状态（空表示任意状态）
    std::string GetFrom() const { return from_; }
    void SetFrom(const std::string& from) { from_ = from; }
    
    // 转换目标状态（空表示任意状态）
    std::string GetTo() const { return to_; }
    void SetTo(const std::string& to) { to_ = to; }
    
    // 转换持续时间
    Duration GetGeneratedDuration() const { return generatedDuration_; }
    void SetGeneratedDuration(Duration duration) { generatedDuration_ = duration; }
    
    // 缓动函数
    std::shared_ptr<EasingFunctionBase> GetGeneratedEasingFunction() const { return easingFunction_; }
    void SetGeneratedEasingFunction(std::shared_ptr<EasingFunctionBase> easing) { easingFunction_ = easing; }
    
    // 自定义故事板（可选）
    std::shared_ptr<Storyboard> GetStoryboard() const { return storyboard_; }
    void SetStoryboard(std::shared_ptr<Storyboard> storyboard) { storyboard_ = storyboard; }
    
    // 检查此转换是否适用于给定的状态转换
    bool IsDefault() const { return from_.empty() && to_.empty(); }
    bool Matches(const std::string& fromState, const std::string& toState) const {
        bool fromMatches = from_.empty() || from_ == fromState;
        bool toMatches = to_.empty() || to_ == toState;
        return fromMatches && toMatches;
    }
    
private:
    std::string from_;
    std::string to_;
    Duration generatedDuration_{std::chrono::milliseconds(0)};
    std::shared_ptr<EasingFunctionBase> easingFunction_;
    std::shared_ptr<Storyboard> storyboard_;
};

} // namespace fk::animation
