#include "fk/resources/ThemeManager.h"
#include "fk/ui/DrawCommand.h"
#include <iostream>

namespace fk::resources {

using namespace fk::ui;

void ThemeManager::CreateDefaultLightTheme() {
    auto theme = std::make_shared<Theme>("Light");
    theme->SetDescription("Default light theme");
    theme->SetAuthor("F__K_UI");
    theme->SetThemeType(Theme::ThemeType::Light);
    
    // 添加基础颜色
    theme->AddResource("PrimaryColor", Color::FromRGB(0, 120, 215));          // 蓝色
    theme->AddResource("SecondaryColor", Color::FromRGB(76, 176, 80));        // 绿色
    theme->AddResource("AccentColor", Color::FromRGB(255, 152, 0));           // 橙色
    
    // 背景颜色
    theme->AddResource("WindowBackground", Color::FromRGB(240, 240, 240));    // 浅灰
    theme->AddResource("PanelBackground", Color::FromRGB(255, 255, 255));     // 白色
    theme->AddResource("ControlBackground", Color::FromRGB(255, 255, 255));   // 白色
    
    // 前景颜色
    theme->AddResource("TextColor", Color::FromRGB(0, 0, 0));                 // 黑色
    theme->AddResource("DisabledTextColor", Color::FromRGB(160, 160, 160));   // 灰色
    
    // 边框颜色
    theme->AddResource("BorderColor", Color::FromRGB(200, 200, 200));         // 浅灰
    theme->AddResource("FocusBorderColor", Color::FromRGB(0, 120, 215));      // 蓝色
    
    // 按钮颜色
    theme->AddResource("ButtonBackground", Color::FromRGB(225, 225, 225));    
    theme->AddResource("ButtonHoverBackground", Color::FromRGB(210, 210, 210));
    theme->AddResource("ButtonPressedBackground", Color::FromRGB(195, 195, 195));
    
    // 字体大小
    theme->AddResource("FontSizeSmall", 12.0);
    theme->AddResource("FontSizeNormal", 14.0);
    theme->AddResource("FontSizeLarge", 18.0);
    theme->AddResource("FontSizeTitle", 24.0);
    
    RegisterTheme(theme);
    std::cout << "Light theme registered" << std::endl;
}

void ThemeManager::CreateDefaultDarkTheme() {
    auto theme = std::make_shared<Theme>("Dark");
    theme->SetDescription("Default dark theme");
    theme->SetAuthor("F__K_UI");
    theme->SetThemeType(Theme::ThemeType::Dark);
    
    // 添加基础颜色
    theme->AddResource("PrimaryColor", Color::FromRGB(30, 144, 255));         // 亮蓝色
    theme->AddResource("SecondaryColor", Color::FromRGB(102, 187, 106));      // 亮绿色
    theme->AddResource("AccentColor", Color::FromRGB(255, 167, 38));          // 亮橙色
    
    // 背景颜色
    theme->AddResource("WindowBackground", Color::FromRGB(32, 32, 32));       // 深灰
    theme->AddResource("PanelBackground", Color::FromRGB(45, 45, 45));        // 中灰
    theme->AddResource("ControlBackground", Color::FromRGB(60, 60, 60));      // 浅灰
    
    // 前景颜色
    theme->AddResource("TextColor", Color::FromRGB(255, 255, 255));           // 白色
    theme->AddResource("DisabledTextColor", Color::FromRGB(120, 120, 120));   // 深灰
    
    // 边框颜色
    theme->AddResource("BorderColor", Color::FromRGB(80, 80, 80));            // 深灰
    theme->AddResource("FocusBorderColor", Color::FromRGB(30, 144, 255));     // 亮蓝色
    
    // 按钮颜色
    theme->AddResource("ButtonBackground", Color::FromRGB(70, 70, 70));
    theme->AddResource("ButtonHoverBackground", Color::FromRGB(85, 85, 85));
    theme->AddResource("ButtonPressedBackground", Color::FromRGB(100, 100, 100));
    
    // 字体大小
    theme->AddResource("FontSizeSmall", 12.0);
    theme->AddResource("FontSizeNormal", 14.0);
    theme->AddResource("FontSizeLarge", 18.0);
    theme->AddResource("FontSizeTitle", 24.0);
    
    RegisterTheme(theme);
    std::cout << "Dark theme registered" << std::endl;
}

} // namespace fk::resources
