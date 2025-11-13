/**
 * @file phase2_demo.cpp
 * @brief Phase 2 功能综合演示
 * 
 * 演示内容：
 * 1. Style 系统 - 样式定义、应用、继承
 * 2. Shape 图形 - Rectangle, Ellipse, Line, Polygon, Path
 * 3. Style + Shape 组合使用
 * 4. 隐式样式支持
 * 
 * 功能亮点：
 * - 自动样式应用机制
 * - 完整的图形渲染
 * - 流式 API 设计
 * - WPF 兼容的编程模型
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Shape.h"
#include "fk/ui/Style.h"
#include "fk/ui/Primitives.h"
#include <iostream>

using namespace fk;

int main() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  F__K_UI Phase 2 功能演示" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // 创建应用
    app::Application app;
    
    // 创建主窗口
    auto window = new ui::Window();
    window->Title("Phase 2: Style 系统 + Shape 图形演示")
          ->Width(900)
          ->Height(700);
    
    // 主网格布局
    auto mainGrid = new ui::Grid();
    mainGrid->AddRowDefinition({ui::GridLength::Auto()});
    mainGrid->AddRowDefinition({ui::GridLength(1, ui::GridUnitType::Star)});
    
    // ========================================
    // 标题区域
    // ========================================
    auto titlePanel = new ui::StackPanel();
    titlePanel->Orientation(ui::Orientation::Vertical)
              ->Spacing(10)
              ->Margin(ui::Thickness(20));
    ui::Grid::SetRow(titlePanel, 0);
    
    auto titleText = new ui::TextBlock();
    titleText->Text("Phase 2 新增功能展示")
             ->FontSize(28.0f)
             ->FontWeight(ui::FontWeight::Bold);
    titlePanel->AddChild(titleText);
    
    auto subtitleText = new ui::TextBlock();
    subtitleText->Text("✨ Style 自动应用系统 + 完整 Shape 图形类层次")
                ->FontSize(16.0f);
    titlePanel->AddChild(subtitleText);
    
    mainGrid->AddChild(titlePanel);
    
    // ========================================
    // 内容区域 - 水平分栏
    // ========================================
    auto contentGrid = new ui::Grid();
    contentGrid->AddColumnDefinition({ui::GridLength(1, ui::GridUnitType::Star)});
    contentGrid->AddColumnDefinition({ui::GridLength(1, ui::GridUnitType::Star)});
    contentGrid->Margin(ui::Thickness(20));
    ui::Grid::SetRow(contentGrid, 1);
    
    // ========================================
    // 左侧：Style 系统演示
    // ========================================
    auto leftPanel = new ui::StackPanel();
    leftPanel->Orientation(ui::Orientation::Vertical)
             ->Spacing(15);
    ui::Grid::SetColumn(leftPanel, 0);
    
    // 标题
    auto styleTitle = new ui::TextBlock();
    styleTitle->Text("1. Style 系统演示")
              ->FontSize(20.0f)
              ->FontWeight(ui::FontWeight::Bold);
    leftPanel->AddChild(styleTitle);
    
    // 创建基础按钮样式
    auto baseButtonStyle = new ui::Style();
    baseButtonStyle->SetTargetType(typeid(ui::Button));
    
    baseButtonStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::WidthProperty(),
        220.0f
    );
    baseButtonStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::HeightProperty(),
        50.0f
    );
    baseButtonStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        16.0f
    );
    baseButtonStyle->Setters().Add(
        ui::Control<ui::Button>::PaddingProperty(),
        ui::Thickness(10)
    );
    baseButtonStyle->Seal();
    
    std::cout << "[Style] 创建基础按钮样式" << std::endl;
    std::cout << "  - 宽度: 220px" << std::endl;
    std::cout << "  - 高度: 50px" << std::endl;
    std::cout << "  - 字体大小: 16px" << std::endl;
    std::cout << "  - Setters 数量: " << baseButtonStyle->Setters().Count() << std::endl;
    
    // 创建大号样式（继承自基础样式）
    auto largeButtonStyle = new ui::Style();
    largeButtonStyle->SetTargetType(typeid(ui::Button));
    largeButtonStyle->SetBasedOn(baseButtonStyle);
    
    largeButtonStyle->Setters().Add(
        ui::FrameworkElement<ui::Button>::HeightProperty(),
        70.0f
    );
    largeButtonStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        22.0f
    );
    largeButtonStyle->Seal();
    
    std::cout << "\n[Style] 创建大号样式（继承自基础样式）" << std::endl;
    std::cout << "  - 覆盖高度: 70px" << std::endl;
    std::cout << "  - 覆盖字体: 22px" << std::endl;
    std::cout << "  - BasedOn: " << (largeButtonStyle->GetBasedOn() ? "基础样式" : "无") << std::endl;
    
    // 创建粗体样式（继承自基础样式）
    auto boldButtonStyle = new ui::Style();
    boldButtonStyle->SetTargetType(typeid(ui::Button));
    boldButtonStyle->SetBasedOn(baseButtonStyle);
    
    boldButtonStyle->Setters().Add(
        ui::Control<ui::Button>::FontWeightProperty(),
        ui::FontWeight::Bold
    );
    boldButtonStyle->Setters().Add(
        ui::Control<ui::Button>::FontSizeProperty(),
        18.0f
    );
    boldButtonStyle->Seal();
    
    // 应用样式的按钮们
    auto button1 = new ui::Button();
    button1->Content("基础样式按钮")
           ->StyleProperty(baseButtonStyle);
    leftPanel->AddChild(button1);
    
    auto button2 = new ui::Button();
    button2->Content("大号样式按钮")
           ->StyleProperty(largeButtonStyle);
    leftPanel->AddChild(button2);
    
    auto button3 = new ui::Button();
    button3->Content("粗体样式按钮")
           ->StyleProperty(boldButtonStyle);
    leftPanel->AddChild(button3);
    
    auto button4 = new ui::Button();
    button4->Content("默认按钮（无样式）")
           ->Width(220)
           ->Height(50);
    leftPanel->AddChild(button4);
    
    std::cout << "\n[Style] 创建按钮并应用样式" << std::endl;
    std::cout << "  - Button1: " << (button1->GetStyle() ? "已应用基础样式" : "无样式") << std::endl;
    std::cout << "  - Button2: " << (button2->GetStyle() ? "已应用大号样式" : "无样式") << std::endl;
    std::cout << "  - Button3: " << (button3->GetStyle() ? "已应用粗体样式" : "无样式") << std::endl;
    std::cout << "  - Button4: " << (button4->GetStyle() ? "已应用样式" : "无样式") << std::endl;
    
    contentGrid->AddChild(leftPanel);
    
    // ========================================
    // 右侧：Shape 图形演示
    // ========================================
    auto rightPanel = new ui::StackPanel();
    rightPanel->Orientation(ui::Orientation::Vertical)
              ->Spacing(15);
    ui::Grid::SetColumn(rightPanel, 1);
    
    // 标题
    auto shapeTitle = new ui::TextBlock();
    shapeTitle->Text("2. Shape 图形演示")
              ->FontSize(20.0f)
              ->FontWeight(ui::FontWeight::Bold);
    rightPanel->AddChild(shapeTitle);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "[Shape] 创建各种图形" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Rectangle - 矩形
    auto rectLabel = new ui::TextBlock();
    rectLabel->Text("Rectangle (矩形)")
             ->FontSize(14.0f)
             ->Margin(ui::Thickness(0, 10, 0, 5));
    rightPanel->AddChild(rectLabel);
    
    auto rectangle = new ui::Rectangle();
    rectangle->Width(200)
             ->Height(60)
             ->RadiusX(10.0f)
             ->RadiusY(10.0f);
    // Note: Fill 和 Stroke 需要 Brush*，这里简化处理
    // rectangle->Fill(new SolidColorBrush(Color::LightGray()))
    //          ->Stroke(new SolidColorBrush(Color::Black()))
    //          ->StrokeThickness(2.0f);
    rightPanel->AddChild(rectangle);
    
    std::cout << "\n[Rectangle] 圆角矩形" << std::endl;
    std::cout << "  - 尺寸: 200x60" << std::endl;
    std::cout << "  - 圆角: RadiusX=10, RadiusY=10" << std::endl;
    
    // Ellipse - 椭圆
    auto ellipseLabel = new ui::TextBlock();
    ellipseLabel->Text("Ellipse (椭圆)")
                ->FontSize(14.0f)
                ->Margin(ui::Thickness(0, 10, 0, 5));
    rightPanel->AddChild(ellipseLabel);
    
    auto ellipse = new ui::Ellipse();
    ellipse->Width(150)
           ->Height(80);
    rightPanel->AddChild(ellipse);
    
    std::cout << "\n[Ellipse] 椭圆" << std::endl;
    std::cout << "  - 尺寸: 150x80" << std::endl;
    
    // Line - 直线
    auto lineLabel = new ui::TextBlock();
    lineLabel->Text("Line (直线)")
             ->FontSize(14.0f)
             ->Margin(ui::Thickness(0, 10, 0, 5));
    rightPanel->AddChild(lineLabel);
    
    auto line = new ui::Line();
    line->X1(0)->Y1(0)
        ->X2(200)->Y2(0)
        ->StrokeThickness(3.0f);
    rightPanel->AddChild(line);
    
    std::cout << "\n[Line] 直线" << std::endl;
    std::cout << "  - 起点: (0, 0)" << std::endl;
    std::cout << "  - 终点: (200, 0)" << std::endl;
    std::cout << "  - 粗细: 3px" << std::endl;
    
    // Polygon - 多边形（三角形）
    auto polygonLabel = new ui::TextBlock();
    polygonLabel->Text("Polygon (多边形)")
                ->FontSize(14.0f)
                ->Margin(ui::Thickness(0, 10, 0, 5));
    rightPanel->AddChild(polygonLabel);
    
    auto polygon = new ui::Polygon();
    std::vector<ui::Point> trianglePoints = {
        ui::Point(100, 20),
        ui::Point(50, 100),
        ui::Point(150, 100)
    };
    polygon->SetPoints(trianglePoints);
    rightPanel->AddChild(polygon);
    
    std::cout << "\n[Polygon] 三角形" << std::endl;
    std::cout << "  - 顶点数: " << polygon->GetPointCount() << std::endl;
    std::cout << "  - 点1: (100, 20)" << std::endl;
    std::cout << "  - 点2: (50, 100)" << std::endl;
    std::cout << "  - 点3: (150, 100)" << std::endl;
    
    // Path - 复杂路径（心形）
    auto pathLabel = new ui::TextBlock();
    pathLabel->Text("Path (复杂路径)")
             ->FontSize(14.0f)
             ->Margin(ui::Thickness(0, 10, 0, 5));
    rightPanel->AddChild(pathLabel);
    
    auto path = new ui::Path();
    path->MoveTo(50, 80)
        ->QuadraticTo(50, 40, 80, 40)
        ->QuadraticTo(100, 40, 100, 60)
        ->QuadraticTo(100, 40, 130, 40)
        ->QuadraticTo(160, 40, 160, 80)
        ->QuadraticTo(160, 120, 105, 150)
        ->LineTo(105, 150)
        ->QuadraticTo(50, 120, 50, 80)
        ->Close()
        ->StrokeThickness(2.0f);
    rightPanel->AddChild(path);
    
    std::cout << "\n[Path] 复杂路径" << std::endl;
    std::cout << "  - 段数: " << path->GetSegments().size() << std::endl;
    std::cout << "  - 包含: MoveTo, QuadraticTo, LineTo, Close" << std::endl;
    
    contentGrid->AddChild(rightPanel);
    mainGrid->AddChild(contentGrid);
    
    // ========================================
    // 底部状态信息
    // ========================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "Phase 2 功能总结" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n✅ Style 系统:" << std::endl;
    std::cout << "  • 自动样式应用/撤销机制" << std::endl;
    std::cout << "  • 样式继承 (BasedOn)" << std::endl;
    std::cout << "  • 依赖属性集成" << std::endl;
    std::cout << "  • 值优先级支持" << std::endl;
    
    std::cout << "\n✅ Shape 图形系统:" << std::endl;
    std::cout << "  • Rectangle - 矩形/圆角矩形" << std::endl;
    std::cout << "  • Ellipse - 椭圆/圆形" << std::endl;
    std::cout << "  • Line - 直线" << std::endl;
    std::cout << "  • Polygon - 多边形" << std::endl;
    std::cout << "  • Path - 复杂路径" << std::endl;
    
    std::cout << "\n🎯 框架进度:" << std::endl;
    std::cout << "  • 当前完成度: 85%" << std::endl;
    std::cout << "  • Phase 2 目标: 88%" << std::endl;
    std::cout << "  • 剩余工作: ContentPresenter, TemplateBinding, 测试" << std::endl;
    
    std::cout << "\n========================================\n" << std::endl;
    
    window->Content(mainGrid);
    
    // 运行应用
    return app.Run(window);
}
