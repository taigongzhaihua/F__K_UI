/**
 * @file brush_system_demo.cpp
 * @brief Brush 画刷系统功能演示
 * 
 * 展示内容：
 * - Color 颜色类使用
 * - SolidColorBrush 纯色画刷
 * - Shape 图形使用 Brush 填充和描边
 * - Brushes 预定义画刷
 */

#include "fk/ui/Brush.h"
#include "fk/ui/Shape.h"
#include <iostream>
#include <iomanip>

using namespace fk::ui;

void PrintSeparator(const std::string& title = "") {
    std::cout << "\n========================================\n";
    if (!title.empty()) {
        std::cout << "  " << title << "\n";
        std::cout << "========================================\n";
    }
}

void DemoColorClass() {
    PrintSeparator("1. Color 颜色类演示");
    
    // 创建颜色
    Color red = Color::Red();
    Color blue = Color(0, 0, 255);
    Color semiTransparent = Color(128, 128, 128, 128);
    
    std::cout << "\n[Color 创建]\n";
    std::cout << "红色 (Float): (" << red.r << ", " << red.g << ", " << red.b << ", " << red.a << ")\n";
    std::cout << "蓝色 (Float): (" << blue.r << ", " << blue.g << ", " << blue.b << ", " << blue.a << ")\n";
    std::cout << "半透明灰 (Float): (" << semiTransparent.r << ", " << semiTransparent.g 
              << ", " << semiTransparent.b << ", " << semiTransparent.a << ")\n";
    
    // 从 RGB 创建
    Color fromRGB = Color::FromRGB(0, 255, 0);  // 不透明绿色
    std::cout << "\n[从 RGB 创建]\n";
    std::cout << "RGB(0, 255, 0) → Float(" << fromRGB.r << ", " << fromRGB.g 
              << ", " << fromRGB.b << ", " << fromRGB.a << ")\n";
    
    // 转换为 ARGB
    uint32_t argb = red.ToArgb();
    std::cout << "\n[转换为 ARGB]\n";
    std::cout << "Red → 0x" << std::hex << std::setfill('0') << std::setw(8) << argb << std::dec << "\n";
    std::cout << "应该是 0xFF0000FF\n";
    
    // 颜色比较
    std::cout << "\n[颜色比较]\n";
    Color red2 = Color::Red();
    std::cout << "Color::Red() == Color::Red(): " << (red == red2 ? "是" : "否") << "\n";
    std::cout << "Color::Red() == Color::Blue(): " << (red == Color::Blue() ? "是" : "否") << "\n";
    
    // 预定义颜色
    std::cout << "\n[预定义颜色]\n";
    std::cout << "- Black\n";
    std::cout << "- White\n";
    std::cout << "- Red, Green, Blue\n";
    std::cout << "- Yellow, Cyan, Magenta\n";
    std::cout << "- Gray, LightGray, DarkGray\n";
    std::cout << "- Transparent\n";
}

void DemoSolidColorBrush() {
    PrintSeparator("2. SolidColorBrush 纯色画刷演示");
    
    // 创建画刷
    auto* redBrush = new SolidColorBrush(Color::Red());
    auto* blueBrush = new SolidColorBrush(0, 0, 255);
    auto* customBrush = new SolidColorBrush(Color(100, 150, 200, 255));
    
    std::cout << "\n[创建画刷]\n";
    std::cout << "红色画刷: Float(" << redBrush->GetColor().r << ", " 
              << redBrush->GetColor().g << ", " << redBrush->GetColor().b << ")\n";
    std::cout << "蓝色画刷: Float(" << blueBrush->GetColor().r << ", " 
              << blueBrush->GetColor().g << ", " << blueBrush->GetColor().b << ")\n";
    std::cout << "自定义画刷: Float(" << customBrush->GetColor().r << ", " 
              << customBrush->GetColor().g << ", " << customBrush->GetColor().b << ")\n";
    
    // 从 ARGB 创建
    auto* argbBrush = SolidColorBrush::FromArgb(0xFF00FF00);  // 绿色
    std::cout << "\n[从 ARGB 创建画刷]\n";
    std::cout << "ARGB 画刷: Float(" << (int)argbBrush->GetColor().r << ", " 
              << (int)argbBrush->GetColor().g << ", " << (int)argbBrush->GetColor().b << ")\n";
    
    // 修改画刷颜色
    redBrush->SetColor(Color::Yellow());
    std::cout << "\n[修改画刷颜色]\n";
    std::cout << "修改后的画刷: Float(" << (int)redBrush->GetColor().r << ", " 
              << (int)redBrush->GetColor().g << ", " << (int)redBrush->GetColor().b << ")\n";
    
    // 克隆画刷
    Brush* clonedBrush = blueBrush->Clone();
    std::cout << "\n[克隆画刷]\n";
    std::cout << "原画刷: Float(" << (int)blueBrush->GetColor().r << ", " 
              << (int)blueBrush->GetColor().g << ", " << (int)blueBrush->GetColor().b << ")\n";
    std::cout << "克隆画刷: Float(" << (int)clonedBrush->GetColor().r << ", " 
              << (int)clonedBrush->GetColor().g << ", " << (int)clonedBrush->GetColor().b << ")\n";
    
    // 清理
    delete redBrush;
    delete blueBrush;
    delete customBrush;
    delete argbBrush;
    delete clonedBrush;
}

void DemoBrushesPresets() {
    PrintSeparator("3. Brushes 预定义画刷演示");
    
    std::cout << "\n[预定义画刷使用]\n";
    std::cout << "Brushes 类提供常用颜色的快速访问：\n\n";
    
    auto* blackBrush = Brushes::Black();
    auto* whiteBrush = Brushes::White();
    auto* redBrush = Brushes::Red();
    auto* greenBrush = Brushes::Green();
    auto* blueBrush = Brushes::Blue();
    
    std::cout << "Brushes::Black() → Float(" << (int)blackBrush->GetColor().r << ", " 
              << (int)blackBrush->GetColor().g << ", " << (int)blackBrush->GetColor().b << ")\n";
    std::cout << "Brushes::White() → Float(" << (int)whiteBrush->GetColor().r << ", " 
              << (int)whiteBrush->GetColor().g << ", " << (int)whiteBrush->GetColor().b << ")\n";
    std::cout << "Brushes::Red() → Float(" << (int)redBrush->GetColor().r << ", " 
              << (int)redBrush->GetColor().g << ", " << (int)redBrush->GetColor().b << ")\n";
    std::cout << "Brushes::Green() → Float(" << (int)greenBrush->GetColor().r << ", " 
              << (int)greenBrush->GetColor().g << ", " << (int)greenBrush->GetColor().b << ")\n";
    std::cout << "Brushes::Blue() → Float(" << (int)blueBrush->GetColor().r << ", " 
              << (int)blueBrush->GetColor().g << ", " << (int)blueBrush->GetColor().b << ")\n";
    
    std::cout << "\n可用的预定义画刷：\n";
    std::cout << "- Brushes::Black()\n";
    std::cout << "- Brushes::White()\n";
    std::cout << "- Brushes::Red()\n";
    std::cout << "- Brushes::Green()\n";
    std::cout << "- Brushes::Blue()\n";
    std::cout << "- Brushes::Yellow()\n";
    std::cout << "- Brushes::Cyan()\n";
    std::cout << "- Brushes::Magenta()\n";
    std::cout << "- Brushes::Gray()\n";
    std::cout << "- Brushes::LightGray()\n";
    std::cout << "- Brushes::DarkGray()\n";
    std::cout << "- Brushes::Transparent()\n";
    
    // 清理
    delete blackBrush;
    delete whiteBrush;
    delete redBrush;
    delete greenBrush;
    delete blueBrush;
}

void DemoShapeWithBrush() {
    PrintSeparator("4. Shape 图形使用 Brush 演示");
    
    std::cout << "\n[Rectangle 使用画刷]\n";
    auto* rectangle = new Rectangle();
    rectangle->Fill(Brushes::Blue())        // 蓝色填充
             ->Stroke(Brushes::Black())     // 黑色描边
             ->StrokeThickness(2.0f);       // 描边粗细 2px
    rectangle->RadiusX(10.0f);              // 圆角 X
    rectangle->RadiusY(10.0f);              // 圆角 Y
    
    std::cout << "矩形配置：\n";
    std::cout << "  填充: Float(" << (int)rectangle->Fill()->GetColor().r << ", " 
              << (int)rectangle->Fill()->GetColor().g << ", " 
              << (int)rectangle->Fill()->GetColor().b << ")\n";
    std::cout << "  描边: Float(" << (int)rectangle->Stroke()->GetColor().r << ", " 
              << (int)rectangle->Stroke()->GetColor().g << ", " 
              << (int)rectangle->Stroke()->GetColor().b << ")\n";
    std::cout << "  描边粗细: " << rectangle->StrokeThickness() << "px\n";
    
    std::cout << "\n[Ellipse 使用画刷]\n";
    auto* ellipse = new Ellipse();
    ellipse->Fill(new SolidColorBrush(Color::Red()))
           ->Stroke(new SolidColorBrush(Color(0, 128, 0)))  // 深绿色
           ->StrokeThickness(3.0f);
    
    std::cout << "椭圆配置：\n";
    std::cout << "  填充: Float(" << (int)ellipse->Fill()->GetColor().r << ", " 
              << (int)ellipse->Fill()->GetColor().g << ", " 
              << (int)ellipse->Fill()->GetColor().b << ")\n";
    std::cout << "  描边: Float(" << (int)ellipse->Stroke()->GetColor().r << ", " 
              << (int)ellipse->Stroke()->GetColor().g << ", " 
              << (int)ellipse->Stroke()->GetColor().b << ")\n";
    
    std::cout << "\n[Line 使用画刷]\n";
    auto* line = new Line();
    line->X1(0)->Y1(0)->X2(100)->Y2(100)
        ->Stroke(Brushes::Red())
        ->StrokeThickness(5.0f);
    
    std::cout << "直线配置：\n";
    std::cout << "  起点: (" << line->GetX1() << ", " << line->GetY1() << ")\n";
    std::cout << "  终点: (" << line->GetX2() << ", " << line->GetY2() << ")\n";
    std::cout << "  颜色: Float(" << (int)line->Stroke()->GetColor().r << ", " 
              << (int)line->Stroke()->GetColor().g << ", " 
              << (int)line->Stroke()->GetColor().b << ")\n";
    
    std::cout << "\n[Polygon 使用画刷]\n";
    auto* polygon = new Polygon();
    polygon->AddPoint(Point(100, 20));
    polygon->AddPoint(Point(50, 100));
    polygon->AddPoint(Point(150, 100));
    polygon->Fill(Brushes::Yellow());
    polygon->Stroke(Brushes::Black());
    polygon->StrokeThickness(2.0f);
    
    std::cout << "多边形配置：\n";
    std::cout << "  顶点数: " << polygon->GetPointCount() << "\n";
    std::cout << "  填充: Float(" << polygon->Fill()->GetColor().r << ", " 
              << polygon->Fill()->GetColor().g << ", " 
              << polygon->Fill()->GetColor().b << ")\n";
    
    std::cout << "\n[Path 使用画刷]\n";
    auto* path = new Path();
    path->MoveTo(10, 10);
    path->LineTo(100, 10);
    path->LineTo(100, 100);
    path->Close();
    path->Stroke(Brushes::Magenta());
    path->StrokeThickness(3.0f);
    
    std::cout << "路径配置：\n";
    std::cout << "  描边: Float(" << path->Stroke()->GetColor().r << ", " 
              << path->Stroke()->GetColor().g << ", " 
              << path->Stroke()->GetColor().b << ")\n";
    
    // 清理
    delete rectangle;
    delete ellipse;
    delete line;
    delete polygon;
    delete path;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "  F__K_UI Brush 画刷系统功能演示\n";
    std::cout << "========================================\n";
    
    // 1. Color 颜色类
    DemoColorClass();
    
    // 2. SolidColorBrush 纯色画刷
    DemoSolidColorBrush();
    
    // 3. Brushes 预定义画刷
    DemoBrushesPresets();
    
    // 4. Shape 图形使用 Brush
    DemoShapeWithBrush();
    
    PrintSeparator("功能总结");
    std::cout << "\n✅ 实现的功能:\n";
    std::cout << "1. Color 颜色类\n";
    std::cout << "   - RGB/RGBA 构造\n";
    std::cout << "   - 预定义颜色常量\n";
    std::cout << "   - ARGB 整数转换\n";
    std::cout << "   - 颜色比较\n\n";
    
    std::cout << "2. SolidColorBrush 纯色画刷\n";
    std::cout << "   - 多种构造方式\n";
    std::cout << "   - 颜色修改\n";
    std::cout << "   - 画刷克隆\n";
    std::cout << "   - 依赖属性支持\n\n";
    
    std::cout << "3. Brushes 预定义画刷\n";
    std::cout << "   - 12 种常用颜色快速访问\n";
    std::cout << "   - WPF 风格 API\n\n";
    
    std::cout << "4. Shape 图形集成\n";
    std::cout << "   - Fill 填充画刷\n";
    std::cout << "   - Stroke 描边画刷\n";
    std::cout << "   - 流式 API 支持\n";
    std::cout << "   - 所有 Shape 类型支持\n\n";
    
    std::cout << "🎯 技术亮点:\n";
    std::cout << "- WPF 兼容 API 设计\n";
    std::cout << "- 类型安全的颜色管理\n";
    std::cout << "- 画刷抽象支持未来扩展（渐变画刷等）\n";
    std::cout << "- 流式 API 提高代码可读性\n";
    std::cout << "- 完整的依赖属性集成\n\n";
    
    std::cout << "📊 代码统计:\n";
    std::cout << "- Brush.h: ~160 行\n";
    std::cout << "- Brush.cpp: ~50 行\n";
    std::cout << "- Shape.cpp 更新: ~30 行修改\n";
    std::cout << "- 总计: ~240 行新代码\n\n";
    
    PrintSeparator();
    
    return 0;
}
