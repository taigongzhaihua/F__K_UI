#include "fk/animation/VisualStateBuilder.h"
#include "fk/ui/DrawCommand.h"
#include <stdexcept>
#include <chrono>

namespace fk::animation {

VisualStateBuilder::VisualStateBuilder(const std::string& groupName) 
    : groupName_(groupName)
    , group_(std::make_shared<VisualStateGroup>(groupName))
{
}

std::shared_ptr<VisualStateBuilder> VisualStateBuilder::CreateGroup(const std::string& groupName) {
    return std::shared_ptr<VisualStateBuilder>(new VisualStateBuilder(groupName));
}

VisualStateBuilder* VisualStateBuilder::State(const std::string& stateName) {
    // 如果有正在构建的状态，先完成它
    FinalizeCurrentState();
    
    // 开始新的状态
    currentStateName_ = stateName;
    currentState_ = std::make_shared<VisualState>(stateName);
    currentStoryboard_ = std::make_shared<Storyboard>();
    
    return this;
}

VisualStateBuilder* VisualStateBuilder::EndState() {
    FinalizeCurrentState();
    return this;
}

VisualStateBuilder* VisualStateBuilder::ColorAnimation(const std::string& targetName,
                                                       const std::string& propertyPath) {
    // 如果有正在构建的动画，先完成它
    FinalizeCurrentAnimation();
    
    if (!currentStoryboard_) {
        throw std::runtime_error("必须先调用State()开始定义状态");
    }
    
    currentAnimationType_ = AnimationType::Color;
    currentTargetName_ = targetName;
    currentPropertyPath_ = propertyPath;
    currentTarget_ = nullptr;  // 使用TargetName方式
    currentProperty_ = nullptr;
    
    // 重置动画参数
    colorFrom_.reset();
    colorTo_.reset();
    durationMs_ = 200;
    
    return this;
}

VisualStateBuilder* VisualStateBuilder::ColorAnimation(binding::DependencyObject* target,
                                                       const binding::DependencyProperty* property) {
    // 如果有正在构建的动画，先完成它
    FinalizeCurrentAnimation();
    
    if (!currentStoryboard_) {
        throw std::runtime_error("必须先调用State()开始定义状态");
    }
    
    currentAnimationType_ = AnimationType::Color;
    currentTarget_ = target;
    currentProperty_ = property;
    currentTargetName_.clear();  // 使用直接对象方式
    currentPropertyPath_.clear();
    
    // 重置动画参数
    colorFrom_.reset();
    colorTo_.reset();
    durationMs_ = 200;
    
    return this;
}

VisualStateBuilder* VisualStateBuilder::DoubleAnimation(const std::string& targetName,
                                                        const std::string& propertyPath) {
    // 如果有正在构建的动画，先完成它
    FinalizeCurrentAnimation();
    
    if (!currentStoryboard_) {
        throw std::runtime_error("必须先调用State()开始定义状态");
    }
    
    currentAnimationType_ = AnimationType::Double;
    currentTargetName_ = targetName;
    currentPropertyPath_ = propertyPath;
    currentTarget_ = nullptr;  // 使用TargetName方式
    currentProperty_ = nullptr;
    
    // 重置动画参数
    doubleFrom_.reset();
    doubleTo_.reset();
    durationMs_ = 200;
    
    return this;
}

VisualStateBuilder* VisualStateBuilder::DoubleAnimation(binding::DependencyObject* target,
                                                        const binding::DependencyProperty* property) {
    // 如果有正在构建的动画，先完成它
    FinalizeCurrentAnimation();
    
    if (!currentStoryboard_) {
        throw std::runtime_error("必须先调用State()开始定义状态");
    }
    
    currentAnimationType_ = AnimationType::Double;
    currentTarget_ = target;
    currentProperty_ = property;
    currentTargetName_.clear();  // 使用直接对象方式
    currentPropertyPath_.clear();
    
    // 重置动画参数
    doubleFrom_.reset();
    doubleTo_.reset();
    durationMs_ = 200;
    
    return this;
}

VisualStateBuilder* VisualStateBuilder::From(const ui::Color& color) {
    if (currentAnimationType_ != AnimationType::Color) {
        throw std::runtime_error("From(Color)只能用于ColorAnimation");
    }
    colorFrom_ = color;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(const ui::Color& color) {
    if (currentAnimationType_ != AnimationType::Color) {
        throw std::runtime_error("To(Color)只能用于ColorAnimation");
    }
    colorTo_ = color;
    return this;
}

VisualStateBuilder* VisualStateBuilder::From(double value) {
    if (currentAnimationType_ != AnimationType::Double) {
        throw std::runtime_error("From(double)只能用于DoubleAnimation");
    }
    doubleFrom_ = value;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(double value) {
    if (currentAnimationType_ != AnimationType::Double) {
        throw std::runtime_error("To(double)只能用于DoubleAnimation");
    }
    doubleTo_ = value;
    return this;
}

VisualStateBuilder* VisualStateBuilder::Duration(int milliseconds) {
    if (currentAnimationType_ == AnimationType::None) {
        throw std::runtime_error("必须先调用ColorAnimation()或DoubleAnimation()");
    }
    durationMs_ = milliseconds;
    return this;
}

VisualStateBuilder* VisualStateBuilder::EndAnimation() {
    FinalizeCurrentAnimation();
    return this;
}

std::shared_ptr<VisualStateGroup> VisualStateBuilder::Build() {
    // 完成任何待处理的状态
    FinalizeCurrentState();
    
    return group_;
}

void VisualStateBuilder::FinalizeCurrentAnimation() {
    if (currentAnimationType_ == AnimationType::None) {
        return;  // 没有待完成的动画
    }
    
    if (!currentStoryboard_) {
        throw std::runtime_error("内部错误：currentStoryboard_为空");
    }
    
    animation::Duration duration{std::chrono::milliseconds(durationMs_)};
    
    if (currentAnimationType_ == AnimationType::Color) {
        auto anim = std::make_shared<animation::ColorAnimation>();
        
        if (colorFrom_.has_value()) {
            anim->SetFrom(colorFrom_.value());
        }
        if (colorTo_.has_value()) {
            anim->SetTo(colorTo_.value());
        }
        
        anim->SetDuration(duration);
        anim->SetTarget(currentTarget_, currentProperty_);
        currentStoryboard_->AddChild(anim);
        
    } else if (currentAnimationType_ == AnimationType::Double) {
        auto anim = std::make_shared<animation::DoubleAnimation>();
        
        if (doubleFrom_.has_value()) {
            anim->SetFrom(doubleFrom_.value());
        }
        if (doubleTo_.has_value()) {
            anim->SetTo(doubleTo_.value());
        }
        
        anim->SetDuration(duration);
        anim->SetTarget(currentTarget_, currentProperty_);
        currentStoryboard_->AddChild(anim);
    }
    
    // 重置动画状态
    currentAnimationType_ = AnimationType::None;
    currentTarget_ = nullptr;
    currentProperty_ = nullptr;
    colorFrom_.reset();
    colorTo_.reset();
    doubleFrom_.reset();
    doubleTo_.reset();
}

void VisualStateBuilder::FinalizeCurrentState() {
    if (!currentState_) {
        return;  // 没有待完成的状态
    }
    
    // 完成任何待处理的动画
    FinalizeCurrentAnimation();
    
    // 将Storyboard设置到状态
    if (currentStoryboard_) {
        currentState_->SetStoryboard(currentStoryboard_);
    }
    
    // 将状态添加到组
    group_->AddState(currentState_);
    
    // 重置状态
    currentStateName_.clear();
    currentState_.reset();
    currentStoryboard_.reset();
}

} // namespace fk::animation
