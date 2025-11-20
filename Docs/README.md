# FK_UI 文档索引

欢迎来到 FK_UI 框架文档！本目录包含完整的开发指南、API 参考和架构设计文档。

## 📚 文档概览

### 🚀 新手入门
- [快速开始](Guides/GettingStarted.md) - 第一个 FK_UI 应用
- [Grid 快速参考](GRID_QUICK_REFERENCE.md) - Grid 布局入门
- [测试指南](Guides/TESTING_GUIDE.md) - 如何测试应用

### 🏗️ 架构与设计
- [架构说明](Architecture.md) - 整体架构设计
- [开发计划](DevelopmentPlan.md) - 项目开发路线图
- [实现状态](Implementation-Status.md) - 功能实现情况

### 📋 开发指南
- [开发手册](Development.md) - 开发流程和规范
- [Linux 编译指南](Linux-Build-Guide.md) - Linux 系统编译说明

### 📚 API 文档

#### 应用框架
- [Application API](API/App/README.md) - 应用程序框架

#### 数据绑定
- [Binding API](API/Binding/README.md) - 数据绑定系统
- [依赖属性](API/Binding/DependencyProperty.md) - 依赖属性详解

#### 核心功能
- [Core API](API/Core/README.md) - 核心功能模块

#### 渲染系统
- [Render API](API/Render/README.md) - 渲染系统

#### UI 控件
- [UI Controls API](API/UI/README.md) - UI 控件完整参考
- [Grid API](GRID_API_REFERENCE.md) - Grid 布局容器
- [Button API](API/UI/Button.md) - 按钮控件
- [TextBlock API](API/UI/TextBlock.md) - 文本显示控件

## 🚀 快速导航

### 我是 FK_UI 新手
1. 从[快速开始](Guides/GettingStarted.md)开始
2. 探索 `/examples` 目录中的示例
3. 查看 [Grid 快速参考](GRID_QUICK_REFERENCE.md)

### 我想构建应用程序
1. 阅读[开发手册](Development.md)
2. 参考 [API 文档](API/README.md)
3. 学习[数据绑定](API/Binding/README.md)

### 我想贡献代码
1. 阅读[开发手册](Development.md)
2. 查看[实现状态](Implementation-Status.md)了解待完成任务
3. 查看[架构说明](Architecture.md)

### 我在寻找特定信息
- **控件**: 参见 [API/UI](API/UI/)
- **数据绑定**: 参见 [API/Binding](API/Binding/)
- **渲染**: 参见 [API/Render](API/Render/)
- **应用程序**: 参见 [API/App](API/App/)

## 🎨 用户指南

### 控件使用
- [Grid 使用指南](GRID_QUICK_START.md) - Grid 布局详解
- [Grid 演示指南](GRID_DEMO_GUIDE.md) - Grid 示例
- [自定义视觉状态](Guides/CUSTOM_VISUAL_STATES_GUIDE.md) - 视觉状态管理
- [TargetName 使用指南](Guides/TARGETNAME_USAGE_GUIDE.md) - 动画目标命名

### 高级功能
- [动画类型使用](ANIMATION_TYPES_USAGE.md) - 动画系统
- [ElementName 绑定](ELEMENTNAME_BINDING_说明.md) - 元素名称绑定
- [命名作用域](命名作用域说明.md) - 命名作用域说明
- [键盘和焦点管理](键盘事件和焦点管理说明.md) - 输入处理
- [鼠标事件验证](鼠标事件验证说明.md) - 鼠标交互

### 性能优化
- [性能优化方案](性能优化方案.md) - 性能优化策略
- [混合优化方案](混合优化方案.md) - 综合优化方法

## 🎯 核心模块

### App 模块 (`fk::app`)
- `Application` - 应用程序生命周期管理
- `Window` - 窗口系统

### Binding 模块 (`fk::binding`)
- `DependencyObject` - 依赖对象基类
- `DependencyProperty` - 依赖属性系统
- `Binding` - 数据绑定
- `BindingExpression` - 绑定表达式

### Core 模块 (`fk::core`)
- `Dispatcher` - 事件调度器
- `Timer` - 定时器
- `Logger` - 日志系统

### Render 模块 (`fk::render`)
- `GlRenderer` - OpenGL 渲染器
- `RenderContext` - 渲染上下文
- `TextRenderer` - 文本渲染器

### UI 模块 (`fk::ui`)
- 基础类: `UIElement`, `FrameworkElement`, `Visual`
- 控件: `Button`, `TextBlock`, `TextBox`, `ComboBox`, `ListBox`
- 容器: `Panel`, `StackPanel`, `Grid`, `Border`
- 模板: `ControlTemplate`, `DataTemplate`

### Animation 模块 (`fk::animation`)
- `AnimationManager` - 动画管理器
- `VisualStateManager` - 视觉状态管理
- 各种动画类型: `DoubleAnimation`, `ColorAnimation` 等

### Performance 模块 (`fk::performance`)
- `GeometryCache` - 几何体缓存
- `MaterialPool` - 材质池
- `RenderBatcher` - 渲染批处理

## 📦 设计文档

详细的模块设计文档位于 [Design](Design/) 目录：
- [应用框架设计](Design/App/)
- [数据绑定设计](Design/Binding/)
- [核心功能设计](Design/Core/)
- [渲染系统设计](Design/Render/)
- [UI 控件设计](Design/UI/)

## 📋 任务与计划

项目任务和阶段规划位于 [Tasks](Tasks/) 目录

## 📦 归档文档

历史文档和修复记录位于 [Archive](Archive/) 目录：
- [Fixes](Archive/Fixes/) - Bug 修复记录
- [Implementation](Archive/Implementation/) - 功能实现总结
- [PhaseReports](Archive/PhaseReports/) - 阶段完成报告

## 🔍 按主题查找

### 布局相关
- Grid: [API](GRID_API_REFERENCE.md) | [快速开始](GRID_QUICK_START.md) | [演示](GRID_DEMO_GUIDE.md)
- StackPanel: [API](API/UI/StackPanel.md)
- Panel: [API](API/UI/Panel.md)

### 控件相关
- Button: [API](API/UI/Button.md) | [视觉状态](按钮视觉状态自定义说明.md)
- TextBlock: [API](API/UI/TextBlock.md)
- Border: [API](API/UI/Border.md)

### 数据绑定
- Binding: [API](API/Binding/Binding.md) | [ElementName](ELEMENTNAME_BINDING_说明.md)
- DependencyProperty: [API](API/Binding/DependencyProperty.md)

### 动画和视觉状态
- VisualStateManager: [使用指南](Guides/CUSTOM_VISUAL_STATES_GUIDE.md)
- Animation: [类型使用](ANIMATION_TYPES_USAGE.md)

## 📝 变更日志

- [CHANGELOG.md](CHANGELOG.md) - 版本变更历史

## 📞 获取帮助

- 查看 [架构文档](Architecture.md) 了解系统设计
- 查看 [开发手册](Development.md) 了解开发流程
- 查看 [API 文档](API/) 了解具体接口
- 提交 Issue 报告问题或提出建议

## 🤝 贡献文档

欢迎改进文档！文档贡献指南：
1. 确保文档清晰、准确
2. 提供代码示例
3. 保持一致的格式
4. 更新索引文件

---

*最后更新: 2025-11-20*
- [x] 渲染系统 (Renderer, RenderBackend)
- [x] **Transform 系统** ⭐
- [x] **Image 真实加载** ⭐
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
