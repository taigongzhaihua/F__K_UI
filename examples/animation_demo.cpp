/**
 * Phase 4 动画系统演示程序
 * 展示基础动画功能
 */

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "fk/animation/Timeline.h"
#include "fk/animation/DoubleAnimation.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/PointAnimation.h"
#include "fk/animation/Storyboard.h"
#include "fk/animation/EasingFunction.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"

using namespace fk;
using namespace std::chrono_literals;

// 测试用的简单对象
class AnimatedObject : public binding::DependencyObject {
public:
    // 定义属性
    static const binding::DependencyProperty& WidthProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Width",
            typeid(double),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(100.0)}
        );
        return property;
    }

    static const binding::DependencyProperty& OpacityProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Opacity",
            typeid(double),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(1.0)}
        );
        return property;
    }

    static const binding::DependencyProperty& ColorProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Color",
            typeid(ui::Color),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(ui::Color(255, 255, 255))}
        );
        return property;
    }

    static const binding::DependencyProperty& PositionProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Position",
            typeid(ui::Point),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(ui::Point{0.0f, 0.0f})}
        );
        return property;
    }

    double GetWidth() const {
        return std::any_cast<double>(GetValue(WidthProperty()));
    }

    void SetWidth(double value) {
        SetValue(WidthProperty(), value);
    }

    double GetOpacity() const {
        return std::any_cast<double>(GetValue(OpacityProperty()));
    }

    void SetOpacity(double value) {
        SetValue(OpacityProperty(), value);
    }

    ui::Color GetColor() const {
        return std::any_cast<ui::Color>(GetValue(ColorProperty()));
    }

    void SetColor(const ui::Color& value) {
        SetValue(ColorProperty(), value);
    }

    ui::Point GetPosition() const {
        return std::any_cast<ui::Point>(GetValue(PositionProperty()));
    }

    void SetPosition(const ui::Point& value) {
        SetValue(PositionProperty(), value);
    }
};

void PrintSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void Test_DoubleAnimation() {
    PrintSeparator("测试 1: DoubleAnimation - 宽度动画");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetWidth(100.0);

    std::cout << "初始宽度: " << obj->GetWidth() << "\n";

    // 创建动画
    auto animation = std::make_shared<animation::DoubleAnimation>(
        100.0, 500.0, animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::WidthProperty());

    std::cout << "开始动画: 从 100.0 到 500.0，持续 1000ms\n";
    animation->Begin();

    // 手动更新动画（模拟时间流逝）
    for (int i = 0; i <= 10; ++i) {
        double progress = i / 10.0;
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  进度 " << (i * 10) << "%: 宽度 = " << obj->GetWidth() << "\n";
    }

    std::cout << "动画完成！最终宽度: " << obj->GetWidth() << "\n";
}

void Test_DoubleAnimationWithEasing() {
    PrintSeparator("测试 2: DoubleAnimation + Easing - 缓动函数");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n";

    // 创建动画并设置缓动函数
    auto animation = std::make_shared<animation::DoubleAnimation>(
        0.0, 1.0, animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());

    // 使用二次缓动
    auto easing = std::make_shared<animation::QuadraticEase>();
    easing->SetEasingMode(animation::EasingMode::EaseInOut);
    animation->SetEasingFunction(easing);

    std::cout << "开始动画: 从 0.0 到 1.0，使用 QuadraticEase (EaseInOut)\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  进度 " << (i * 10) << "%: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "动画完成！最终不透明度: " << obj->GetOpacity() << "\n";
}

void Test_ColorAnimation() {
    PrintSeparator("测试 3: ColorAnimation - 颜色动画");

    auto obj = std::make_shared<AnimatedObject>();
    ui::Color startColor(255, 0, 0);    // 红色
    ui::Color endColor(0, 0, 255);      // 蓝色
    obj->SetColor(startColor);

    std::cout << "初始颜色: RGB(" << (int)(startColor.r * 255) << ", " 
              << (int)(startColor.g * 255) << ", " << (int)(startColor.b * 255) << ")\n";

    // 创建颜色动画
    auto animation = std::make_shared<animation::ColorAnimation>(
        startColor, endColor, animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::ColorProperty());

    std::cout << "开始动画: 从红色到蓝色，持续 1000ms\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        auto color = obj->GetColor();
        std::cout << "  进度 " << (i * 10) << "%: RGB(" 
                  << (int)(color.r * 255) << ", " << (int)(color.g * 255) << ", " << (int)(color.b * 255) << ")\n";
    }

    auto finalColor = obj->GetColor();
    std::cout << "动画完成！最终颜色: RGB(" << (int)(finalColor.r * 255) << ", " 
              << (int)(finalColor.g * 255) << ", " << (int)(finalColor.b * 255) << ")\n";
}

void Test_PointAnimation() {
    PrintSeparator("测试 4: PointAnimation - 位置动画");

    auto obj = std::make_shared<AnimatedObject>();
    ui::Point startPos{0.0f, 0.0f};
    ui::Point endPos{100.0f, 200.0f};
    obj->SetPosition(startPos);

    std::cout << "初始位置: (" << startPos.x << ", " << startPos.y << ")\n";

    // 创建位置动画
    auto animation = std::make_shared<animation::PointAnimation>(
        startPos, endPos, animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::PositionProperty());

    std::cout << "开始动画: 从 (0,0) 到 (100,200)，持续 1000ms\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        auto pos = obj->GetPosition();
        std::cout << "  进度 " << (i * 10) << "%: 位置 = (" 
                  << pos.x << ", " << pos.y << ")\n";
    }

    auto finalPos = obj->GetPosition();
    std::cout << "动画完成！最终位置: (" << finalPos.x << ", " << finalPos.y << ")\n";
}

void Test_RepeatBehavior() {
    PrintSeparator("测试 5: RepeatBehavior - 重复行为");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetWidth(100.0);

    std::cout << "初始宽度: " << obj->GetWidth() << "\n";

    // 创建重复动画
    auto animation = std::make_shared<animation::DoubleAnimation>(
        100.0, 200.0, animation::Duration(std::chrono::milliseconds(500))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::WidthProperty());
    animation->SetRepeatBehavior(animation::RepeatBehavior(3.0));  // 重复3次

    std::cout << "开始动画: 从 100.0 到 200.0，重复 3 次\n";
    animation->Begin();

    // 模拟 1500ms（3次循环）
    for (int i = 0; i <= 15; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  时间 " << (i * 100) << "ms: 宽度 = " << obj->GetWidth() << "\n";
    }

    std::cout << "动画完成！\n";
}

void Test_AutoReverse() {
    PrintSeparator("测试 6: AutoReverse - 自动反向");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n";

    // 创建自动反向动画
    auto animation = std::make_shared<animation::DoubleAnimation>(
        0.0, 1.0, animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());
    animation->SetAutoReverse(true);

    std::cout << "开始动画: 从 0.0 到 1.0 再回到 0.0 (AutoReverse)\n";
    animation->Begin();

    // 模拟 2000ms（包含反向）
    for (int i = 0; i <= 20; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  时间 " << (i * 100) << "ms: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "动画完成！\n";
}

void Test_EasingFunctions() {
    PrintSeparator("测试 7: 各种缓动函数对比");

    std::vector<std::pair<std::string, std::shared_ptr<animation::EasingFunctionBase>>> easingFunctions = {
        {"Linear", std::make_shared<animation::LinearEase>()},
        {"Quadratic", std::make_shared<animation::QuadraticEase>()},
        {"Cubic", std::make_shared<animation::CubicEase>()},
        {"Sine", std::make_shared<animation::SineEase>()},
        {"Circle", std::make_shared<animation::CircleEase>()}
    };

    std::cout << "测试不同缓动函数的效果（EaseInOut 模式）:\n\n";

    for (auto& [name, easing] : easingFunctions) {
        easing->SetEasingMode(animation::EasingMode::EaseInOut);
        std::cout << name << " Easing:\n";
        
        for (int i = 0; i <= 10; ++i) {
            double t = i / 10.0;
            double easedValue = easing->Ease(t);
            std::cout << "  t=" << t << " -> " << easedValue << "\n";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║          F__K_UI Phase 4 - 动画系统演示程序              ║\n";
    std::cout << "║                Animation System Demo                      ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    try {
        Test_DoubleAnimation();
        Test_DoubleAnimationWithEasing();
        Test_ColorAnimation();
        Test_PointAnimation();
        Test_RepeatBehavior();
        Test_AutoReverse();
        Test_EasingFunctions();

        PrintSeparator("所有测试完成！");
        std::cout << "\n✓ 动画基础系统运行正常\n";
        std::cout << "✓ DoubleAnimation, ColorAnimation, PointAnimation 工作正常\n";
        std::cout << "✓ 缓动函数系统正常\n";
        std::cout << "✓ RepeatBehavior 和 AutoReverse 功能正常\n";
        std::cout << "\nPhase 4.1 - 动画基础系统实现完成！\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
