#pragma once

#include "fk/animation/VisualState.h"
#include "fk/animation/VisualTransition.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace fk::animation {

// VisualStateGroup - 视觉状态组
class VisualStateGroup {
public:
    VisualStateGroup() = default;
    explicit VisualStateGroup(const std::string& name) : name_(name) {}
    ~VisualStateGroup() = default;
    
    // 克隆
    std::shared_ptr<VisualStateGroup> Clone() const {
        auto clone = std::make_shared<VisualStateGroup>(name_);
        for (const auto& state : states_) {
            if (state) {
                clone->AddState(state->Clone());
            }
        }
        for (const auto& transition : transitions_) {
            if (transition) {
                // TODO: 如果需要，添加 VisualTransition 的克隆
                clone->AddTransition(transition);
            }
        }
        return clone;
    }
    
    // 组名称
    std::string GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }
    
    // 当前状态
    VisualState* GetCurrentState() const { return currentState_; }
    
    // 状态集合
    void AddState(std::shared_ptr<VisualState> state) {
        if (state) {
            states_.push_back(state);
        }
    }
    
    void RemoveState(const std::string& stateName) {
        states_.erase(
            std::remove_if(states_.begin(), states_.end(),
                [&stateName](const std::shared_ptr<VisualState>& state) {
                    return state && state->GetName() == stateName;
                }),
            states_.end()
        );
    }
    
    VisualState* FindState(const std::string& stateName) const {
        auto it = std::find_if(states_.begin(), states_.end(),
            [&stateName](const std::shared_ptr<VisualState>& state) {
                return state && state->GetName() == stateName;
            });
        return (it != states_.end()) ? it->get() : nullptr;
    }
    
    const std::vector<std::shared_ptr<VisualState>>& GetStates() const {
        return states_;
    }
    
    // 转换集合
    void AddTransition(std::shared_ptr<VisualTransition> transition) {
        if (transition) {
            transitions_.push_back(transition);
        }
    }
    
    void RemoveTransition(VisualTransition* transition) {
        transitions_.erase(
            std::remove_if(transitions_.begin(), transitions_.end(),
                [transition](const std::shared_ptr<VisualTransition>& t) {
                    return t.get() == transition;
                }),
            transitions_.end()
        );
    }
    
    VisualTransition* FindBestTransition(const std::string& fromState, const std::string& toState) const {
        // 优先级：精确匹配 > 部分匹配 > 默认转换
        VisualTransition* exactMatch = nullptr;
        VisualTransition* partialMatch = nullptr;
        VisualTransition* defaultTransition = nullptr;
        
        for (const auto& transition : transitions_) {
            if (!transition) continue;
            
            if (transition->GetFrom() == fromState && transition->GetTo() == toState) {
                exactMatch = transition.get();
                break;  // 找到精确匹配，立即返回
            } else if (transition->Matches(fromState, toState)) {
                if (transition->IsDefault()) {
                    defaultTransition = transition.get();
                } else {
                    partialMatch = transition.get();
                }
            }
        }
        
        return exactMatch ? exactMatch : (partialMatch ? partialMatch : defaultTransition);
    }
    
    const std::vector<std::shared_ptr<VisualTransition>>& GetTransitions() const {
        return transitions_;
    }
    
    // 设置当前状态（内部使用）
    void SetCurrentState(VisualState* state) {
        currentState_ = state;
    }
    
private:
    std::string name_;
    std::vector<std::shared_ptr<VisualState>> states_;
    std::vector<std::shared_ptr<VisualTransition>> transitions_;
    VisualState* currentState_{nullptr};
};

} // namespace fk::animation
