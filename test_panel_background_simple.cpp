/**
 * @file test_panel_background_simple.cpp
 * @brief 简单测试 Panel Background 和 CornerRadius 功能
 */

#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/render/RenderContext.h"
#include "fk/render/RenderList.h"
#include "fk/render/RenderCommand.h"
#include "fk/render/TextRenderer.h"
#include <iostream>

using namespace fk::ui;

int main() {
    std::cout << "=== 测试 Panel Background 和 CornerRadius ===\n\n";
    
    // 测试 1: StackPanel 带统一圆角
    {
        std::cout << "--- 测试 1: StackPanel 带背景和统一圆角 ---\n";
        
        auto window = new Window();
        auto brush = new SolidColorBrush(Color(0.9f, 0.9f, 0.9f, 1.0f));  // 浅灰色
        auto stackPanel = (new StackPanel())
            ->Background(brush)
            ->WithCornerRadius(10.0f)  // 统一圆角
            ->Width(200)
            ->Height(150);
        
        window->SetContent(stackPanel);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        fk::render::RenderList renderList;
        fk::render::TextRenderer textRenderer;
        fk::render::RenderContext context(&renderList, &textRenderer);
        window->CollectDrawCommands(context);
        
        // 查找 DrawRectangle 命令
        const auto& commands = renderList.GetCommands();
        bool found = false;
        for (const auto& cmd : commands) {
            if (cmd.type == fk::render::CommandType::DrawRectangle) {
                const auto& payload = std::get<fk::render::RectanglePayload>(cmd.payload);
                std::cout << "✓ 找到 DrawRectangle 命令\n";
                std::cout << "  尺寸: " << payload.rect.width << " x " << payload.rect.height << "\n";
                std::cout << "  圆角: TL=" << payload.cornerRadius.topLeft
                         << ", TR=" << payload.cornerRadius.topRight
                         << ", BR=" << payload.cornerRadius.bottomRight
                         << ", BL=" << payload.cornerRadius.bottomLeft << "\n";
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "✗ 未找到 DrawRectangle 命令\n";
        }
        
        delete window;
        std::cout << "\n";
    }
    
    // 测试 2: Grid 带四角不同圆角
    {
        std::cout << "--- 测试 2: Grid 带四角不同圆角 ---\n";
        
        auto window = new Window();
        auto brush = new SolidColorBrush(Color(1.0f, 0.9f, 0.7f, 1.0f));  // 米色
        auto grid = (new Grid())
            ->Background(brush)
            ->WithCornerRadius(CornerRadius(20, 10, 5, 15))  // 四角不同
            ->Width(200)
            ->Height(150);
        
        window->SetContent(grid);
        window->Measure(Size(800, 600));
        window->Arrange(Rect(0, 0, 800, 600));
        
        fk::render::RenderList renderList;
        fk::render::TextRenderer textRenderer;
        fk::render::RenderContext context(&renderList, &textRenderer);
        window->CollectDrawCommands(context);
        
        const auto& commands = renderList.GetCommands();
        bool found = false;
        for (const auto& cmd : commands) {
            if (cmd.type == fk::render::CommandType::DrawRectangle) {
                const auto& payload = std::get<fk::render::RectanglePayload>(cmd.payload);
                std::cout << "✓ 找到 DrawRectangle 命令\n";
                std::cout << "  圆角: TL=" << payload.cornerRadius.topLeft
                         << ", TR=" << payload.cornerRadius.topRight
                         << ", BR=" << payload.cornerRadius.bottomRight
                         << ", BL=" << payload.cornerRadius.bottomLeft << "\n";
                
                if (payload.cornerRadius.topLeft == 20 &&
                    payload.cornerRadius.topRight == 10 &&
                    payload.cornerRadius.bottomRight == 5 &&
                    payload.cornerRadius.bottomLeft == 15) {
                    std::cout << "✓ 圆角值正确！\n";
                } else {
                    std::cout << "✗ 圆角值不正确\n";
                }
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "✗ 未找到 DrawRectangle 命令\n";
        }
        
        delete window;
        std::cout << "\n";
    }
    
    std::cout << "=== 测试完成 ===\n";
    return 0;
}
