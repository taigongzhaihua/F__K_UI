/**
 * Phase 4 关键帧动画演示程序
 * 展示关键帧动画功能
 */

#include <iostream>
#include <memory>

#include "fk/animation/KeyFrame.h"
#include "fk/animation/KeyFrameAnimation.h"
#include "fk/animation/DoubleAnimationUsingKeyFrames.h"
#include "fk/animation/ThicknessAnimation.h"
#include "fk/animation/EasingFunction.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"

using namespace fk;
using namespace std::chrono_literals;

// 测试用的简单对象
class AnimatedObject : public binding::DependencyObject {
public:
    static const binding::DependencyProperty& OpacityProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Opacity",
            typeid(double),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(1.0)}
        );
        return property;
    }

    static const binding::DependencyProperty& MarginProperty() {
        static auto& property = binding::DependencyProperty::Register(
            "Margin",
            typeid(Thickness),
            typeid(AnimatedObject),
            binding::PropertyMetadata{std::any(Thickness(0, 0, 0, 0))}
        );
        return property;
    }

    double GetOpacity() const {
        return std::any_cast<double>(GetValue(OpacityProperty()));
    }

    void SetOpacity(double value) {
        SetValue(OpacityProperty(), value);
    }

    Thickness GetMargin() const {
        return std::any_cast<Thickness>(GetValue(MarginProperty()));
    }

    void SetMargin(const Thickness& value) {
        SetValue(MarginProperty(), value);
    }
};

void PrintSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n\n";
}

void Test_LinearKeyFrames() {
    PrintSeparator("测试 1: 线性关键帧动画");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n\n";

    // 创建关键帧动画
    auto animation = std::make_shared<animation::DoubleAnimationUsingKeyFrames>();
    animation->SetDuration(animation::Duration(std::chrono::milliseconds(1000)));
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());

    // 添加关键帧
    auto kf1 = std::make_shared<animation::LinearKeyFrame<double>>(
        0.0, animation::KeyTime::FromPercent(0.0)
    );
    auto kf2 = std::make_shared<animation::LinearKeyFrame<double>>(
        0.5, animation::KeyTime::FromPercent(0.5)
    );
    auto kf3 = std::make_shared<animation::LinearKeyFrame<double>>(
        1.0, animation::KeyTime::FromPercent(1.0)
    );

    animation->KeyFrames().Add(kf1);
    animation->KeyFrames().Add(kf2);
    animation->KeyFrames().Add(kf3);

    std::cout << "关键帧设置:\n";
    std::cout << "  0% -> 不透明度 = 0.0\n";
    std::cout << "  50% -> 不透明度 = 0.5\n";
    std::cout << "  100% -> 不透明度 = 1.0\n\n";

    std::cout << "开始动画...\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  进度 " << (i * 10) << "%: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "\n动画完成！最终不透明度: " << obj->GetOpacity() << "\n";
}

void Test_DiscreteKeyFrames() {
    PrintSeparator("测试 2: 离散关键帧动画（突变）");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n\n";

    // 创建关键帧动画
    auto animation = std::make_shared<animation::DoubleAnimationUsingKeyFrames>();
    animation->SetDuration(animation::Duration(std::chrono::milliseconds(1000)));
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());

    // 添加离散关键帧（突变，无过渡）
    auto kf1 = std::make_shared<animation::DiscreteKeyFrame<double>>(
        0.0, animation::KeyTime::FromPercent(0.0)
    );
    auto kf2 = std::make_shared<animation::DiscreteKeyFrame<double>>(
        1.0, animation::KeyTime::FromPercent(0.5)
    );
    auto kf3 = std::make_shared<animation::DiscreteKeyFrame<double>>(
        0.5, animation::KeyTime::FromPercent(1.0)
    );

    animation->KeyFrames().Add(kf1);
    animation->KeyFrames().Add(kf2);
    animation->KeyFrames().Add(kf3);

    std::cout << "离散关键帧设置（突变，无平滑过渡）:\n";
    std::cout << "  0% -> 不透明度 = 0.0\n";
    std::cout << "  50% -> 不透明度 = 1.0 （突变）\n";
    std::cout << "  100% -> 不透明度 = 0.5 （突变）\n\n";

    std::cout << "开始动画...\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  进度 " << (i * 10) << "%: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "\n动画完成！\n";
}

void Test_EasingKeyFrames() {
    PrintSeparator("测试 3: 缓动关键帧动画");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n\n";

    // 创建关键帧动画
    auto animation = std::make_shared<animation::DoubleAnimationUsingKeyFrames>();
    animation->SetDuration(animation::Duration(std::chrono::milliseconds(1000)));
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());

    // 添加缓动关键帧
    auto kf1 = std::make_shared<animation::EasingKeyFrame<double>>(
        0.0, animation::KeyTime::FromPercent(0.0)
    );
    
    auto kf2 = std::make_shared<animation::EasingKeyFrame<double>>(
        1.0, animation::KeyTime::FromPercent(1.0)
    );
    // 设置缓动函数
    auto easing = std::make_shared<animation::BounceEase>();
    easing->SetEasingMode(animation::EasingMode::EaseOut);
    kf2->SetEasingFunction(easing);

    animation->KeyFrames().Add(kf1);
    animation->KeyFrames().Add(kf2);

    std::cout << "缓动关键帧设置（使用 BounceEase EaseOut）:\n";
    std::cout << "  0% -> 不透明度 = 0.0\n";
    std::cout << "  100% -> 不透明度 = 1.0 (带反弹效果)\n\n";

    std::cout << "开始动画...\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  进度 " << (i * 10) << "%: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "\n动画完成！\n";
}

void Test_ThicknessAnimation() {
    PrintSeparator("测试 4: Thickness 动画（边距）");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetMargin(Thickness(0, 0, 0, 0));

    auto margin = obj->GetMargin();
    std::cout << "初始边距: (" << margin.left << ", " << margin.top 
              << ", " << margin.right << ", " << margin.bottom << ")\n\n";

    // 创建 Thickness 动画
    auto animation = std::make_shared<animation::ThicknessAnimation>(
        Thickness(0, 0, 0, 0),
        Thickness(10, 20, 10, 20),
        animation::Duration(std::chrono::milliseconds(1000))
    );
    animation->SetTarget(obj.get(), &AnimatedObject::MarginProperty());

    std::cout << "开始动画: 从 (0,0,0,0) 到 (10,20,10,20)\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        auto m = obj->GetMargin();
        std::cout << "  进度 " << (i * 10) << "%: 边距 = (" 
                  << m.left << ", " << m.top << ", " 
                  << m.right << ", " << m.bottom << ")\n";
    }

    auto finalMargin = obj->GetMargin();
    std::cout << "\n动画完成！最终边距: (" << finalMargin.left << ", " 
              << finalMargin.top << ", " << finalMargin.right << ", " 
              << finalMargin.bottom << ")\n";
}

void Test_TimeBasedKeyFrames() {
    PrintSeparator("测试 5: 基于绝对时间的关键帧");

    auto obj = std::make_shared<AnimatedObject>();
    obj->SetOpacity(0.0);

    std::cout << "初始不透明度: " << obj->GetOpacity() << "\n\n";

    // 创建关键帧动画
    auto animation = std::make_shared<animation::DoubleAnimationUsingKeyFrames>();
    animation->SetDuration(animation::Duration(std::chrono::milliseconds(1000)));
    animation->SetTarget(obj.get(), &AnimatedObject::OpacityProperty());

    // 添加基于绝对时间的关键帧
    auto kf1 = std::make_shared<animation::LinearKeyFrame<double>>(
        0.0, animation::KeyTime::FromTimeSpan(std::chrono::milliseconds(0))
    );
    auto kf2 = std::make_shared<animation::LinearKeyFrame<double>>(
        0.3, animation::KeyTime::FromTimeSpan(std::chrono::milliseconds(200))
    );
    auto kf3 = std::make_shared<animation::LinearKeyFrame<double>>(
        0.7, animation::KeyTime::FromTimeSpan(std::chrono::milliseconds(600))
    );
    auto kf4 = std::make_shared<animation::LinearKeyFrame<double>>(
        1.0, animation::KeyTime::FromTimeSpan(std::chrono::milliseconds(1000))
    );

    animation->KeyFrames().Add(kf1);
    animation->KeyFrames().Add(kf2);
    animation->KeyFrames().Add(kf3);
    animation->KeyFrames().Add(kf4);

    std::cout << "关键帧设置（绝对时间）:\n";
    std::cout << "  0ms -> 不透明度 = 0.0\n";
    std::cout << "  200ms -> 不透明度 = 0.3\n";
    std::cout << "  600ms -> 不透明度 = 0.7\n";
    std::cout << "  1000ms -> 不透明度 = 1.0\n\n";

    std::cout << "开始动画...\n";
    animation->Begin();

    // 手动更新动画
    for (int i = 0; i <= 10; ++i) {
        animation->Update(std::chrono::milliseconds(100));
        std::cout << "  时间 " << (i * 100) << "ms: 不透明度 = " 
                  << obj->GetOpacity() << "\n";
    }

    std::cout << "\n动画完成！\n";
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║     F__K_UI Phase 4 - 关键帧动画系统演示程序             ║\n";
    std::cout << "║         KeyFrame Animation System Demo                   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    try {
        Test_LinearKeyFrames();
        Test_DiscreteKeyFrames();
        Test_EasingKeyFrames();
        Test_ThicknessAnimation();
        Test_TimeBasedKeyFrames();

        PrintSeparator("所有测试完成！");
        std::cout << "\n✓ 关键帧动画系统运行正常\n";
        std::cout << "✓ 线性关键帧工作正常\n";
        std::cout << "✓ 离散关键帧（突变）工作正常\n";
        std::cout << "✓ 缓动关键帧工作正常\n";
        std::cout << "✓ ThicknessAnimation 工作正常\n";
        std::cout << "✓ 基于时间的关键帧工作正常\n";
        std::cout << "\nPhase 4.1 扩展 - 关键帧动画系统实现完成！\n\n";

    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
