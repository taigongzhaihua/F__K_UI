#pragma once

#include "fk/animation/VisualStateGroup.h"
#include "fk/binding/DependencyObject.h"
#include "fk/core/Event.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>

namespace fk::animation {

// VisualStateManager - 视觉状态管理器
class VisualStateManager {
public:
    // 获取指定对象的 VisualStateManager
    static VisualStateManager* GetVisualStateManager(binding::DependencyObject* obj);
    
    // 设置指定对象的 VisualStateManager
    static void SetVisualStateManager(binding::DependencyObject* obj, std::shared_ptr<VisualStateManager> manager);
    
    // 状态转换（静态方法）
    static bool GoToState(binding::DependencyObject* obj, const std::string& stateName, bool useTransitions);
    
    // 状态转换（虚方法，可由子类重写）
    virtual bool GoToStateCore(binding::DependencyObject* obj, 
                               VisualStateGroup* group,
                               VisualState* state,
                               bool useTransitions);
    
    // 状态组管理
    void AddStateGroup(std::shared_ptr<VisualStateGroup> group) {
        if (group) {
            stateGroups_.push_back(group);
        }
    }
    
    void RemoveStateGroup(const std::string& groupName) {
        stateGroups_.erase(
            std::remove_if(stateGroups_.begin(), stateGroups_.end(),
                [&groupName](const std::shared_ptr<VisualStateGroup>& group) {
                    return group && group->GetName() == groupName;
                }),
            stateGroups_.end()
        );
    }
    
    VisualStateGroup* FindStateGroup(const std::string& groupName) const {
        auto it = std::find_if(stateGroups_.begin(), stateGroups_.end(),
            [&groupName](const std::shared_ptr<VisualStateGroup>& group) {
                return group && group->GetName() == groupName;
            });
        return (it != stateGroups_.end()) ? it->get() : nullptr;
    }
    
    const std::vector<std::shared_ptr<VisualStateGroup>>& GetStateGroups() const {
        return stateGroups_;
    }
    
    // 事件
    core::Event<VisualStateGroup*, VisualState*, VisualState*> CurrentStateChanging;  // 状态改变前
    core::Event<VisualStateGroup*, VisualState*, VisualState*> CurrentStateChanged;   // 状态改变后
    
public:
    VisualStateManager() = default;
    virtual ~VisualStateManager() = default;

protected:
    
    // 查找包含指定状态的组
    VisualStateGroup* FindGroupContainingState(const std::string& stateName) const {
        for (const auto& group : stateGroups_) {
            if (group && group->FindState(stateName)) {
                return group.get();
            }
        }
        return nullptr;
    }
    
    // 停止当前状态的故事板
    void StopCurrentStoryboards(VisualStateGroup* group) {
        if (!group) return;
        
        auto currentState = group->GetCurrentState();
        if (currentState && currentState->GetStoryboard()) {
            currentState->GetStoryboard()->Stop();
        }
    }
    
    // 开始新状态的故事板
    void StartStateStoryboard(VisualState* state, bool useTransitions) {
        if (!state || !state->GetStoryboard()) return;
        
        state->GetStoryboard()->Begin();
    }
    
    // 重新应用所有活动状态（用于处理跨状态组冲突）
    // 按状态组定义顺序重新应用，后定义的状态组会覆盖前面的
    void ReapplyAllActiveStates() {
        // 停止所有当前动画
        for (const auto& group : stateGroups_) {
            if (group) {
                StopCurrentStoryboards(group.get());
            }
        }
        
        // 按顺序重新启动所有活动状态的动画
        for (const auto& group : stateGroups_) {
            if (group && group->GetCurrentState()) {
                StartStateStoryboard(group->GetCurrentState(), false);
            }
        }
    }
    
    // 应用状态转换
    void ApplyTransition(binding::DependencyObject* obj,
                        VisualStateGroup* group,
                        VisualState* fromState,
                        VisualState* toState,
                        VisualTransition* transition) {
        if (!transition) return;
        
        // 如果有自定义故事板，使用它
        if (transition->GetStoryboard()) {
            transition->GetStoryboard()->Begin();
        }
        // 否则使用自动生成的转换动画
        else {
            // TODO: 实现自动生成转换动画
            // 根据 GeneratedDuration 和 GeneratedEasingFunction 创建平滑过渡
        }
    }

private:
    std::vector<std::shared_ptr<VisualStateGroup>> stateGroups_;
    
    // 静态映射：对象 -> VisualStateManager
    static std::unordered_map<binding::DependencyObject*, std::shared_ptr<VisualStateManager>> managerMap_;
    static std::mutex managerMapMutex_;
};

} // namespace fk::animation
