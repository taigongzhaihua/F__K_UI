# F__K_UI API 参考

F__K_UI 框架所有类的完整 API 参考。

## 模块索引

### [App 模块](App/) - 应用程序和窗口管理
- [Application](App/Application.md) - 应用程序单例和生命周期

### [Binding 模块](Binding/) - 数据绑定和依赖属性
核心绑定系统类：
- [DependencyObject](Binding/DependencyObject.md) - 具有依赖属性支持的基类
- [DependencyProperty](Binding/DependencyProperty.md) - 属性元数据和注册
- [Binding](Binding/Binding.md) - 数据绑定配置
- [BindingExpression](Binding/BindingExpression.md) - 活动绑定实例
- [BindingContext](Binding/BindingContext.md) - 绑定上下文管理
- [BindingPath](Binding/BindingPath.md) - 属性路径解析

高级绑定：
- [MultiBinding](Binding/MultiBinding.md) - 多源绑定
- [MultiBindingExpression](Binding/MultiBindingExpression.md) - 多绑定实例
- [TemplateBinding](Binding/TemplateBinding.md) - 模板属性绑定

值转换器：
- [IValueConverter](Binding/IValueConverter.md) - 值转换器接口
- [IMultiValueConverter](Binding/IMultiValueConverter.md) - 多值转换器接口
- [ValueConverters](Binding/ValueConverters.md) - 内置转换器

验证：
- [ValidationRule](Binding/ValidationRule.md) - 验证规则基类
- [INotifyDataErrorInfo](Binding/INotifyDataErrorInfo.md) - 错误通知接口

属性变更通知：
- [INotifyPropertyChanged](Binding/INotifyPropertyChanged.md) - 属性变更接口
- [ObservableObject](Binding/ObservableObject.md) - ViewModel 基类

工具类：
- [PropertyStore](Binding/PropertyStore.md) - 属性值存储
- [PropertyAccessors](Binding/PropertyAccessors.md) - 属性访问辅助

### [Core 模块](Core/) - 核心基础设施
- [Dispatcher](Core/Dispatcher.md) - 线程安全的消息调度
- [Event](Core/Event.md) - 类型安全的事件系统
- [Clock](Core/Clock.md) - 时间和帧跟踪
- [Timer](Core/Timer.md) - 定时器实现
- [Logger](Core/Logger.md) - 日志基础设施

### [Render 模块](Render/) - 渲染系统
- [Renderer](Render/Renderer.md) - 主渲染器
- [IRenderer](Render/IRenderer.md) - 渲染器接口
- [GlRenderer](Render/GlRenderer.md) - OpenGL 渲染器实现
- [RenderBackend](Render/RenderBackend.md) - 平台抽象
- [RenderContext](Render/RenderContext.md) - 渲染上下文
- [RenderHost](Render/RenderHost.md) - 渲染宿主管理
- [RenderScene](Render/RenderScene.md) - 场景管理
- [RenderTreeBuilder](Render/RenderTreeBuilder.md) - 渲染树构建
- [RenderCommandBuffer](Render/RenderCommandBuffer.md) - 命令缓冲
- [RenderList](Render/RenderList.md) - 渲染列表管理
- [TextRenderer](Render/TextRenderer.md) - 文本渲染（FreeType）
- [ColorUtils](Render/ColorUtils.md) - 颜色工具

### [UI 模块](UI/) - 用户界面元素

#### 基类
- [Visual](UI/Visual.md) - 视觉树节点基类
- [UIElement](UI/UIElement.md) - 交互元素基类
- [FrameworkElement](UI/FrameworkElement.md) - 布局感知元素基类
- [Control](UI/Control.md) - 可模板化控件基类
- [ContentControl](UI/ContentControl.md) - 单内容控件基类

#### 布局容器
- [Panel](UI/Panel.md) - 多子元素容器基类
- [StackPanel](UI/StackPanel.md) - 顺序布局
- [Grid](UI/Grid.md) - 网格布局
- [VisualCollection](UI/VisualCollection.md) - 视觉子元素集合

#### 控件
- [Button](UI/Button.md) - 按钮控件
- [TextBlock](UI/TextBlock.md) - 文本显示
- [Border](UI/Border.md) - 边框和背景装饰器
- [Image](UI/Image.md) - 图像显示
- [ItemsControl](UI/ItemsControl.md) - 项目集合控件
- [Window](UI/Window.md) - 顶级窗口

#### 形状
- [Shape](UI/Shape.md) - 形状基类
- [Rectangle](UI/Rectangle.md) - 矩形形状
- [Ellipse](UI/Ellipse.md) - 椭圆/圆形形状

#### 样式和模板
- [Style](UI/Style.md) - 样式定义
- [Setter](UI/Setter.md) - 属性设置器
- [FrameworkTemplate](UI/FrameworkTemplate.md) - 模板基类
- [ControlTemplate](UI/ControlTemplate.md) - 控件模板
- [DataTemplate](UI/DataTemplate.md) - 数据模板
- [ResourceDictionary](UI/ResourceDictionary.md) - 资源存储

#### 变换
- [Transform](UI/Transform.md) - 变换基类
- [TranslateTransform](UI/TranslateTransform.md) - 平移变换
- [ScaleTransform](UI/ScaleTransform.md) - 缩放变换
- [RotateTransform](UI/RotateTransform.md) - 旋转变换
- [SkewTransform](UI/SkewTransform.md) - 倾斜变换
- [MatrixTransform](UI/MatrixTransform.md) - 矩阵变换
- [TransformGroup](UI/TransformGroup.md) - 变换组合

#### 输入管理
- [InputManager](UI/InputManager.md) - 输入事件路由
- [FocusManager](UI/FocusManager.md) - 焦点管理

#### 集合
- [ObservableCollection](UI/ObservableCollection.md) - 可观察集合

#### 渲染
- [DrawCommand](UI/DrawCommand.md) - 绘图命令

#### 基本类型和枚举
- [Primitives](UI/Primitives.md) - 基本类型（Point、Size、Rect 等）
- [Enums](UI/Enums.md) - 常用枚举
- [TextEnums](UI/TextEnums.md) - 文本相关枚举
- [Alignment](UI/Alignment.md) - 对齐枚举
- [Thickness](UI/Thickness.md) - 厚度结构
- [CornerRadius](UI/CornerRadius.md) - 圆角半径结构

#### 路由事件
- [RoutedEvent](UI/RoutedEvent.md) - 路由事件系统

## 文档约定

每个类的文档包括：

1. **概述** - 类的目的和职责
2. **继承** - 基类和派生类
3. **属性** - 依赖属性和常规属性
4. **方法** - 公共方法及其参数和返回值
5. **事件** - 可用事件
6. **使用示例** - 演示用法的代码示例
7. **相关类** - 相关文档的链接

## 快速链接

- [入门指南](../GettingStarted.md)
- [开发指南](../Development.md)
- [架构文档](../Designs/)
- [实现状态](../Implementation-Status.md)
