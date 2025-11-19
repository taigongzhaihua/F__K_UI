#include "fk/animation/VisualStateBuilder.h"
#include <iostream>

using namespace fk;
using namespace fk::animation;

/**
 * KeyFrame动画演示 - 使用VisualStateBuilder API
 * 
 * 这是一个纯粹的演示,展示KeyFrame动画的API使用方法。
 * 实际视觉效果需要在GUI环境中运行,请参考examples/中的完整示例。
 * 
 * 展示的KeyFrame动画类型：
 * 1. ColorKeyFrameAnimation - 颜色在多个关键帧间变化
 * 2. LinearKeyFrame - 线性插值关键帧
 * 3. DiscreteKeyFrame - 离散跳变关键帧
 */

int main() {
    try {
        std::cout << "\n=== KeyFrame Animation Builder API Demo ===\n";
        std::cout << "演示如何使用VisualStateBuilder API创建关键帧动画\n\n";
        
        // ===== 示例1: 颜色线性关键帧动画 =====
        std::cout << "【示例1】颜色线性关键帧动画\n";
        std::cout << "----------------------------------------\n";
        
        auto colorLinearStates = VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(ui::Color::FromRGB(240, 240, 240))
                    ->Duration(200)
                ->EndAnimation()
            ->EndState()
            
            ->State("MouseOver")
                ->ColorKeyFrameAnimation("RootBorder", "Background.Color")
                    ->LinearKeyFrame(0, ui::Color::FromRGB(240, 240, 240))    // 灰色
                    ->LinearKeyFrame(100, ui::Color::FromRGB(100, 150, 255))  // 蓝色
                    ->LinearKeyFrame(200, ui::Color::FromRGB(100, 255, 150))  // 绿色
                    ->LinearKeyFrame(300, ui::Color::FromRGB(255, 255, 100))  // 黄色
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        std::cout << "✓ 创建了颜色线性关键帧动画\n";
        std::cout << "  动画时长: 300ms\n";
        std::cout << "  关键帧数: 4个\n";
        std::cout << "  插值方式: 线性\n";
        std::cout << "  颜色变化: 灰色 -> 蓝色 -> 绿色 -> 黄色\n\n";
        
        // ===== 示例2: 颜色离散关键帧动画 (闪烁效果) =====
        std::cout << "【示例2】颜色离散关键帧动画\n";
        std::cout << "----------------------------------------\n";
        
        auto colorDiscreteStates = VisualStateBuilder::CreateGroup("CommonStates")
            ->State("Normal")
                ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(ui::Color::FromRGB(240, 240, 240))
                    ->Duration(100)
                ->EndAnimation()
            ->EndState()
            
            ->State("MouseOver")
                ->ColorKeyFrameAnimation("RootBorder", "Background.Color")
                    ->DiscreteKeyFrame(0, ui::Color::FromRGB(255, 0, 0))     // 红色
                    ->DiscreteKeyFrame(100, ui::Color::FromRGB(0, 255, 0))   // 绿色
                    ->DiscreteKeyFrame(200, ui::Color::FromRGB(0, 0, 255))   // 蓝色
                    ->DiscreteKeyFrame(300, ui::Color::FromRGB(255, 255, 0)) // 黄色
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        std::cout << "✓ 创建了颜色离散关键帧动画\n";
        std::cout << "  动画时长: 300ms\n";
        std::cout << "  关键帧数: 4个\n";
        std::cout << "  插值方式: 离散(瞬间跳变)\n";
        std::cout << "  颜色变化: 红 -> 绿 -> 蓝 -> 黄 (无过渡)\n\n";
        
        // ===== 示例3: 混合关键帧动画 =====
        std::cout << "【示例3】混合线性和离散关键帧\n";
        std::cout << "----------------------------------------\n";
        
        auto mixedStates = VisualStateBuilder::CreateGroup("CommonStates")
            ->State("MouseOver")
                ->ColorKeyFrameAnimation("RootBorder", "Background.Color")
                    ->LinearKeyFrame(0, ui::Color::FromRGB(240, 240, 240))    // 灰色
                    ->LinearKeyFrame(150, ui::Color::FromRGB(100, 150, 255))  // 线性过渡到蓝色
                    ->DiscreteKeyFrame(151, ui::Color::FromRGB(255, 100, 100)) // 突变到红色
                    ->LinearKeyFrame(300, ui::Color::FromRGB(100, 255, 150))  // 线性过渡到绿色
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        std::cout << "✓ 创建了混合关键帧动画\n";
        std::cout << "  动画时长: 300ms\n";
        std::cout << "  关键帧数: 4个\n";
        std::cout << "  插值方式: 混合 (线性+离散)\n";
        std::cout << "  效果: 平滑渐变 -> 突然跳变 -> 平滑渐变\n\n";
        
        // ===== 示例4: 双精度关键帧动画 =====
        std::cout << "【示例4】双精度关键帧动画\n";
        std::cout << "----------------------------------------\n";
        
        auto doubleStates = VisualStateBuilder::CreateGroup("CommonStates")
            ->State("MouseOver")
                ->DoubleKeyFrameAnimation("RootBorder", "Opacity")
                    ->LinearKeyFrame(0, 1.0)
                    ->LinearKeyFrame(200, 0.3)   // 先变透明
                    ->LinearKeyFrame(400, 0.8)   // 再变不透明
                    ->LinearKeyFrame(600, 0.5)   // 再变半透明
                    ->LinearKeyFrame(800, 1.0)   // 最后完全不透明
                ->EndAnimation()
            ->EndState()
            ->Build();
        
        std::cout << "✓ 创建了透明度关键帧动画\n";
        std::cout << "  动画时长: 800ms\n";
        std::cout << "  关键帧数: 5个\n";
        std::cout << "  插值方式: 线性\n";
        std::cout << "  透明度变化: 1.0 -> 0.3 -> 0.8 -> 0.5 -> 1.0\n\n";
        
        std::cout << "========================================\n";
        std::cout << "\n** API 使用总结 **\n";
        std::cout << "1. 使用 ColorKeyFrameAnimation() 创建颜色关键帧动画\n";
        std::cout << "2. 使用 DoubleKeyFrameAnimation() 创建双精度关键帧动画\n";
        std::cout << "3. 使用 PointKeyFrameAnimation() 创建点位置关键帧动画\n";
        std::cout << "4. 使用 ThicknessKeyFrameAnimation() 创建厚度关键帧动画\n";
        std::cout << "5. 使用 LinearKeyFrame(time, value) 添加线性关键帧\n";
        std::cout << "6. 使用 DiscreteKeyFrame(time, value) 添加离散关键帧\n";
        std::cout << "7. 每个关键帧指定时间(ms)和值\n";
        std::cout << "8. 动画总时长由最后一个关键帧的时间决定\n\n";
        
        std::cout << "✓ KeyFrame动画集成完成！\n";
        std::cout << "========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
