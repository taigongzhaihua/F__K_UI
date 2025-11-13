/**
 * @file gradient_brush_demo.cpp
 * @brief 渐变画刷系统演示
 * 
 * 本演示展示 F__K_UI 的渐变画刷功能：
 * 1. LinearGradientBrush - 线性渐变
 * 2. RadialGradientBrush - 径向渐变
 * 3. ImageBrush - 图像画刷
 * 4. 多个渐变停止点
 */

#include "fk/ui/Brush.h"
#include "fk/ui/Shape.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include <iostream>
#include <memory>

using namespace fk::ui;

void PrintSeparator() {
    std::cout << "\n" << std::string(60, '=') << "\n\n";
}

void Test1_LinearGradientBasic() {
    std::cout << "测试 1: 基础线性渐变\n";
    std::cout << "-------------------\n";
    
    // 创建从红到蓝的线性渐变
    auto gradient = new LinearGradientBrush(Color::Red(), Color::Blue());
    
    std::cout << "✓ 创建线性渐变 (红 → 蓝)\n";
    std::cout << "  起点: (0, 0)\n";
    std::cout << "  终点: (1, 1)\n";
    std::cout << "  渐变停止点数量: " << gradient->GetGradientStops().size() << "\n";
    
    // 应用到矩形
    auto rect = new Rectangle();
    rect->Width(200)->Height(100);
    rect->Fill(gradient);
    
    std::cout << "✓ 应用到矩形 (200x100)\n";
    
    delete rect;
    PrintSeparator();
}

void Test2_LinearGradientMultipleStops() {
    std::cout << "测试 2: 多停止点线性渐变\n";
    std::cout << "-------------------------\n";
    
    // 创建彩虹渐变
    auto gradient = new LinearGradientBrush();
    gradient->ClearGradientStops();
    gradient->AddGradientStop(Color::Red(), 0.0f);
    gradient->AddGradientStop(Color::Yellow(), 0.25f);
    gradient->AddGradientStop(Color::Green(), 0.5f);
    gradient->AddGradientStop(Color::Cyan(), 0.75f);
    gradient->AddGradientStop(Color::Blue(), 1.0f);
    
    std::cout << "✓ 创建彩虹渐变\n";
    std::cout << "  渐变停止点:\n";
    for (const auto& stop : gradient->GetGradientStops()) {
        std::cout << "    - 偏移 " << stop.offset << ": "
                  << "RGBA(" << stop.color.r << ", " << stop.color.g 
                  << ", " << stop.color.b << ", " << stop.color.a << ")\n";
    }
    
    // 设置自定义起点和终点
    gradient->SetStartPoint(Point(0.0f, 0.5f));  // 左中
    gradient->SetEndPoint(Point(1.0f, 0.5f));    // 右中
    
    std::cout << "✓ 水平方向渐变\n";
    std::cout << "  起点: (" << gradient->GetStartPoint().x 
              << ", " << gradient->GetStartPoint().y << ")\n";
    std::cout << "  终点: (" << gradient->GetEndPoint().x 
              << ", " << gradient->GetEndPoint().y << ")\n";
    
    delete gradient;
    PrintSeparator();
}

void Test3_RadialGradientBasic() {
    std::cout << "测试 3: 基础径向渐变\n";
    std::cout << "-------------------\n";
    
    // 创建从白色到黑色的径向渐变
    auto gradient = new RadialGradientBrush(Color::White(), Color::Black());
    
    std::cout << "✓ 创建径向渐变 (白 → 黑)\n";
    std::cout << "  中心点: (" << gradient->GetCenter().x 
              << ", " << gradient->GetCenter().y << ")\n";
    std::cout << "  X半径: " << gradient->GetRadiusX() << "\n";
    std::cout << "  Y半径: " << gradient->GetRadiusY() << "\n";
    std::cout << "  渐变停止点数量: " << gradient->GetGradientStops().size() << "\n";
    
    // 应用到圆形
    auto ellipse = new Ellipse();
    ellipse->Width(150)->Height(150);
    ellipse->Fill(gradient);
    
    std::cout << "✓ 应用到圆形 (150x150)\n";
    
    delete ellipse;
    PrintSeparator();
}

void Test4_RadialGradientCustom() {
    std::cout << "测试 4: 自定义径向渐变\n";
    std::cout << "---------------------\n";
    
    // 创建太阳效果（中心黄色，边缘橙红色）
    auto gradient = new RadialGradientBrush();
    gradient->ClearGradientStops();
    gradient->AddGradientStop(Color::Yellow(), 0.0f);
    gradient->AddGradientStop(Color::FromRGB(255, 200, 0), 0.5f);
    gradient->AddGradientStop(Color::FromRGB(255, 100, 0), 1.0f);
    
    // 偏心渐变焦点
    gradient->SetCenter(Point(0.5f, 0.5f));
    gradient->SetGradientOrigin(Point(0.3f, 0.3f));  // 焦点偏向左上
    gradient->SetRadiusX(0.5f);
    gradient->SetRadiusY(0.5f);
    
    std::cout << "✓ 创建太阳效果渐变\n";
    std::cout << "  中心点: (" << gradient->GetCenter().x 
              << ", " << gradient->GetCenter().y << ")\n";
    std::cout << "  渐变焦点: (" << gradient->GetGradientOrigin().x 
              << ", " << gradient->GetGradientOrigin().y << ")\n";
    std::cout << "  渐变停止点:\n";
    for (const auto& stop : gradient->GetGradientStops()) {
        std::cout << "    - 偏移 " << stop.offset << "\n";
    }
    
    delete gradient;
    PrintSeparator();
}

void Test5_ImageBrush() {
    std::cout << "测试 5: 图像画刷\n";
    std::cout << "---------------\n";
    
    // 创建图像画刷
    auto imageBrush = new ImageBrush();
    imageBrush->SetImageSource("test_image.png");
    imageBrush->SetTileMode(TileMode::Tile);
    
    std::cout << "✓ 创建图像画刷\n";
    std::cout << "  图像源: " << imageBrush->GetImageSource() << "\n";
    std::cout << "  平铺模式: Tile\n";
    std::cout << "  纹理ID: " << imageBrush->GetTextureId() << "\n";
    
    // 应用到矩形
    auto rect = new Rectangle();
    rect->Width(300)->Height(200);
    rect->Fill(imageBrush);
    
    std::cout << "✓ 应用到矩形 (300x200)\n";
    
    delete rect;
    PrintSeparator();
}

void Test6_BrushCloning() {
    std::cout << "测试 6: 画刷克隆\n";
    std::cout << "---------------\n";
    
    // 创建原始渐变
    auto original = new LinearGradientBrush(Color::Red(), Color::Blue());
    original->SetStartPoint(Point(0.0f, 0.0f));
    original->SetEndPoint(Point(1.0f, 0.0f));
    
    // 克隆
    auto* cloned = dynamic_cast<LinearGradientBrush*>(original->Clone());
    
    std::cout << "✓ 克隆线性渐变画刷\n";
    std::cout << "  原始起点: (" << original->GetStartPoint().x 
              << ", " << original->GetStartPoint().y << ")\n";
    std::cout << "  克隆起点: (" << cloned->GetStartPoint().x 
              << ", " << cloned->GetStartPoint().y << ")\n";
    std::cout << "  渐变停止点数量: " << cloned->GetGradientStops().size() << "\n";
    
    delete original;
    delete cloned;
    PrintSeparator();
}

void Test7_GradientWithShapes() {
    std::cout << "测试 7: 渐变与Shape集成\n";
    std::cout << "----------------------\n";
    
    // 创建多个Shape使用不同渐变
    std::cout << "创建Shape集合:\n";
    
    // 1. 矩形 + 线性渐变
    auto rect = new Rectangle();
    auto linearGrad = new LinearGradientBrush(Color::Blue(), Color::Cyan());
    rect->Width(100)->Height(60);
    rect->Fill(linearGrad);
    std::cout << "  ✓ 矩形 (线性渐变: 蓝 → 青)\n";
    
    // 2. 圆形 + 径向渐变
    auto ellipse = new Ellipse();
    auto radialGrad = new RadialGradientBrush(Color::Yellow(), Color::Red());
    ellipse->Width(80)->Height(80);
    ellipse->Fill(radialGrad);
    std::cout << "  ✓ 圆形 (径向渐变: 黄 → 红)\n";
    
    // 3. 多边形 + 彩虹渐变
    auto polygon = new Polygon();
    auto rainbowGrad = new LinearGradientBrush();
    rainbowGrad->ClearGradientStops();
    rainbowGrad->AddGradientStop(Color::Red(), 0.0f);
    rainbowGrad->AddGradientStop(Color::Green(), 0.5f);
    rainbowGrad->AddGradientStop(Color::Blue(), 1.0f);
    polygon->AddPoint({50, 0});
    polygon->AddPoint({100, 50});
    polygon->AddPoint({50, 100});
    polygon->AddPoint({0, 50});
    polygon->Fill(rainbowGrad);
    std::cout << "  ✓ 菱形 (彩虹渐变)\n";
    
    delete rect;
    delete ellipse;
    delete polygon;
    PrintSeparator();
}

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "║       F__K_UI 渐变画刷系统演示 (Phase 3)               ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    PrintSeparator();
    
    try {
        Test1_LinearGradientBasic();
        Test2_LinearGradientMultipleStops();
        Test3_RadialGradientBasic();
        Test4_RadialGradientCustom();
        Test5_ImageBrush();
        Test6_BrushCloning();
        Test7_GradientWithShapes();
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                   所有测试完成！                        ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "║  ✓ LinearGradientBrush - 线性渐变                       ║\n";
        std::cout << "║  ✓ RadialGradientBrush - 径向渐变                       ║\n";
        std::cout << "║  ✓ ImageBrush - 图像画刷                                ║\n";
        std::cout << "║  ✓ 多渐变停止点支持                                     ║\n";
        std::cout << "║  ✓ 画刷克隆                                             ║\n";
        std::cout << "║  ✓ Shape 集成                                           ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ 错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
