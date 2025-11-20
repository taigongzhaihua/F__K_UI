# FK_UI 项目结构说明

## 项目概述

FK_UI 是一个 C++ UI 框架，借鉴了 WPF 的设计理念，提供了丰富的 UI 控件、数据绑定、动画系统和渲染功能。

## 目录结构

```
F__K_UI/
├── include/fk/              # 头文件目录
│   ├── animation/           # 动画系统头文件
│   │   ├── AnimationClock.h
│   │   ├── AnimationManager.h
│   │   ├── ColorAnimation.h
│   │   ├── DoubleAnimation.h
│   │   ├── PointAnimation.h
│   │   ├── ThicknessAnimation.h
│   │   ├── Storyboard.h
│   │   ├── Timeline.h
│   │   ├── VisualState.h
│   │   ├── VisualStateGroup.h
│   │   ├── VisualStateManager.h
│   │   └── VisualTransition.h
│   │
│   ├── app/                 # 应用程序框架头文件
│   │   └── Application.h
│   │
│   ├── binding/             # 数据绑定系统头文件
│   │   ├── Binding.h
│   │   ├── BindingContext.h
│   │   ├── BindingExpression.h
│   │   ├── BindingPath.h
│   │   ├── DependencyObject.h
│   │   ├── DependencyProperty.h
│   │   ├── INotifyPropertyChanged.h
│   │   ├── MultiBinding.h
│   │   ├── ObservableObject.h
│   │   ├── PropertyStore.h
│   │   ├── TemplateBinding.h
│   │   └── ValueConverters.h
│   │
│   ├── core/                # 核心功能头文件
│   │   ├── Clock.h
│   │   ├── Dispatcher.h
│   │   ├── Event.h
│   │   ├── Logger.h
│   │   └── Timer.h
│   │
│   ├── performance/         # 性能优化模块头文件
│   │   ├── GeometryCache.h
│   │   ├── MaterialPool.h
│   │   ├── ObjectPool.h
│   │   └── RenderBatcher.h
│   │
│   ├── render/              # 渲染系统头文件
│   │   ├── ColorUtils.h
│   │   ├── GlRenderer.h
│   │   ├── IRenderer.h
│   │   ├── RenderCommand.h
│   │   ├── RenderCommandBuffer.h
│   │   ├── RenderContext.h
│   │   ├── RenderHost.h
│   │   ├── RenderList.h
│   │   ├── RenderScene.h
│   │   ├── RenderTreeBuilder.h
│   │   └── TextRenderer.h
│   │
│   ├── resources/           # 资源管理头文件
│   │   ├── DynamicResource.h
│   │   ├── Theme.h
│   │   └── ThemeManager.h
│   │
│   └── ui/                  # UI 控件头文件
│       ├── Alignment.h
│       ├── Border.h
│       ├── Brush.h
│       ├── Button.h
│       ├── ComboBox.h
│       ├── ContentControl.h
│       ├── ContentPresenter.h
│       ├── Control.h
│       ├── ControlTemplate.h
│       ├── CornerRadius.h
│       ├── DataTemplate.h
│       ├── DrawCommand.h
│       ├── Enums.h
│       ├── FocusManager.h
│       ├── FrameworkElement.h
│       ├── FrameworkTemplate.h
│       ├── Grid.h
│       ├── Image.h
│       ├── InputManager.h
│       ├── ItemsControl.h
│       ├── ListBox.h
│       ├── NameScope.h
│       ├── Panel.h
│       ├── Popup.h
│       ├── PropertyMacros.h
│       ├── RenderBackend.h
│       ├── Renderer.h
│       ├── ScrollBar.h
│       ├── ScrollViewer.h
│       ├── Setter.h
│       ├── Shape.h
│       ├── StackPanel.h
│       ├── Style.h
│       ├── TextBlock.h
│       ├── TextBox.h
│       ├── Thickness.h
│       ├── Transform.h
│       ├── UIElement.h
│       ├── Visual.h
│       └── Window.h
│
├── src/                     # 源文件目录（与 include 结构对应）
│   ├── animation/           # 动画系统实现
│   ├── app/                 # 应用程序框架实现
│   ├── binding/             # 数据绑定系统实现
│   ├── core/                # 核心功能实现
│   ├── performance/         # 性能优化模块实现
│   ├── render/              # 渲染系统实现
│   ├── resources/           # 资源管理实现
│   └── ui/                  # UI 控件实现
│
├── examples/                # 示例代码
│   └── main.cpp             # 主示例程序
│
├── third_party/             # 第三方依赖库
│   ├── include/             # 第三方库头文件
│   │   ├── GLFW/            # GLFW 窗口库
│   │   ├── glad/            # OpenGL 加载库
│   │   └── freetype/        # FreeType 字体库
│   ├── lib/                 # 第三方库静态库/动态库
│   └── src/                 # 第三方库源码
│       ├── glad/            # GLAD 源码
│       └── freetype/        # FreeType 源码
│
├── Docs/                    # 文档目录
│   ├── API/                 # API 文档
│   │   ├── App/             # 应用程序 API
│   │   ├── Binding/         # 绑定系统 API
│   │   ├── Core/            # 核心 API
│   │   ├── Render/          # 渲染 API
│   │   └── UI/              # UI 控件 API
│   │
│   ├── Design/              # 设计文档
│   │   ├── App/
│   │   ├── Binding/
│   │   ├── Core/
│   │   ├── Render/
│   │   └── UI/
│   │
│   ├── Tasks/               # 任务规划文档
│   │
│   ├── Guides/              # 用户指南
│   │   ├── CUSTOM_VISUAL_STATES_GUIDE.md
│   │   ├── GRID_QUICK_START.md
│   │   ├── TARGETNAME_USAGE_GUIDE.md
│   │   ├── TESTING_GUIDE.md
│   │   └── ...
│   │
│   ├── Archive/             # 归档文档
│   │   ├── Fixes/           # 修复记录
│   │   ├── Implementation/  # 实现总结
│   │   └── PhaseReports/    # 阶段报告
│   │
│   ├── Architecture.md      # 架构说明
│   ├── Development.md       # 开发指南
│   ├── CHANGELOG.md         # 变更日志
│   └── README.md            # 文档说明
│
├── build/                   # 构建输出目录（git 忽略）
│
├── .vscode/                 # VS Code 配置目录（git 忽略）
│
├── CMakeLists.txt           # CMake 构建配置
├── .gitignore               # Git 忽略配置
└── PROJECT_STRUCTURE.md     # 本文件

```

## 核心模块说明

### 1. Animation（动画系统）
- **功能**: 提供动画、过渡效果和视觉状态管理
- **核心类**: 
  - `AnimationManager`: 动画管理器
  - `VisualStateManager`: 视觉状态管理器
  - `DoubleAnimation`, `ColorAnimation`: 各种动画类型
  - `Storyboard`: 故事板，组合多个动画

### 2. Binding（数据绑定）
- **功能**: 实现 MVVM 模式的数据绑定机制
- **核心类**:
  - `DependencyObject`: 依赖对象基类
  - `DependencyProperty`: 依赖属性系统
  - `Binding`: 数据绑定
  - `BindingExpression`: 绑定表达式
  - `INotifyPropertyChanged`: 属性变更通知接口

### 3. Core（核心功能）
- **功能**: 提供基础设施，如日志、定时器、调度器等
- **核心类**:
  - `Dispatcher`: 事件调度器
  - `Timer`: 定时器
  - `Clock`: 时钟
  - `Logger`: 日志系统

### 4. Render（渲染系统）
- **功能**: 处理图形渲染、场景管理和渲染优化
- **核心类**:
  - `GlRenderer`: OpenGL 渲染器
  - `RenderContext`: 渲染上下文
  - `RenderScene`: 渲染场景
  - `TextRenderer`: 文本渲染器

### 5. Performance（性能优化）
- **功能**: 提供缓存、对象池等性能优化机制
- **核心类**:
  - `GeometryCache`: 几何体缓存
  - `MaterialPool`: 材质池
  - `RenderBatcher`: 渲染批处理器

### 6. Resources（资源管理）
- **功能**: 管理主题、样式和动态资源
- **核心类**:
  - `ThemeManager`: 主题管理器
  - `DynamicResource`: 动态资源

### 7. UI（用户界面）
- **功能**: 提供各种 UI 控件和布局容器
- **核心类**:
  - **基础类**: `UIElement`, `FrameworkElement`, `Visual`
  - **控件**: `Button`, `TextBlock`, `TextBox`, `ComboBox`, `ListBox`
  - **容器**: `Panel`, `StackPanel`, `Grid`, `Border`
  - **模板**: `ControlTemplate`, `DataTemplate`
  - **其他**: `Window`, `Popup`, `ScrollViewer`

## 编译说明

### Windows (MinGW)
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

### Linux
```bash
mkdir build
cd build
cmake ..
make
```

## 依赖库

1. **GLFW**: 窗口创建和输入处理
2. **GLAD**: OpenGL 函数加载
3. **FreeType**: 字体渲染

## 开发规范

1. **头文件**: 放在 `include/fk/` 对应模块目录下
2. **源文件**: 放在 `src/` 对应模块目录下
3. **命名空间**: 所有类都在 `fk` 命名空间下
4. **命名约定**: 
   - 类名使用 PascalCase
   - 函数/方法使用 PascalCase
   - 变量使用 camelCase
   - 常量使用 UPPER_SNAKE_CASE

## 相关文档

- [架构说明](Docs/Architecture.md)
- [开发指南](Docs/Development.md)
- [快速开始](Docs/Guides/GettingStarted.md)
- [API 文档](Docs/API/README.md)
- [变更日志](Docs/CHANGELOG.md)

## 许可证

（根据实际情况填写）

## 贡献

欢迎提交 Issue 和 Pull Request！
