#include "fk/animation/Storyboard.h"
#include <algorithm>
#include <iostream>

namespace fk::animation {

// 静态成员初始化
std::unordered_map<Timeline*, binding::DependencyObject*> Storyboard::targetMap_;
std::unordered_map<Timeline*, std::string> Storyboard::targetPropertyMap_;
std::unordered_map<Timeline*, std::string> Storyboard::targetNameMap_;
std::unordered_map<Storyboard*, binding::DependencyObject*> Storyboard::templateRootMap_;

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
    
    // 在启动子动画前，先解析所有使用TargetName的动画
    auto templateRoot = GetTemplateRoot(this);
    if (templateRoot) {
        for (auto& child : children_) {
            if (!child) continue;
            
            // 检查是否使用了TargetName
            std::string targetName = GetTargetName(child.get());
            if (!targetName.empty()) {
                // TODO: 实现在模板树中查找命名元素的逻辑
                // 这需要访问UIElement的FindName方法
                // 暂时跳过，因为需要将DependencyObject转换为UIElement
            }
        }
    }
    
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

void Storyboard::SetTargetName(Timeline* timeline, const std::string& targetName) {
    if (timeline) {
        targetNameMap_[timeline] = targetName;
    }
}

std::string Storyboard::GetTargetName(Timeline* timeline) {
    if (timeline && targetNameMap_.count(timeline)) {
        return targetNameMap_[timeline];
    }
    return "";
}

void Storyboard::SetTemplateRoot(Storyboard* storyboard, binding::DependencyObject* root) {
    if (storyboard) {
        templateRootMap_[storyboard] = root;
    }
}

binding::DependencyObject* Storyboard::GetTemplateRoot(Storyboard* storyboard) {
    if (storyboard && templateRootMap_.count(storyboard)) {
        return templateRootMap_[storyboard];
    }
    return nullptr;
}

} // namespace fk::animation
