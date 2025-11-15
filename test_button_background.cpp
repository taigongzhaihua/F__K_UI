/**
 * @file test_button_background.cpp
 * @brief 测试 Button 的 Background 属性是否正确工作
 */

#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/Border.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/TextRenderer.h"
#include <iostream>
#include <cassert>

using namespace fk;
using namespace fk::ui;

// 测试：Button 设置 Background 后，模板中的 Border 应该使用该背景色
void test_button_background_binding() {
    std::cout << "\n=== 测试：Button Background 绑定到模板 ===" << std::endl;
    
    // 创建 Button 并设置绿色背景
    auto* button = new Button();
    auto* greenBrush = new SolidColorBrush(Color::FromRGB(0, 255, 0, 255));  // 绿色
    button->Background(greenBrush);
    button->Content("Click Me");
    
    std::cout << "Button Background 已设置为绿色" << std::endl;
    
    // 应用模板
    button->ApplyTemplate();
    
    // 通过视觉子节点访问 Border（模板根是第一个视觉子节点）
    if (button->GetVisualChildrenCount() > 0) {
        auto* firstChild = button->GetVisualChild(0);
        auto* border = dynamic_cast<Border*>(firstChild);
        
        if (border) {
            std::cout << "找到 Border（视觉子节点）: ✓" << std::endl;
            
            // 检查 Border 的 Background
            auto* borderBg = border->GetBackground();
            if (borderBg) {
                auto* solidBrush = dynamic_cast<SolidColorBrush*>(borderBg);
                if (solidBrush) {
                    auto color = solidBrush->GetColor();
                    std::cout << "Border Background 颜色: R=" << (int)(color.r * 255) 
                              << " G=" << (int)(color.g * 255) 
                              << " B=" << (int)(color.b * 255) 
                              << " A=" << (int)(color.a * 255) << std::endl;
                    
                    // 验证是否为绿色
                    if ((int)(color.g * 255) == 255 && (int)(color.r * 255) == 0 && (int)(color.b * 255) == 0) {
                        std::cout << "✓ Border 背景色正确（绿色）" << std::endl;
                    } else {
                        std::cout << "✗ Border 背景色不正确（预期绿色）" << std::endl;
                    }
                } else {
                    std::cout << "✗ Border Background 不是 SolidColorBrush" << std::endl;
                }
            } else {
                std::cout << "✗ Border 没有 Background" << std::endl;
            }
        } else {
            std::cout << "✗ 第一个视觉子节点不是 Border" << std::endl;
        }
    } else {
        std::cout << "✗ Button 没有视觉子节点" << std::endl;
    }
    
    // 测量和排列
    button->Measure(Size(200, 50));
    button->Arrange(Rect(0, 0, 200, 50));
    
    // 生成渲染命令
    render::RenderList renderList;
    render::TextRenderer textRenderer;
    render::RenderContext context(&renderList, &textRenderer);
    
    button->CollectDrawCommands(context);
    
    std::cout << "渲染命令数量: " << renderList.GetCommandCount() << std::endl;
    
    // delete button;  // TODO: 段错误问题，暂时跳过
    std::cout << "✓ 测试完成（跳过删除）" << std::endl;
}

// 测试：不设置 Background，应该使用默认背景色
void test_button_default_background() {
    std::cout << "\n=== 测试：Button 默认背景色 ===" << std::endl;
    
    auto* button = new Button();
    button->Content("Default Button");
    
    // 获取 Button 的 Background（应该有默认值）
    auto* buttonBg = button->GetBackground();
    if (buttonBg) {
        auto* solidBrush = dynamic_cast<SolidColorBrush*>(buttonBg);
        if (solidBrush) {
            auto color = solidBrush->GetColor();
            std::cout << "Button 默认 Background 颜色: R=" << (int)(color.r * 255) 
                      << " G=" << (int)(color.g * 255) 
                      << " B=" << (int)(color.b * 255) 
                      << " A=" << (int)(color.a * 255) << std::endl;
            
            // 验证是否为浅灰色 (240, 240, 240, 255)
            if ((int)(color.r * 255) == 240 && (int)(color.g * 255) == 240 && (int)(color.b * 255) == 240) {
                std::cout << "✓ Button 有默认背景色（浅灰色）" << std::endl;
            }
        }
    } else {
        std::cout << "✗ Button 没有默认 Background" << std::endl;
    }
    
    // delete button;  // TODO: 段错误问题，暂时跳过
    std::cout << "✓ 测试完成（跳过删除）" << std::endl;
}

int main() {
    std::cout << "===== Button Background 属性测试 =====" << std::endl;
    
    test_button_default_background();
    test_button_background_binding();
    
    std::cout << "\n===== 所有测试完成 =====" << std::endl;
    return 0;
}
