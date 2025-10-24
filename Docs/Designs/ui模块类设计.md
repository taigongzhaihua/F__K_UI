# fk::ui 模块类设计

## 1. 总览

本文档基于《ui模块设计.md》的整体规划，对核心类与关键接口进行更细化的设计说明，涵盖对象职责、继承关系、生命周期与与 binding 模块的协作方式。目标是为后续实现提供清晰的类图级别参考。

## 2. 基础设施

### 2.1 DispatcherObject
- 继承自 `fk::core::DispatcherObject`（若后续抽象该类），记录所属的 `core::Dispatcher*`。
- 对外提供 `core::Dispatcher* Dispatcher() const` 与 `void VerifyAccess() const`。
- `DependencyObject` 与所有 UI 元素的根基。

### 2.2 DependencyObject (ui)
- 继承自 `fk::binding::DependencyObject`，在 UI 层添加：
  - 逻辑树接口：`GetLogicalParent()`, `LogicalChildren()` 等（利用 binding 层已有逻辑树支持）。
  - 生命周期 hook：`OnAttachedToLogicalTree`, `OnDetachedFromLogicalTree`。
- 负责：
  - 依赖属性访问 `GetValue/SetValue`。
  - DataContext 继承（binding 已实现，UI 侧仅在逻辑树变动时调用）。

## 3. UI 元素体系

### 3.1 UIElement
- 继承 `DependencyObject`，引入布局与输入基础：
  - 布局接口：`Measure(Size available)`, `Arrange(Rect finalRect)`；内部调用 `MeasureCore/ArrangeCore`。
  - 输入事件：`core::Event<KeyEventArgs> KeyDown` 等。
  - 状态属性（作为依赖属性）：`Visibility`, `IsEnabled`, `Opacity`, `Clip`, `RenderTransform`。
- 关键虚函数：
  - `Size MeasureCore(Size available)`：派生类可重写。
  - `Size ArrangeCore(Rect finalRect)`：派生类可重写。
  - `void OnKeyDown(const KeyEventArgs&)` 等输入回调。
- 逻辑树事件：默认实现 `OnAttachedToLogicalTree`, `OnDetachedFromLogicalTree`，在其中连接或断开 DataContext 继承。

### 3.2 FrameworkElement
- 继承 `UIElement`，增加：
  - 布局属性：`Width`, `Height`, `MinWidth`, `Margin`, `HorizontalAlignment`, `VerticalAlignment`。
  - DataContext 暴露：`SetDataContext`, `DataContext()`（调用 binding 层接口）。
  - 资源访问：`ResourceDictionary Resources`（后续扩展）。
  - 生命周期事件：`Initialized`, `Loaded`, `Unloaded`, `OnApplyTemplate()`。
- 逻辑树管理：
  - 提供 `AddLogicalChild(ViewElement*)`, `RemoveLogicalChild(ViewElement*)` 等封装，在内部调用基类的逻辑树方法。
  - `OnAttachedToLogicalTree` 时：标记 `isLoaded_`，激活绑定。

### 3.3 View<TDerived>
- CRTP 帮助类，继承 `FrameworkElement`：
  - 提供链式 API（`Derived* Width(float)` 等），通过 `static_cast<Derived*>(this)` 返回。
  - 每个链式方法底层调用 `SetValue(DependencyProperty, ...)`。
  - `Ptr` 类型别名定义为 `std::shared_ptr<Derived>`。
  - 提供 `static Ptr Create(...)` 工厂方法。

### 3.4 Control<TDerived>
- 继承 `View<TDerived>`，针对控件场景扩展：
  - 常用属性：`IsEnabled`, `IsFocused`, `TabIndex`, `Cursor`（依赖属性形式）。
  - 内容模型：`SetContent(ViewElementPtr)`。
  - 输入命令相关 hook：`Command`, `CommandParameter`（后续实现命令系统时使用）。
  - 默认输入处理：`virtual void OnClick()`, `OnKeyDown`, `OnMouseDown` 等。

## 4. 容器与特殊元素

### 4.1 Panel
- 继承 `FrameworkElement`，维护子元素集合：`std::vector<UIElement*> children_`。
- 提供：
  - `void AddChild(UIElement* child)`：
    - 更新逻辑树（`AddLogicalChild`），设置视觉树父子关系（后续）。
    - 触发 `InvalidateMeasure()`。
  - `void RemoveChild(UIElement* child)`：移除并触发布局更新。
- 纯虚布局接口：
  - `Size MeasureOverride(const Size& available)`。
  - `Size ArrangeOverride(const Rect& finalRect)`。
- 典型派生：`StackPanel`, `Grid`, `DockPanel`, `Canvas`。

### 4.2 ContentControl
- 继承 `Control<TDerived>`，管理单一内容：
  - 依赖属性 `Content`。
  - 在 `OnContentChanged(old, new)` 中更新逻辑树关系。
  - 布局时将内容作为唯一子元素执行测量/排列。

### 4.3 ItemsControl
- 继承 `Control<TDerived>`：
  - 属性：`ItemsSource`, `ItemsPanel`, `ItemTemplate`。
  - 内部维护 `std::vector<UIElement*> realizedItems_`。
  - 绑定触发时重新生成子项，使用 `BindingExpression` 监听数据集合变动（后期与 ObservableCollection 对接）。

### 4.4 Decorator
- 继承 `FrameworkElement`，包装一个子元素。
  - 负责额外绘制（如边框、背景）。
  - 布局时将子元素视为唯一内容。

## 5. 命名与模板部件
- 所有 `DependencyObject` 支持 `SetElementName` 与 `FindElementByName`（已在 binding 层实现）。
- `FrameworkElement` 扩展：
  - 提供 `RegisterName(std::string name, DependencyObject* element)` 与 `FindName`，内部委托到逻辑树查找。
  - 模板应用 (`ApplyTemplate`) 时注册命名部件。

## 6. 依赖属性注册
- 每个 UI 类需静态注册自身依赖属性。例如：
  ```cpp
  class UIElement : public DependencyObject {
  public:
      static const DependencyProperty& VisibilityProperty();
  };
  ```
- `DependencyProperty` 的 `BindingOptions` 决定默认绑定模式与数据上下文继承：
  - 例如 `FrameworkElement::DataContextProperty` 设置 `bindingOptions.inheritsDataContext = true`。
- 使用已提供的宏（`FK_BINDING_REGISTER_PROPERTY` 等）注册数据模型 getter/setter。

## 7. 绑定激活流程
1. 元素加入逻辑树：
   - `SetLogicalParent()` -> 更新 `binding::BindingContext` 的父级，DataContext 自动继承。
   - 调用 `OnAttachedToLogicalTree`，并触发 `Loaded` 事件。
2. 依赖属性绑定：`SetBinding` -> `BindingExpression::Activate`。
3. RelativeSource / ElementName 解析依赖逻辑树接口，现已在 binding 层实现。
4. 元素离开逻辑树时：`OnDetachedFromLogicalTree` -> 解除数据上下文、解绑事件。

## 8. 输入与路由事件
- `UIElement` 定义事件：`RoutedEvent KeyDownEvent`, `MouseDownEvent` 等。
- `RaiseEvent(RoutedEventArgs&)` 顺着逻辑树 `Bubble/Tunnel` 传播。
- 控件通过重写 `On*` 系列方法处理具体输入。

## 9. 布局协作
- `LayoutManager`（后续实现）维护待布局元素队列。
  - `UIElement::InvalidateMeasure()`/`InvalidateArrange()` 将元素加入队列。
  - 统一在 UI 线程调度测量与排列。
- `Panel` 与 `ContentControl` 在面向子元素的测量/安排时应调用子元素的 `Measure`、`Arrange`。

## 10. 扩展点
- `ResourceDictionary`、`Style`、`Template` 等将在后续文档中进一步展开。
- 需与 binding 模块协同的部分：
  - `BindingExpression::ValidationErrorsChanged` 可映射到 UI 层的视觉状态（如显示错误提示）。
  - UpdateSourceTrigger 与焦点事件（`LostFocus`）对接。

## 11. 下一步
- 基于此类设计，编写对应的头文件与实现文件骨架。
- 制定依赖属性注册宏的使用示例，确保链式 API 与绑定能力兼容。
- 规划测试用例：逻辑树变更、DataContext 继承、RelativeSource/ElementName 绑定、基本布局行为。
