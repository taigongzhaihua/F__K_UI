/**
 * @file example_panel_backgrounds.cpp
 * @brief 展示 Panel (StackPanel/Grid) 的 Background 和 CornerRadius 功能
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>

using namespace fk::ui;

int main() {
    try {
        fk::Application app;
        auto window = new Window();
        
        window->Title("Panel Background & CornerRadius Demo")
              ->Width(900)
              ->Height(700);
        
        // 主容器：垂直 StackPanel
        auto mainPanel = (new StackPanel())
            ->Orientation(Orientation::Vertical)
            ->Background(new SolidColorBrush(Color(0.95f, 0.95f, 0.95f, 1.0f)))
            ->Padding(Thickness(20));
        
        // ===== 标题 =====
        mainPanel->AddChild(
            (new TextBlock())
                ->Text("Panel Background & CornerRadius Examples")
                ->FontSize(24)
                ->Foreground(new SolidColorBrush(Color(0.2f, 0.2f, 0.2f, 1.0f)))
                ->Margin(Thickness(0, 0, 0, 20))
        );
        
        // ===== 示例 1: StackPanel 统一圆角 =====
        mainPanel->AddChild(
            (new Border())
                ->Background(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                ->BorderBrush(new SolidColorBrush(Color(0.8f, 0.8f, 0.8f, 1.0f)))
                ->BorderThickness(Thickness(1))
                ->CornerRadius(CornerRadius(8))
                ->Padding(Thickness(15))
                ->Margin(Thickness(0, 0, 0, 15))
                ->Child(
                    (new StackPanel())
                        ->Orientation(Orientation::Vertical)
                        ->Children({
                            (new TextBlock())
                                ->Text("1. StackPanel with Uniform Corner Radius")
                                ->FontSize(16)
                                ->FontWeight(FontWeight::Bold)
                                ->Margin(Thickness(0, 0, 0, 10)),
                            
                            (new StackPanel())
                                ->Orientation(Orientation::Vertical)
                                ->Background(new SolidColorBrush(Color(0.4f, 0.7f, 0.9f, 1.0f)))
                                ->WithCornerRadius(10.0f)  // 统一圆角
                                ->Padding(Thickness(15))
                                ->Children({
                                    (new TextBlock())
                                        ->Text("Item 1")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->FontSize(14),
                                    (new TextBlock())
                                        ->Text("Item 2")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->FontSize(14)
                                        ->Margin(Thickness(0, 5, 0, 0)),
                                    (new TextBlock())
                                        ->Text("Item 3")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->FontSize(14)
                                        ->Margin(Thickness(0, 5, 0, 0))
                                })
                        })
                )
        );
        
        // ===== 示例 2: Grid 四角不同圆角 =====
        mainPanel->AddChild(
            (new Border())
                ->Background(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                ->BorderBrush(new SolidColorBrush(Color(0.8f, 0.8f, 0.8f, 1.0f)))
                ->BorderThickness(Thickness(1))
                ->CornerRadius(CornerRadius(8))
                ->Padding(Thickness(15))
                ->Margin(Thickness(0, 0, 0, 15))
                ->Child(
                    (new StackPanel())
                        ->Orientation(Orientation::Vertical)
                        ->Children({
                            (new TextBlock())
                                ->Text("2. Grid with Independent Corner Radii")
                                ->FontSize(16)
                                ->FontWeight(FontWeight::Bold)
                                ->Margin(Thickness(0, 0, 0, 10)),
                            
                            (new Grid())
                                ->Background(new SolidColorBrush(Color(0.9f, 0.7f, 0.4f, 1.0f)))
                                ->WithCornerRadius(CornerRadius(30, 10, 20, 5))  // TL=30, TR=10, BR=20, BL=5
                                ->Width(400)
                                ->Height(150)
                                ->Children({
                                    (new TextBlock())
                                        ->Text("TopLeft: 30px")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->HorizontalAlignment(HorizontalAlignment::Left)
                                        ->VerticalAlignment(VerticalAlignment::Top)
                                        ->Margin(Thickness(15, 10, 0, 0)),
                                    (new TextBlock())
                                        ->Text("TopRight: 10px")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->HorizontalAlignment(HorizontalAlignment::Right)
                                        ->VerticalAlignment(VerticalAlignment::Top)
                                        ->Margin(Thickness(0, 10, 15, 0)),
                                    (new TextBlock())
                                        ->Text("BottomRight: 20px")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->HorizontalAlignment(HorizontalAlignment::Right)
                                        ->VerticalAlignment(VerticalAlignment::Bottom)
                                        ->Margin(Thickness(0, 0, 15, 10)),
                                    (new TextBlock())
                                        ->Text("BottomLeft: 5px")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->HorizontalAlignment(HorizontalAlignment::Left)
                                        ->VerticalAlignment(VerticalAlignment::Bottom)
                                        ->Margin(Thickness(15, 0, 0, 10))
                                })
                        })
                )
        );
        
        // ===== 示例 3: Border 对比 =====
        mainPanel->AddChild(
            (new Border())
                ->Background(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                ->BorderBrush(new SolidColorBrush(Color(0.8f, 0.8f, 0.8f, 1.0f)))
                ->BorderThickness(Thickness(1))
                ->CornerRadius(CornerRadius(8))
                ->Padding(Thickness(15))
                ->Margin(Thickness(0, 0, 0, 15))
                ->Child(
                    (new StackPanel())
                        ->Orientation(Orientation::Vertical)
                        ->Children({
                            (new TextBlock())
                                ->Text("3. Border with Custom Corner Radii & Border")
                                ->FontSize(16)
                                ->FontWeight(FontWeight::Bold)
                                ->Margin(Thickness(0, 0, 0, 10)),
                            
                            (new Border())
                                ->Background(new SolidColorBrush(Color(0.7f, 0.4f, 0.9f, 1.0f)))
                                ->BorderBrush(new SolidColorBrush(Color(0.5f, 0.2f, 0.7f, 1.0f)))
                                ->BorderThickness(Thickness(3))
                                ->CornerRadius(CornerRadius(25, 5, 25, 5))  // 交替圆角
                                ->Width(400)
                                ->Height(120)
                                ->Child(
                                    (new TextBlock())
                                        ->Text("Border with alternating corners (25, 5, 25, 5)")
                                        ->Foreground(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                                        ->HorizontalAlignment(HorizontalAlignment::Center)
                                        ->VerticalAlignment(VerticalAlignment::Center)
                                )
                        })
                )
        );
        
        // ===== 示例 4: 嵌套组合 =====
        mainPanel->AddChild(
            (new Border())
                ->Background(new SolidColorBrush(Color(1.0f, 1.0f, 1.0f, 1.0f)))
                ->BorderBrush(new SolidColorBrush(Color(0.8f, 0.8f, 0.8f, 1.0f)))
                ->BorderThickness(Thickness(1))
                ->CornerRadius(CornerRadius(8))
                ->Padding(Thickness(15))
                ->Child(
                    (new StackPanel())
                        ->Orientation(Orientation::Vertical)
                        ->Children({
                            (new TextBlock())
                                ->Text("4. Nested Panels with Different Backgrounds")
                                ->FontSize(16)
                                ->FontWeight(FontWeight::Bold)
                                ->Margin(Thickness(0, 0, 0, 10)),
                            
                            (new StackPanel())
                                ->Orientation(Orientation::Horizontal)
                                ->Background(new SolidColorBrush(Color(0.9f, 0.9f, 0.7f, 1.0f)))
                                ->WithCornerRadius(8.0f)
                                ->Padding(Thickness(10))
                                ->Children({
                                    (new StackPanel())
                                        ->Background(new SolidColorBrush(Color(1.0f, 0.8f, 0.8f, 1.0f)))
                                        ->WithCornerRadius(6.0f)
                                        ->Padding(Thickness(10))
                                        ->Margin(Thickness(0, 0, 10, 0))
                                        ->Children({
                                            (new TextBlock())->Text("Red Panel")->FontSize(12),
                                            (new TextBlock())->Text("Nested")->FontSize(10)->Margin(Thickness(0, 5, 0, 0))
                                        }),
                                    
                                    (new StackPanel())
                                        ->Background(new SolidColorBrush(Color(0.8f, 1.0f, 0.8f, 1.0f)))
                                        ->WithCornerRadius(6.0f)
                                        ->Padding(Thickness(10))
                                        ->Margin(Thickness(0, 0, 10, 0))
                                        ->Children({
                                            (new TextBlock())->Text("Green Panel")->FontSize(12),
                                            (new TextBlock())->Text("Nested")->FontSize(10)->Margin(Thickness(0, 5, 0, 0))
                                        }),
                                    
                                    (new StackPanel())
                                        ->Background(new SolidColorBrush(Color(0.8f, 0.8f, 1.0f, 1.0f)))
                                        ->WithCornerRadius(6.0f)
                                        ->Padding(Thickness(10))
                                        ->Children({
                                            (new TextBlock())->Text("Blue Panel")->FontSize(12),
                                            (new TextBlock())->Text("Nested")->FontSize(10)->Margin(Thickness(0, 5, 0, 0))
                                        })
                                })
                        })
                )
        );
        
        window->SetContent(mainPanel);
        
        std::cout << "Panel Background & CornerRadius Demo\n";
        std::cout << "Features demonstrated:\n";
        std::cout << "1. StackPanel with uniform corner radius\n";
        std::cout << "2. Grid with independent corner radii (4 different values)\n";
        std::cout << "3. Border with custom corner radii and border\n";
        std::cout << "4. Nested panels with different backgrounds\n\n";
        
        return app.Run(window);
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
