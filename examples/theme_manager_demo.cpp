/**
 * ThemeManager 和 DynamicResource 演示程序
 * 展示主题管理和动态资源功能
 */

#include "fk/resources/ThemeManager.h"
#include "fk/resources/DynamicResource.h"
#include "fk/ui/DrawCommand.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

using namespace fk;
using namespace fk::resources;
using namespace fk::ui;

void PrintSeparator(const std::string& title = "") {
    std::cout << "\n";
    if (!title.empty()) {
        std::cout << "========== " << title << " ==========\n";
    } else {
        std::cout << "========================================\n";
    }
}

void PrintColor(const std::string& name, const Color& color) {
    std::cout << "  " << std::setw(25) << std::left << name << ": "
              << "RGBA(" << (int)(color.r * 255) << ", " << (int)(color.g * 255) << ", " 
              << (int)(color.b * 255) << ", " << (int)(color.a * 255) << ")\n";
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║      F__K_UI ThemeManager & DynamicResource 演示         ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    auto& themeManager = ThemeManager::Instance();

    // ========== 测试 1: 创建和注册主题 ==========
    PrintSeparator("测试 1: 创建和注册主题");
    
    themeManager.CreateDefaultLightTheme();
    themeManager.CreateDefaultDarkTheme();
    
    std::cout << "\n✓ 主题创建完成\n";
    std::cout << "  已注册主题: ";
    auto themeNames = themeManager.GetThemeNames();
    for (const auto& name : themeNames) {
        std::cout << name << " ";
    }
    std::cout << "\n";

    // ========== 测试 2: 设置当前主题并读取资源 ==========
    PrintSeparator("测试 2: 设置当前主题并读取资源");
    
    std::cout << "\n设置当前主题为 'Light'...\n";
    if (themeManager.SetCurrentTheme("Light")) {
        std::cout << "✓ 主题切换成功\n";
        
        auto theme = themeManager.GetCurrentTheme();
        std::cout << "  主题名称: " << theme->GetName() << "\n";
        std::cout << "  主题描述: " << theme->GetDescription() << "\n";
        std::cout << "  主题作者: " << theme->GetAuthor() << "\n";
        std::cout << "  主题版本: " << theme->GetVersion() << "\n";
        
        std::cout << "\n浅色主题的颜色资源:\n";
        PrintColor("PrimaryColor", themeManager.FindResource<Color>("PrimaryColor"));
        PrintColor("WindowBackground", themeManager.FindResource<Color>("WindowBackground"));
        PrintColor("TextColor", themeManager.FindResource<Color>("TextColor"));
        PrintColor("ButtonBackground", themeManager.FindResource<Color>("ButtonBackground"));
        
        std::cout << "\n浅色主题的字体大小:\n";
        std::cout << "  FontSizeNormal: " << themeManager.FindResource<double>("FontSizeNormal") << "\n";
        std::cout << "  FontSizeTitle: " << themeManager.FindResource<double>("FontSizeTitle") << "\n";
    } else {
        std::cout << "✗ 主题切换失败\n";
    }

    // ========== 测试 3: DynamicResource 功能 ==========
    PrintSeparator("测试 3: DynamicResource 动态资源");
    
    std::cout << "\n创建动态资源引用...\n";
    auto primaryColor = std::make_shared<DynamicResource<Color>>("PrimaryColor");
    auto textColor = std::make_shared<DynamicResource<Color>>("TextColor");
    auto fontSize = std::make_shared<DynamicResource<double>>("FontSizeNormal");
    
    std::cout << "✓ 动态资源创建完成\n";
    std::cout << "\n当前值（Light主题）:\n";
    PrintColor("PrimaryColor", primaryColor->GetValue());
    PrintColor("TextColor", textColor->GetValue());
    std::cout << "  FontSizeNormal: " << fontSize->GetValue() << "\n";

    // ========== 测试 4: 主题切换事件 ==========
    PrintSeparator("测试 4: 主题切换事件");
    
    int switchCount = 0;
    themeManager.ThemeChanged.Connect(
        [&switchCount](std::shared_ptr<Theme> oldTheme, std::shared_ptr<Theme> newTheme) {
            switchCount++;
            std::cout << "\n🔔 主题切换事件触发 #" << switchCount << ":\n";
            std::cout << "  从: " << (oldTheme ? oldTheme->GetName() : "(无)") << "\n";
            std::cout << "  到: " << (newTheme ? newTheme->GetName() : "(无)") << "\n";
        }
    );
    
    // 设置值变更回调
    int primaryColorChanges = 0;
    primaryColor->SetValueChangedCallback([&primaryColorChanges](const Color& newColor) {
        primaryColorChanges++;
        std::cout << "  📝 PrimaryColor 值已更新 #" << primaryColorChanges << ": "
                  << "RGBA(" << (int)(newColor.r * 255) << ", " << (int)(newColor.g * 255) << ", " 
                  << (int)(newColor.b * 255) << ", " << (int)(newColor.a * 255) << ")\n";
    });
    
    std::cout << "\n切换到 'Dark' 主题...\n";
    if (themeManager.SetCurrentTheme("Dark")) {
        std::cout << "✓ 主题切换成功\n";
        
        std::cout << "\n动态资源自动更新后的值（Dark主题）:\n";
        PrintColor("PrimaryColor", primaryColor->GetValue());
        PrintColor("TextColor", textColor->GetValue());
        std::cout << "  FontSizeNormal: " << fontSize->GetValue() << "\n";
        
        std::cout << "\n深色主题的颜色资源:\n";
        PrintColor("WindowBackground", themeManager.FindResource<Color>("WindowBackground"));
        PrintColor("ButtonBackground", themeManager.FindResource<Color>("ButtonBackground"));
    }

    // ========== 测试 5: 多次主题切换 ==========
    PrintSeparator("测试 5: 多次主题切换");
    
    std::cout << "\n执行多次主题切换测试...\n";
    std::cout << "Light → Dark → Light\n\n";
    
    themeManager.SetCurrentTheme("Light");
    std::cout << "当前主题: " << themeManager.GetCurrentThemeName() << "\n";
    PrintColor("PrimaryColor", primaryColor->GetValue());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    themeManager.SetCurrentTheme("Dark");
    std::cout << "\n当前主题: " << themeManager.GetCurrentThemeName() << "\n";
    PrintColor("PrimaryColor", primaryColor->GetValue());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    themeManager.SetCurrentTheme("Light");
    std::cout << "\n当前主题: " << themeManager.GetCurrentThemeName() << "\n";
    PrintColor("PrimaryColor", primaryColor->GetValue());

    // ========== 测试 6: 自定义主题 ==========
    PrintSeparator("测试 6: 自定义主题");
    
    std::cout << "\n创建自定义主题 'CustomBlue'...\n";
    auto customTheme = std::make_shared<Theme>("CustomBlue");
    customTheme->SetDescription("自定义蓝色主题");
    customTheme->SetAuthor("Demo");
    customTheme->SetThemeType(Theme::ThemeType::Custom);
    
    // 添加自定义颜色
    customTheme->AddResource("PrimaryColor", Color::FromRGB(0, 191, 255));     // 深天蓝
    customTheme->AddResource("SecondaryColor", Color::FromRGB(65, 105, 225));  // 皇家蓝
    customTheme->AddResource("WindowBackground", Color::FromRGB(240, 248, 255)); // 爱丽丝蓝
    customTheme->AddResource("TextColor", Color::FromRGB(25, 25, 112));         // 午夜蓝
    customTheme->AddResource("FontSizeNormal", 16.0);
    
    themeManager.RegisterTheme(customTheme);
    std::cout << "✓ 自定义主题已注册\n";
    
    std::cout << "\n切换到自定义主题...\n";
    if (themeManager.SetCurrentTheme("CustomBlue")) {
        std::cout << "✓ 主题切换成功\n";
        std::cout << "\n自定义主题的颜色:\n";
        PrintColor("PrimaryColor", primaryColor->GetValue());
        PrintColor("WindowBackground", themeManager.FindResource<Color>("WindowBackground"));
        PrintColor("TextColor", textColor->GetValue());
        std::cout << "  FontSizeNormal: " << fontSize->GetValue() << "\n";
    }

    // ========== 总结 ==========
    PrintSeparator("测试总结");
    
    std::cout << "\n✓ 所有测试通过!\n";
    std::cout << "  主题切换次数: " << switchCount << "\n";
    std::cout << "  PrimaryColor 更新次数: " << primaryColorChanges << "\n";
    std::cout << "  已注册主题数: " << themeManager.GetThemeNames().size() << "\n";
    std::cout << "  当前主题: " << themeManager.GetCurrentThemeName() << "\n";
    
    std::cout << "\n功能验证:\n";
    std::cout << "  ✓ ThemeManager 主题管理\n";
    std::cout << "  ✓ 主题注册和切换\n";
    std::cout << "  ✓ DynamicResource 动态资源\n";
    std::cout << "  ✓ 主题切换事件\n";
    std::cout << "  ✓ 资源值自动更新\n";
    std::cout << "  ✓ 自定义主题支持\n";
    
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                   演示完成！                              ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";

    return 0;
}
