**注：** 本项目所有内容(包括本文档)，除第三方库外均为ai生成
# FK_UI - C++ UI 框架

一个借鉴 WPF 设计理念的现代 C++ UI 框架，提供丰富的控件、数据绑定、动画系统和高性能渲染。

## ✨ 特性

- 🎨 **丰富的 UI 控件**: Button, TextBlock, TextBox, ListBox, Grid, StackPanel 等
- 🔗 **MVVM 数据绑定**: 完整的依赖属性和数据绑定系统
- 🎬 **动画系统**: 支持多种动画类型和视觉状态管理
- 🎯 **模板系统**: ControlTemplate 和 DataTemplate 支持
- 🚀 **高性能渲染**: 基于 OpenGL 的硬件加速渲染
- 📐 **灵活布局**: Grid、StackPanel 等多种布局容器
- 🎨 **主题系统**: 支持动态主题切换

## 🚀 快速开始

### 环境要求

- C++23 或更高版本
- CMake 3.10+
- OpenGL 3.3+

**Windows**: MinGW-w64 或 MSVC  
**Linux**: GCC 或 Clang，需要安装 GLFW 和 FreeType

### 编译步骤

#### Windows (MinGW)
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

#### Linux
```bash
# 安装依赖
sudo apt-get install libglfw3-dev libfreetype6-dev

# 编译
mkdir build
cd build
cmake ..
make
```

### 运行示例
```bash
# Windows
.\build\example_app.exe

# Linux
./build/example_app
```

## 📖 示例代码

### 基础示例

```cpp
#include <fk/app/Application.h>
#include <fk/ui/Window.h>
#include <fk/ui/Button.h>
#include <fk/ui/StackPanel.h>

using namespace fk;

int main() {
    // 创建应用程序
    auto app = std::make_shared<Application>();
    
    // 创建主窗口
    auto window = std::make_shared<Window>();
    window->SetTitle("FK_UI 示例");
    window->SetWidth(800);
    window->SetHeight(600);
    
    // 创建布局容器
    auto panel = std::make_shared<StackPanel>();
    
    // 创建按钮
    auto button = std::make_shared<Button>();
    button->SetContent("点击我");
    button->SetWidth(200);
    button->SetHeight(50);
    
    // 添加到布局
    panel->AddChild(button);
    window->SetContent(panel);
    
    // 运行应用
    return app->Run(window);
}
```

### DataTemplate 示例

**是的，FK_UI完全支持类似WPF中DataTemplate的效果！**

DataTemplate 可用于**任意数据类型**，不需要继承特定基类：

```cpp
#include <fk/ui/DataTemplate.h>
#include <fk/ui/ListBox.h>

// 简单数据结构（POCO - 不需要任何特殊基类）
struct UserData {
    std::string name;
    int age;
    std::string email;
};

// 创建DataTemplate来自定义数据的显示方式
auto CreateUserTemplate() {
    auto dataTemplate = std::make_shared<ui::DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> ui::UIElement* {
        auto user = std::any_cast<std::shared_ptr<UserData>>(dataContext);
        
        // 创建视觉树
        auto border = new ui::Border();
        border->SetBackground(new ui::SolidColorBrush(240, 240, 255));
        border->SetPadding(ui::Thickness(10));
        
        auto panel = new ui::StackPanel();
        panel->SetOrientation(ui::Orientation::Vertical);
        
        auto nameText = new ui::TextBlock();
        nameText->SetText("姓名: " + user->name);  // 直接访问成员
        nameText->SetFontSize(18);
        panel->AddChild(nameText);
        
        auto ageText = new ui::TextBlock();
        ageText->SetText("年龄: " + std::to_string(user->age));
        panel->AddChild(ageText);
        
        border->SetChild(panel);
        return border;
    });
    
    return dataTemplate;
}

// 使用DataTemplate
auto listBox = new ui::ListBox();
listBox->SetItemTemplate(CreateUserTemplate().get());
```

> **提示**：只有在需要数据绑定（UI 自动更新）时才需要 `ObservableProperty`。
> DataTemplate 本身只是将数据转换为可视化元素。

详细文档：[DataTemplate使用指南](Docs/Guides/DataTemplate使用指南.md) | [示例代码](examples/datatemplate/)

## 📚 文档

- [项目结构说明](PROJECT_STRUCTURE.md) - 详细的目录结构和模块说明
- [架构文档](Docs/Architecture.md) - 系统架构设计
- [开发指南](Docs/Development.md) - 开发流程和规范
- [API 文档](Docs/API/README.md) - 详细的 API 参考
- [快速入门](Docs/Guides/GettingStarted.md) - 入门教程

## 🏗️ 项目结构

```
F__K_UI/
├── include/fk/          # 头文件
│   ├── animation/       # 动画系统
│   ├── binding/         # 数据绑定
│   ├── core/            # 核心功能
│   ├── render/          # 渲染系统
│   ├── ui/              # UI 控件
│   └── ...
├── src/                 # 源文件
├── examples/            # 示例代码
├── third_party/         # 第三方依赖
├── Docs/                # 文档
└── CMakeLists.txt       # CMake 配置
```

详细结构请参考 [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md)

## 🎯 核心模块

| 模块 | 说明 |
|------|------|
| **animation** | 动画系统，包括各种动画类型和视觉状态管理 |
| **binding** | 数据绑定系统，实现 MVVM 模式 |
| **core** | 核心功能，如日志、定时器、调度器等 |
| **render** | 渲染系统，基于 OpenGL 的高性能渲染 |
| **performance** | 性能优化模块，包括缓存和对象池 |
| **resources** | 资源管理，主题和样式系统 |
| **ui** | UI 控件和布局容器 |
| **app** | 应用程序框架 |

## 🔧 依赖库

- [GLFW](https://www.glfw.org/) - 窗口创建和输入处理
- [GLAD](https://glad.dav1d.de/) - OpenGL 函数加载
- [FreeType](https://www.freetype.org/) - 字体渲染

## 🤝 贡献

欢迎贡献代码、报告问题或提出建议！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 提交 Pull Request

## 📝 开发状态

当前版本: 0.1 (开发中)

已完成的功能：
- ✅ 基础 UI 控件（Button, TextBlock, TextBox 等）
- ✅ 布局系统（Grid, StackPanel）
- ✅ 数据绑定系统
- ✅ 动画系统
- ✅ 主题系统
- ✅ OpenGL 渲染器

计划中的功能：
- 🔲 更多控件（TreeView, DataGrid 等）
- 🔲 拖放支持
- 🔲 多窗口支持
- 🔲 插件系统
- 🔲 可视化设计器

## 📄 许可证

（根据实际情况填写许可证信息）

## 📧 联系方式

如有问题或建议，请提交 Issue 或联系项目维护者。

---

**注意**: 本项目仍在开发中，API 可能会发生变化。
