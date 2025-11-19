#pragma once

#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/DoubleAnimation.h"
#include "fk/animation/PointAnimation.h"
#include "fk/animation/ThicknessAnimation.h"
#include "fk/animation/ColorAnimationUsingKeyFrames.h"
#include "fk/animation/DoubleAnimationUsingKeyFrames.h"
#include "fk/animation/PointAnimationUsingKeyFrames.h"
#include "fk/animation/ThicknessAnimationUsingKeyFrames.h"
#include "fk/animation/KeyFrame.h"
#include "fk/binding/DependencyProperty.h"
#include <memory>
#include <functional>

namespace fk::animation {

/**
 * @brief 视觉状态构建器 - 提供声明式API来创建视觉状态
 * 
 * 这个类模拟WPF/WinUI中在XAML中定义视觉状态的方式，
 * 但使用C++的链式调用API实现。
 * 
 * 示例用法：
 * @code
 * auto group = VisualStateBuilder::CreateGroup("CommonStates")
 *     ->State("Normal")
 *         ->ColorAnimation(border, &Border::BackgroundProperty())
 *             ->To(Color::FromRGB(240, 240, 240))
 *             ->Duration(200)
 *         ->EndAnimation()
 *     ->EndState()
 *     ->State("MouseOver")
 *         ->ColorAnimation(border, &Border::BackgroundProperty())
 *             ->To(Color::FromRGB(229, 241, 251))
 *             ->Duration(150)
 *         ->EndAnimation()
 *     ->EndState()
 *     ->Build();
 * @endcode
 */
class VisualStateBuilder {
public:
    /**
     * @brief 创建一个新的视觉状态组构建器
     * 
     * @param groupName 状态组名称（如"CommonStates"）
     * @return 构建器实例
     */
    static std::shared_ptr<VisualStateBuilder> CreateGroup(const std::string& groupName);
    
    /**
     * @brief 开始定义一个新的视觉状态
     * 
     * @param stateName 状态名称（如"Normal", "MouseOver"等）
     * @return this（支持链式调用）
     */
    VisualStateBuilder* State(const std::string& stateName);
    
    /**
     * @brief 结束当前状态的定义
     * 
     * @return this（支持链式调用）
     */
    VisualStateBuilder* EndState();
    
    /**
     * @brief 为当前状态添加颜色动画（使用TargetName引用模板中的元素）
     * 
     * @param targetName 模板中元素的名称（通过x:Name或SetName设置）
     * @param propertyPath 属性路径（如"Background.Color"）
     * @return 动画构建器（支持配置动画参数）
     */
    VisualStateBuilder* ColorAnimation(const std::string& targetName, 
                                       const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加颜色动画（直接指定目标对象）
     * 
     * @param target 动画目标对象
     * @param property 目标属性
     * @return 动画构建器（支持配置动画参数）
     */
    VisualStateBuilder* ColorAnimation(binding::DependencyObject* target, 
                                       const binding::DependencyProperty* property);
    
    /**
     * @brief 为当前状态添加双精度浮点数动画（使用TargetName）
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径（如"Opacity"）
     * @return 动画构建器（支持配置动画参数）
     */
    VisualStateBuilder* DoubleAnimation(const std::string& targetName,
                                        const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加双精度浮点数动画（直接指定目标对象）
     * 
     * @param target 动画目标对象
     * @param property 目标属性
     * @return 动画构建器（支持配置动画参数）
     */
    VisualStateBuilder* DoubleAnimation(binding::DependencyObject* target,
                                        const binding::DependencyProperty* property);
    
    /**
     * @brief 设置当前动画的起始值（颜色）
     * 
     * @param color 起始颜色
     * @return this（支持链式调用）
     */
    VisualStateBuilder* From(const ui::Color& color);
    
    /**
     * @brief 设置当前动画的目标值（颜色）
     * 
     * @param color 目标颜色
     * @return this（支持链式调用）
     */
    VisualStateBuilder* To(const ui::Color& color);
    
    /**
     * @brief 设置当前动画的目标值（从属性绑定）
     * 
     * 类似 WPF 的 {TemplateBinding}，从 TemplatedParent 的属性动态获取颜色值
     * 
     * @param property TemplatedParent 的属性（如 Button::MouseOverBackgroundProperty()）
     * @return this（支持链式调用）
     * 
     * 示例：
     * ```cpp
     * ->State("MouseOver")
     *     ->ColorAnimation("RootBorder", "Background.Color")
     *         ->ToBinding(Button::MouseOverBackgroundProperty())  // 从属性绑定
     *         ->Duration(150)
     * ```
     */
    VisualStateBuilder* ToBinding(const binding::DependencyProperty& property);
    
    /**
     * @brief 设置当前动画的起始值（双精度）
     * 
     * @param value 起始值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* From(double value);
    
    /**
     * @brief 设置当前动画的目标值（双精度）
     * 
     * @param value 目标值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* To(double value);
    
    /**
     * @brief 为当前状态添加点位置动画（使用TargetName）
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径
     * @return 动画构建器
     */
    VisualStateBuilder* PointAnimation(const std::string& targetName,
                                       const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加点位置动画（直接指定目标对象）
     * 
     * @param target 动画目标对象
     * @param property 目标属性
     * @return 动画构建器
     */
    VisualStateBuilder* PointAnimation(binding::DependencyObject* target,
                                       const binding::DependencyProperty* property);
    
    /**
     * @brief 为当前状态添加厚度动画（使用TargetName）
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径（如"Margin", "Padding"）
     * @return 动画构建器
     */
    VisualStateBuilder* ThicknessAnimation(const std::string& targetName,
                                           const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加厚度动画（直接指定目标对象）
     * 
     * @param target 动画目标对象
     * @param property 目标属性
     * @return 动画构建器
     */
    VisualStateBuilder* ThicknessAnimation(binding::DependencyObject* target,
                                           const binding::DependencyProperty* property);
    
    /**
     * @brief 设置当前动画的起始值（点）
     * 
     * @param point 起始点
     * @return this（支持链式调用）
     */
    VisualStateBuilder* From(const ui::Point& point);
    
    /**
     * @brief 设置当前动画的目标值（点）
     * 
     * @param point 目标点
     * @return this（支持链式调用）
     */
    VisualStateBuilder* To(const ui::Point& point);
    
    /**
     * @brief 设置当前动画的起始值（厚度）
     * 
     * @param thickness 起始厚度
     * @return this（支持链式调用）
     */
    VisualStateBuilder* From(const Thickness& thickness);
    
    /**
     * @brief 设置当前动画的目标值（厚度）
     * 
     * @param thickness 目标厚度
     * @return this（支持链式调用）
     */
    VisualStateBuilder* To(const Thickness& thickness);
    
    /**
     * @brief 设置当前动画的持续时间（毫秒）
     * 
     * @param milliseconds 持续时间
     * @return this（支持链式调用）
     */
    VisualStateBuilder* Duration(int milliseconds);
    
    /**
     * @brief 为当前状态添加颜色关键帧动画
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径
     * @return 动画构建器
     */
    VisualStateBuilder* ColorKeyFrameAnimation(const std::string& targetName,
                                               const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加双精度关键帧动画
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径
     * @return 动画构建器
     */
    VisualStateBuilder* DoubleKeyFrameAnimation(const std::string& targetName,
                                                const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加点位置关键帧动画
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径
     * @return 动画构建器
     */
    VisualStateBuilder* PointKeyFrameAnimation(const std::string& targetName,
                                               const std::string& propertyPath);
    
    /**
     * @brief 为当前状态添加厚度关键帧动画
     * 
     * @param targetName 模板中元素的名称
     * @param propertyPath 属性路径
     * @return 动画构建器
     */
    VisualStateBuilder* ThicknessKeyFrameAnimation(const std::string& targetName,
                                                   const std::string& propertyPath);
    
    /**
     * @brief 添加线性关键帧（颜色）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* LinearKeyFrame(int keyTimeMs, const ui::Color& value);
    
    /**
     * @brief 添加线性关键帧（双精度）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* LinearKeyFrame(int keyTimeMs, double value);
    
    /**
     * @brief 添加线性关键帧（点）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* LinearKeyFrame(int keyTimeMs, const ui::Point& value);
    
    /**
     * @brief 添加线性关键帧（厚度）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* LinearKeyFrame(int keyTimeMs, const Thickness& value);
    
    /**
     * @brief 添加离散关键帧（颜色）- 瞬间跳变
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* DiscreteKeyFrame(int keyTimeMs, const ui::Color& value);
    
    /**
     * @brief 添加离散关键帧（双精度）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* DiscreteKeyFrame(int keyTimeMs, double value);
    
    /**
     * @brief 添加离散关键帧（点）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* DiscreteKeyFrame(int keyTimeMs, const ui::Point& value);
    
    /**
     * @brief 添加离散关键帧（厚度）
     * 
     * @param keyTime 关键帧时间（毫秒）
     * @param value 关键帧值
     * @return this（支持链式调用）
     */
    VisualStateBuilder* DiscreteKeyFrame(int keyTimeMs, const Thickness& value);
    
    /**
     * @brief 结束当前动画的定义
     * 
     * @return this（支持链式调用）
     */
    VisualStateBuilder* EndAnimation();
    
    /**
     * @brief 完成构建并返回视觉状态组
     * 
     * @return 构建好的视觉状态组
     */
    std::shared_ptr<VisualStateGroup> Build();

private:
    VisualStateBuilder(const std::string& groupName);
    
    std::string groupName_;
    std::shared_ptr<VisualStateGroup> group_;
    
    // 当前正在构建的状态
    std::string currentStateName_;
    std::shared_ptr<VisualState> currentState_;
    std::shared_ptr<Storyboard> currentStoryboard_;
    
    // 当前正在构建的动画
    enum class AnimationType { 
        None, 
        Color, Double, Point, Thickness,
        ColorKeyFrame, DoubleKeyFrame, PointKeyFrame, ThicknessKeyFrame
    };
    AnimationType currentAnimationType_{AnimationType::None};
    
    // 动画目标（两种方式：直接对象 或 TargetName）
    binding::DependencyObject* currentTarget_{nullptr};
    const binding::DependencyProperty* currentProperty_{nullptr};
    std::string currentTargetName_;      // 使用TargetName方式时的目标名称
    std::string currentPropertyPath_;    // 使用TargetName方式时的属性路径
    
    // 颜色动画参数
    std::optional<ui::Color> colorFrom_;
    std::optional<ui::Color> colorTo_;
    const binding::DependencyProperty* colorToBinding_{nullptr};  // To 值绑定到属性
    
    // 双精度动画参数
    std::optional<double> doubleFrom_;
    std::optional<double> doubleTo_;
    
    // 点动画参数
    std::optional<ui::Point> pointFrom_;
    std::optional<ui::Point> pointTo_;
    
    // 厚度动画参数
    std::optional<Thickness> thicknessFrom_;
    std::optional<Thickness> thicknessTo_;
    
    // 动画持续时间
    int durationMs_{200};  // 默认200ms
    
    // KeyFrame动画的关键帧集合
    std::vector<std::shared_ptr<KeyFrame<ui::Color>>> colorKeyFrames_;
    std::vector<std::shared_ptr<KeyFrame<double>>> doubleKeyFrames_;
    std::vector<std::shared_ptr<KeyFrame<ui::Point>>> pointKeyFrames_;
    std::vector<std::shared_ptr<KeyFrame<Thickness>>> thicknessKeyFrames_;
    
    // 内部辅助函数
    void InitAnimation(AnimationType type, const std::string& targetName, const std::string& propertyPath);
    void InitAnimation(AnimationType type, binding::DependencyObject* target, const binding::DependencyProperty* property);
    void InitKeyFrameAnimation(AnimationType type, const std::string& targetName, const std::string& propertyPath);
    void ResetAnimationParams();
    void CheckAnimationType(AnimationType expected, const char* methodName) const;
    
    template<typename T, typename KeyFrameType>
    void AddKeyFrameImpl(AnimationType expectedType, const char* typeName, 
                        std::vector<std::shared_ptr<KeyFrame<T>>>& keyFrames,
                        int keyTimeMs, const T& value);
    
    void FinalizeCurrentAnimation();
    void FinalizeCurrentState();
};

} // namespace fk::animation
