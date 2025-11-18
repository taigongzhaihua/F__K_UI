/**
 * @file test_button_color_fix.cpp
 * @brief 测试 Button 的 MouseOverBackground 和 PressedBackground 颜色设置
 */

#include "fk/ui/Button.h"
#include "fk/ui/Brush.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualState.h"
#include "fk/animation/VisualStateGroup.h"
#include "fk/animation/ColorAnimation.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;
using namespace fk::animation;

void TestButtonColorProperties() {
    std::cout << "=== 测试 Button 颜色属性 ===\n\n";
    
    // 创建按钮
    auto* button = new Button();
    std::cout << "[1] 创建 Button 实例\n";
    
    // 设置自定义的悬停颜色（红色）
    std::cout << "[2] 设置 MouseOverBackground 为红色 RGB(255, 0, 0)\n";
    button->MouseOverBackground(Color::FromRGB(255, 0, 0, 255));
    
    // 设置自定义的按下颜色（蓝色）
    std::cout << "[3] 设置 PressedBackground 为蓝色 RGB(0, 0, 255)\n";
    button->PressedBackground(Color::FromRGB(0, 0, 255, 255));
    
    // 验证属性设置成功
    auto* mouseOverBg = button->GetMouseOverBackground();
    auto* mouseOverBrush = dynamic_cast<SolidColorBrush*>(mouseOverBg);
    assert(mouseOverBrush != nullptr);
    Color mouseOverColor = mouseOverBrush->GetColor();
    std::cout << "   ✓ MouseOverBackground 颜色: RGB(" 
              << (int)(mouseOverColor.r * 255) << ", "
              << (int)(mouseOverColor.g * 255) << ", "
              << (int)(mouseOverColor.b * 255) << ")\n";
    assert(mouseOverColor.r == 1.0f && mouseOverColor.g == 0.0f && mouseOverColor.b == 0.0f);
    
    auto* pressedBg = button->GetPressedBackground();
    auto* pressedBrush = dynamic_cast<SolidColorBrush*>(pressedBg);
    assert(pressedBrush != nullptr);
    Color pressedColor = pressedBrush->GetColor();
    std::cout << "   ✓ PressedBackground 颜色: RGB(" 
              << (int)(pressedColor.r * 255) << ", "
              << (int)(pressedColor.g * 255) << ", "
              << (int)(pressedColor.b * 255) << ")\n";
    assert(pressedColor.r == 0.0f && pressedColor.g == 0.0f && pressedColor.b == 1.0f);
    
    std::cout << "\n=== 属性设置测试通过 ✓ ===\n\n";
    
    delete button;
}

void TestButtonVisualStates() {
    std::cout << "=== 测试 Button 视觉状态 ===\n\n";
    
    // 创建按钮并设置自定义颜色
    auto* button = new Button();
    button->MouseOverBackground(Color::FromRGB(255, 0, 0, 255));  // 红色
    button->PressedBackground(Color::FromRGB(0, 0, 255, 255));    // 蓝色
    
    std::cout << "[1] 应用模板\n";
    button->ApplyTemplate();
    
    // 获取 VisualStateManager
    auto* manager = VisualStateManager::GetVisualStateManager(button);
    std::cout << "[2] 检查 VisualStateManager: " 
              << (manager ? "存在 ✓" : "不存在 ✗") << "\n";
    assert(manager != nullptr);
    
    // 获取 CommonStates 状态组
    auto& stateGroups = manager->GetStateGroups();
    std::cout << "[3] 状态组数量: " << stateGroups.size() << "\n";
    assert(stateGroups.size() > 0);
    
    // 查找 MouseOver 和 Pressed 状态
    std::shared_ptr<VisualState> mouseOverState = nullptr;
    std::shared_ptr<VisualState> pressedState = nullptr;
    
    for (auto& group : stateGroups) {
        if (!group) continue;
        for (auto& state : group->GetStates()) {
            if (!state) continue;
            if (state->GetName() == "MouseOver") {
                mouseOverState = state;
            } else if (state->GetName() == "Pressed") {
                pressedState = state;
            }
        }
    }
    
    std::cout << "[4] MouseOver 状态: " 
              << (mouseOverState ? "找到 ✓" : "未找到 ✗") << "\n";
    std::cout << "[5] Pressed 状态: " 
              << (pressedState ? "找到 ✓" : "未找到 ✗") << "\n";
    
    assert(mouseOverState != nullptr);
    assert(pressedState != nullptr);
    
    // 检查 MouseOver 状态的颜色动画
    if (mouseOverState) {
        auto storyboard = mouseOverState->GetStoryboard();
        if (storyboard) {
            auto& children = storyboard->GetChildren();
            std::cout << "[6] MouseOver 状态动画数量: " << children.size() << "\n";
            
            for (auto& child : children) {
                auto* colorAnim = dynamic_cast<ColorAnimation*>(child.get());
                if (colorAnim && colorAnim->HasTo()) {
                    Color color = colorAnim->GetTo();
                    std::cout << "   ✓ MouseOver 动画目标颜色: RGB(" 
                              << (int)(color.r * 255) << ", "
                              << (int)(color.g * 255) << ", "
                              << (int)(color.b * 255) << ")\n";
                    // 验证颜色是用户设置的红色，而不是默认的浅蓝色
                    assert(color.r == 1.0f && color.g == 0.0f && color.b == 0.0f);
                    std::cout << "   ✓✓ 颜色已正确设置为用户自定义的红色！\n";
                }
            }
        }
    }
    
    // 检查 Pressed 状态的颜色动画
    if (pressedState) {
        auto storyboard = pressedState->GetStoryboard();
        if (storyboard) {
            auto& children = storyboard->GetChildren();
            std::cout << "[7] Pressed 状态动画数量: " << children.size() << "\n";
            
            for (auto& child : children) {
                auto* colorAnim = dynamic_cast<ColorAnimation*>(child.get());
                if (colorAnim && colorAnim->HasTo()) {
                    Color color = colorAnim->GetTo();
                    std::cout << "   ✓ Pressed 动画目标颜色: RGB(" 
                              << (int)(color.r * 255) << ", "
                              << (int)(color.g * 255) << ", "
                              << (int)(color.b * 255) << ")\n";
                    // 验证颜色是用户设置的蓝色，而不是默认的深蓝色
                    assert(color.r == 0.0f && color.g == 0.0f && color.b == 1.0f);
                    std::cout << "   ✓✓ 颜色已正确设置为用户自定义的蓝色！\n";
                }
            }
        }
    }
    
    std::cout << "\n=== 视觉状态测试通过 ✓ ===\n\n";
    
    delete button;
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Button 悬停和按下颜色设置修复测试                      ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    
    try {
        TestButtonColorProperties();
        TestButtonVisualStates();
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║  ✓✓✓ 所有测试通过！修复成功！                         ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ 测试失败: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n✗ 测试失败：未知错误\n";
        return 1;
    }
}
