/**
 * @file example_panel_backgrounds_simple.cpp
 * @brief 简单展示 Panel Background 和 CornerRadius 功能
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>
#include <memory>

int main() {
    try {
        fk::Application app;
        auto window = std::make_shared<fk::ui::Window>();
        
        window->Title("Panel Background & CornerRadius Demo")
              ->Width(800)
              ->Height(600);
        
        // 主容器
        auto mainPanel = new fk::ui::StackPanel();
        mainPanel->SetOrientation(fk::ui::Orientation::Vertical);
        mainPanel->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(0.95f, 0.95f, 0.95f, 1.0f)));
        mainPanel->SetPadding(fk::Thickness(20));
        
        // 标题
        auto title = new fk::ui::TextBlock();
        title->SetText("Panel Background & CornerRadius Examples");
        title->SetFontSize(24);
        title->SetForeground(new fk::ui::SolidColorBrush(fk::ui::Color(0.2f, 0.2f, 0.2f, 1.0f)));
        title->SetMargin(fk::Thickness(0, 0, 0, 20));
        mainPanel->AddChild(title);
        
        // ===== 示例 1: StackPanel 统一圆角 =====
        auto section1 = new fk::ui::Border();
        section1->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        section1->SetBorderBrush(new fk::ui::SolidColorBrush(fk::ui::Color(0.8f, 0.8f, 0.8f, 1.0f)));
        section1->SetBorderThickness(fk::Thickness(1));
        section1->SetCornerRadius(fk::ui::CornerRadius(8));
        section1->SetPadding(fk::Thickness(15));
        section1->SetMargin(fk::Thickness(0, 0, 0, 15));
        
        auto section1Content = new fk::ui::StackPanel();
        section1Content->SetOrientation(fk::ui::Orientation::Vertical);
        
        auto label1 = new fk::ui::TextBlock();
        label1->SetText("1. StackPanel with Uniform Corner Radius");
        label1->SetFontSize(16);
        label1->SetFontWeight(fk::ui::FontWeight::Bold);
        label1->SetMargin(fk::Thickness(0, 0, 0, 10));
        section1Content->AddChild(label1);
        
        auto panel1 = new fk::ui::StackPanel();
        panel1->SetOrientation(fk::ui::Orientation::Vertical);
        panel1->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(0.4f, 0.7f, 0.9f, 1.0f)));
        panel1->SetCornerRadius(fk::ui::CornerRadius(10.0f));  // 统一圆角
        panel1->SetPadding(fk::Thickness(15));
        panel1->SetWidth(400);
        panel1->SetHeight(100);
        
        auto item1 = new fk::ui::TextBlock();
        item1->SetText("Item 1");
        item1->SetForeground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        panel1->AddChild(item1);
        
        auto item2 = new fk::ui::TextBlock();
        item2->SetText("Item 2");
        item2->SetForeground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        item2->SetMargin(fk::Thickness(0, 5, 0, 0));
        panel1->AddChild(item2);
        
        section1Content->AddChild(panel1);
        section1->SetChild(section1Content);
        mainPanel->AddChild(section1);
        
        // ===== 示例 2: Grid 四角不同圆角 =====
        auto section2 = new fk::ui::Border();
        section2->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        section2->SetBorderBrush(new fk::ui::SolidColorBrush(fk::ui::Color(0.8f, 0.8f, 0.8f, 1.0f)));
        section2->SetBorderThickness(fk::Thickness(1));
        section2->SetCornerRadius(fk::ui::CornerRadius(8));
        section2->SetPadding(fk::Thickness(15));
        section2->SetMargin(fk::Thickness(0, 0, 0, 15));
        
        auto section2Content = new fk::ui::StackPanel();
        section2Content->SetOrientation(fk::ui::Orientation::Vertical);
        
        auto label2 = new fk::ui::TextBlock();
        label2->SetText("2. Grid with Independent Corner Radii (30, 10, 20, 5)");
        label2->SetFontSize(16);
        label2->SetFontWeight(fk::ui::FontWeight::Bold);
        label2->SetMargin(fk::Thickness(0, 0, 0, 10));
        section2Content->AddChild(label2);
        
        auto grid1 = new fk::ui::Grid();
        grid1->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(0.9f, 0.7f, 0.4f, 1.0f)));
        grid1->SetCornerRadius(fk::ui::CornerRadius(30, 10, 20, 5));  // 四角不同
        grid1->SetWidth(400);
        grid1->SetHeight(120);
        
        auto gridLabel = new fk::ui::TextBlock();
        gridLabel->SetText("Four different corner radii");
        gridLabel->SetForeground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        gridLabel->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Center);
        gridLabel->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
        grid1->AddChild(gridLabel);
        
        section2Content->AddChild(grid1);
        section2->SetChild(section2Content);
        mainPanel->AddChild(section2);
        
        // ===== 示例 3: Border 对比 =====
        auto section3 = new fk::ui::Border();
        section3->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        section3->SetBorderBrush(new fk::ui::SolidColorBrush(fk::ui::Color(0.8f, 0.8f, 0.8f, 1.0f)));
        section3->SetBorderThickness(fk::Thickness(1));
        section3->SetCornerRadius(fk::ui::CornerRadius(8));
        section3->SetPadding(fk::Thickness(15));
        
        auto section3Content = new fk::ui::StackPanel();
        section3Content->SetOrientation(fk::ui::Orientation::Vertical);
        
        auto label3 = new fk::ui::TextBlock();
        label3->SetText("3. Border with Alternating Corners (25, 5, 25, 5)");
        label3->SetFontSize(16);
        label3->SetFontWeight(fk::ui::FontWeight::Bold);
        label3->SetMargin(fk::Thickness(0, 0, 0, 10));
        section3Content->AddChild(label3);
        
        auto border1 = new fk::ui::Border();
        border1->SetBackground(new fk::ui::SolidColorBrush(fk::ui::Color(0.7f, 0.4f, 0.9f, 1.0f)));
        border1->SetBorderBrush(new fk::ui::SolidColorBrush(fk::ui::Color(0.5f, 0.2f, 0.7f, 1.0f)));
        border1->SetBorderThickness(fk::Thickness(3));
        border1->SetCornerRadius(fk::ui::CornerRadius(25, 5, 25, 5));
        border1->SetWidth(400);
        border1->SetHeight(100);
        
        auto borderLabel = new fk::ui::TextBlock();
        borderLabel->SetText("Border with alternating corners");
        borderLabel->SetForeground(new fk::ui::SolidColorBrush(fk::ui::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        borderLabel->SetHorizontalAlignment(fk::ui::HorizontalAlignment::Center);
        borderLabel->SetVerticalAlignment(fk::ui::VerticalAlignment::Center);
        border1->SetChild(borderLabel);
        
        section3Content->AddChild(border1);
        section3->SetChild(section3Content);
        mainPanel->AddChild(section3);
        
        window->SetContent(mainPanel);
        
        std::cout << "Panel Background & CornerRadius Demo\n";
        std::cout << "Features demonstrated:\n";
        std::cout << "1. StackPanel with uniform corner radius (10px)\n";
        std::cout << "2. Grid with independent corner radii (30, 10, 20, 5)\n";
        std::cout << "3. Border with alternating corners (25, 5, 25, 5)\n\n";
        std::cout << "Close the window to exit.\n";
        
        app.Run(window);
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
