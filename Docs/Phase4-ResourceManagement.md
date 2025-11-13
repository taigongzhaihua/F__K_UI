# Phase 4.3: 高级资源管理系统

## 概述

Phase 4.3 实现了完整的资源管理和主题系统，为F__K_UI框架提供了灵活的主题切换和动态资源功能。

## 核心组件

### 1. Theme - 主题类

主题类封装了一组视觉资源（颜色、字体、样式等）。

**主要特性**：
- 主题元数据（名称、描述、作者、版本）
- 主题类型（Light/Dark/Custom）
- 资源字典管理
- 类型安全的资源访问

**使用示例**：
```cpp
// 创建主题
auto theme = std::make_shared<Theme>("MyTheme");
theme->SetDescription("自定义主题");
theme->SetThemeType(Theme::ThemeType::Custom);

// 添加资源
theme->AddResource("PrimaryColor", Color::FromRGB(0, 120, 215));
theme->AddResource("FontSizeNormal", 14.0);

// 获取资源
auto color = theme->GetResource<Color>("PrimaryColor");
```

### 2. ThemeManager - 主题管理器

ThemeManager 是单例类，负责管理应用程序的所有主题。

**主要功能**：
- 主题注册和管理
- 当前主题设置和切换
- 全局资源查找
- 主题切换事件

**线程安全**：
- 使用 `std::mutex` 保护内部状态
- 支持多线程访问

**使用示例**：
```cpp
auto& manager = ThemeManager::Instance();

// 注册主题
manager.RegisterTheme(theme);

// 设置当前主题
manager.SetCurrentTheme("MyTheme");

// 查找资源
auto color = manager.FindResource<Color>("PrimaryColor");

// 订阅主题切换事件
manager.ThemeChanged.Connect([](auto oldTheme, auto newTheme) {
    std::cout << "主题从 " << oldTheme->GetName() 
              << " 切换到 " << newTheme->GetName() << std::endl;
});
```

### 3. DynamicResource - 动态资源

DynamicResource 提供对主题资源的动态引用，自动响应主题切换。

**主要特性**：
- 自动更新：主题切换时自动获取新值
- 值变更回调：支持自定义回调函数
- 类型安全：模板化设计

**使用示例**：
```cpp
// 创建动态资源
auto primaryColor = std::make_shared<DynamicResource<Color>>("PrimaryColor");
auto fontSize = std::make_shared<DynamicResource<double>>("FontSizeNormal");

// 获取当前值
Color color = primaryColor->GetValue();

// 设置值变更回调
primaryColor->SetValueChangedCallback([](const Color& newColor) {
    std::cout << "颜色已更新" << std::endl;
});

// 主题切换时，DynamicResource 自动更新
```

## 预定义主题

### Light 主题（浅色）

**颜色方案**：
- PrimaryColor: RGB(0, 120, 215) - 蓝色
- WindowBackground: RGB(240, 240, 240) - 浅灰
- TextColor: RGB(0, 0, 0) - 黑色
- ButtonBackground: RGB(225, 225, 225)

**字体大小**：
- FontSizeSmall: 12
- FontSizeNormal: 14
- FontSizeLarge: 18
- FontSizeTitle: 24

### Dark 主题（深色）

**颜色方案**：
- PrimaryColor: RGB(30, 144, 255) - 亮蓝色
- WindowBackground: RGB(32, 32, 32) - 深灰
- TextColor: RGB(255, 255, 255) - 白色
- ButtonBackground: RGB(70, 70, 70)

**字体大小**：（与Light主题相同）

## 完整使用示例

```cpp
#include "fk/resources/ThemeManager.h"
#include "fk/resources/DynamicResource.h"

int main() {
    auto& manager = ThemeManager::Instance();
    
    // 创建默认主题
    manager.CreateDefaultLightTheme();
    manager.CreateDefaultDarkTheme();
    
    // 设置当前主题
    manager.SetCurrentTheme("Light");
    
    // 创建动态资源
    auto primaryColor = MakeDynamicResource<Color>("PrimaryColor");
    auto textColor = MakeDynamicResource<Color>("TextColor");
    
    std::cout << "Light 主题颜色" << std::endl;
    // 使用资源...
    
    // 切换到深色主题
    manager.SetCurrentTheme("Dark");
    
    std::cout << "Dark 主题颜色（自动更新）" << std::endl;
    // DynamicResource 已自动更新
    
    // 创建自定义主题
    auto customTheme = std::make_shared<Theme>("CustomBlue");
    customTheme->AddResource("PrimaryColor", Color::FromRGB(0, 191, 255));
    customTheme->AddResource("TextColor", Color::FromRGB(25, 25, 112));
    
    manager.RegisterTheme(customTheme);
    manager.SetCurrentTheme("CustomBlue");
    
    return 0;
}
```

## 技术实现

### 资源查找机制

```
1. 从当前主题的资源字典查找
2. 如果未找到，返回默认值
```

### 主题切换流程

```
1. 验证目标主题存在
2. 更新 currentTheme_ 指针
3. 触发 ThemeChanged 事件
4. DynamicResource 接收事件并更新值
5. 调用用户定义的值变更回调
```

### 线程安全

- ThemeManager 使用 `std::mutex` 保护内部状态
- 事件在锁外触发，避免死锁
- DynamicResource 在主题切换时自动同步

## 测试结果

### 功能测试

✅ **主题管理**
- 主题注册：成功
- 主题切换：成功
- 资源查找：成功

✅ **动态资源**
- 资源创建：成功
- 值获取：成功
- 自动更新：成功（需要保持连接）

✅ **自定义主题**
- 主题创建：成功
- 资源添加：成功
- 主题应用：成功

### 性能特性

- **主题切换**：O(1) - 直接指针替换
- **资源查找**：O(1) - 哈希表查找
- **事件通知**：O(n) - n为订阅者数量

## WPF 兼容性

| WPF 概念 | F__K_UI 实现 | 说明 |
|---------|--------------|------|
| ResourceDictionary | ResourceDictionary | 资源字典 |
| StaticResource | 直接查找 | 静态资源引用 |
| DynamicResource | DynamicResource | 动态资源引用 |
| ThemeManager | ThemeManager | 第三方库概念 |
| Theme | Theme | 主题定义 |

## 已知限制

1. **DynamicResource 连接管理**
   - 当前实现中，Event::Connection 需要手动管理
   - 建议：保存 Connection 对象，避免自动断开

2. **资源查找优化**
   - 当前为单层字典查找
   - 未来可以添加多级查找支持

3. **主题热加载**
   - 当前不支持从文件加载主题
   - 未来可以添加 JSON/XML 主题文件支持

## 扩展建议

### 1. 主题文件支持

```cpp
// 从文件加载主题
auto theme = ThemeLoader::LoadFromFile("mytheme.json");
manager.RegisterTheme(theme);
```

### 2. 主题继承

```cpp
// 创建派生主题
auto derivedTheme = std::make_shared<Theme>("DarkBlue", baseTheme);
derivedTheme->AddResource("PrimaryColor", newColor);
```

### 3. 资源绑定

```cpp
// 将DynamicResource绑定到控件属性
button->SetBinding(Button::BackgroundProperty(), 
    MakeDynamicResource<Color>("ButtonBackground"));
```

## 代码统计

| 模块 | 文件 | 行数 |
|------|------|------|
| Theme | Theme.h | 95 |
| ThemeManager | ThemeManager.h | 170 |
| ThemeManager | ThemeManager.cpp | 86 |
| DynamicResource | DynamicResource.h | 95 |
| 演示程序 | theme_manager_demo.cpp | 225 |
| **总计** | **5** | **671** |

## 总结

Phase 4.3 成功实现了完整的主题管理系统：

✅ **核心功能**
- Theme 主题定义
- ThemeManager 主题管理器
- DynamicResource 动态资源

✅ **特性**
- 线程安全
- 事件驱动
- 类型安全
- WPF 兼容

✅ **预定义主题**
- Light（浅色）
- Dark（深色）
- 支持自定义主题

资源管理系统为 F__K_UI 框架提供了灵活的主题切换能力，使应用程序可以轻松支持多种视觉风格。
