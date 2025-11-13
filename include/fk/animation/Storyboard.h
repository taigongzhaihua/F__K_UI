#pragma once

#include "fk/animation/Timeline.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace fk::animation {

// Storyboard - 故事板，管理一组动画的容器
class Storyboard : public Timeline {
public:
    Storyboard();
    ~Storyboard() override = default;

    // 添加子动画
    void AddChild(std::shared_ptr<Timeline> child);
    void RemoveChild(Timeline* child);
    void ClearChildren();
    
    // 获取子动画列表
    const std::vector<std::shared_ptr<Timeline>>& GetChildren() const { return children_; }
    
    // 控制所有子动画
    void Begin() override;
    void Stop() override;
    void Pause() override;
    void Resume() override;
    void Seek(std::chrono::milliseconds offset) override;

    // 附加属性：设置动画目标
    static void SetTarget(Timeline* timeline, binding::DependencyObject* target);
    static binding::DependencyObject* GetTarget(Timeline* timeline);
    
    static void SetTargetProperty(Timeline* timeline, const std::string& propertyPath);
    static std::string GetTargetProperty(Timeline* timeline);

protected:
    Duration GetNaturalDuration() const override;
    void UpdateCurrentValue(double progress) override;

private:
    std::vector<std::shared_ptr<Timeline>> children_;
    
    // 存储附加属性的静态映射
    static std::unordered_map<Timeline*, binding::DependencyObject*> targetMap_;
    static std::unordered_map<Timeline*, std::string> targetPropertyMap_;
};

} // namespace fk::animation
