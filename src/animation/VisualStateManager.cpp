#include "fk/animation/VisualStateManager.h"
#include <iostream>

namespace fk::animation {

// 静态成员初始化
std::unordered_map<binding::DependencyObject*, std::shared_ptr<VisualStateManager>> VisualStateManager::managerMap_;
std::mutex VisualStateManager::managerMapMutex_;

VisualStateManager* VisualStateManager::GetVisualStateManager(binding::DependencyObject* obj) {
    if (!obj) return nullptr;
    
    std::lock_guard<std::mutex> lock(managerMapMutex_);
    auto it = managerMap_.find(obj);
    return (it != managerMap_.end()) ? it->second.get() : nullptr;
}

void VisualStateManager::SetVisualStateManager(binding::DependencyObject* obj, 
                                                std::shared_ptr<VisualStateManager> manager) {
    if (!obj) return;
    
    std::lock_guard<std::mutex> lock(managerMapMutex_);
    if (manager) {
        managerMap_[obj] = manager;
    } else {
        managerMap_.erase(obj);
    }
}

bool VisualStateManager::GoToState(binding::DependencyObject* obj, 
                                   const std::string& stateName, 
                                   bool useTransitions) {
    if (!obj || stateName.empty()) return false;
    
    std::cout << "[VSM::GoToState] stateName=" << stateName << std::endl;
    
    // 获取对象的 VisualStateManager
    auto manager = GetVisualStateManager(obj);
    
    if (!manager) {
        std::cout << "[VSM::GoToState] No manager found, creating new one" << std::endl;
        // 如果没有管理器，创建一个默认的
        auto newManager = std::make_shared<VisualStateManager>();
        SetVisualStateManager(obj, newManager);
        manager = newManager.get();
    }
    
    // 查找包含目标状态的组
    auto group = manager->FindGroupContainingState(stateName);
    if (!group) {
        std::cout << "[VSM::GoToState] No group found for state: " << stateName << std::endl;
        return false;
    }
    
    std::cout << "[VSM::GoToState] Found group: " << group->GetName() << std::endl;
    
    // 查找目标状态
    auto toState = group->FindState(stateName);
    if (!toState) {
        std::cout << "[VSM::GoToState] State not found in group: " << stateName << std::endl;
        return false;
    }
    
    std::cout << "[VSM::GoToState] Found state, calling GoToStateCore" << std::endl;
    
    // 执行状态转换
    return manager->GoToStateCore(obj, group, toState, useTransitions);
}

bool VisualStateManager::GoToStateCore(binding::DependencyObject* obj,
                                       VisualStateGroup* group,
                                       VisualState* state,
                                       bool useTransitions) {
    if (!obj || !group || !state) return false;
    
    auto currentState = group->GetCurrentState();
    
    std::cout << "[VSM::GoToStateCore] currentState=" 
              << (currentState ? currentState->GetName() : "null")
              << " -> targetState=" << state->GetName() << std::endl;
    
    // 如果已经在目标状态，不需要转换
    if (currentState == state) {
        std::cout << "[VSM::GoToStateCore] Already in target state, skipping" << std::endl;
        return true;
    }
    
    // 触发状态改变前事件
    CurrentStateChanging(group, currentState, state);
    
    // 停止当前状态的动画
    StopCurrentStoryboards(group);
    
    // 应用状态转换
    if (useTransitions && currentState) {
        std::string fromStateName = currentState->GetName();
        std::string toStateName = state->GetName();
        
        auto transition = group->FindBestTransition(fromStateName, toStateName);
        if (transition) {
            ApplyTransition(obj, group, currentState, state, transition);
        }
    }
    
    // 更新当前状态
    group->SetCurrentState(state);
    
    // 开始新状态的动画
    std::cout << "[VSM::GoToStateCore] Calling StartStateStoryboard for: " << state->GetName() << std::endl;
    StartStateStoryboard(state, useTransitions);
    
    // 触发状态改变后事件
    CurrentStateChanged(group, currentState, state);
    
    return true;
}

} // namespace fk::animation
