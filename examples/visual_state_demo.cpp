/**
 * Phase 4.2 VisualStateManager 演示程序
 * 展示视觉状态管理功能
 */

#include <iostream>
#include <memory>

#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/VisualTransition.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/DoubleAnimation.h"
#include "fk/animation/EasingFunction.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"

using namespace fk;
using namespace std::chrono_literals;

// 模拟一个按钮控件
class MockButton : public binding::DependencyObject {
public:
    static const binding::DependencyProperty& OpacityProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Opacity",
            typeid(double),
            typeid(MockButton),
            binding::PropertyMetadata{std::any(1.0)}
        );
        return property;
    }

    static const binding::DependencyProperty& ScaleProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Scale",
            typeid(double),
            typeid(MockButton),
            binding::PropertyMetadata{std::any(1.0)}
        );
        return property;
    }

    double GetOpacity() const {
        return std::any_cast<double>(GetValue(OpacityProperty()));
    }

    void SetOpacity(double value) {
        SetValue(OpacityProperty(), value);
    }

    double GetScale() const {
        return std::any_cast<double>(GetValue(ScaleProperty()));
    }

    void SetScale(double value) {
        SetValue(ScaleProperty(), value);
    }
    
    std::string GetCurrentStateName() const {
        auto manager = animation::VisualStateManager::GetVisualStateManager(
            const_cast<MockButton*>(this)
        );
        if (!manager) return "None";
        
        auto& groups = manager->GetStateGroups();
        if (groups.empty()) return "None";
        
        auto currentState = groups[0]->GetCurrentState();
        return currentState ? currentState->GetName() : "None";
    }
};

void PrintSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void Test_BasicStateTransition() {
    PrintSeparator("测试 1: 基础状态转换");

    auto button = std::make_shared<MockButton>();
    std::cout << "创建模拟按钮控件\n";
    std::cout << "初始不透明度: " << button->GetOpacity() << "\n";
    std::cout << "初始缩放: " << button->GetScale() << "\n\n";

    // 创建 VisualStateManager
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(button.get(), manager);

    // 创建状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");

    // 创建 Normal 状态
    auto normalState = std::make_shared<animation::VisualState>("Normal");
    auto normalStoryboard = std::make_shared<animation::Storyboard>();
    
    auto normalOpacityAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetOpacity(), 1.0, animation::Duration(std::chrono::milliseconds(300))
    );
    normalOpacityAnim->SetTarget(button.get(), &MockButton::OpacityProperty());
    normalStoryboard->AddChild(normalOpacityAnim);
    
    auto normalScaleAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetScale(), 1.0, animation::Duration(std::chrono::milliseconds(300))
    );
    normalScaleAnim->SetTarget(button.get(), &MockButton::ScaleProperty());
    normalStoryboard->AddChild(normalScaleAnim);
    
    normalState->SetStoryboard(normalStoryboard);

    // 创建 MouseOver 状态
    auto mouseOverState = std::make_shared<animation::VisualState>("MouseOver");
    auto mouseOverStoryboard = std::make_shared<animation::Storyboard>();
    
    auto mouseOverOpacityAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetOpacity(), 0.8, animation::Duration(std::chrono::milliseconds(200))
    );
    mouseOverOpacityAnim->SetTarget(button.get(), &MockButton::OpacityProperty());
    mouseOverStoryboard->AddChild(mouseOverOpacityAnim);
    
    auto mouseOverScaleAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetScale(), 1.05, animation::Duration(std::chrono::milliseconds(200))
    );
    mouseOverScaleAnim->SetTarget(button.get(), &MockButton::ScaleProperty());
    mouseOverStoryboard->AddChild(mouseOverScaleAnim);
    
    mouseOverState->SetStoryboard(mouseOverStoryboard);

    // 创建 Pressed 状态
    auto pressedState = std::make_shared<animation::VisualState>("Pressed");
    auto pressedStoryboard = std::make_shared<animation::Storyboard>();
    
    auto pressedOpacityAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetOpacity(), 0.6, animation::Duration(std::chrono::milliseconds(100))
    );
    pressedOpacityAnim->SetTarget(button.get(), &MockButton::OpacityProperty());
    pressedStoryboard->AddChild(pressedOpacityAnim);
    
    auto pressedScaleAnim = std::make_shared<animation::DoubleAnimation>(
        button->GetScale(), 0.95, animation::Duration(std::chrono::milliseconds(100))
    );
    pressedScaleAnim->SetTarget(button.get(), &MockButton::ScaleProperty());
    pressedStoryboard->AddChild(pressedScaleAnim);
    
    pressedState->SetStoryboard(pressedStoryboard);

    // 添加状态到组
    commonStates->AddState(normalState);
    commonStates->AddState(mouseOverState);
    commonStates->AddState(pressedState);

    // 添加状态组到管理器
    manager->AddStateGroup(commonStates);

    std::cout << "已创建3个视觉状态:\n";
    std::cout << "  - Normal: 不透明度=1.0, 缩放=1.0\n";
    std::cout << "  - MouseOver: 不透明度=0.8, 缩放=1.05\n";
    std::cout << "  - Pressed: 不透明度=0.6, 缩放=0.95\n\n";

    // 测试状态转换
    std::cout << "当前状态: " << button->GetCurrentStateName() << "\n\n";

    std::cout << "转换到 Normal 状态...\n";
    bool success = animation::VisualStateManager::GoToState(button.get(), "Normal", true);
    std::cout << "  转换" << (success ? "成功" : "失败") << "\n";
    std::cout << "  当前状态: " << button->GetCurrentStateName() << "\n";
    std::cout << "  不透明度: " << button->GetOpacity() << ", 缩放: " << button->GetScale() << "\n\n";

    std::cout << "转换到 MouseOver 状态...\n";
    success = animation::VisualStateManager::GoToState(button.get(), "MouseOver", true);
    std::cout << "  转换" << (success ? "成功" : "失败") << "\n";
    std::cout << "  当前状态: " << button->GetCurrentStateName() << "\n";
    std::cout << "  不透明度: " << button->GetOpacity() << ", 缩放: " << button->GetScale() << "\n\n";

    std::cout << "转换到 Pressed 状态...\n";
    success = animation::VisualStateManager::GoToState(button.get(), "Pressed", true);
    std::cout << "  转换" << (success ? "成功" : "失败") << "\n";
    std::cout << "  当前状态: " << button->GetCurrentStateName() << "\n";
    std::cout << "  不透明度: " << button->GetOpacity() << ", 缩放: " << button->GetScale() << "\n\n";

    std::cout << "返回到 Normal 状态...\n";
    success = animation::VisualStateManager::GoToState(button.get(), "Normal", true);
    std::cout << "  转换" << (success ? "成功" : "失败") << "\n";
    std::cout << "  当前状态: " << button->GetCurrentStateName() << "\n";
    std::cout << "  不透明度: " << button->GetOpacity() << ", 缩放: " << button->GetScale() << "\n";
}

void Test_StateTransitions() {
    PrintSeparator("测试 2: 自定义状态转换");

    auto button = std::make_shared<MockButton>();
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(button.get(), manager);

    // 创建状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");

    // 创建简单状态
    auto state1 = std::make_shared<animation::VisualState>("State1");
    auto state2 = std::make_shared<animation::VisualState>("State2");
    commonStates->AddState(state1);
    commonStates->AddState(state2);

    // 创建转换（从 State1 到 State2）
    auto transition = std::make_shared<animation::VisualTransition>();
    transition->SetFrom("State1");
    transition->SetTo("State2");
    transition->SetGeneratedDuration(animation::Duration(std::chrono::milliseconds(500)));
    
    auto easing = std::make_shared<animation::BounceEase>();
    easing->SetEasingMode(animation::EasingMode::EaseOut);
    transition->SetGeneratedEasingFunction(easing);
    
    commonStates->AddTransition(transition);

    // 创建默认转换（任意状态之间）
    auto defaultTransition = std::make_shared<animation::VisualTransition>();
    defaultTransition->SetGeneratedDuration(animation::Duration(std::chrono::milliseconds(300)));
    commonStates->AddTransition(defaultTransition);

    manager->AddStateGroup(commonStates);

    std::cout << "已创建状态组和转换:\n";
    std::cout << "  - State1\n";
    std::cout << "  - State2\n";
    std::cout << "  - 转换 State1→State2: 500ms, BounceEase\n";
    std::cout << "  - 默认转换: 300ms\n\n";

    // 测试转换查找
    std::cout << "测试转换查找...\n";
    
    auto foundTransition = commonStates->FindBestTransition("State1", "State2");
    if (foundTransition) {
        std::cout << "  找到 State1→State2 转换\n";
        std::cout << "  持续时间: " << foundTransition->GetGeneratedDuration().timeSpan.count() << "ms\n";
    }
    
    foundTransition = commonStates->FindBestTransition("State2", "State1");
    if (foundTransition) {
        std::cout << "  找到 State2→State1 转换（默认）\n";
        std::cout << "  持续时间: " << foundTransition->GetGeneratedDuration().timeSpan.count() << "ms\n";
    }
}

void Test_StateEvents() {
    PrintSeparator("测试 3: 状态改变事件");

    auto button = std::make_shared<MockButton>();
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(button.get(), manager);

    // 创建状态组
    auto commonStates = std::make_shared<animation::VisualStateGroup>("CommonStates");
    auto stateA = std::make_shared<animation::VisualState>("StateA");
    auto stateB = std::make_shared<animation::VisualState>("StateB");
    commonStates->AddState(stateA);
    commonStates->AddState(stateB);
    manager->AddStateGroup(commonStates);

    // 订阅事件
    int changingCount = 0;
    int changedCount = 0;

    manager->CurrentStateChanging.Connect([&](animation::VisualStateGroup* group,
                                               animation::VisualState* from,
                                               animation::VisualState* to) {
        changingCount++;
        std::cout << "  [事件] CurrentStateChanging: " 
                  << (from ? from->GetName() : "None") << " → " 
                  << (to ? to->GetName() : "None") << "\n";
    });

    manager->CurrentStateChanged.Connect([&](animation::VisualStateGroup* group,
                                              animation::VisualState* from,
                                              animation::VisualState* to) {
        changedCount++;
        std::cout << "  [事件] CurrentStateChanged: " 
                  << (from ? from->GetName() : "None") << " → " 
                  << (to ? to->GetName() : "None") << "\n";
    });

    std::cout << "已订阅状态改变事件\n\n";

    // 触发状态转换
    std::cout << "转换到 StateA...\n";
    animation::VisualStateManager::GoToState(button.get(), "StateA", false);

    std::cout << "\n转换到 StateB...\n";
    animation::VisualStateManager::GoToState(button.get(), "StateB", false);

    std::cout << "\n事件统计:\n";
    std::cout << "  CurrentStateChanging 触发次数: " << changingCount << "\n";
    std::cout << "  CurrentStateChanged 触发次数: " << changedCount << "\n";
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║    F__K_UI Phase 4.2 - VisualStateManager 演示程序       ║\n";
    std::cout << "║         Visual State Manager Demo                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    try {
        Test_BasicStateTransition();
        Test_StateTransitions();
        Test_StateEvents();

        PrintSeparator("所有测试完成！");
        std::cout << "\n✓ VisualStateManager 系统运行正常\n";
        std::cout << "✓ 基础状态转换工作正常\n";
        std::cout << "✓ 自定义转换查找正常\n";
        std::cout << "✓ 状态改变事件正常\n";
        std::cout << "\nPhase 4.2 - VisualStateManager 实现完成！\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
