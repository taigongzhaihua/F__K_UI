# fk::ui 模块设计

## 1. 模块定位
- 提供 UI 元素的层级体系，涵盖视觉树、逻辑树、布局、输入与命令处理。
- 将 `fk::binding` 的依赖属性机制落地到控件上，并提供链式 API。
- 与 `fk::render` 配合生成绘制命令，与 `fk::core` 的 Dispatcher/事件协同。

## 2. 核心概念

### 2.1 DispatcherObject
- 所有 UI 元素都继承自 `DispatcherObject`，记录所在线程的 `core::Dispatcher*`。
- 提供 `VerifyAccess()`，确保 UI 操作发生在同一线程。

### 2.2 DependencyObject
- 继承 `DispatcherObject`，组合 `binding::PropertyStore`。
- 提供模板方法 `GetValue<T>(const DependencyProperty&)`、`SetValue()`、`ClearValue()`。
- 处理 `DataContext` 继承与 `PropertyChanged` 事件分发。

### 2.3 Logical Tree vs Visual Tree
- **逻辑树**：表示元素在概念上的父子关系（用于资源、DataContext、事件路由）。
  - 接口：`GetLogicalParent()`, `LogicalChildren()`。
  - 事件：`OnAttachedToLogicalTree`, `OnDetachedFromLogicalTree`。
- **视觉树**：表示实际渲染树，`Visual` 对象组成。
  - `Visual` 持有变换、裁剪、子视觉集合。
  - `Visual` 负责与 `fk::render` 交互，生成 `RenderNode`。
- 对于控件，逻辑树和视觉树通常一致；对于模板控件，视觉树可与逻辑树不同。

### 2.4 UIElement
- 继承 `DependencyObject`，增加输入与布局接口。
- 属性：`Visibility`, `IsEnabled`, `Opacity`, `Clip`, `RenderTransform`。
- 布局流程：
  - `Measure(const Size& availableSize)` → `Size DesiredSize`。
  - `Arrange(const Rect& finalRect)` → `Rect RenderRect`。
  - `MeasureOverride` 与 `ArrangeOverride` 供派生类重写。
- 输入事件：`Mouse`, `Keyboard`, `Focus`, 采用路由事件 (`RoutingStrategy::Tunnel/Bubble/Direct`)。

### 2.5 FrameworkElement
- 引入以下功能：
  - `Width/Height`, `MinWidth`, `Margin`, `HorizontalAlignment/VerticalAlignment`。
  - `DataContext`, `ResourceDictionary`。
  - `Style`, `Template`, `Triggers`。
  - 生命周期事件：`Initialized`, `Loaded`, `Unloaded`, `OnApplyTemplate()`。
- 管理 `LogicalChildren` 集合，提供 `AddLogicalChild`, `RemoveLogicalChild`。

### 2.6 View<TDerived>
- 基于 `FrameworkElement` 的 CRTP 类，支持链式 API。
- 示例：
  ```cpp
  template <typename Derived>
  class View : public FrameworkElement {
  public:
      Derived* Width(float value) {
          SetValue(WidthProperty(), value);
          return static_cast<Derived*>(this);
      }
      // 其他链式方法...
  };
  ```
- `Control<TDerived>` 继承 `View<TDerived>`，添加控件专有属性和命令支持。

### 2.7 控件分类
- **内容控件 (`ContentControl`)**：单一内容，支持 `ContentTemplate`。
- **项目控件 (`ItemsControl`)**：管理集合，支持 `ItemsPanel`, `ItemTemplate`。
- **装饰器 (`Decorator`)**：包装单子元素，添加边框/阴影等。
- **面板 (`Panel`)**：布局子元素，典型实现：`StackPanel`, `Grid`, `DockPanel`, `Canvas`。
- **基础控件**：`TextBlock`, `TextBox`, `Button`, `ToggleButton`, `ListBox` 等。

### 2.8 模板与样式
- `Style`：由 `Setter` 集合构成，可指定 `TargetType`。
- `ControlTemplate`：描述控件的视觉结构，使用 `TemplateBinding` 将模板部件与控件属性连接。
- `DataTemplate`：用于 `ItemsControl` 将数据对象转换成 UI 元素。
- 模板应用流程：
  1. 控件加载时 `ApplyTemplate()`。
  2. 创建 Template Visual Tree，注入命名部件。
  3. 调用 `OnApplyTemplate()` 让控件获取部件引用。

### 2.9 命令系统
- `ICommandSource` 接口：`GetCommand()`, `GetCommandParameter()`。
- `CommandBinding`：绑定命令与处理程序，支持 `CanExecute`/`Executed`。
- 控件（如 `ButtonBase`）处理输入事件并触发命令。

### 2.10 事件路由
- 路由事件类型：`Direct`, `Bubble`, `Tunnel`。
- 通过逻辑树传播，`RaiseEvent(RoutedEventArgs&)`。
- 支持 `Handled` 标记、`AddHandler`, `RemoveHandler`。

## 3. 布局系统设计
- `Panel` 重写 `MeasureOverride`, `ArrangeOverride`。
- `StackPanel`：
  ```cpp
  Size StackPanel::MeasureOverride(const Size& available) override;
  Size StackPanel::ArrangeOverride(const Size& finalSize) override;
  ```
- `Grid`：管理行列定义，支持星号、自动尺寸。
- `Canvas`：基于绝对坐标的面板。
- `LayoutManager`：调度布局计算，标记脏元素，批量刷新。

## 4. 输入处理
- 集成 GLFW 输入回调 → 转换为 `RoutedEventArgs`。
- 焦点管理：
  - `FocusManager` 记录当前焦点元素，处理 Tab 导航。
  - `UIElement::Focus()` 更改焦点。
- 鼠标命中测试：使用视觉树中的 `HitTest`。

## 5. 链式 API 示例
```cpp
auto window = fk::window()
    ->Title("Dashboard")
    ->Content(StackPanel()
        ->Orientation(Orientation::Vertical)
        ->Children({
            TextBlock()->Text(Bind("Header")),
            TextBox()->Text(Bind("SearchText").Mode(BindingMode::TwoWay)),
            Button()->Content("Search")->Command(BindCommand(&ViewModel::Search))
        }));
```

## 6. 与其他模块的依赖
- 依赖 `fk::core`：Dispatcher, Event, Timer。
- 依赖 `fk::binding`：DependencyProperty, Binding。
- 与 `fk::render` 协作：`Visual` 生成 `RenderNode`，由渲染器消费。
- 与 `fk::app`：`Window` 作为顶层控件与 Application 集成。

## 7. 测试计划
- 链式 API：验证属性设置、返回类型正确性。
- 布局：Measure/Arrange 结果正确，控件嵌套场景。
- 输入：焦点切换、路由事件传播。
- 模板：`OnApplyTemplate` 执行正确、命名部件存在。
- DataContext：继承、Binding 激活与更新。

## 8. 实现路线
1. 建立基础类型：`DispatcherObject`, `DependencyObject`, `UIElement`, `FrameworkElement`。
2. 实现布局系统与面板基类。
3. 加入路由事件与命令机制。
4. 完整化控件库与模板系统。
5. 与 `fk::binding` 协调数据上下文与依赖属性注册。

---
> 本设计旨在为后续实现提供结构化指导，细化类图与接口定义时需与 binding/render 模块保持一致，确保链式 API、绑定与渲染协同工作。