# UI 模块重构架构说明

## 1. 总体目标

- 构建一套分层清晰的 UI 内核，使依赖属性、渲染、输入、布局等能力在不同抽象层逐步叠加。
- 保持 `binding` 模块的现有能力，并与新的 UI 层级无缝对接。
- 为后续 ControlTemplate、资源系统、主题体系等扩展留出清晰的挂接点。
- 渲染采用命令缓冲模式，与平台无关，可替换底层实现。

## 2. 核心层级关系

```
DependencyObject
  |
  -> Visual
     |
     -> UIElement
        |
        -> FrameworkElement
           |-- Control
           |     |-- ContentControl
           |     `-- ItemsControl
           `-- Panel
```

### 2.1 DependencyObject

- 依赖属性系统的根基，负责属性元数据、默认值、变更回调。
- 提供 `SetValue`/`GetValue`、属性变更通知、绑定目标/源的注册点。
- 暴露 `BindingContext` 挂接点，使绑定表达式通过统一接口访问。
- 提供 `PropertyChanged` 事件供上层监听。

### 2.2 Visual

- 专注渲染相关能力：变换、坐标系、命中测试、剪裁。
- 持有 `VisualTree` 关系：父指针、子集合（只读或受保护的管理接口）。
- 提供统一的子节点集合管理器（例如内部 `VisualCollection` 或外部 `VisualTreeManager`），屏蔽不同派生类对视觉树操作的差异。
- 定义渲染数据生成流程：`BuildRenderTree(RenderContext&)` 或 `CollectDrawCommands(Renderer&)`。
- 与渲染器 `Renderer` 解耦：Visual 只输出 `DrawCommand`，Renderer 决定如何执行。

### 2.3 UIElement

- 引入输入与布局的核心能力：命中测试、输入事件冒泡/隧道、`Measure/Arrange` 模型。
- 管理通用的可视属性：`Visibility`、`IsEnabled`、`Opacity`、`Clip` 等。
- 负责将布局结果传递给 Visual 层，维护 `DesiredSize`、`RenderSize`。
- 定义通用事件：`OnPointerPressed`、`OnKeyDown` 等，并提供路由事件机制。

### 2.4 FrameworkElement

- 提供框架级功能：`DataContext`、`Resources`、`Styles`、`Template` 引用点。
- 集成 `binding` 模块：`DataContext` 变更触发子树继承并通知已绑定表达式重求值。
- 支持生命周期钩子：`OnInitialized`、`OnLoaded`、`OnUnloaded`。
- 管理尺寸相关属性：`Width`、`Height`、`Min/Max`，并与 `Measure/Arrange` 协调。

### 2.5 Control

- 面向控件开发者的最低层：
  - 提供 `ControlTemplate` 机制，用于生成视觉树。
  - 管理交互状态（Focus、Hover、Pressed），统一映射到视觉状态机。
- 支持主题样式查找顺序：显式样式 → 资源 → 默认主题。

### 2.6 ContentControl

- 从 `Control` 继承，增加 `Content`、`ContentTemplate`。
- 接受任意对象或 UIElement 作为内容，若为数据对象则通过模板生成为视觉树。
- 常用控件（`Button`、`Window` 等）基于此实现。

### 2.7 ItemsControl

- 控件的集合化版本：管理 `ItemsSource`、`ItemsPanel`、`ItemTemplate`。
- 定义 `ItemContainerGenerator`，负责数据项 → UI 容器的生命周期。
- 事件和选择状态由 `Selector` 等派生类扩展。

### 2.8 Panel

- 直接继承自 `FrameworkElement`（保持与 WPF/WinUI 一致），避免额外的内容包装开销。
- 提供子元素集合管理，并覆写 `MeasureOverride`、`ArrangeOverride` 实现不同布局策略。
- 与 `ItemsControl` 配合，通过 `ItemsPanel` 指定布局容器类型。
- 不引入 `ContentControl` 层级，布局容器专注于多子元素排列，逻辑树（父子关系）与视觉树保持一一对应。

### 2.9 常用控件继承关系示例

- **轻量元素（直接继承 `FrameworkElement`）**
  - `TextBlock → FrameworkElement`
  - `Image → FrameworkElement`
  - `Border → Decorator → FrameworkElement`
  - `Rectangle`/`Ellipse`/`Path` 等 → `Shape → FrameworkElement`
- **布局容器**
  - `Grid`/`StackPanel`/`DockPanel` → `Panel → FrameworkElement`
- **内容控件链路**
  - `Button → ButtonBase → ContentControl → Control`
  - `Window → ContentControl → Control`
- **集合控件链路**
  - `ListBox → Selector → ItemsControl → Control`
  - `ListView → ListViewBase → ItemsControl → Control`

> 以上沿用 WPF/WinUI 的分类方法，帮助在迁移或实现现有控件时快速定位基类及应复用的框架层能力。

## 3. 渲染管线设计

### 3.1 DrawCommand 模型

```cpp
struct DrawCommand {
    enum class Type { Rectangle, Text, Image, Custom };
    Type type;
    Vec2 position;
    Vec2 size;
    Color color;
    std::string text;
    std::shared_ptr<Texture> texture; // 可选
    std::function<void(RenderBackend&)> customCallback; // 扩展钩子
};
```

- Visual 在 `CollectDrawCommands` 中填充命令缓冲。
- 增设 `Custom` 类型允许高级控件执行特定绘制逻辑。

### 3.2 Renderer 职责

- 提供帧级命令缓冲：`Submit(const DrawCommand&)`、`RenderFrame()`。
- 在 `RenderFrame` 中按层深排序（必要时引入 Z-Index）并调用具体后端实现。
- 后端抽象 `RenderBackend` 封装平台（OpenGL/Direct2D/软件渲染）。
- 支持批量合并、纹理管理等优化。

### 3.3 Visual -> Renderer 流程

1. UI 框架驱动根 Visual 执行 `Layout` → `CollectDrawCommands`。
2. 每个 Visual 根据状态生成命令并提交给 `Renderer`。
3. Renderer 将命令提交至后端完成绘制，帧结束清空缓冲。

### 3.4 控件到渲染的责任链

| 阶段 | 主要类型 | 职责 | 可选辅助组件 |
| --- | --- | --- | --- |
| 控件逻辑 | `Control` / `ContentControl` / `ItemsControl` | 管理属性、状态、命令等交互逻辑；在 `OnApplyTemplate` 中触发模板应用 | `VisualStateManager`（状态机），`InteractionTracker` |
| 模板实例化 | `ControlTemplate` + `TemplateBuilder` | 依据样式生成实际的元素树，解析命名部件并绑定到控件 | `TemplatePartResolver`（定位模板部件），`TemplateCache` |
| 视觉树挂接 | `Visual` + `VisualCollection`/`VisualTreeManager` | 维护父子关系，确保模板生成的元素正确插入视觉树；处理变换与剪裁 | `VisualInvalidationScope`（批量无效化） |
| 绘制命令生成 | `Visual::CollectDrawCommands` | 根据当前状态生成 `DrawCommand` 序列 | `RenderTreeBuilder`（将复杂控件拆解为命令序列），`GeometryCache` |
| 顶点/纹理准备 | `Renderer` + `RenderTreeCompiler` | 将命令映射到后端资源，构建顶点/索引缓冲或文本布局 | `MaterialPool`、`DynamicMeshBuffer` |
| 渲染执行 | `RenderBackend`（OpenGL/Direct2D/软件） | 设置管线状态、提交批次，完成屏幕输出 | `RenderStatsCollector`、`FrameGraph` |

- 表中“可选辅助组件”并非强制要求，可根据项目规模逐步引入；`TemplateBuilder`、`RenderTreeBuilder` 等帮助隔离复杂流程，便于单元测试与替换实现。

## 4. 数据绑定与依赖属性集成

- `DependencyObject` 持有 `BindingStore`，为每个依赖属性保存绑定表达式。
- `FrameworkElement::DataContext` 改写时触发子元素继承，`BindingExpression` 重新解析路径。
- 依赖属性变更通过 `PropertyChanged` 事件向上冒泡，必要时触发布局/渲染无效化：
  - 外观属性变更 → `InvalidateVisual()`。
  - 布局属性变更 → `InvalidateMeasure()` 或 `InvalidateArrange()`。
- 支持双向绑定：`BindingExpression` 监听目标属性变更，通过 `UpdateSourceTrigger` 控制回写时机。

## 5. 输入与事件系统

- `UIElement` 定义路由事件体系：
  - 隧道（Preview）→ 冒泡（Normal）。
  - `RaiseEvent` 根据视觉树关系向父层传播。
- 输入管理：
  - `InputManager` 收集平台事件，命中测试确定目标 `UIElement`。
  - 焦点管理、键盘导航由 `FocusManager` 负责。
- 命中测试依赖 Visual 的几何信息，可扩展自定义命中逻辑。

## 6. 布局系统

- 采用两阶段流程：
  - `Measure(Size available)`：子元素汇报期望尺寸。
  - `Arrange(Rect finalRect)`：确定最终渲染区域。
- `UIElement` 提供公共实现，`Panel` 和 `FrameworkElement` 通过虚函数自定义。
- 支持布局无效化：尺寸相关依赖属性变更时重启流程。

## 7. 样式与模板

- 样式系统依托依赖属性：
  - `Style` 绑定属性 setter、触发器。
  - `Style` 可层层基于键值（控件类型、键）查询。
- `ControlTemplate` 描述控件视觉树：
  - 应用时实例化 Template 并链接到 `Control` 的命名部件。
  - 模板中的 `Binding` 默认使用父控件的 `DataContext`。

## 8. 扩展点与实现建议

- 新增控件通常派生自 `Control` 或 `ContentControl`，覆写 `OnApplyTemplate` 获取模板部件。
- 自定义布局容器继承 `Panel`，实现 `MeasureOverride` 与 `ArrangeOverride`。
- `Visual` 提供 `AddChild`、`RemoveChild` 受保护接口，保证树结构的一致性。
- Renderer 后端可通过策略模式注入，支持运行时切换或多后端共存。

## 9. 兼容性策略

- 保留旧有 API，通过适配层重定向至新架构（过渡期）。
- 依赖属性与绑定接口保持向后兼容，旧控件只需继承新基类并调整初始化流程。
- 渲染命令与旧渲染管线可并行运行，逐步迁移每个 Visual。

## 10. 需要实现的核心类型

- **依赖属性与绑定基础**：`DependencyObject`、`DependencyProperty`、`BindingExpression` 适配层。
- **视觉层**：`Visual`、`VisualCollection`（或同等管理器）、`HitTestResult`。
- **输入与布局层**：`UIElement`、`InputManager`、`FocusManager`、`MeasureContext`/`ArrangeContext`。
- **框架层**：`FrameworkElement`、`ResourceDictionary`、`Style`、`StyleSelector`、`FrameworkTemplate`、`DataTemplate`。
- **控件层**：`Control`、`ContentControl`、`ItemsControl`、`ItemContainerGenerator`、`ControlTemplate`、`TemplateBuilder`。
- **布局容器**：`Panel` 及首批具体实现（`StackPanel`、`Grid` 等）。
- **轻量元素**：`TextBlock`、`Border`、`Image`、`Shape` 基类。
- **渲染管线**：`DrawCommand`、`Renderer`、`RenderBackend`、`RenderTreeBuilder`、`MaterialPool`。
- **辅助设施**：`VisualStateManager`、`InteractionTracker`、`VisualTreeManager`、`GeometryCache`、`TemplatePartResolver`。

## 11. 接口设计要点

### 11.1 DependencyObject

**职责**：依赖属性系统核心，管理属性值存储、变更通知、绑定挂接。

**继承**：无（根基类）

- `void SetValue(DependencyProperty* dp, const std::any& value)`：设置依赖属性值并触发变更通知、绑定回写。
- `std::any GetValue(DependencyProperty* dp) const`：读取当前有效值（本地值 → 样式 → 默认值）。
- `void ClearValue(DependencyProperty* dp)`：移除本地值，回退到下一层来源。
- `void SetBinding(DependencyProperty* dp, std::shared_ptr<BindingExpression> binding)`：附加绑定表达式，注册更新回调。
- `BindingExpression* GetBinding(DependencyProperty* dp) const`：查询现有绑定，用于断开或调试。
- `Event<PropertyChangedEventArgs>& PropertyChanged()`：订阅属性变更事件，供上层无效化逻辑使用。

### 11.2 DependencyProperty

**职责**：依赖属性元数据，描述类型、默认值、变更回调。

**继承**：无（独立元数据类）

- `static DependencyProperty* Register(const std::string& name, const std::type_info& propertyType, const std::type_info& ownerType, PropertyMetadata* metadata)`：注册新依赖属性。
- `const std::string& GetName() const`：获取属性名称。
- `const std::type_info& GetPropertyType() const`：获取属性值类型。
- `PropertyMetadata* GetMetadata(const std::type_info& forType) const`：查询类型特定的元数据。
- `std::any GetDefaultValue(const std::type_info& forType) const`：获取默认值。

### 11.3 BindingExpression

**职责**：表示单个绑定实例，管理源与目标的同步。

**继承**：无（独立绑定类）

- `void UpdateTarget()`：从源读取并更新目标属性。
- `void UpdateSource()`：从目标回写到源（双向绑定）。
- `void Attach(DependencyObject* target, DependencyProperty* targetProperty)`：附加到目标对象。
- `void Detach()`：解除绑定。
- `BindingStatus GetStatus() const`：获取绑定状态（Active/Detached/Error）。
- `const std::any& GetSourceValue() const`：获取当前源值。

### 11.4 Visual

**职责**：视觉树节点基础，管理父子关系、变换、命中测试、绘制命令生成。

**继承**：`DependencyObject`

- `void AddVisualChild(Visual* child)`：添加子视觉节点并更新父指针。
- `void RemoveVisualChild(Visual* child)`：移除子节点。
- `Visual* GetVisualParent() const`：获取父视觉。
- `size_t GetVisualChildrenCount() const`：获取子节点数量。
- `Visual* GetVisualChild(size_t index) const`：按索引访问子节点。
- `void SetTransform(const Matrix3x2& transform)`：设置局部变换矩阵。
- `const Matrix3x2& GetTransform() const`：获取当前变换。
- `Matrix3x2 GetAbsoluteTransform() const`：计算从根节点到当前节点的累积变换。
- `bool HitTest(const Point& point, HitTestResult& result) const`：命中测试（默认基于边界框）。
- `virtual void CollectDrawCommands(RenderContext& context)`：生成绘制命令并递归子节点（可覆写）。
- `Rect GetBounds() const`：获取局部坐标系的边界。

### 11.5 VisualCollection

**职责**：封装 Visual 的子节点集合，提供类型安全的操作接口。

**继承**：无（集合辅助类）

- `void Add(Visual* child)`：添加子节点到末尾。
- `void Insert(size_t index, Visual* child)`：在指定位置插入子节点。
- `void Remove(Visual* child)`：移除指定子节点。
- `void RemoveAt(size_t index)`：按索引移除子节点。
- `void Clear()`：清空所有子节点。
- `size_t Count() const`：获取子节点数量。
- `Visual* operator[](size_t index) const`：索引访问。
- `iterator begin()/end()`：支持范围遍历。

### 11.6 UIElement

**职责**：引入输入事件、布局系统、路由事件机制。

**继承**：`Visual`

- `void Measure(const Size& availableSize)`：测量期望尺寸。
- `void Arrange(const Rect& finalRect)`：确定最终布局位置与大小。
- `void InvalidateMeasure()`：标记需要重新测量。
- `void InvalidateArrange()`：标记需要重新排列。
- `Size GetDesiredSize() const`：获取测量阶段计算的期望尺寸。
- `Size GetRenderSize() const`：获取排列后的实际渲染尺寸。
- `void SetVisibility(Visibility value)`：设置可见性（Visible/Hidden/Collapsed）。
- `Visibility GetVisibility() const`：获取可见性。
- `void SetIsEnabled(bool value)`：设置是否启用。
- `bool GetIsEnabled() const`：获取启用状态。
- `void RaiseEvent(const RoutedEventArgs& args)`：触发路由事件。
- `void AddHandler(RoutedEvent* routedEvent, EventHandler handler)`：注册事件处理器。
- `void RemoveHandler(RoutedEvent* routedEvent, EventHandler handler)`：移除事件处理器。
- `virtual void OnPointerPressed(PointerEventArgs& e)`：指针按下事件虚函数。
- `virtual void OnPointerReleased(PointerEventArgs& e)`：指针释放事件虚函数。
- `virtual void OnPointerMoved(PointerEventArgs& e)`：指针移动事件虚函数。
- `virtual void OnKeyDown(KeyEventArgs& e)`：键盘按下事件虚函数。
- `virtual void OnKeyUp(KeyEventArgs& e)`：键盘释放事件虚函数。
- `virtual Size MeasureCore(const Size& availableSize)`：核心测量逻辑（派生类覆写）。
- `virtual void ArrangeCore(const Rect& finalRect)`：核心排列逻辑（派生类覆写）。

### 11.7 InputManager

**职责**：管理输入事件分发、命中测试、指针捕获。

**继承**：无（独立管理器）

- `void ProcessPointerEvent(const PlatformPointerEvent& event)`：处理平台指针事件并分发到目标元素。
- `void ProcessKeyboardEvent(const PlatformKeyEvent& event)`：处理键盘事件并分发到焦点元素。
- `UIElement* HitTest(const Point& screenPoint, Visual* root)`：执行命中测试并返回目标元素。
- `void CapturePointer(UIElement* element, int pointerId)`：捕获指针到指定元素。
- `void ReleasePointerCapture(int pointerId)`：释放指针捕获。
- `UIElement* GetPointerCapture(int pointerId) const`：获取当前捕获指针的元素。
- `void SetRoot(Visual* root)`：设置根视觉节点。

### 11.8 FocusManager

**职责**：管理焦点导航、焦点状态。

**继承**：无（独立管理器）

- `void SetFocusedElement(UIElement* element)`：设置焦点到指定元素。
- `UIElement* GetFocusedElement() const`：获取当前焦点元素。
- `bool MoveFocus(FocusNavigationDirection direction)`：按方向移动焦点（Tab/Arrow导航）。
- `Event<FocusChangedEventArgs>& FocusChanged()`：焦点变更事件。

### 11.9 FrameworkElement

**职责**：提供数据上下文、样式、资源、生命周期事件等框架级功能。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`UIElement`

- `void SetDataContext(const std::any& value)`：设置数据上下文。
- `const std::any& GetDataContext() const`：获取数据上下文。
- `Derived* DataContext(const std::any& value)`：链式调用设置数据上下文。
- `const std::any& DataContext() const`：无参重载，获取数据上下文。
- `void SetWidth(float value)` / `float GetWidth() const`：设置/获取宽度。
- `Derived* Width(float value)`：链式调用设置宽度。
- `float Width() const`：无参重载，获取宽度。
- `void SetHeight(float value)` / `float GetHeight() const`：设置/获取高度。
- `Derived* Height(float value)`：链式调用设置高度。
- `float Height() const`：无参重载，获取高度。
- `void SetMinWidth/MaxWidth/MinHeight/MaxHeight(float value)`：尺寸约束接口。
- `Derived* MinWidth/MaxWidth/MinHeight/MaxHeight(float value)`：链式调用版本。
- `float MinWidth/MaxWidth/MinHeight/MaxHeight() const`：无参重载版本。
- `ResourceDictionary& GetResources()`：访问资源字典。
- `Style* GetStyle() const`：获取当前样式。
- `void SetStyle(Style* style)`：设置样式。
- `Derived* Style(Style* style)`：链式调用设置样式。
- `Style* Style() const`：无参重载，获取样式。
- `void ApplyTemplate()`：应用模板（调用 `OnApplyTemplate`）。
- `virtual void OnApplyTemplate()`：模板应用钩子（派生类覆写）。
- `Event<EventArgs>& Loaded()`：加载完成事件。
- `Event<EventArgs>& Unloaded()`：卸载事件。
- `virtual Size MeasureOverride(const Size& availableSize)`：自定义测量逻辑。
- `virtual Size ArrangeOverride(const Size& finalSize)`：自定义排列逻辑。

### 11.10 ResourceDictionary

**职责**：存储和查找资源（样式、模板、数据等）。

**继承**：无（资源容器类）

- `void Add(const std::string& key, const std::any& value)`：添加资源。
- `bool TryGetValue(const std::string& key, std::any& outValue) const`：查找资源。
- `void Remove(const std::string& key)`：移除资源。
- `bool Contains(const std::string& key) const`：检查资源是否存在。
- `std::vector<ResourceDictionary*>& GetMergedDictionaries()`：获取合并的资源字典列表。
- `void Clear()`：清空所有资源。

### 11.11 Style

**职责**：描述一组属性设置器和触发器。

**继承**：无（样式描述类）

- `void AddSetter(DependencyProperty* property, const std::any& value)`：添加属性设置器。
- `void Apply(FrameworkElement* element)`：将样式应用到元素。
- `void Unapply(FrameworkElement* element)`：撤销样式。
- `Style* GetBasedOn() const`：获取基础样式。
- `void SetBasedOn(Style* baseStyle)`：设置基础样式继承。
- `const std::type_info& GetTargetType() const`：获取目标类型。

### 11.12 FrameworkTemplate

**职责**：模板基类，提供模板实例化和资源查找的通用功能。

- `virtual UIElement* Instantiate() = 0`：实例化模板并返回根元素（纯虚函数）。
- `void Seal()`：密封模板，禁止后续修改。
- `bool IsSealed() const`：检查模板是否已密封。
- `ResourceDictionary& GetResources()`：访问模板内的资源字典。

### 11.13 DataTemplate

**职责**：描述数据对象的视觉表示模板。

**继承**：`FrameworkTemplate`

- `UIElement* Instantiate(const std::any& dataContext)`：根据数据上下文实例化模板。
- `void SetFactory(std::function<UIElement*(const std::any&)> factory)`：设置模板工厂函数。

### 11.13 Control

**职责**：提供控件模板、视觉状态管理、默认样式查找。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`FrameworkElement<Derived>`

- `void SetTemplate(ControlTemplate* tmpl)`：设置控件模板。
- `ControlTemplate* GetTemplate() const`：获取当前模板。
- `Derived* Template(ControlTemplate* tmpl)`：链式调用设置模板。
- `ControlTemplate* Template() const`：无参重载，获取模板。
- `virtual void OnApplyTemplate()`：模板应用钩子，用于获取模板部件。
- `virtual std::string GetDefaultStyleKey() const`：返回默认样式查找键。
- `void SetForeground(Brush* brush)`：设置前景画刷。
- `Brush* GetForeground() const`：获取前景画刷。
- `Derived* Foreground(Brush* brush)`：链式调用设置前景。
- `Brush* Foreground() const`：无参重载，获取前景。
- `void SetBackground(Brush* brush)`：设置背景画刷。
- `Brush* GetBackground() const`：获取背景画刷。
- `Derived* Background(Brush* brush)`：链式调用设置背景。
- `Brush* Background() const`：无参重载，获取背景。
- `void SetBorderBrush(Brush* brush)`：设置边框画刷。
- `Brush* GetBorderBrush() const`：获取边框画刷。
- `Derived* BorderBrush(Brush* brush)`：链式调用设置边框画刷。
- `Brush* BorderBrush() const`：无参重载，获取边框画刷。
- `void SetBorderThickness(const Thickness& thickness)`：设置边框厚度。
- `const Thickness& GetBorderThickness() const`：获取边框厚度。
- `Derived* BorderThickness(const Thickness& thickness)`：链式调用设置边框厚度。
- `Derived* BorderThickness(float uniform)`：便捷重载，统一厚度。
- `Derived* BorderThickness(float horizontal, float vertical)`：便捷重载。
- `Derived* BorderThickness(float left, float top, float right, float bottom)`：便捷重载。
- `const Thickness& BorderThickness() const`：无参重载，获取边框厚度。
- `void SetPadding(const Thickness& padding)`：设置内边距。
- `const Thickness& GetPadding() const`：获取内边距。
- `Derived* Padding(const Thickness& padding)`：链式调用设置内边距。
- `Derived* Padding(float uniform)`：便捷重载，统一内边距。
- `Derived* Padding(float horizontal, float vertical)`：便捷重载。
- `Derived* Padding(float left, float top, float right, float bottom)`：便捷重载。
- `const Thickness& Padding() const`：无参重载，获取内边距。
- `bool GetIsFocused() const`：获取焦点状态。
- `bool GetIsMouseOver() const`：获取鼠标悬停状态。

### 11.14 ControlTemplate

**职责**：描述控件的视觉树结构。

**继承**：`FrameworkTemplate`

- `UIElement* Instantiate(Control* templatedParent)`：实例化模板并返回根元素。
- `void SetFactory(std::function<UIElement*(Control*)> factory)`：设置模板工厂。
- `const std::type_info& GetTargetType() const`：获取目标控件类型。
- `void SetTargetType(const std::type_info& type)`：设置目标控件类型。

### 11.15 ContentControl

**职责**：承载单个内容对象或 UI 元素。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`Control<Derived>`

- `void SetContent(const std::any& content)`：设置内容（可以是数据或 UIElement）。
- `const std::any& GetContent() const`：获取内容。
- `Derived* Content(const std::any& content)`：链式调用设置内容并返回 this。
- `const std::any& Content() const`：无参重载，获取内容。
- `void SetContentTemplate(DataTemplate* tmpl)`：设置内容模板。
- `DataTemplate* GetContentTemplate() const`：获取内容模板。
- `Derived* ContentTemplate(DataTemplate* tmpl)`：链式调用设置内容模板。
- `DataTemplate* ContentTemplate() const`：无参重载，获取内容模板。
- `virtual void OnContentChanged(const std::any& oldContent, const std::any& newContent)`：内容变更钩子。

### 11.16 ItemsControl

**职责**：展示集合数据，管理项模板和容器生成。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`Control<Derived>`

- `void SetItemsSource(std::shared_ptr<IEnumerable> source)`：绑定数据源。
- `std::shared_ptr<IEnumerable> GetItemsSource() const`：获取数据源。
- `Derived* ItemsSource(std::shared_ptr<IEnumerable> source)`：链式调用设置数据源。
- `std::shared_ptr<IEnumerable> ItemsSource() const`：无参重载，获取数据源。
- `ObservableCollection<std::any>& GetItems()`：直接访问项集合（无数据源时使用）。
- `void SetItemTemplate(DataTemplate* tmpl)`：设置项模板。
- `DataTemplate* GetItemTemplate() const`：获取项模板。
- `Derived* ItemTemplate(DataTemplate* tmpl)`：链式调用设置项模板。
- `DataTemplate* ItemTemplate() const`：无参重载，获取项模板。
- `void SetItemsPanel(ItemsPanelTemplate* panel)`：设置项容器面板模板。
- `ItemsPanelTemplate* GetItemsPanel() const`：获取项容器面板模板。
- `Derived* ItemsPanel(ItemsPanelTemplate* panel)`：链式调用设置项容器面板。
- `ItemsPanelTemplate* ItemsPanel() const`：无参重载，获取项容器面板。
- `ItemContainerGenerator* GetItemContainerGenerator() const`：获取容器生成器。
- `virtual bool IsItemItsOwnContainer(const std::any& item) const`：判断项是否已经是容器。
- `virtual UIElement* GetContainerForItem(const std::any& item)`：为数据项创建容器。
- `virtual void PrepareContainerForItem(UIElement* container, const std::any& item)`：准备容器（设置数据上下文等）。

### 11.17 ItemContainerGenerator

**职责**：管理数据项到 UI 容器的映射和生命周期。

**继承**：无（容器生成器辅助类）

- `UIElement* Generate(const std::any& item, int index)`：为数据项生成 UI 容器。
- `void Recycle(UIElement* container)`：回收容器到对象池。
- `UIElement* ContainerFromItem(const std::any& item) const`：根据数据项查找容器。
- `std::any ItemFromContainer(UIElement* container) const`：根据容器查找数据项。
- `int IndexFromContainer(UIElement* container) const`：根据容器获取索引。

### 11.18 Panel

**职责**：布局容器基类，管理子元素集合和布局策略。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`FrameworkElement<Derived>`

- `UIElementCollection& GetChildren()`：访问子元素集合。
- `Derived* AddChild(UIElement* child)`：添加子元素并返回 this（链式调用）。
- `Derived* Children(std::initializer_list<UIElement*> children)`：批量添加子元素（链式调用）。
- `virtual Size MeasureOverride(const Size& availableSize)`：测量子元素并返回期望尺寸。
- `virtual Size ArrangeOverride(const Size& finalSize)`：排列子元素并返回实际使用的尺寸。
- `void SetBackground(Brush* brush)`：设置背景画刷。
- `Brush* GetBackground() const`：获取背景画刷。
- `Derived* Background(Brush* brush)`：链式调用设置背景。
- `Brush* Background() const`：无参重载，获取背景。

### 11.19 UIElementCollection

**职责**：管理 Panel 的子元素集合。

**继承**：无（集合辅助类）

- `void Add(UIElement* element)`：添加子元素。
- `void Insert(size_t index, UIElement* element)`：插入子元素。
- `void Remove(UIElement* element)`：移除子元素。
- `void RemoveAt(size_t index)`：按索引移除。
- `void Clear()`：清空集合。
- `size_t Count() const`：获取子元素数量。
- `UIElement* operator[](size_t index) const`：索引访问。
- `iterator begin()/end()`：范围遍历支持。

### 11.20 StackPanel

**职责**：按方向线性排列子元素。

**继承**：`Panel<StackPanel>`

- `void SetOrientation(Orientation value)`：设置排列方向（Horizontal/Vertical）。
- `Orientation GetOrientation() const`：获取排列方向。
- `StackPanel* Orientation(Orientation value)`：链式调用设置方向。
- `Orientation Orientation() const`：无参重载，获取方向。
- `void SetSpacing(float value)`：设置子元素间距。
- `float GetSpacing() const`：获取间距。
- `StackPanel* Spacing(float value)`：链式调用设置间距。
- `float Spacing() const`：无参重载，获取间距。

### 11.21 Grid

**职责**：按行列网格排列子元素。

**继承**：`Panel<Grid>`

- `RowDefinitionCollection& GetRowDefinitions()`：访问行定义集合。
- `ColumnDefinitionCollection& GetColumnDefinitions()`：访问列定义集合。
- `Grid* AddRowDefinition(const RowDefinition& row)`：添加行定义（链式调用）。
- `Grid* AddColumnDefinition(const ColumnDefinition& column)`：添加列定义（链式调用）。
- `static void SetRow(UIElement* element, int row)`：设置元素所在行（附加属性）。
- `static int GetRow(UIElement* element)`：获取元素所在行。
- `static void SetColumn(UIElement* element, int column)`：设置元素所在列。
- `static int GetColumn(UIElement* element)`：获取元素所在列。
- `static void SetRowSpan/ColumnSpan(UIElement*, int span)`：设置跨行/列数。

### 11.22 TextBlock

**职责**：显示只读文本，支持字体样式和对齐。

**继承**：`FrameworkElement<TextBlock>`

- `void SetText(const std::string& text)`：设置文本内容。
- `const std::string& GetText() const`：获取文本内容。
- `TextBlock* Text(const std::string& text)`：链式调用设置文本。
- `const std::string& Text() const`：无参重载，获取文本。
- `void SetFontFamily(const std::string& family)`：设置字体族。
- `const std::string& GetFontFamily() const`：获取字体族。
- `TextBlock* FontFamily(const std::string& family)`：链式调用设置字体族。
- `const std::string& FontFamily() const`：无参重载，获取字体族。
- `void SetFontSize(float size)`：设置字体大小。
- `float GetFontSize() const`：获取字体大小。
- `TextBlock* FontSize(float size)`：链式调用设置字体大小。
- `float FontSize() const`：无参重载，获取字体大小。
- `void SetFontWeight(FontWeight weight)`：设置字体粗细。
- `FontWeight GetFontWeight() const`：获取字体粗细。
- `TextBlock* FontWeight(FontWeight weight)`：链式调用设置字体粗细。
- `FontWeight FontWeight() const`：无参重载，获取字体粗细。
- `void SetFontStyle(FontStyle style)`：设置字体样式（Normal/Italic）。
- `FontStyle GetFontStyle() const`：获取字体样式。
- `TextBlock* FontStyle(FontStyle style)`：链式调用设置字体样式。
- `FontStyle FontStyle() const`：无参重载，获取字体样式。
- `void SetTextAlignment(TextAlignment align)`：设置文本对齐（Left/Center/Right）。
- `TextAlignment GetTextAlignment() const`：获取文本对齐。
- `TextBlock* TextAlignment(TextAlignment align)`：链式调用设置文本对齐。
- `TextAlignment TextAlignment() const`：无参重载，获取文本对齐。
- `void SetForeground(Brush* brush)`：设置前景色。
- `Brush* GetForeground() const`：获取前景色。
- `TextBlock* Foreground(Brush* brush)`：链式调用设置前景色。
- `Brush* Foreground() const`：无参重载，获取前景色。
- `void SetTextWrapping(TextWrapping wrap)`：设置文本换行模式。
- `TextWrapping GetTextWrapping() const`：获取文本换行模式。
- `TextBlock* TextWrapping(TextWrapping wrap)`：链式调用设置文本换行。
- `TextWrapping TextWrapping() const`：无参重载，获取文本换行。

### 11.23 Border

**职责**：装饰单个子元素，提供边框、圆角、背景。

**继承**：`FrameworkElement<Border>`

- `void SetChild(UIElement* child)`：设置子元素。
- `UIElement* GetChild() const`：获取子元素。
- `Border* Child(UIElement* child)`：链式调用设置子元素。
- `UIElement* Child() const`：无参重载，获取子元素。
- `void SetBorderBrush(Brush* brush)`：设置边框画刷。
- `Brush* GetBorderBrush() const`：获取边框画刷。
- `Border* BorderBrush(Brush* brush)`：链式调用设置边框画刷。
- `Brush* BorderBrush() const`：无参重载，获取边框画刷。
- `void SetBorderThickness(const Thickness& thickness)`：设置边框厚度。
- `const Thickness& GetBorderThickness() const`：获取边框厚度。
- `Border* BorderThickness(const Thickness& thickness)`：链式调用设置边框厚度。
- `Border* BorderThickness(float uniform)`：便捷重载，统一厚度。
- `Border* BorderThickness(float left, float top, float right, float bottom)`：便捷重载。
- `const Thickness& BorderThickness() const`：无参重载，获取边框厚度。
- `void SetCornerRadius(const CornerRadius& radius)`：设置圆角半径。
- `const CornerRadius& GetCornerRadius() const`：获取圆角半径。
- `Border* CornerRadius(const CornerRadius& radius)`：链式调用设置圆角。
- `Border* CornerRadius(float uniform)`：便捷重载，统一圆角。
- `Border* CornerRadius(float topLeft, float topRight, float bottomRight, float bottomLeft)`：便捷重载。
- `const CornerRadius& CornerRadius() const`：无参重载，获取圆角。
- `void SetBackground(Brush* brush)`：设置背景画刷。
- `Brush* GetBackground() const`：获取背景画刷。
- `Border* Background(Brush* brush)`：链式调用设置背景。
- `Brush* Background() const`：无参重载，获取背景。
- `void SetPadding(const Thickness& padding)`：设置内边距。
- `const Thickness& GetPadding() const`：获取内边距。
- `Border* Padding(const Thickness& padding)`：链式调用设置内边距。
- `Border* Padding(float uniform)`：便捷重载，统一内边距。
- `Border* Padding(float left, float top, float right, float bottom)`：便捷重载。
- `const Thickness& Padding() const`：无参重载，获取内边距。

### 11.24 Image

**职责**：显示图像资源。

**继承**：`FrameworkElement<Image>`

- `void SetSource(ImageSource* source)`：设置图像源。
- `ImageSource* GetSource() const`：获取图像源。
- `Image* Source(ImageSource* source)`：链式调用设置图像源。
- `Image* Source(const std::string& path)`：便捷重载，从路径加载。
- `ImageSource* Source() const`：无参重载，获取图像源。
- `void SetStretch(Stretch mode)`：设置拉伸模式（None/Fill/Uniform/UniformToFill）。
- `Stretch GetStretch() const`：获取拉伸模式。
- `Image* Stretch(Stretch mode)`：链式调用设置拉伸模式。
- `Stretch Stretch() const`：无参重载，获取拉伸模式。

### 11.25 Shape

**职责**：几何图形基类（矩形、椭圆、路径等）。

**模板参数**：`template<typename Derived>`（CRTP 模式，派生类传入自身类型）

**继承**：`FrameworkElement<Derived>`

- `void SetFill(Brush* brush)`：设置填充画刷。
- `Brush* GetFill() const`：获取填充画刷。
- `Derived* Fill(Brush* brush)`：链式调用设置填充画刷。
- `Brush* Fill() const`：无参重载，获取填充画刷。
- `void SetStroke(Brush* brush)`：设置描边画刷。
- `Brush* GetStroke() const`：获取描边画刷。
- `Derived* Stroke(Brush* brush)`：链式调用设置描边画刷。
- `Brush* Stroke() const`：无参重载，获取描边画刷。
- `void SetStrokeThickness(float thickness)`：设置描边厚度。
- `float GetStrokeThickness() const`：获取描边厚度。
- `Derived* StrokeThickness(float thickness)`：链式调用设置描边厚度。
- `float StrokeThickness() const`：无参重载，获取描边厚度。
- `virtual Geometry* GetGeometry() const = 0`：获取几何数据（纯虚函数）。

### 11.26 DrawCommand

**职责**：描述单个绘制指令。

**继承**：无（数据结构）

- `void SetType(DrawCommandType type)`：设置命令类型（Rectangle/Text/Image/Custom）。
- `DrawCommandType GetType() const`：获取命令类型。
- `void SetPosition(const Vec2& pos)`：设置位置。
- `void SetSize(const Vec2& size)`：设置尺寸。
- `void SetColor(const Color& color)`：设置颜色。
- `void SetText(const std::string& text)`：设置文本内容。
- `void SetTexture(std::shared_ptr<Texture> texture)`：设置纹理。
- `void SetCustomCallback(std::function<void(RenderBackend&)> callback)`：设置自定义绘制回调。

### 11.27 Renderer

**职责**：管理绘制命令缓冲，驱动渲染帧。

**继承**：无（渲染管理器）

- `void Submit(const DrawCommand& cmd)`：提交绘制命令到缓冲。
- `void RenderFrame()`：执行当前帧的所有命令并清空缓冲。
- `void Clear()`：清空命令缓冲。
- `void SetBackend(std::shared_ptr<RenderBackend> backend)`：设置渲染后端。
- `RenderBackend* GetBackend() const`：获取当前后端。
- `void SetViewport(const Rect& viewport)`：设置视口区域。

### 11.28 RenderBackend

**职责**：平台图形 API 抽象接口。

**继承**：无（抽象接口基类）

- `virtual void BeginFrame() = 0`：开始帧渲染。
- `virtual void EndFrame() = 0`：结束帧渲染并呈现。
- `virtual void DrawRectangle(const Rect& rect, const Color& color) = 0`：绘制矩形。
- `virtual void DrawText(const Vec2& position, const std::string& text, const TextFormat& format, const Color& color) = 0`：绘制文本。
- `virtual void DrawImage(const Rect& destRect, Texture* texture, const Rect& sourceRect) = 0`：绘制图像。
- `virtual void SetTransform(const Matrix3x2& transform) = 0`：设置变换矩阵。
- `virtual void PushClip(const Rect& clipRect) = 0`：压入裁剪区域。
- `virtual void PopClip() = 0`：弹出裁剪区域。

### 11.29 RenderTreeBuilder

**职责**：将视觉树转换为绘制命令流（可选辅助类）。

**继承**：无（辅助构建器）

- `void BeginVisual(const Visual& visual)`：开始处理一个视觉节点。
- `void EndVisual()`：结束当前视觉节点。
- `void EmitQuad(const Rect& rect, const Color& color)`：生成矩形命令。
- `void EmitText(const Vec2& position, const std::string& text, const TextFormat& format, const Color& color)`：生成文本命令。
- `void EmitImage(const Rect& rect, Texture* texture)`：生成图像命令。
- `std::vector<DrawCommand> GetCommands() const`：获取生成的命令列表。

### 11.30 MaterialPool

**职责**：管理材质和纹理资源缓存（可选辅助类）。

**继承**：无（资源池管理器）

- `MaterialHandle Acquire(const MaterialDesc& desc)`：获取或创建材质。
- `void Release(MaterialHandle handle)`：释放材质引用。
- `Texture* GetTexture(const std::string& path)`：加载或获取纹理。
- `void Flush()`：清空未使用的资源。

### 11.31 VisualStateManager

**职责**：管理控件的视觉状态和状态转换。

**继承**：无（状态管理器，静态工具类）

- `static bool GoToState(Control& control, const std::string& stateName, bool useTransitions)`：切换到指定状态。
- `static void RegisterStates(Control& control, const VisualStateGroupCollection& groups)`：注册状态组。
- `static VisualState* GetCurrentState(Control& control, const std::string& groupName)`：获取当前状态。

### 11.32 InteractionTracker

**职责**：跟踪控件的交互状态（按下、悬停等）。

**继承**：无（交互状态跟踪器）

- `void TrackPressed(Control& control, bool pressed)`：设置按下状态。
- `void TrackPointerOver(Control& control, bool over)`：设置鼠标悬停状态。
- `bool IsPressed(const Control& control) const`：查询按下状态。
- `bool IsPointerOver(const Control& control) const`：查询悬停状态。

### 11.33 VisualTreeManager

**职责**：提供视觉树遍历、调试、批量操作工具。

**继承**：无（视觉树工具管理器）

- `void Traverse(Visual& root, const std::function<void(Visual&)>& visitor)`：遍历视觉树并对每个节点调用访问器。
- `void InvalidateSubtree(Visual& root)`：批量标记子树需要重绘。
- `Visual* FindVisualByName(Visual& root, const std::string& name)`：按名称查找视觉节点。
- `std::vector<Visual*> FindVisualsOfType(Visual& root, const std::type_info& type)`：按类型查找节点。

### 11.34 GeometryCache

**职责**：缓存复杂几何数据（路径、曲线等）。

**继承**：无（几何缓存管理器）

- `Geometry* GetOrCreate(const GeometryKey& key, const std::function<Geometry*()>& factory)`：获取或创建几何体。
- `void Remove(const GeometryKey& key)`：移除缓存。
- `void Clear()`：清空所有缓存。
- `size_t GetCacheSize() const`：获取缓存大小。

### 11.35 TemplatePartResolver

**职责**：从模板中查找命名部件并执行断言。

**继承**：无（模板部件解析工具）

- `UIElement* FindPart(FrameworkElement& templatedParent, const std::string& partName)`：查找模板部件。
- `template<typename T> T* FindPartAs(FrameworkElement& templatedParent, const std::string& partName)`：查找并转换为指定类型。
- `void AssertPart(FrameworkElement& templatedParent, const std::string& partName)`：断言部件存在，否则抛出异常。

## 12. 链式 API 设计约定

为支持流畅的链式调用风格（如 `button->Content("Click")->Padding(5, 10)`），框架中所有可设置的属性都提供三种形式的 API：

1. **传统 Getter/Setter**：`SetXxx(value)` / `GetXxx()`，遵循明确命名约定。
2. **链式 Setter**：`Xxx(value)` 返回 `this` 指针，支持方法链。
3. **无参 Getter**：`Xxx()` 无参重载返回属性值，与链式 setter 同名。

### 12.1 基本示例

```cpp
// 传统方式
button->SetContent("Click");
button->SetPadding(Thickness(5, 10, 5, 10));
auto content = button->GetContent();

// 链式方式
button->Content("Click")->Padding(5, 10, 5, 10);
auto content = button->Content();  // 无参重载获取值
```

### 12.2 指针类型选择

链式 API 返回**裸指针**而非智能指针，原因如下：

- **生命周期由外部管理**：UI 元素的生命周期由父容器或资源系统控制，链式调用不应转移所有权。
- **避免智能指针开销**：返回 `std::shared_ptr<T>` 会强制整个调用链使用智能指针，增加引用计数开销且与树结构管理冲突。
- **语义清晰**：裸指针 `this` 表示调用者仅获得临时引用，不拥有对象。

### 12.3 CRTP 实现类型安全继承

为确保派生类的链式方法返回正确的派生类型（而非基类指针），采用 **CRTP（奇异递归模板模式）** 实现：

```cpp
// 基类使用 CRTP 模板参数
template<typename Derived>
class FrameworkElement : public UIElement {
public:
    // 链式方法返回 Derived* 而非 FrameworkElement*
    Derived* Width(float value) {
        SetWidth(value);
        return static_cast<Derived*>(this);
    }
    
    float Width() const {
        return GetWidth();
    }
    
    Derived* Height(float value) {
        SetHeight(value);
        return static_cast<Derived*>(this);
    }
    
    float Height() const {
        return GetHeight();
    }
    
    // 其他属性同理...
};

// 派生类继承时传入自身类型
template<typename Derived>
class Control : public FrameworkElement<Derived> {
public:
    Derived* Padding(float uniform) {
        SetPadding(Thickness(uniform));
        return static_cast<Derived*>(this);
    }
    
    Derived* Padding(float left, float top, float right, float bottom) {
        SetPadding(Thickness(left, top, right, bottom));
        return static_cast<Derived*>(this);
    }
    
    const Thickness& Padding() const {
        return GetPadding();
    }
};

// 具体控件继承
class Button : public Control<Button> {
    // 自动继承所有链式方法，返回类型为 Button*
};

// 使用示例
auto btn = new Button();
btn->Width(100)      // 返回 Button*
   ->Height(30)      // 返回 Button*
   ->Padding(5, 10)  // 返回 Button*
   ->Content("OK");  // 返回 Button*
```

**CRTP 优势**：
- **类型安全**：链式调用始终返回最派生类指针，无需手动转型。
- **零运行时开销**：编译期静态绑定，无虚函数调用。
- **自动传播**：基类定义一次，所有派生类自动获得正确类型的链式接口。

**实现注意事项**：
- 每个可实例化的类需要"封闭"CRTP 链，传入自身类型（如 `class Button : public Control<Button>`）。
- 纯抽象基类（如 `Control` 本身作为基类）保持模板形式，由具体类完成实例化。
- 便捷重载（如 `Padding(float uniform)`）在派生类中补充，以保持接口简洁。

### 12.4 实现规则

- 链式 setter 返回 `Derived*`（通过 CRTP），便于类型安全的链式调用。
- 便捷重载（如 `Padding(float uniform)` 或 `Padding(float left, float top, float right, float bottom)`）提升易用性。
- 无参 getter 使用 `const` 修饰符以区分重载，且不与有参版本冲突。
- 传统 `Get/Set` 方法保留以保持向后兼容和明确语义场景。

### 12.5 适用范围

- `FrameworkElement` 及其派生类的所有常用属性（Width、Height、DataContext、Style 等）。
- `Control` 及其派生类的视觉属性（Foreground、Background、Padding、BorderThickness 等）。
- `ContentControl`、`ItemsControl` 等专用属性（Content、ItemsSource 等）。
- `Panel` 及其派生类的布局属性（Background、Orientation、Spacing 等）以及子元素批量添加接口。

**完整链式调用示例**：
```cpp
// Panel 示例
auto stack = new StackPanel();
stack->Orientation(Orientation::Vertical)
     ->Spacing(10)
     ->Background(Brushes::White())
     ->Children({
         new TextBlock()->Text("Header")->FontSize(20)->Foreground(Brushes::Black()),
         new Button()->Content("Click Me")->Padding(10, 5),
         new TextBlock()->Text("Footer")->FontSize(12)->TextAlignment(TextAlignment::Center)
     });

auto grid = new Grid();
grid->AddRowDefinition(RowDefinition::Auto())
    ->AddRowDefinition(RowDefinition::Star(1))
    ->AddColumnDefinition(ColumnDefinition::Fixed(100))
    ->AddColumnDefinition(ColumnDefinition::Star(1))
    ->AddChild(new TextBlock()->Text("Label"))
    ->AddChild(new TextBox());

// 轻量元素示例
auto border = new Border();
border->BorderBrush(Brushes::Gray())
      ->BorderThickness(1)
      ->CornerRadius(5)
      ->Padding(10)
      ->Background(Brushes::White())
      ->Child(new TextBlock()->Text("Bordered Content"));

auto image = new Image();
image->Source("assets/logo.png")
     ->Stretch(Stretch::Uniform)
     ->Width(200)
     ->Height(200);

// ItemsControl 示例
auto listBox = new ListBox();
listBox->ItemsSource(myDataCollection)
       ->ItemTemplate(myItemTemplate)
       ->Width(300)
       ->Height(400);

// Shape 示例（假设有 Rectangle 派生类）
auto rect = new Rectangle();
rect->Fill(Brushes::Blue())
    ->Stroke(Brushes::Black())
    ->StrokeThickness(2)
    ->Width(100)
    ->Height(50);
```

## 13. 依赖属性系统集成实现

### 13.1 设计原则

所有 UI 元素的可配置属性均通过依赖属性系统实现，以支持：
- **数据绑定**：属性可绑定到数据源，自动更新
- **样式设置**：样式可以设置依赖属性值
- **属性值优先级**：本地值 > 样式值 > 默认值
- **变更通知**：属性变更自动触发布局/渲染失效
- **动画支持**：依赖属性可作为动画目标

### 13.2 模板类依赖属性实现

由于 `FrameworkElement`、`Control` 等为 CRTP 模板类，依赖属性的注册采用以下方案：

1. **头文件声明**：在模板类中声明静态依赖属性访问器
   ```cpp
   template<typename Derived>
   class FrameworkElement : public UIElement {
   public:
       static const binding::DependencyProperty& WidthProperty();
       // ...
   };
   ```

2. **实现文件注册**：在 `.cpp` 文件中为模板类提供属性注册实现
   ```cpp
   template<typename Derived>
   const binding::DependencyProperty& FrameworkElement<Derived>::WidthProperty() {
       static auto* property = binding::DependencyProperty::Register(
           "Width", typeid(float), typeid(FrameworkElement<Derived>),
           binding::PropertyMetadata::Create(-1.0f)
       );
       return *property;
   }
   ```

3. **属性访问封装**：getter/setter 通过 `GetValue<T>()`/`SetValue()` 访问
   ```cpp
   void SetWidth(float value) { 
       SetValue(WidthProperty(), value); 
       InvalidateMeasure(); 
   }
   float GetWidth() const { 
       return GetValue<float>(WidthProperty()); 
   }
   ```

### 13.3 已实现的依赖属性

#### UIElement（3 个属性）
| 属性名 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `Visibility` | `Visibility` | `Visible` | 可见性（Visible/Hidden/Collapsed） |
| `IsEnabled` | `bool` | `true` | 是否启用 |
| `Opacity` | `float` | `1.0f` | 不透明度（0.0-1.0） |

#### FrameworkElement（7 个属性）
| 属性名 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `Width` | `float` | `-1.0f` | 宽度（-1 表示自动） |
| `Height` | `float` | `-1.0f` | 高度（-1 表示自动） |
| `MinWidth` | `float` | `0.0f` | 最小宽度 |
| `MaxWidth` | `float` | `∞` | 最大宽度 |
| `MinHeight` | `float` | `0.0f` | 最小高度 |
| `MaxHeight` | `float` | `∞` | 最大高度 |
| `DataContext` | `std::any` | `std::any()` | 数据上下文 |

**待添加属性**：`Margin`, `HorizontalAlignment`, `VerticalAlignment`

#### Control（5 个属性）
| 属性名 | 类型 | 默认值 | 说明 |
|--------|------|--------|------|
| `Foreground` | `Brush*` | `nullptr` | 前景画刷 |
| `Background` | `Brush*` | `nullptr` | 背景画刷 |
| `BorderBrush` | `Brush*` | `nullptr` | 边框画刷 |
| `BorderThickness` | `Thickness` | `Thickness(0)` | 边框厚度 |
| `Padding` | `Thickness` | `Thickness(0)` | 内边距 |

**待添加属性**：`FontFamily`, `FontSize`, `FontWeight`

### 13.4 待实现的依赖属性

#### ContentControl
- `Content` - 内容对象（std::any）
- `ContentTemplate` - 内容模板（DataTemplate*）

#### ItemsControl
- `ItemsSource` - 数据源（std::shared_ptr<IEnumerable>）
- `ItemTemplate` - 项模板（DataTemplate*）
- `ItemsPanel` - 项容器面板（ItemsPanelTemplate*）

#### Panel
- `Background` - 背景画刷（Brush*）

#### TextBlock
- `Text` - 文本内容（std::string）
- `FontFamily` - 字体族（std::string）
- `FontSize` - 字体大小（float）
- `FontWeight` - 字体粗细（FontWeight）
- `FontStyle` - 字体样式（FontStyle）
- `TextAlignment` - 文本对齐（TextAlignment）
- `Foreground` - 前景色（Brush*）
- `TextWrapping` - 文本换行（TextWrapping）

#### Border
- `Child` - 子元素（UIElement*）
- `BorderBrush` - 边框画刷（Brush*）
- `BorderThickness` - 边框厚度（Thickness）
- `CornerRadius` - 圆角半径（CornerRadius）
- `Background` - 背景画刷（Brush*）
- `Padding` - 内边距（Thickness）

### 13.5 实现要点

1. **成员变量移除**：原有的私有成员变量（如 `width_`, `foreground_` 等）已移除，完全由依赖属性系统管理。

2. **属性失效化**：属性设置时自动调用失效化方法：
   - 布局属性（Width/Height/Padding 等）→ `InvalidateMeasure()`
   - 外观属性（Foreground/Background 等）→ `InvalidateVisual()`

3. **模板类特殊处理**：
   - 依赖属性在 `.cpp` 文件中定义为模板函数
   - 使用静态局部变量确保单例注册
   - 通过 `this->template GetValue<T>()` 在模板类中调用

4. **链式 API 兼容**：所有链式调用接口保持不变，内部委托到依赖属性系统。

5. **类型安全**：使用 `typeid()` 确保类型匹配，运行时检查属性类型。

### 13.6 完整依赖属性规划

#### UIElement (已实现 3/3)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Visibility | `Visibility` | `Visible` | 可见性 | ✅ |
| IsEnabled | `bool` | `true` | 是否启用 | ✅ |
| Opacity | `float` | `1.0f` | 不透明度 | ✅ |
| Clip | `Geometry*` | `nullptr` | 裁剪区域 | 📋 待实现 |
| RenderTransform | `Transform*` | `nullptr` | 渲染变换 | 📋 待实现 |

#### FrameworkElement (已实现 7/10)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Width | `float` | `-1.0f` | 宽度 | ✅ |
| Height | `float` | `-1.0f` | 高度 | ✅ |
| MinWidth | `float` | `0.0f` | 最小宽度 | ✅ |
| MaxWidth | `float` | `∞` | 最大宽度 | ✅ |
| MinHeight | `float` | `0.0f` | 最小高度 | ✅ |
| MaxHeight | `float` | `∞` | 最大高度 | ✅ |
| DataContext | `std::any` | `std::any()` | 数据上下文 | ✅ |
| Margin | `Thickness` | `Thickness(0)` | 外边距 | ✅ |
| HorizontalAlignment | `HorizontalAlignment` | `Stretch` | 水平对齐 | ✅ |
| VerticalAlignment | `VerticalAlignment` | `Stretch` | 垂直对齐 | ✅ |

#### Control (已实现 8/8)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Foreground | `Brush*` | `nullptr` | 前景画刷 | ✅ |
| Background | `Brush*` | `nullptr` | 背景画刷 | ✅ |
| BorderBrush | `Brush*` | `nullptr` | 边框画刷 | ✅ |
| BorderThickness | `Thickness` | `Thickness(0)` | 边框厚度 | ✅ |
| Padding | `Thickness` | `Thickness(0)` | 内边距 | ✅ |
| FontFamily | `std::string` | `"Arial"` | 字体族 | ✅ |
| FontSize | `float` | `14.0f` | 字体大小 | ✅ |
| FontWeight | `FontWeight` | `Normal` | 字体粗细 | ✅ |

#### ContentControl (已实现 2/2)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Content | `std::any` | `std::any()` | 内容对象 | ✅ |
| ContentTemplate | `DataTemplate*` | `nullptr` | 内容模板 | ✅ |

#### ItemsControl (已实现 3/3)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| ItemsSource | `std::any` | `std::any()` | 数据源（可存储任意集合类型） | ✅ |
| ItemTemplate | `DataTemplate*` | `nullptr` | 项模板 | ✅ |
| ItemsPanel | `UIElement*` | `nullptr` | 项容器面板 | ✅ |

#### Panel (已实现 1/1)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Background | `Brush*` | `nullptr` | 背景画刷 | ✅ |

#### StackPanel (已实现 2/2)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Orientation | `Orientation` | `Vertical` | 排列方向 | ✅ |
| Spacing | `float` | `0.0f` | 子元素间距 | ✅ |

#### Grid (未实现 0/0)
**注意**：Grid 的 Row/Column 是附加属性，使用全局 map 实现，不计入依赖属性系统

#### TextBlock (已实现 8/8)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Text | `std::string` | `""` | 文本内容 | ✅ |
| FontFamily | `std::string` | `"Arial"` | 字体族 | ✅ |
| FontSize | `float` | `12.0f` | 字体大小 | ✅ |
| FontWeight | `FontWeight` | `Normal` | 字体粗细 | ✅ |
| FontStyle | `FontStyle` | `Normal` | 字体样式 | ✅ |
| TextAlignment | `TextAlignment` | `Left` | 文本对齐 | ✅ |
| Foreground | `Brush*` | `nullptr` | 前景色 | ✅ |
| TextWrapping | `TextWrapping` | `NoWrap` | 文本换行 | ✅ |

#### Border (已实现 6/6)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Child | `UIElement*` | `nullptr` | 子元素 | ✅ |
| BorderBrush | `Brush*` | `nullptr` | 边框画刷 | ✅ |
| BorderThickness | `Thickness` | `Thickness(0)` | 边框厚度 | ✅ |
| CornerRadius | `CornerRadius` | `CornerRadius(0)` | 圆角半径 | ✅ |
| Background | `Brush*` | `nullptr` | 背景画刷 | ✅ |
| Padding | `Thickness` | `Thickness(0)` | 内边距 | ✅ |

#### Button (未实现 0/0)
**注意**：Button 从 ContentControl 继承，无额外依赖属性

#### Window (已实现 6/6)
| 属性名 | C++ 类型 | 默认值 | 说明 | 状态 |
|--------|----------|--------|------|------|
| Title | `std::string` | `""` | 窗口标题 | ✅ |
| WindowState | `WindowState` | `Normal` | 窗口状态 | ✅ |
| Left | `float` | `0.0f` | 窗口左边距 | ✅ |
| Top | `float` | `0.0f` | 窗口顶边距 | ✅ |
| ShowInTaskbar | `bool` | `true` | 是否显示在任务栏 | ✅ |
| Topmost | `bool` | `false` | 是否置顶 | ✅ |

**继承关系**：`DependencyObject → Visual → UIElement → FrameworkElement → Control → ContentControl → Window`

**统计**：
- ✅ 已实现：**49 个属性**（11 个类/模块完成）
- 📋 待实现：7 个属性（可选扩展）
- **总计**：56 个依赖属性
- **完成度**：**87.5%** 🎉

### 13.7 实现优先级

#### 第一批（核心布局和外观）✅ 已完成
1. ✅ UIElement - 基础可见性和交互（3 个属性）
2. ✅ FrameworkElement - 尺寸约束和数据上下文（7 个属性）
3. ✅ Control - 外观属性（5 个属性）
4. ✅ ContentControl - 内容展示（2 个属性）

#### 第二批（文本和容器）✅ 已完成
5. ✅ TextBlock - 文本显示（8 个属性）
6. ✅ Border - 装饰容器（6 个属性）
7. ✅ Panel - 布局基类（1 个属性）
8. ✅ StackPanel - 线性布局（2 个属性）

#### 第三批（集合控件）✅ 已完成
9. ✅ ItemsControl - 集合控件（3 个属性）

#### 第四批（扩展属性）✅ 已完成
10. ✅ FrameworkElement 扩展 - Margin、HorizontalAlignment、VerticalAlignment（3 个属性）
11. ✅ Control 扩展 - FontFamily、FontSize、FontWeight（3 个属性）

#### 第五批（窗口类）✅ 已完成
12. ✅ Window - 顶层窗口容器（6 个属性）

#### 第六批（可选扩展）📋 未实现
13. ⏸️ UIElement 扩展 - Clip、RenderTransform（2 个属性，需 Geometry 和 Transform 类型支持）
14. ⏸️ Button 扩展 - ClickMode、IsDefault、IsCancel（3 个属性，可选功能）
15. ⏸️ 其他控件特定属性（按需扩展）

### 13.8 新增类型和文件

#### 新增枚举类型
1. **TextEnums.h**
   - `FontWeight` - 字体粗细（Thin, Light, Normal, Bold, Black 等）
   - `FontStyle` - 字体样式（Normal, Italic, Oblique）
   - `TextAlignment` - 文本对齐（Left, Center, Right, Justify）
   - `TextWrapping` - 文本换行（NoWrap, Wrap, WrapWithOverflow）

2. **Alignment.h**
   - `HorizontalAlignment` - 水平对齐（Left, Center, Right, Stretch）
   - `VerticalAlignment` - 垂直对齐（Top, Center, Bottom, Stretch）

3. **Window.h**
   - `WindowState` - 窗口状态（Normal, Minimized, Maximized）
   - `WindowStartupLocation` - 窗口启动位置（Manual, CenterScreen, CenterOwner）

#### 新增结构体类型
1. **CornerRadius.h**
   - `CornerRadius` - 圆角半径（支持四角独立设置）

#### 新增类文件
1. **Window** (ContentControl → Window)
   - `include/fk/ui/Window.h` - 窗口类声明
   - `src/ui/Window.cpp` - 窗口类实现

#### 新增依赖属性实现文件
1. `src/ui/Panel.cpp` - Panel 模板类依赖属性注册
2. `src/ui/ItemsControl.cpp` - ItemsControl 模板类依赖属性注册
3. `src/ui/Window.cpp` - Window 类依赖属性注册及窗口操作

### 13.9 后续优化方向

1. **属性变更回调**：为关键属性添加 `PropertyChangedCallback`，实现自定义逻辑。
2. **属性值继承**：实现 `DataContext` 等属性的自动继承机制。
3. **附加属性支持**：实现 `Grid.Row`、`Grid.Column` 等附加属性（当前使用全局 map）。
4. **属性验证**：为数值属性添加范围验证（如 Opacity 0.0-1.0）。
5. **性能优化**：对频繁访问的属性添加缓存机制。
6. **类型系统完善**：实现 Geometry、Transform 基类以支持 Clip 和 RenderTransform 属性。

## 14. 后续工作项

1. ✅ 核心类依赖属性集成（UIElement, FrameworkElement, Control, ContentControl）
2. ✅ 完成主要控件依赖属性（TextBlock, Border, Panel, StackPanel, ItemsControl）
3. ✅ 实现布局和对齐扩展属性（Margin, HorizontalAlignment, VerticalAlignment）
4. ✅ 实现字体扩展属性（FontFamily, FontSize, FontWeight）
5. 📋 编写核心单元测试：依赖属性、绑定冒泡、布局无效化、事件路由
6. 📋 实现基础 Renderer 和 Canvas，验证 DrawCommand 流程
7. 📋 迁移现有控件到新层级，确保示例程序可运行
8. 📋 梳理资源、主题系统与新架构的对接方案
9. 📋 实现属性值继承和附加属性机制
10. 📋 实现 Geometry 和 Transform 基类，支持 Clip 和 RenderTransform 属性
