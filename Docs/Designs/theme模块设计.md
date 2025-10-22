# fk::theme 模块设计

## 1. 模块目标
- 管理 UI 框架中的资源（颜色、画刷、字体、样式等），实现主题切换与资源查找。
- 与 `fk::ui`、`fk::binding` 协作，使控件可根据状态动态响应资源变化。
- 支持多主题、动态主题切换、局部覆盖等机制。

## 2. 核心概念

### 2.1 ResourceDictionary
- 键值映射结构：`std::unordered_map<ResourceKey, ResourceEntry>`。
- `ResourceKey` 支持字符串、类型、组合键等形式。
- `ResourceEntry` 包含：
  - `Any value`：资源对象（颜色、Brush、Style 等）。
  - `ResourceType type`：类型信息。
  - `ResourceLifetime lifetime`：Static / Dynamic。
- 提供 API：
  ```cpp
  void Add(ResourceKey key, Any value);
  bool TryGet(ResourceKey key, Any& out) const;
  void Remove(ResourceKey key);
  ```
- 支持合并字典：`Merge(const ResourceDictionary& other)`。
- 支持层级继承：子字典可以引用父字典，查找时递归。

### 2.2 Theme
- 表示一个具名主题集合。结构：
  ```cpp
  struct Theme {
      std::string name;
      ResourceDictionary resources;
  };
  ```
- 可通过文件加载（JSON/YAML）或代码构建。
- 支持主题之间的差异增量，复用基底主题。

### 2.3 ThemeManager
- 管理主题集合与当前主题。
- API：
  ```cpp
  void RegisterTheme(Theme theme);
  void SetCurrentTheme(const std::string& name);
  const Theme& CurrentTheme() const;

  core::Event<const Theme&> ThemeChanged;
  ```
- 切换主题时触发 `ThemeChanged`，通知 UI 更新资源引用。

### 2.4 DynamicResource / StaticResource
- `StaticResource`：加载时即解析，引用固定值。
- `DynamicResource`：持有资源 key 的引用，在主题切换时重新解析。
- UI 控件的 `SetValue` 可根据资源引用类型，决定是否订阅 `ThemeChanged`。

### 2.5 Brush / Color / Font API
- `Color`：封装 RGBA 颜色，可支持线性/伽马空间。
- `Brush`：
  - `SolidColorBrush`
  - `LinearGradientBrush`
  - `RadialGradientBrush`
- `FontFamily`, `FontStyle`：与渲染模块协作，定位字体资源。
- 这些对象注册到资源字典中，控件通过 `FindResource` 获取。

### 2.6 Style 与 Setter
- `Style`：定义一组 Setter 与 Trigger。
  ```cpp
  struct Style {
      std::type_index targetType;
      std::vector<Setter> setters;
      std::vector<StyleTrigger> triggers;
  };
  ```
- `Setter`：对应 `DependencyProperty` 与值（或资源引用）。
- `StyleTrigger`：根据属性或状态（如 `IsMouseOver`）应用不同 Setter。
- 支持基于主题的样式切换（类似 WPF 的 `BasedOn`）。

### 2.7 Resource Lookup 流程
1. 控件请求资源 `FindResource(key)`：
   - 优先在自身 `Resources()` 查找。
   - 逐层查找父逻辑树节点的资源字典。
   - 若仍未找到，查找 `Application::Resources()`。
   - 最后查 `ThemeManager::CurrentTheme().resources`。
2. 对于 `DynamicResource`，控件注册监听主题变化，主题切换时重新取值。

## 3. 数据结构
- `ResourceKey`
  - 实现哈希/等价比较，支持 `std::string`, `std::type_index`, `std::pair` 等。
- `ResourceReference`
  - `struct ResourceReference { ResourceKey key; bool dynamic; }`。
- `ThemeDefinition`
  - 解析文件时的临时结构，转换为 `Theme`。

## 4. 纯 C++ 主题定义
- 主题默认通过 C++ 代码注册，避免运行时解析：
- 主题初始化建议遵循链式 API：
  ```cpp
  Theme CreateDarkTheme() {
    return Theme("Dark")
      .Resources(ResourceDictionary()
        .Add("Color.Primary", Color::FromRgb(0x1F, 0x1F, 0x1F))
        .Add("Brush.Accent", SolidColorBrush(Color::FromRgb(0x3F, 0x8E, 0xFC)))
        .Add("Style.Button.Primary", Style(typeid(Button))
          .Setter(Button::BackgroundProperty(), ResourceReference{"Brush.Accent", true})
          .Setter(Button::ForegroundProperty(), Color::White())));
  }

  void RegisterThemes(ThemeManager& manager) {
    manager.RegisterTheme(CreateDarkTheme());
    manager.SetCurrentTheme("Dark");
  }
  ```
- 应用可在构建阶段将配置脚本生成上述初始化代码，也可直接手写主题逻辑。框架仅提供 C++ API，不强制使用脚本或外部格式。
- 如需运行时热加载，可额外提供插件式解析器（可放在应用层实现），但不影响默认纯 C++ 路径。

## 5. 主题切换流程
1. `ThemeManager::SetCurrentTheme(name)`：
   - 更新当前主题。
   - 发布 `ThemeChanged` 事件。
2. `Application` 或控件订阅该事件：
   - `Application`：若有全局资源使用 `DynamicResource`，重新评估。
   - 控件：若属性绑定到 `DynamicResource`，调用 `SetValue` 更新。
3. 渲染层：资源变化会触发 UI 元素重新渲染。

## 6. 与其他模块的接口
- **ui**：
  - `FrameworkElement::Resources()` 返回 `ResourceDictionary&`。
  - `Style` 与 `Setter` 作为 UI 属性应用机制的一部分。
- **binding**：
  - `Setter` 值可能为 `Binding` 或 `ResourceReference`，需要在绑定激活时解析。
- **app**：
  - `Application::Resources()` 与 `ThemeManager` 协作，提供全局资源访问。
- **render**：
  - 颜色、Brush、字体等资源最终转换为渲染层对象。

## 7. 测试计划
- 资源查找顺序：控件本地 → 父 → 应用 → 主题。
- DynamicResource 更新：主题切换后控件属性自动刷新。
- Style 应用：根据状态触发 Setter，验证属性值变更。
- 文件加载：解析 JSON/YAML 生成正确的资源字典。

## 8. 实现路线
1. 实现 `ResourceDictionary` 与基础资源类型 (Color, Brush)。
2. 加入 `Theme` 与 `ThemeManager`，支持注册与切换。
3. 将 `ResourceDictionary` 集成到 `FrameworkElement` 与 `Application`。
4. 实现 `DynamicResource` 引用与主题更新通知。
5. 引入 `Style`、`Setter`、`StyleTrigger`，与 UI 控件整合。
6. 扩展文件加载、主题包管理等高级功能。

---
> `fk::theme` 模块与 UI/Binding 紧密耦合，需要在实现时同步定义资源解析与属性应用流程，以确保主题切换能够实时反映到界面上。