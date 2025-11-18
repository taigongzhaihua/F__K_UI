#pragma once

#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/DoubleAnimation.h"
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
     * @brief 设置当前动画的持续时间（毫秒）
     * 
     * @param milliseconds 持续时间
     * @return this（支持链式调用）
     */
    VisualStateBuilder* Duration(int milliseconds);
    
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
    enum class AnimationType { None, Color, Double };
    AnimationType currentAnimationType_{AnimationType::None};
    
    // 动画目标（两种方式：直接对象 或 TargetName）
    binding::DependencyObject* currentTarget_{nullptr};
    const binding::DependencyProperty* currentProperty_{nullptr};
    std::string currentTargetName_;      // 使用TargetName方式时的目标名称
    std::string currentPropertyPath_;    // 使用TargetName方式时的属性路径
    
    // 颜色动画参数
    std::optional<ui::Color> colorFrom_;
    std::optional<ui::Color> colorTo_;
    
    // 双精度动画参数
    std::optional<double> doubleFrom_;
    std::optional<double> doubleTo_;
    
    // 动画持续时间
    int durationMs_{200};  // 默认200ms
    
    void FinalizeCurrentAnimation();
    void FinalizeCurrentState();
};

} // namespace fk::animation
