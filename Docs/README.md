# F__K_UI 文档

欢迎来到 F__K_UI 框架文档！本目录包含使用 F__K_UI 进行开发的全面指南和参考资料。

## 📚 文档结构

### 入门指南
- **[入门指南](GettingStarted.md)** - F__K_UI 的第一步
  - 安装和配置
  - 第一个应用程序
  - 核心概念和示例
  - 常用模式

### 开发文档
- **[开发指南](Development.md)** - 面向贡献者和高级用户
  - 项目结构
  - 架构概览
  - 添加新控件
  - 代码风格和约定
  - 调试技巧

### API 参考
- **[API 文档](API/README.md)** - 完整的 API 参考
  - 按模块的文档
  - 类参考
  - 属性和方法详情

### 架构与设计
- **[架构重构](Designs/Architecture-Refactoring.md)** - 框架架构
- **[UI 重构架构](Designs/UI-Refactor-Architecture.md)** - 详细的类设计
- **[内存管理策略](Designs/Memory-Management-Strategy.md)** - 内存处理
- **[窗口实现](Designs/Window-Implementation.md)** - 窗口系统设计

### API 设计文档
- **[可链式内容 API](Designs/ChainableContentAPI.md)** - 流畅 API 设计
- **[子元素批量 API](Designs/ChildrenBatchAPI.md)** - 批量操作
- **[嵌套链式风格](Designs/NestedChainingStyle.md)** - 链式模式

### 状态与进度
- **[实现状态](Implementation-Status.md)** - 当前进度和完成情况跟踪

## 🚀 快速导航

### 我是 F__K_UI 新手
从这里开始：[入门指南](GettingStarted.md)

### 我想构建应用程序
1. 阅读[入门指南](GettingStarted.md)
2. 探索 `/examples` 目录中的示例
3. 查看 [API 参考](API/README.md)了解具体控件

### 我想贡献代码
1. 阅读[开发指南](Development.md)
2. 查看[实现状态](Implementation-Status.md)了解待完成任务
3. 查看[架构文档](Designs/Architecture-Refactoring.md)

### 我在寻找特定信息
- **控件**：参见 [API/UI](API/UI/)
- **数据绑定**：参见 [API/Binding](API/Binding/)
- **渲染**：参见 [API/Render](API/Render/)
- **应用程序**：参见 [API/App](API/App/)

## 🎯 框架概述

F__K_UI 是一个受 WPF 启发的现代 C++ UI 框架，具有以下特点：

### 核心特性
- **声明式 UI**：用于构建界面的流畅、可链式调用的 API
- **数据绑定**：完整的依赖属性和绑定系统
- **灵活布局**：StackPanel、Grid、Canvas 和自定义面板
- **丰富控件**：Button、TextBlock、TextBox、Image、Border 等
- **变换系统**：旋转、缩放、平移、倾斜和矩阵变换
- **事件系统**：鼠标、键盘和焦点管理
- **现代渲染**：基于 OpenGL 的渲染管线

### 当前状态（完成度 82%）
- ✅ **依赖属性**：带变更通知的完整实现
- ✅ **数据绑定**：双向绑定、转换器和表达式
- ✅ **视觉树**：完整的父子层次结构管理
- ✅ **布局系统**：基于约束的测量/排列布局
- ✅ **核心控件**：Button、TextBlock、Border、Image、Grid、StackPanel
- ✅ **变换系统**：完整的 2D 变换支持
- ✅ **输入管理**：鼠标、键盘和焦点导航
- ✅ **渲染**：完整的 OpenGL 后端，支持文本和图像
- ⏳ **样式/模板**：计划在 Phase 2 实现

## 📦 模块概述

### App 模块（`fk::app`）
应用程序生命周期和窗口管理
- `Application` - 应用程序单例
- `Window` - 顶级窗口

### Core 模块（`fk::core`）
基础工具和基础设施
- `DependencyObject` - 具有依赖属性的对象基类
- `Dispatcher` - 线程安全的事件调度
- `Event<T>` - 类型安全的事件系统

### Binding 模块（`fk::binding`）
数据绑定和依赖属性系统
- `DependencyProperty` - 属性元数据
- `Binding` - 绑定配置
- `BindingExpression` - 活动绑定
- `ObservableObject` - ViewModel 基类

### UI 模块（`fk::ui`）
可视元素和控件
- **元素**：`Visual`、`UIElement`、`FrameworkElement`
- **容器**：`Panel`、`StackPanel`、`Grid`、`Canvas`
- **控件**：`Control`、`ContentControl`、`Button`、`TextBox`
- **显示**：`TextBlock`、`Border`、`Image`
- **集合**：`ItemsControl`、`ScrollViewer`

### Render 模块（`fk::render`）
渲染管线和图形
- `IRenderer` - 渲染器接口
- `GlRenderer` - OpenGL 实现
- `RenderCommand` - 绘图命令
- `TextRenderer` - 字体渲染

## 🔧 技术栈

- **语言**：C++17/20
- **构建**：CMake 3.20+
- **图形**：OpenGL 3.3+
- **窗口**：GLFW 3.x
- **图像**：stb_image
- **字体**：FreeType

## 📖 示例代码

### Hello World
```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

int main() {
    auto app = app::Application::Create();
    auto window = app->CreateWindow();
    window->Title("Hello F__K_UI")->Width(800)->Height(600);
    
    auto button = window->SetContent<Button>();
    button->Content("Click Me!")->Width(200)->Height(50);
    
    return app->Run();
}
```

### 布局示例
```cpp
auto stack = window->SetContent<StackPanel>();
stack->Orientation(Orientation::Vertical)->Spacing(10);

stack->AddChild<Button>()->Content("Button 1");
stack->AddChild<Button>()->Content("Button 2");
stack->AddChild<TextBlock>()->Text("Some text");
```

### 变换示例
```cpp
auto rotateTransform = new RotateTransform(45.0f, 100.0f, 100.0f);
element->SetRenderTransform(rotateTransform);
```

## 🛠️ 构建框架

```bash
# Clone
git clone https://github.com/taigongzhaihua/F__K_UI.git
cd F__K_UI

# Build
mkdir build && cd build
cmake ..
cmake --build . -j8

# Run examples
./hello_world
./phase1_enhancement_demo
```

## 📈 开发路线图

### ✅ Phase 1（82% - 已完成）
核心框架，包含基本控件和变换

### 🔄 Phase 2（进行中）
样式、模板和高级控件

### 🎯 Phase 3（计划中）
动画、主题和高级功能

详细进度请参见[实现状态](Implementation-Status.md)。

## 💡 获取帮助

- **入门**：[入门指南](GettingStarted.md)
- **API 问题**：查看 [API 文档](API/README.md)
- **架构**：查看[设计文档](Designs/)
- **错误/问题**：在 GitHub 上提交 issue

## 🤝 贡献

欢迎贡献！查看[开发指南](Development.md)了解：
- 代码风格指南
- 架构概述
- 添加新功能
- 测试流程

---

**快乐编码！🎉**

*文档最后更新：2025年11月*
