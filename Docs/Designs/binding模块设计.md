# fk::binding 模块设计

## 1. 模块职责
- 为 UI 控件提供统一的属性系统，实现类似 WPF 的依赖属性与绑定机制。
- 支持链式 API 中的 `Bind()` 调用，处理数据上下文继承、值转换、双向同步。
- 与 `fk::core` 的事件/调度模块协作，同时保持与 `fk::ui`、`fk::core` 的最小耦合。

## 2. 关键概念

### 2.1 DependencyProperty
- 静态注册的属性描述，包含：
  - 属性名称、属性类型、所属类型 `OwnerType`。
  - 默认值、值变更回调 `PropertyChangedCallback`。
  - 验证回调 `ValidateValueCallback`。
  - 绑定元数据（`BindingOptions`，包含默认 BindingMode、UpdateSourceTrigger 等）。
- 注册方式：
  ```cpp
  inline const DependencyProperty& Button::ContentProperty() {
      static const DependencyProperty property = DependencyProperty::Register(
          "Content",
          typeid(ViewPtr),
          typeid(Button),
          PropertyMetadata{}
      );
      return property;
  }
  ```

### 2.2 PropertyStore
- 每个 `DependencyObject` 持有一个 `PropertyStore`，存储属性值优先级：
  1. 本地值 (Local)
  2. 绑定表达式 (Binding)
  3. 样式 Setter (Style)
  4. 继承值 (Inherited)
  5. 默认值 (Default)
- `PropertyValue` 结构：`{ Variant value; ValueSource source; BindingExpression* binding; }`
- 提供 `GetValue`, `SetValue`, `ClearValue`，并触发 `PropertyChanged` 事件。

### 2.3 Binding 与 BindingExpression
- `Binding`
  - 配置结构体，支持链式设置 Source, Path, Mode, UpdateSourceTrigger, Converter 等。
  - 默认 Mode: OneWay，UpdateSourceTrigger: PropertyChanged (若高频可设为 LostFocus)。
- `BindingExpression`
  - 将 Binding 应用于目标属性的运行时对象。
  - 维护目标 `DependencyObject* target`、目标属性 `DependencyProperty`、源对象引用。
  - 监听源对象 `INotifyPropertyChanged` 事件；目标属性变更时触发 `UpdateSource`。

### 2.4 BindingPath
- 对源对象路径解析，支持：
  - 简单属性：`"Name"`
  - 嵌套属性：`"Address.City"`
  - 索引器：`"Items[0]"`
- 解析策略：按路径分段，基于**预注册的 `PropertyAccessor`** 查找 getter/setter。由于 C++ 缺少运行时反射，我们通过宏或模板在类型初始化阶段注册访问器映射，例如：
  ```cpp
  FK_REGISTER_PROPERTY(ViewModel, Name,
      .Getter(&ViewModel::GetName)
      .Setter(&ViewModel::SetName));
  ```
  `BindingPath` 通过类型 + 成员名组合键在注册表中查找 `PropertyAccessor`，并缓存解析结果以提高性能。
- 对于容器索引和动态属性，可扩展注册接口（如 `IndexedPropertyAccessor`）。若需更丰富的元信息，可选用外部库（RTTR 等）提供的反射适配器。

### 2.5 DataContext 与 BindingContext
- `BindingContext` 挂载在 `DependencyObject` 上，与 `fk::ui` 的逻辑树结合。
- `DataContext` 继承策略：
  - 子元素默认继承父元素的 DataContext。
  - 控件显式设置 DataContext 后，停止继承。
- 进入逻辑树 (`OnAttachedToLogicalTree`) 时创建或更新上下文。
- Binding 解析时优先读取目标元素的 DataContext。

### 2.6 值转换与验证
- `IValueConverter`
  - 接口：`Convert(value, targetType, parameter)`、`ConvertBack(value, sourceType, parameter)`。
- `ValidationRule`
  - 提供 `Validate(value)`，返回 `ValidationResult`。
  - 绑定表达式可关联验证器集合。
- 验证失败时可触发 UI 层视觉状态（例如 TextBox 红框）。

### 2.7 多重绑定（扩展）
- 预留 `MultiBinding`, `PriorityBinding` 接口，未来支持多个源或优先级绑定。

## 3. API 草图
```cpp
namespace fk::binding {

enum class BindingMode { OneTime, OneWay, TwoWay, OneWayToSource }; 
enum class UpdateSourceTrigger { Default, PropertyChanged, LostFocus, Explicit };

template <class Converter>
concept ValueConverter = requires(Converter c, Any value, const std::type_info& targetType) {
    { c.Convert(value, targetType, nullptr) } -> std::same_as<Any>;
};

class Binding {
public:
    Binding& Path(std::string path);
    Binding& Source(Any source);
    Binding& Mode(BindingMode mode);
    Binding& UpdateSourceTrigger(UpdateSourceTrigger trigger);
    Binding& Converter(std::shared_ptr<IValueConverter> converter);
    Binding& ValidatesOnDataErrors(bool enable);

    std::unique_ptr<BindingExpression> CreateExpression(DependencyObject* target,
        const DependencyProperty& targetProperty) const;
};

class BindingExpression {
public:
    void Activate();
    void Detach();
    void UpdateTarget();
    void UpdateSource();

private:
    void OnSourceChanged();
    void OnTargetChanged();
};

} // namespace fk::binding
```

## 4. 工作流程
1. 控件在 `SetValue(property, binding)` 时识别参数类型为 `Binding`。
2. `Binding::CreateExpression()` 创建 `BindingExpression`，存入 `PropertyStore`。
3. 在元素加载 (`OnAttachedToLogicalTree`) 时执行 `BindingExpression::Activate()`：
   - 确定源对象（显式 Source > RelativeSource > ElementName > DataContext）。
   - 解析路径，订阅源对象 `PropertyChanged`。
   - 将源值写入目标属性 (`UpdateTarget`)。
4. 当源对象属性变更，`OnSourceChanged()` 触发 `UpdateTarget()`。
5. 若模式为 `TwoWay`，目标属性变更触发 `UpdateSource()` 写回源对象。
6. 解除绑定：元素离开逻辑树时调用 `Detach()`，释放监听。

## 5. 依赖关系
- 依赖 `fk::core`：事件、Dispatcher、Any/Variant、Logger。
- 向 `fk::ui` 暴露 API：`DependencyObject`, `DependencyProperty`, `Binding`。
- 不依赖 `fk::render`、`fk::app`、`fk::theme`。

## 6. 性能与内存考量
- `BindingExpression` 避免反射开销：
  - 编译期生成访问器或使用 `std::function` 缓存 getter/setter。
  - 支持绑定路径缓存，以第一次解析结果复用。
- `PropertyStore` 使用稀疏存储（如 `std::unordered_map<const DependencyProperty*, PropertyValue>`），默认不分配。
- 绑定大量变更时以 `Dispatcher` 合并更新（防抖）。

## 7. 测试计划
- 绑定基础逻辑：
  - OneWay/TwoWay/OneTime 更新。
  - DataContext 继承。
  - Converter 与 Validation。
- Property 优先级：本地值覆盖绑定值、本地值清除恢复绑定值。
- 生命周期：控件 attach/detach，绑定自动激活/释放。
- 并发：跨线程更新源对象时确保 UI 更新在 Dispatcher 上执行。

## 8. 迭代路线
1. 定义 `DependencyProperty`/`PropertyStore`，完成基本属性设置与变更通知。
2. 实现 Binding → BindingExpression 的创建与激活。
3. 支持 DataContext 与 Source/ElementName 查找。
4. 加入 Converter、Validation、UpdateSourceTrigger。
5. 扩展多重绑定、命令绑定等高级特性。

---
> 此设计与 `fk::ui` 模块紧密相连，后续需要同步定义 `DependencyObject`、`FrameworkElement` 的接口以保证绑定正常工作。