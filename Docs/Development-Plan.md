# F__K_UI 框架开发计划

**制定日期**：2025-11-11  
**当前完成度**：72%  
**目标完成度**：95%+

---

## 📊 当前状态总结

### 已完成（23个类，64%）
- ✅ 依赖属性系统完整
- ✅ 数据绑定系统完整
- ✅ 视觉树管理完整
- ✅ 布局系统完整
- ✅ 基础控件完整（Button, TextBox, TextBlock, Border, Grid）
- ✅ 渲染管线完整（OpenGL 后端）

### 需要完善（3个类，8%）
- ⚠️ ItemsControl (60%) - 缺少容器生成
- ⚠️ InteractionTracker (60%) - 功能分散
- ⚠️ VisualTreeManager (70%) - 功能分散

### 缺失（10个类，28%）
- ❌ Image 控件
- ❌ Shape 基类
- ❌ InputManager/FocusManager
- ❌ Style/Template 系统（7个类）

---

## 🎯 开发阶段规划

### 阶段 1：基础补全（目标 85%）⭐ 优先级：最高

**时间估计**：2-3 周  
**目标**：补全常用功能，达到生产可用

#### 1.1 实现 Image 控件 ✨ 最高优先级
**完成度提升**：+3%  
**难度**：⭐⭐ 简单  
**预估时间**：1-2 天

**实现内容**：
```cpp
// include/fk/ui/Image.h
class Image : public FrameworkElement<Image> {
public:
    // 依赖属性
    static const DependencyProperty& SourceProperty();
    static const DependencyProperty& StretchProperty();
    
    // 图像源
    std::string GetSource() const;
    void SetSource(const std::string& path);
    Image* Source(const std::string& path);
    
    // 拉伸模式
    Stretch GetStretch() const;
    void SetStretch(Stretch mode);
    Image* Stretch(Stretch mode);
    
protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;
    void OnRender(RenderContext& context) override;
};
```

**关键点**：
- 支持常见图像格式（PNG, JPEG）
- 实现 Stretch 模式（None, Fill, Uniform, UniformToFill）
- 集成到现有渲染管线

**依赖**：
- 图像加载库（stb_image 或类似）
- RenderBackend 纹理支持

---

#### 1.2 实现 InputManager ⭐ 高优先级
**完成度提升**：+2%  
**难度**：⭐⭐⭐ 中等  
**预估时间**：2-3 天

**实现内容**：
```cpp
// include/fk/ui/InputManager.h
class InputManager {
public:
    static InputManager& GetInstance();
    
    // 输入处理
    void ProcessMouseMove(const Point& position);
    void ProcessMouseButton(MouseButton button, bool down);
    void ProcessKeyboard(Key key, bool down);
    
    // 命中测试
    UIElement* HitTest(Visual* root, const Point& point);
    
    // 捕获管理
    void CaptureMouse(UIElement* element);
    void ReleaseMouseCapture();
    UIElement* GetMouseCapture() const;
    
    // 当前状态
    Point GetMousePosition() const;
    bool IsMouseButtonDown(MouseButton button) const;
    bool IsKeyDown(Key key) const;
};
```

**关键点**：
- 全局单例管理
- 鼠标捕获机制
- 命中测试算法
- 事件冒泡/隧道

**集成点**：
- Application 主循环
- Window 消息处理
- UIElement 事件分发

---

#### 1.3 实现 FocusManager ⭐ 高优先级
**完成度提升**：+2%  
**难度**：⭐⭐⭐ 中等  
**预估时间**：2-3 天

**实现内容**：
```cpp
// include/fk/ui/FocusManager.h
class FocusManager {
public:
    static FocusManager& GetInstance();
    
    // 焦点控制
    bool SetFocus(UIElement* element);
    UIElement* GetFocusedElement() const;
    void ClearFocus();
    
    // 焦点导航
    bool MoveFocus(FocusNavigationDirection direction);
    UIElement* PredictFocus(FocusNavigationDirection direction);
    
    // 焦点作用域
    void SetFocusScope(UIElement* scope);
    UIElement* GetFocusScope(UIElement* element);
    
    // 事件
    core::Event<UIElement*, UIElement*> FocusChanged;
};
```

**关键点**：
- Tab 键导航
- 方向键导航（↑↓←→）
- 焦点作用域（模态对话框）
- 焦点可见性指示器

**关联修改**：
- UIElement 添加 IsFocusable 属性
- Control 添加焦点视觉效果

---

#### 1.4 完善 ItemsControl ⭐ 中优先级
**完成度提升**：+1%  
**难度**：⭐⭐⭐⭐ 较难  
**预估时间**：3-4 天

**实现内容**：
```cpp
// 完善 include/fk/ui/ItemsControl.h
class ItemsControl {
    // 已有基础功能...
    
    // 新增：容器生成
    UIElement* GetContainerForItem(const std::any& item);
    void PrepareContainerForItem(UIElement* container, const std::any& item);
    void ClearContainerForItem(UIElement* container, const std::any& item);
    
    // 新增：虚拟化准备（可选）
    bool IsVirtualizing() const;
    void SetIsVirtualizing(bool value);
    
protected:
    // 容器生成器（简化版）
    virtual UIElement* CreateContainer(const std::any& item);
    virtual void ApplyItemTemplate(UIElement* container, const std::any& item);
};
```

**关键点**：
- 自动创建容器（ContentControl）
- 应用 ItemTemplate
- 数据绑定到容器
- 支持自定义容器样式

**测试场景**：
- ListBox 实现（继承 ItemsControl）
- 数据绑定到 vector<string>
- 选择功能

---

### 阶段 2：样式系统（目标 90%）⭐ 优先级：中等

**时间估计**：3-4 周  
**目标**：实现 WPF 风格的样式和模板系统

#### 2.1 实现 Style 类
**完成度提升**：+1%  
**难度**：⭐⭐⭐ 中等  
**预估时间**：3-4 天

**实现内容**：
```cpp
// include/fk/ui/Style.h
class Style {
public:
    // 目标类型
    const std::type_info* GetTargetType() const;
    void SetTargetType(const std::type_info* type);
    
    // 属性设置器集合
    void AddSetter(const DependencyProperty& property, const std::any& value);
    void RemoveSetter(const DependencyProperty& property);
    std::vector<Setter> GetSetters() const;
    
    // 应用样式
    void Apply(DependencyObject* target);
    void Unapply(DependencyObject* target);
    
    // 基础样式（继承）
    Style* GetBasedOn() const;
    void SetBasedOn(Style* baseStyle);
};
```

**关键点**：
- Setter 存储和应用
- 样式继承（BasedOn）
- 类型检查
- 值优先级（Local > Style > Default）

---

#### 2.2 实现 ControlTemplate
**完成度提升**：+1%  
**难度**：⭐⭐⭐⭐ 较难  
**预估时间**：4-5 天

**实现内容**：
```cpp
// include/fk/ui/ControlTemplate.h
class ControlTemplate : public FrameworkTemplate {
public:
    // 目标类型
    const std::type_info* GetTargetType() const;
    
    // 模板内容工厂
    using TemplateFactory = std::function<UIElement*()>;
    void SetVisualTreeFactory(TemplateFactory factory);
    
    // 实例化模板
    UIElement* LoadContent();
    
    // 触发器（可选，简化版）
    void AddTrigger(const Trigger& trigger);
};
```

**关键点**：
- 视觉树工厂模式
- 命名部件查找
- 模板绑定（TemplateBinding）
- Trigger 系统（简化）

---

#### 2.3 实现 DataTemplate
**完成度提升**：+1%  
**难度**：⭐⭐⭐ 中等  
**预估时间**：2-3 天

**实现内容**：
```cpp
// include/fk/ui/DataTemplate.h
class DataTemplate : public FrameworkTemplate {
public:
    // 数据类型
    const std::type_info* GetDataType() const;
    
    // 模板内容
    using TemplateFactory = std::function<UIElement*()>;
    void SetVisualTreeFactory(TemplateFactory factory);
    
    // 实例化
    UIElement* LoadContent();
};
```

**使用场景**：
- ItemsControl.ItemTemplate
- ContentControl.ContentTemplate
- 列表项自定义显示

---

### 阶段 3：图形增强（目标 93%）⭐ 优先级：低

**时间估计**：2-3 周  
**目标**：支持矢量图形

#### 3.1 实现 Shape 基类
**完成度提升**：+1%  
**难度**：⭐⭐⭐⭐ 较难  
**预估时间**：4-5 天

**实现内容**：
```cpp
// include/fk/ui/Shape.h
template<typename Derived>
class Shape : public FrameworkElement<Derived> {
public:
    // 填充
    Brush* GetFill() const;
    void SetFill(Brush* brush);
    Derived* Fill(Brush* brush);
    
    // 描边
    Brush* GetStroke() const;
    void SetStroke(Brush* brush);
    Derived* Stroke(Brush* brush);
    
    float GetStrokeThickness() const;
    void SetStrokeThickness(float thickness);
    
    // 几何数据（纯虚）
    virtual Geometry* GetGeometry() const = 0;
    
protected:
    void OnRender(RenderContext& context) override;
};
```

**派生类示例**：
```cpp
// Rectangle.h
class Rectangle : public Shape<Rectangle> {
    Geometry* GetGeometry() const override;
};

// Ellipse.h
class Ellipse : public Shape<Ellipse> {
    Geometry* GetGeometry() const override;
};

// Path.h
class Path : public Shape<Path> {
    void SetData(Geometry* geometry);
    Geometry* GetGeometry() const override;
};
```

---

### 阶段 4：性能优化（目标 95%+）⭐ 优先级：最低

**时间估计**：长期迭代

#### 4.1 缓存系统
- MaterialPool - 材质缓存
- GeometryCache - 几何缓存
- 纹理资源池

#### 4.2 高级功能
- VisualStateManager - 状态动画
- InteractionTracker 独立类
- VisualTreeManager 工具集

---

## 🛠️ 技术债务处理

### 代码质量
- [ ] 添加单元测试覆盖
- [ ] 统一错误处理
- [ ] 完善日志系统
- [ ] 内存泄漏检测

### 文档完善
- [ ] API 文档生成（Doxygen）
- [ ] 示例程序集合
- [ ] 教程和最佳实践
- [ ] 架构设计文档更新

### 性能优化
- [ ] 布局缓存优化
- [ ] 渲染批处理
- [ ] 脏区域裁剪
- [ ] 多线程渲染（可选）

---

## 📅 里程碑时间表

| 里程碑 | 目标完成度 | 预估时间 | 主要内容 |
|--------|-----------|---------|---------|
| **M1: 基础补全** | 85% | 2-3 周 | Image, InputManager, FocusManager, ItemsControl |
| **M2: 样式系统** | 90% | 3-4 周 | Style, ControlTemplate, DataTemplate |
| **M3: 图形增强** | 93% | 2-3 周 | Shape, Rectangle, Ellipse, Path |
| **M4: 性能优化** | 95%+ | 持续迭代 | 缓存、状态管理、高级功能 |

---

## 🎯 成功标准

### M1 完成标准（85%）
- ✅ Image 控件可显示 PNG/JPEG
- ✅ 键盘/鼠标输入正常工作
- ✅ Tab 键焦点导航可用
- ✅ ItemsControl 可绑定数据集合
- ✅ 能构建一个完整的表单应用

### M2 完成标准（90%）
- ✅ Style 可设置控件外观
- ✅ ControlTemplate 可自定义控件
- ✅ DataTemplate 可定义数据显示
- ✅ 主题切换功能
- ✅ 能构建具有统一风格的应用

### M3 完成标准（93%）
- ✅ 矢量图形正常渲染
- ✅ 支持基本形状（矩形、圆、线）
- ✅ 支持复杂路径
- ✅ 能构建图表应用

### M4 完成标准（95%+）
- ✅ 渲染性能优秀（60fps）
- ✅ 内存占用合理
- ✅ 大型应用稳定运行
- ✅ 完整的文档和示例

---

## 💡 实施建议

### 开发顺序
1. **先易后难** - 从 Image 开始，建立信心
2. **逐步推进** - 每完成一个功能就测试
3. **及时集成** - 避免大规模重构
4. **持续验证** - 更新 Implementation-Status.md

### 质量保证
- 每个功能配套示例程序
- 边写边测，不积累问题
- 代码审查和重构
- 性能基准测试

### 风险控制
- **技术风险** - Shape 渲染复杂度高 → 简化设计
- **时间风险** - Template 系统工作量大 → 分阶段实现
- **兼容风险** - API 变更影响现有代码 → 保持向后兼容

---

## 📈 预期成果

### 完成 M1 后（85%）
- **可用性**：生产可用，适合中小型应用
- **功能性**：覆盖常见 UI 需求
- **稳定性**：核心功能稳定

### 完成 M2 后（90%）
- **可用性**：企业级可用
- **功能性**：支持复杂定制
- **美观性**：统一主题风格

### 完成 M3 后（93%）
- **可用性**：专业级框架
- **功能性**：接近 WPF
- **表现力**：丰富的视觉效果

### 完成 M4 后（95%+）
- **性能**：商业级性能
- **完整性**：功能全面
- **成熟度**：可对外发布

---

## 🔄 持续改进

- 根据实际使用反馈调整优先级
- 定期更新开发计划
- 保持与社区沟通
- 参考其他框架的最佳实践

---

**当前焦点**：阶段 1 - 实现 Image 控件 ✨

**下一步行动**：创建 Image.h 和 Image.cpp，实现基础图像显示功能
