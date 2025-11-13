#include "fk/animation/Storyboard.h"
#include <algorithm>

namespace fk::animation {

// 静态成员初始化
std::unordered_map<Timeline*, binding::DependencyObject*> Storyboard::targetMap_;
std::unordered_map<Timeline*, std::string> Storyboard::targetPropertyMap_;

Storyboard::Storyboard() {
}

void Storyboard::AddChild(std::shared_ptr<Timeline> child) {
    if (!child) return;
    
    // 检查是否已存在
    auto it = std::find_if(children_.begin(), children_.end(),
        [&child](const std::shared_ptr<Timeline>& t) {
            return t.get() == child.get();
        });
    
    if (it == children_.end()) {
        children_.push_back(child);
    }
}

void Storyboard::RemoveChild(Timeline* child) {
    if (!child) return;
    
    children_.erase(
        std::remove_if(children_.begin(), children_.end(),
            [child](const std::shared_ptr<Timeline>& t) {
                return t.get() == child;
            }),
        children_.end()
    );
}

void Storyboard::ClearChildren() {
    children_.clear();
}

void Storyboard::Begin() {
    Timeline::Begin();
    
    // 启动所有子动画
    for (auto& child : children_) {
        if (child) {
            child->Begin();
        }
    }
}

void Storyboard::Stop() {
    Timeline::Stop();
    
    // 停止所有子动画
    for (auto& child : children_) {
        if (child) {
            child->Stop();
        }
    }
}

void Storyboard::Pause() {
    Timeline::Pause();
    
    // 暂停所有子动画
    for (auto& child : children_) {
        if (child && child->IsActive()) {
            child->Pause();
        }
    }
}

void Storyboard::Resume() {
    Timeline::Resume();
    
    // 恢复所有子动画
    for (auto& child : children_) {
        if (child && child->IsPaused()) {
            child->Resume();
        }
    }
}

void Storyboard::Seek(std::chrono::milliseconds offset) {
    Timeline::Seek(offset);
    
    // 移动所有子动画到指定时间
    for (auto& child : children_) {
        if (child) {
            child->Seek(offset);
        }
    }
}

Duration Storyboard::GetNaturalDuration() const {
    // 返回所有子动画中最长的持续时间
    std::chrono::milliseconds maxDuration(0);
    
    for (const auto& child : children_) {
        if (child) {
            Duration childDuration = child->GetDuration();
            if (childDuration.HasTimeSpan()) {
                auto childEnd = child->GetBeginTime() + childDuration.timeSpan;
                if (childEnd > maxDuration) {
                    maxDuration = childEnd;
                }
            }
        }
    }
    
    return Duration(maxDuration);
}

void Storyboard::UpdateCurrentValue(double progress) {
    // Storyboard 本身不更新值，由子动画负责
    // 这里可以添加整体进度的监控逻辑
}

// 附加属性实现
void Storyboard::SetTarget(Timeline* timeline, binding::DependencyObject* target) {
    if (timeline) {
        targetMap_[timeline] = target;
    }
}

binding::DependencyObject* Storyboard::GetTarget(Timeline* timeline) {
    if (timeline && targetMap_.count(timeline)) {
        return targetMap_[timeline];
    }
    return nullptr;
}

void Storyboard::SetTargetProperty(Timeline* timeline, const std::string& propertyPath) {
    if (timeline) {
        targetPropertyMap_[timeline] = propertyPath;
    }
}

std::string Storyboard::GetTargetProperty(Timeline* timeline) {
    if (timeline && targetPropertyMap_.count(timeline)) {
        return targetPropertyMap_[timeline];
    }
    return "";
}

} // namespace fk::animation
