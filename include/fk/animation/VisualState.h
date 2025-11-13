#pragma once

#include "fk/animation/Storyboard.h"
#include <string>
#include <memory>

namespace fk::animation {

// VisualState - 视觉状态
class VisualState {
public:
    VisualState() = default;
    explicit VisualState(const std::string& name) : name_(name) {}
    
    // 状态名称
    std::string GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }
    
    // 故事板（定义状态的视觉表现）
    std::shared_ptr<Storyboard> GetStoryboard() const { return storyboard_; }
    void SetStoryboard(std::shared_ptr<Storyboard> storyboard) { storyboard_ = storyboard; }
    
private:
    std::string name_;
    std::shared_ptr<Storyboard> storyboard_;
};

} // namespace fk::animation
