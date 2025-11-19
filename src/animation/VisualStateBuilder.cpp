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

namespace {
    constexpr const char* kErrorNoState = "必须先调用State()开始定义状态";
    constexpr const char* kErrorNoAnimation = "必须先调用Animation方法";
}

// 辅助函数：初始化动画创建的通用逻辑
void VisualStateBuilder::InitAnimation(AnimationType type, const std::string& targetName, const std::string& propertyPath) {
    FinalizeCurrentAnimation();
    if (!currentStoryboard_) {
        throw std::runtime_error(kErrorNoState);
    }
    currentAnimationType_ = type;
    currentTargetName_ = targetName;
    currentPropertyPath_ = propertyPath;
    currentTarget_ = nullptr;
    currentProperty_ = nullptr;
    ResetAnimationParams();
}

void VisualStateBuilder::InitAnimation(AnimationType type, binding::DependencyObject* target, const binding::DependencyProperty* property) {
    FinalizeCurrentAnimation();
    if (!currentStoryboard_) {
        throw std::runtime_error(kErrorNoState);
    }
    currentAnimationType_ = type;
    currentTarget_ = target;
    currentProperty_ = property;
    currentTargetName_.clear();
    currentPropertyPath_.clear();
    ResetAnimationParams();
}

void VisualStateBuilder::ResetAnimationParams() {
    colorFrom_.reset();
    colorTo_.reset();
    colorToBinding_ = nullptr;
    doubleFrom_.reset();
    doubleTo_.reset();
    pointFrom_.reset();
    pointTo_.reset();
    thicknessFrom_.reset();
    thicknessTo_.reset();
    durationMs_ = 200;
}

VisualStateBuilder* VisualStateBuilder::ColorAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitAnimation(AnimationType::Color, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::ColorAnimation(binding::DependencyObject* target, const binding::DependencyProperty* property) {
    InitAnimation(AnimationType::Color, target, property);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DoubleAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitAnimation(AnimationType::Double, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DoubleAnimation(binding::DependencyObject* target, const binding::DependencyProperty* property) {
    InitAnimation(AnimationType::Double, target, property);
    return this;
}

// 辅助函数：类型检查
inline void VisualStateBuilder::CheckAnimationType(AnimationType expected, const char* methodName) const {
    if (currentAnimationType_ != expected) {
        throw std::runtime_error(std::string(methodName) + "仅适用于对应的Animation类型");
    }
}

VisualStateBuilder* VisualStateBuilder::From(const ui::Color& color) {
    CheckAnimationType(AnimationType::Color, "From(Color)");
    colorFrom_ = color;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(const ui::Color& color) {
    CheckAnimationType(AnimationType::Color, "To(Color)");
    colorTo_ = color;
    colorToBinding_ = nullptr;
    return this;
}

VisualStateBuilder* VisualStateBuilder::ToBinding(const binding::DependencyProperty& property) {
    CheckAnimationType(AnimationType::Color, "ToBinding()");
    colorToBinding_ = &property;
    colorTo_.reset();
    return this;
}

VisualStateBuilder* VisualStateBuilder::From(double value) {
    CheckAnimationType(AnimationType::Double, "From(double)");
    doubleFrom_ = value;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(double value) {
    CheckAnimationType(AnimationType::Double, "To(double)");
    doubleTo_ = value;
    return this;
}

VisualStateBuilder* VisualStateBuilder::PointAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitAnimation(AnimationType::Point, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::PointAnimation(binding::DependencyObject* target, const binding::DependencyProperty* property) {
    InitAnimation(AnimationType::Point, target, property);
    return this;
}

VisualStateBuilder* VisualStateBuilder::ThicknessAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitAnimation(AnimationType::Thickness, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::ThicknessAnimation(binding::DependencyObject* target, const binding::DependencyProperty* property) {
    InitAnimation(AnimationType::Thickness, target, property);
    return this;
}

VisualStateBuilder* VisualStateBuilder::From(const ui::Point& point) {
    CheckAnimationType(AnimationType::Point, "From(Point)");
    pointFrom_ = point;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(const ui::Point& point) {
    CheckAnimationType(AnimationType::Point, "To(Point)");
    pointTo_ = point;
    return this;
}

VisualStateBuilder* VisualStateBuilder::From(const Thickness& thickness) {
    CheckAnimationType(AnimationType::Thickness, "From(Thickness)");
    thicknessFrom_ = thickness;
    return this;
}

VisualStateBuilder* VisualStateBuilder::To(const Thickness& thickness) {
    CheckAnimationType(AnimationType::Thickness, "To(Thickness)");
    thicknessTo_ = thickness;
    return this;
}

VisualStateBuilder* VisualStateBuilder::Duration(int milliseconds) {
    if (currentAnimationType_ == AnimationType::None) {
        throw std::runtime_error(kErrorNoAnimation);
    }
    durationMs_ = milliseconds;
    return this;
}

// 辅助函数：初始化KeyFrame动画
void VisualStateBuilder::InitKeyFrameAnimation(AnimationType type, const std::string& targetName, const std::string& propertyPath) {
    FinalizeCurrentAnimation();
    if (!currentStoryboard_) {
        throw std::runtime_error(kErrorNoState);
    }
    currentAnimationType_ = type;
    currentTargetName_ = targetName;
    currentPropertyPath_ = propertyPath;
    ResetAnimationParams();
    
    // 清空KeyFrame集合(ResetAnimationParams已清空普通动画参数)
    colorKeyFrames_.clear();
    doubleKeyFrames_.clear();
    pointKeyFrames_.clear();
    thicknessKeyFrames_.clear();
    durationMs_ = 0;  // KeyFrame动画的duration由最后一个关键帧决定
}

// KeyFrame动画创建方法
VisualStateBuilder* VisualStateBuilder::ColorKeyFrameAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitKeyFrameAnimation(AnimationType::ColorKeyFrame, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DoubleKeyFrameAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitKeyFrameAnimation(AnimationType::DoubleKeyFrame, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::PointKeyFrameAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitKeyFrameAnimation(AnimationType::PointKeyFrame, targetName, propertyPath);
    return this;
}

VisualStateBuilder* VisualStateBuilder::ThicknessKeyFrameAnimation(const std::string& targetName, const std::string& propertyPath) {
    InitKeyFrameAnimation(AnimationType::ThicknessKeyFrame, targetName, propertyPath);
    return this;
}

// 辅助模板函数：添加关键帧的通用逻辑
template<typename T, typename KeyFrameType>
void VisualStateBuilder::AddKeyFrameImpl(AnimationType expectedType, const char* typeName, 
                                         std::vector<std::shared_ptr<KeyFrame<T>>>& keyFrames,
                                         int keyTimeMs, const T& value) {
    if (currentAnimationType_ != expectedType) {
        throw std::runtime_error(std::string("只能为") + typeName + "添加对应类型的关键帧");
    }
    auto keyFrame = std::make_shared<KeyFrameType>();
    keyFrame->SetKeyTime(animation::KeyTime::FromTimeSpan(std::chrono::milliseconds(keyTimeMs)));
    keyFrame->SetValue(value);
    keyFrames.push_back(keyFrame);
    if (keyTimeMs > durationMs_) {
        durationMs_ = keyTimeMs;
    }
}

// 线性关键帧
VisualStateBuilder* VisualStateBuilder::LinearKeyFrame(int keyTimeMs, const ui::Color& value) {
    AddKeyFrameImpl<ui::Color, animation::LinearKeyFrame<ui::Color>>(
        AnimationType::ColorKeyFrame, "ColorKeyFrameAnimation", colorKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::LinearKeyFrame(int keyTimeMs, double value) {
    AddKeyFrameImpl<double, animation::LinearKeyFrame<double>>(
        AnimationType::DoubleKeyFrame, "DoubleKeyFrameAnimation", doubleKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::LinearKeyFrame(int keyTimeMs, const ui::Point& value) {
    AddKeyFrameImpl<ui::Point, animation::LinearKeyFrame<ui::Point>>(
        AnimationType::PointKeyFrame, "PointKeyFrameAnimation", pointKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::LinearKeyFrame(int keyTimeMs, const Thickness& value) {
    AddKeyFrameImpl<Thickness, animation::LinearKeyFrame<Thickness>>(
        AnimationType::ThicknessKeyFrame, "ThicknessKeyFrameAnimation", thicknessKeyFrames_, keyTimeMs, value);
    return this;
}

// 离散关键帧
VisualStateBuilder* VisualStateBuilder::DiscreteKeyFrame(int keyTimeMs, const ui::Color& value) {
    AddKeyFrameImpl<ui::Color, animation::DiscreteKeyFrame<ui::Color>>(
        AnimationType::ColorKeyFrame, "ColorKeyFrameAnimation", colorKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DiscreteKeyFrame(int keyTimeMs, double value) {
    AddKeyFrameImpl<double, animation::DiscreteKeyFrame<double>>(
        AnimationType::DoubleKeyFrame, "DoubleKeyFrameAnimation", doubleKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DiscreteKeyFrame(int keyTimeMs, const ui::Point& value) {
    AddKeyFrameImpl<ui::Point, animation::DiscreteKeyFrame<ui::Point>>(
        AnimationType::PointKeyFrame, "PointKeyFrameAnimation", pointKeyFrames_, keyTimeMs, value);
    return this;
}

VisualStateBuilder* VisualStateBuilder::DiscreteKeyFrame(int keyTimeMs, const Thickness& value) {
    AddKeyFrameImpl<Thickness, animation::DiscreteKeyFrame<Thickness>>(
        AnimationType::ThicknessKeyFrame, "ThicknessKeyFrameAnimation", thicknessKeyFrames_, keyTimeMs, value);
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
    
    // 通用的target设置逻辑
    auto setAnimationTarget = [this](auto anim) {
        if (!currentTargetName_.empty()) {
            animation::Storyboard::SetTargetName(anim.get(), currentTargetName_);
            animation::Storyboard::SetTargetProperty(anim.get(), currentPropertyPath_);
        } else {
            anim->SetTarget(currentTarget_, currentProperty_);
        }
    };
    
    // 通用的动画创建和配置函数
    auto createAnimation = [this, &duration, &setAnimationTarget](auto anim, const auto& from, const auto& to) {
        if (from.has_value()) anim->SetFrom(from.value());
        if (to.has_value()) anim->SetTo(to.value());
        anim->SetDuration(duration);
        setAnimationTarget(anim);
        currentStoryboard_->AddChild(anim);
    };
    
    // KeyFrame动画的通用配置和添加函数
    auto createKeyFrameAnimation = [this, &duration, &setAnimationTarget](auto anim, auto& keyFrames) {
        for (auto& keyFrame : keyFrames) {
            anim->KeyFrames().Add(keyFrame);
        }
        anim->SetDuration(duration);
        setAnimationTarget(anim);
        currentStoryboard_->AddChild(anim);
        keyFrames.clear();
    };
    
    switch (currentAnimationType_) {
        case AnimationType::Color: {
            auto anim = std::make_shared<animation::ColorAnimation>();
            if (colorToBinding_) {
                anim->SetToBinding(colorToBinding_);
            }
            createAnimation(anim, colorFrom_, colorTo_);
            break;
        }
        case AnimationType::Double:
            createAnimation(std::make_shared<animation::DoubleAnimation>(), doubleFrom_, doubleTo_);
            break;
            
        case AnimationType::Point:
            createAnimation(std::make_shared<animation::PointAnimation>(), pointFrom_, pointTo_);
            break;
            
        case AnimationType::Thickness:
            createAnimation(std::make_shared<animation::ThicknessAnimation>(), thicknessFrom_, thicknessTo_);
            break;
            
        case AnimationType::ColorKeyFrame:
            createKeyFrameAnimation(std::make_shared<animation::ColorAnimationUsingKeyFrames>(), colorKeyFrames_);
            break;
        
        case AnimationType::DoubleKeyFrame:
            createKeyFrameAnimation(std::make_shared<animation::DoubleAnimationUsingKeyFrames>(), doubleKeyFrames_);
            break;
        
        case AnimationType::PointKeyFrame:
            createKeyFrameAnimation(std::make_shared<animation::PointAnimationUsingKeyFrames>(), pointKeyFrames_);
            break;
        
        case AnimationType::ThicknessKeyFrame:
            createKeyFrameAnimation(std::make_shared<animation::ThicknessAnimationUsingKeyFrames>(), thicknessKeyFrames_);
            break;
            
        default:
            break;
    }
    
    // 重置动画状态
    currentAnimationType_ = AnimationType::None;
    currentTarget_ = nullptr;
    currentProperty_ = nullptr;
    currentTargetName_.clear();
    currentPropertyPath_.clear();
    ResetAnimationParams();
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
