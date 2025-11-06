# F__K_UI API 文档索引

本目录包含 F__K_UI 框架的完整 API 文档。

## 📚 文档结构

### 核心模块
- **[Core 模块](./Core.md)** - 核心基础设施
  - Dispatcher - 线程调度器
  - Event - 事件系统
  - Logger - 日志系统
  - Timer - 定时器
  - Clock - 时钟系统

### 数据绑定模块
- **[Binding 模块](./Binding.md)** - 数据绑定与依赖属性系统
  - DependencyProperty - 依赖属性
  - DependencyObject - 依赖对象
  - Binding - 数据绑定
  - BindingExpression - 绑定表达式
  - ObservableObject - 可观察对象
  - INotifyPropertyChanged - 属性变更通知接口

### UI 模块
- **[UI 模块](./UI.md)** - 用户界面控件
  - 基础类
    - DispatcherObject - 调度器对象
    - DependencyObject - UI 依赖对象
    - Visual - 视觉元素
    - UIElement - UI 元素基类
    - FrameworkElement - 框架元素基类
  - 控件
    - Control - 控件基类
    - ContentControl - 内容控件
    - Button - 按钮
    - TextBlock - 文本块
  - 布局
    - Panel - 面板基类
    - StackPanel - 堆栈面板
    - Decorator - 装饰器
  - 容器
    - ItemsControl - 项集合控件
    - ScrollViewer - 滚动视图
    - ScrollBar - 滚动条
  - 窗口
    - Window - 窗口类
    - WindowInteropHelper - 窗口互操作帮助器
    - View - 视图基类

### 渲染模块
- **[Render 模块](./Render.md)** - 渲染引擎
  - IRenderer - 渲染器接口
  - GlRenderer - OpenGL 渲染器
  - RenderCommand - 渲染命令
  - RenderCommandBuffer - 渲染命令缓冲
  - RenderHost - 渲染主机
  - RenderScene - 渲染场景
  - TextRenderer - 文本渲染器
  - ColorUtils - 颜色工具

### 应用模块
- **[App 模块](./App.md)** - 应用程序框架
  - Application - 应用程序类

## 🔍 快速导航

### 按功能分类

#### 窗口与应用
- [Application](./App.md#application) - 应用程序生命周期管理
- [Window](./UI.md#window) - 窗口创建与管理
- [View](./UI.md#view) - 视图基类

#### 布局系统
- [Panel](./UI.md#panel) - 面板基类
- [StackPanel](./UI.md#stackpanel) - 堆栈布局
- [ScrollViewer](./UI.md#scrollviewer) - 滚动容器

#### 控件系统
- [Control](./UI.md#control) - 控件基类
- [Button](./UI.md#button) - 按钮控件
- [TextBlock](./UI.md#textblock) - 文本显示
- [ItemsControl](./UI.md#itemscontrol) - 列表控件

#### 数据绑定
- [DependencyProperty](./Binding.md#dependencyproperty) - 依赖属性系统
- [Binding](./Binding.md#binding) - 数据绑定
- [ObservableObject](./Binding.md#observableobject) - MVVM 支持

#### 事件与调度
- [Event](./Core.md#event) - 事件系统
- [Dispatcher](./Core.md#dispatcher) - 线程调度
- [Timer](./Core.md#timer) - 定时器

#### 渲染系统
- [IRenderer](./Render.md#irenderer) - 渲染接口
- [GlRenderer](./Render.md#glrenderer) - OpenGL 实现
- [RenderCommand](./Render.md#rendercommand) - 渲染命令

## 📖 使用指南

### 新手入门
1. 从 [Application](./App.md#application) 开始了解应用程序框架
2. 学习 [Window](./UI.md#window) 和基本控件的使用
3. 了解 [DependencyProperty](./Binding.md#dependencyproperty) 和数据绑定

### 进阶主题
- 自定义控件开发
- 渲染管道定制
- 性能优化技巧

## 🔗 相关文档

- [设计文档](../Designs/) - 框架设计理念与架构
- [示例代码](../../examples/) - 实际使用示例
- [API 一致性检查](../API-Consistency-Check.md)
- [API 设计问题](../API-Design-Issues.md)
- [命名规范](../Naming-Convention.md)

## 📝 文档说明

- 所有 API 文档包含类/方法的签名、参数说明和使用示例
- 标注了线程安全性信息
- 包含版本更新记录
- 提供了最佳实践建议

## 🔄 更新日志

- 2025-11-03: 初始版本,完整梳理所有已实现 API
