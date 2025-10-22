# FK UI 框架设计草案

## 1. 设计目标
- **链式 API 构建界面**：所有 UI 元素通过流畅的链式调用完成配置，鼓励声明式写法。
- **数据驱动**：为后续实现双向数据绑定预留基础设施，支持 MVVM 风格开发。
- **可扩展的控件体系**：提供可组合的逻辑树与视觉树结构，方便派生控件和自定义样式。
- **跨平台渲染后端**：以 OpenGL/GLFW 为首个实现，未来可以替换或扩展其他后端。
- **松耦合模块划分**：核心事件、绑定、UI 控件、渲染后端、应用入口解耦，支持独立测试与迭代。

## 2. 模块划分
```
fk::core     -> 事件、调度器、基础类型
fk::binding  -> 依赖属性系统、BindingExpression、数据上下文
fk::ui       -> View/Control 层级、布局系统、模板与资源
fk::render   -> 渲染抽象、OpenGL 实现、字体渲染
fk::app      -> Application 生命周期、窗口管理
fk::theme    -> 资源字典、样式系统（未来）
```

### 2.1 fk::core
- `Event<T...>`：沿用现有实现，提供 RAII `Connection`。
- `Dispatcher`：单线程 UI 循环调度任务，后端由 GLFW 事件循环驱动。
- `Clock`/`Timer`：节流动画、定时任务。
- `PropertyChangedEvent`：通知属性变化，供绑定层使用。

### 2.2 fk::binding
- `DependencyProperty`：描述属性元数据，包含默认值、验证器、变更回调。
- `PropertyStore`：针对每个对象维护属性优先级（本地值、绑定值、继承值、动画值）。
- `Binding` 与 `BindingExpression`：封装源路径解析、模式（OneWay/TwoWay/OneTime）、更新触发。
- `BindingContext`：与逻辑树同步，提供 `DataContext` 值继承。
- `ObservableObject`：类似 `INotifyPropertyChanged`，提供 `RaisePropertyChanged`。

### 2.3 fk::ui
- `DependencyObject`：绑定层基类，持有 `PropertyStore`。
- `DispatcherObject`：封装线程关联，与 GLFW 主线程绑定。
- `Visual`：承载视觉树节点，管理 `Render` 与 `HitTest`。
- `UIElement`：输入与布局接口，方法 `Measure(const Size&)` / `Arrange(const Rect&)`。
- `FrameworkElement`：加入 `Width/Height/Margin`、`Resources`、`DataContext`、`OnApplyTemplate`。
- `View<T>`/`Control<T>`：在 `FrameworkElement` 基础上提供链式配置；`Control` 增加模板、命令、焦点管理。
- 布局类：`Panel`（抽象），`StackPanel`、`Grid`、`DockPanel` 等。
- 内容控件：`ContentControl`（单子内容），`ItemsControl`（集合 + `ItemsPanel` + `ItemTemplate`）。
- 资源系统：`ResourceDictionary`、`Style`、`Setter`、`Trigger`（后续迭代）。

### 2.4 fk::render
- `IRenderer` 接口：`BeginFrame`、`EndFrame`、`DrawGeometry`。
- `GlRenderer`：初始化 glad/GLFW，维持 SwapChain。
- `FontAtlas`, `TextLayout`：基于 FreeType 的排版和字形缓存。
- `RenderTreeBuilder`：由 `Visual` 层生成绘制命令。

### 2.5 fk::app
- `Application`：唯一实例，负责窗口列表、全局资源字典、事件 `Startup/Exit`。
- `Window`：作为顶层 `FrameworkElement` 派生，持有根视觉树、与平台窗口对应。
- `WindowManager`：未来支持多平台窗口实现（Win32、X11 等）。

## 3. 链式 API 设计
- 采用 CRTP 模式保证返回派生类型：
  ```cpp
  template <class Derived>
  class View : public FrameworkElement {
  public:
      Derived* Width(float value) { SetValue(WidthProperty(), value); return static_cast<Derived*>(this); }
  };
  ```
- 控件在构造时注册依赖属性：
  ```cpp
  FK_DEPENDENCY_PROPERTY_DECL(Button, Content, fk::ui::ViewPtr);
  ```
- 绑定语法：
  ```cpp
  using namespace fk::binding;
  textBox->Text(Bind("Name")
      .Mode(BindingMode::TwoWay)
      .UpdateSourceTrigger(UpdateSourceTrigger::PropertyChanged));
  ```
- 资源引用：
  ```cpp
  button->Style(FindResource("PrimaryButton"));
  ```

## 4. 布局流程
1. `Measure` 阶段自上而下传递可用空间，元素计算 `DesiredSize`。
2. `Arrange` 阶段自上而下确定最终摆放矩形，保存到 `RenderBounds`。
3. `RenderTreeBuilder` 深度遍历视觉树，依据 `RenderBounds` 生成绘制指令。

## 5. 数据绑定机制
- 对象需继承 `DependencyObject` 或实现 `IBindingTarget`。
- 绑定表达式在元素进入逻辑树时解析，监听 `PropertyChangedEvent`。
- 双向绑定：
  - 目标属性变更触发 `BindingExpression::UpdateSource()` 写回。
  - 使用值转换器 (`IValueConverter`) 支持类型转换。
- `BindingPath` 支持简单点号导航和索引：`"Address.City"`、`"Items[0].Name"`。

## 6. 事件与命令
- 路由事件系统：隧道(`Preview`) + 冒泡，与逻辑树结合。
- 命令接口：
  ```cpp
  class ICommand {
  public:
      virtual bool CanExecute(const Any& parameter) const = 0;
      virtual void Execute(const Any& parameter) = 0;
  };
  ```
- 控件通过 `CommandBinding` 连接事件与命令。

## 7. 渲染与平台适配
- GLFW 窗口驱动：
  - 注册输入回调，将事件转化为路由事件。
  - 使用 `Dispatcher` 投递任务回 UI 线程。
- OpenGL Rendering：
  - 基础形状：矩形、圆角矩形、路径。
  - 文本：FreeType + SDF 纹理，实现清晰缩放。
- 后续可扩展 Vulkan/DirectX 渲染器，实现 `IRenderer`。

## 8. 资源与主题
- 全局 `ResourceDictionary` 收纳 `Color`, `Brush`, `Style`。
- 主题切换通过合并字典实现：`Application::Resources().Merge(lightTheme);`
- `Style` 定义 Setter 和 Trigger，允许基于 `DependencyProperty` 响应状态变化。

## 9. 测试与示例
- 单元测试：
  - 事件系统；
  - 依赖属性注册与值优先级；
  - 绑定表达式更新逻辑；
  - 布局测量与安排。
- 示例应用：
  - `HelloControls`：展示 Button、TextBox、ListView、Template。
  - `BindingDemo`：演示双向绑定、命令与验证。
  - `ThemeGallery`：切换不同资源字典。

## 10. 迭代路线
1. **基础框架**：完成模块划分、链式控件 API、OpenGL 渲染管线。
2. **布局与输入**：实现面板、路由事件、命令系统。
3. **绑定核心**：引入依赖属性、BindingExpression、数据上下文。
4. **样式与模板**：ControlTemplate、DataTemplate、资源字典。
5. **高级特性**：动画系统、视觉状态管理、3D 视图支持。

---
> 本文档为设计起点，后续迭代时应补充类图、序列图及详细接口说明。