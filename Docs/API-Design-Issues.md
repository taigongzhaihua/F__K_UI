# F__K_UI API 设计问题清单

**日期**: 2025年10月25日  
**审查范围**: 所有已实现的类和API

---

## 一、命名不一致问题 (Naming Inconsistency)

### 1.1 Getter 方法命名风格不统一

**严重程度**: 🔴 高

**问题描述**:  
框架中同时存在三种 Getter 命名风格,导致API不统一:

| 风格 | 示例 | 使用位置 |
|------|------|----------|
| `Get-` 前缀 | `GetWidth()`, `GetHeight()`, `GetMargin()` | `FrameworkElement` |
| 无前缀 (属性名) | `Opacity()`, `Visibility()`, `IsEnabled()` | `UIElement` |
| `Get-` 前缀 | `GetContent()`, `GetChild()` | `Control`, `Decorator` |

**具体案例**:

```cpp
// FrameworkElement - 使用 Get 前缀
float GetWidth() const;
float GetHeight() const;
const Thickness& GetMargin() const;
HorizontalAlignment GetHorizontalAlignment() const;

// UIElement - 不使用 Get 前缀
Visibility Visibility() const;
bool IsEnabled() const;
float Opacity() const;

// Panel - 混合风格
std::span<const std::shared_ptr<UIElement>> Children() const;  // 无前缀
std::size_t ChildCount() const;                                 // 无前缀

// StackPanel - 使用 Get 前缀
Orientation GetOrientation() const;

// Decorator - 使用 Get 前缀
std::shared_ptr<UIElement> GetChild() const;
```

**影响**:
- 用户需要记忆不同类的不同命名风格
- 代码可读性差,不符合"最少惊讶原则"
- 降低API学习曲线

**建议方案**:
```cpp
// 方案A: 统一使用 Get 前缀 (推荐,与C++ STL一致)
float GetWidth() const;
float GetOpacity() const;
Visibility GetVisibility() const;
bool GetIsEnabled() const;  // 或 IsEnabled()
Orientation GetOrientation() const;

// 方案B: 对布尔值使用 Is/Has,其他使用 Get
bool IsEnabled() const;
bool HasContent() const;
float GetOpacity() const;
Visibility GetVisibility() const;
```

---

### 1.2 流式API与普通Setter混用

**严重程度**: 🟡 中

**问题描述**:  
`Window` 类同时提供流式API和普通Setter,但其他类只有普通Setter。

```cpp
// Window - 流式API (返回 Ptr)
Ptr Title(const std::string& t);
Ptr Width(int w);
Ptr Height(int h);

// Window - 也有Getter重载
std::string Title() const;
int Width() const;
int Height() const;

// UIElement - 只有普通 Setter
void SetVisibility(Visibility visibility);
void SetIsEnabled(bool enabled);
void SetOpacity(float value);

// View<T> - 提供流式API包装
Ptr Width(float value) { SetWidth(value); return Self(); }
Ptr Height(float value) { SetHeight(value); return Self(); }
```

**影响**:
- Window使用 getter/setter 重载,其他类使用 Set前缀
- View模板提供流式API,但只对继承的类可用
- 混合风格导致用户困惑

**建议方案**:
```cpp
// 统一方案: 分离 Getter/Setter,提供流式包装类
class Window {
public:
    void SetTitle(const std::string& title);
    std::string GetTitle() const;
    
    void SetWidth(int w);
    int GetWidth() const;
};

// 通过 View 模板提供流式API
template<typename Derived>
class WindowView : public View<Derived, Window> {
    Ptr Title(const std::string& t) { SetTitle(t); return Self(); }
    Ptr Width(int w) { SetWidth(w); return Self(); }
};
```

---

### 1.3 Bool属性命名不一致

**严重程度**: 🟡 中

**问题描述**:  
布尔值属性的命名不统一,同时存在 `Is-`、`Has-` 和无前缀三种风格。

```cpp
// Is- 前缀
bool IsEnabled() const;
bool IsMeasureValid() const;
bool IsArrangeValid() const;
bool IsInitialized() const;
bool IsLoaded() const;
bool IsVisible() const;  // Window
bool IsAttached() const;  // DependencyProperty

// Has- 前缀
bool HasThreadAccess() const;
bool HasDataContext() const;
bool HasAppliedTemplate() const;
bool HasRenderContent() const;
bool HasDirtyElements() const;

// 无前缀
bool visible_;  // Window 内部成员,但公共API是 IsVisible()
```

**影响**:
- 用户难以预测方法名
- Is/Has 区分不明确

**建议规则**:
- `Is-`: 表示状态 (IsEnabled, IsVisible, IsLoaded)
- `Has-`: 表示拥有/存在 (HasContent, HasChildren, HasDataContext)
- 对于 Valid 后缀: `IsMeasureValid()` ✅

---

## 二、重复实现问题 (Code Duplication)

### 2.1 DependencyObject 双重实现

**严重程度**: 🔴 高

**问题描述**:  
存在两个 `DependencyObject` 类:
1. `fk::binding::DependencyObject` - 依赖属性系统
2. `fk::ui::DependencyObject` - UI依赖对象,继承自 `DispatcherObject` 和 `binding::DependencyObject`

```cpp
// fk/binding/DependencyObject.h
namespace fk::binding {
class DependencyObject {
    // 属性存储、绑定、逻辑树管理
    void SetValue(const DependencyProperty& property, std::any value);
    const std::any& GetValue(const DependencyProperty& property) const;
    void SetLogicalParent(DependencyObject* parent);
    void AddLogicalChild(DependencyObject* child);
};
}

// fk/ui/DependencyObject.h
namespace fk::ui {
class DependencyObject : 
    public DispatcherObject, 
    public fk::binding::DependencyObject {
    // 添加 Dispatcher 关联和逻辑树附加
    void AttachToLogicalTree(std::shared_ptr<core::Dispatcher> dispatcher);
    void AddLogicalChild(DependencyObject* child);  // ⚠️ 重复声明
};
}
```

**问题**:
- `AddLogicalChild()` / `RemoveLogicalChild()` 在两个类中都声明
- `ui::DependencyObject` 使用 `using` 引入基类方法,但又重新声明了部分
- 逻辑树管理在两个层次重复实现

**影响**:
- 混淆的类职责边界
- 潜在的多重继承钻石问题
- 维护复杂度高

**建议方案**:
```cpp
// 方案A: 合并为单一 DependencyObject
namespace fk::ui {
class DependencyObject : public DispatcherObject {
    // 包含所有属性、绑定、逻辑树功能
    PropertyStore propertyStore_;
    DependencyObject* logicalParent_{nullptr};
    std::vector<DependencyObject*> logicalChildren_{};
};
}

// 方案B: 清晰的层次分离
// binding::DependencyObject - 纯数据绑定(无UI依赖)
// ui::DependencyObject - 添加Dispatcher + 逻辑树附加
// 但移除重复的方法声明,只用 using
```

---

### 2.2 Child管理的重复模式

**严重程度**: 🟡 中

**问题描述**:  
多个类都实现了子元素管理,但实现方式不一致。

```cpp
// Panel - 多子元素
void AddChild(std::shared_ptr<UIElement> child);
bool RemoveChild(UIElement* child);
void ClearChildren();
std::span<const std::shared_ptr<UIElement>> Children() const;

// Decorator - 单子元素
void SetChild(std::shared_ptr<UIElement> child);
std::shared_ptr<UIElement> GetChild() const;
void ClearChild();

// Control - 通过 Content 属性
void SetContent(std::shared_ptr<UIElement> content);
std::shared_ptr<UIElement> GetContent() const;
void ClearContent();

// DependencyObject - 逻辑子元素
void AddLogicalChild(DependencyObject* child);
void RemoveLogicalChild(DependencyObject* child);
const std::vector<DependencyObject*>& GetLogicalChildren() const;
```

**问题**:
- 命名不统一: `Child` vs `Children` vs `Content`
- 参数不一致: `shared_ptr` vs 裸指针
- Clear方法: `ClearChildren()` vs `ClearChild()` vs `ClearContent()`

**建议规则**:
```cpp
// 单子元素容器统一使用 Child
void SetChild(std::shared_ptr<UIElement> child);
std::shared_ptr<UIElement> GetChild() const;
void ClearChild();

// 多子元素容器统一使用 Children
void AddChild(std::shared_ptr<UIElement> child);
bool RemoveChild(const UIElement* child);  // const指针
void ClearChildren();
std::span<const std::shared_ptr<UIElement>> GetChildren() const;

// Content 专用于 ContentControl
void SetContent(std::shared_ptr<UIElement> content);
std::shared_ptr<UIElement> GetContent() const;
void ClearContent();
```

---

### 2.3 DataContext 的多份声明

**严重程度**: 🟡 中

**问题描述**:  
DataContext 在多个层次声明:

```cpp
// binding::DependencyObject
void SetDataContext(std::any value);
template<typename T> void SetDataContext(T&& value);
void ClearDataContext();
const std::any& GetDataContext() const noexcept;

// FrameworkElement (继承自 ui::DependencyObject)
void SetDataContext(std::any value);           // ⚠️ 重复声明
template<typename T> void SetDataContext(T&& value);  // ⚠️ 重复
void ClearDataContext();                       // ⚠️ 重复
const std::any& GetDataContext() const noexcept;     // ⚠️ 重复
```

**原因**: `FrameworkElement` 可能想处理DataContext属性变化通知,但这导致重复。

**建议**: 使用属性变化回调而不是重新声明方法。

---

## 三、API设计不一致问题

### 3.1 指针类型使用不统一

**严重程度**: 🟡 中

**问题描述**:

| 场景 | 使用类型 | 示例 |
|------|----------|------|
| 子元素添加 | `shared_ptr` | `Panel::AddChild(shared_ptr<UIElement>)` |
| 子元素移除 | 裸指针 | `Panel::RemoveChild(UIElement*)` |
| 父元素获取 | 裸指针 | `GetLogicalParent()` 返回 `DependencyObject*` |
| 子元素列表 | `vector<shared_ptr>` | `GetLogicalChildren()` 返回 `vector<DependencyObject*>` ⚠️ |
| 查找元素 | 裸指针 | `FindElementByName()` 返回 `DependencyObject*` |

**不一致点**:
```cpp
// 添加用 shared_ptr,移除用裸指针
void AddChild(std::shared_ptr<UIElement> child);
bool RemoveChild(UIElement* child);  // 为何不是 const shared_ptr&?

// 逻辑树返回裸指针
DependencyObject* GetLogicalParent() const;
const std::vector<DependencyObject*>& GetLogicalChildren() const;  // 裸指针向量
```

**建议**:
```cpp
// 统一使用 shared_ptr 用于所有权转移
void AddChild(std::shared_ptr<UIElement> child);
bool RemoveChild(const std::shared_ptr<UIElement>& child);

// 观察者关系用裸指针
UIElement* GetLogicalParent() const;  // 不拥有
std::span<UIElement* const> GetLogicalChildren() const;  // 观察视图
```

---

### 3.2 Size和Rect类型定义重复

**严重程度**: 🟢 低

**问题描述**:  
`Size` 和 `Rect` 定义在 `UIElement.h` 中,但这些是通用几何类型,应该独立。

```cpp
// 当前: 在 fk/ui/UIElement.h
namespace fk::ui {
struct Size { float width, height; };
struct Rect { float x, y, width, height; };
}
```

**建议**:
```cpp
// 移到独立头文件: fk/ui/Geometry.h 或 fk/core/Geometry.h
namespace fk {
struct Size { float width{0}, height{0}; };
struct Rect { float x{0}, y{0}, width{0}, height{0}; };
struct Point { float x{0}, y{0}; };
}
```

---

### 3.3 事件命名不一致

**严重程度**: 🟢 低

**问题描述**:

```cpp
// FrameworkElement - 过去时
Event<FrameworkElement&> Initialized;
Event<FrameworkElement&> Loaded;

// DependencyObject - 过去时 + To + 对象
Event<DependencyObject&> AttachedToLogicalTree;
Event<DependencyObject&> DetachedFromLogicalTree;

// UIElement - 过去时 + 分词
Event<UIElement&> MeasureInvalidated;
Event<UIElement&> ArrangeInvalidated;

// Window - 过去时
Event<> Closed;
Event<> Opened;
Event<int, int> Resized;

// ContentControl - 过去时 + 参数
Event<UIElement*, UIElement*> ContentChanged;
```

**建议统一**:
- 状态事件: 过去时 (Loaded, Closed)
- 操作事件: -ing (Loading, Closing) 或 -ed (Loaded, Closed)
- 属性变化: XxxChanged (ContentChanged, VisibilityChanged)

---

## 四、缺失的API对称性

### 4.1 缺少对应的Getter

**严重程度**: 🟡 中

**问题**:

```cpp
// ✅ 有对应 Getter
void SetVisibility(Visibility v);
Visibility Visibility() const;

// ❌ 缺少 Getter
void SetOrientation(Orientation o);
// 缺少: Orientation GetOrientation() const;  // ✅ 实际上有

// ✅ 检查后发现大部分都有
```

**实际情况**: 大部分Setter都有对应Getter,此问题较小。

---

### 4.2 缺少 HasXxx 检查方法

**严重程度**: 🟢 低

**建议添加**:
```cpp
// Panel
bool HasChildren() const { return !children_.empty(); }

// Decorator
bool HasChild() const { return child_ != nullptr; }

// ContentControl
bool HasContent() const { return GetContent() != nullptr; }
```

---

## 五、线程安全API设计

### 5.1 ThreadingConfig 的 AutoDispatch 模式未实现

**严重程度**: 🟡 中

**问题描述**:  
`ThreadCheckMode::AutoDispatch` 已定义但未实现:

```cpp
enum class ThreadCheckMode {
    Disabled,
    WarnOnly,
    ThrowException,
    AutoDispatch  // ⚠️ 未实现,当前只是fallback到ThrowException
};

// src/ui/UIElement.cpp
case ThreadCheckMode::AutoDispatch:
    // TODO: 实现自动调度到正确线程
    throw std::runtime_error("Cross-thread operation not allowed on UIElement");
```

**影响**: 功能不完整,文档与实现不符。

**建议**: 实现或移除此模式。

---

## 六、总结与优先级建议

### 🔴 高优先级 (必须修复)

1. **统一 Getter 命名风格** (1.1)
   - 影响: 全局API一致性
   - 工作量: 大 (需要重命名大量方法)
   - 建议: 使用 `Get-` 前缀 + `Is-`/`Has-` 布尔前缀

2. **重构 DependencyObject 双重实现** (2.1)
   - 影响: 架构清晰度
   - 工作量: 中
   - 建议: 明确职责边界,移除重复声明

### 🟡 中优先级 (建议修复)

3. **统一流式API设计** (1.2)
   - 工作量: 中
   - 建议: 通过 View 模板统一提供

4. **统一指针使用规范** (3.1)
   - 工作量: 中
   - 建议: 所有权用 shared_ptr,观察用裸指针

5. **实现或移除 AutoDispatch** (5.1)
   - 工作量: 大 (实现) 或 小 (移除)

### 🟢 低优先级 (可选优化)

6. **独立几何类型** (3.2)
7. **统一事件命名** (3.3)
8. **添加 HasXxx 便利方法** (4.2)

---

## 七、重构路线图建议

### 阶段1: API命名统一 (1-2周)
- [ ] 制定命名规范文档
- [ ] 重命名所有 Getter 方法
- [ ] 更新所有调用代码
- [ ] 更新示例和文档

### 阶段2: 架构清理 (1周)
- [ ] 重构 DependencyObject 层次
- [ ] 统一子元素管理API
- [ ] 清理重复声明

### 阶段3: 指针规范化 (1周)
- [ ] 统一 shared_ptr/裸指针使用
- [ ] 更新所有接口签名
- [ ] 验证内存安全性

### 阶段4: 完善线程安全 (可选)
- [ ] 实现 AutoDispatch 或移除
- [ ] 完善 ThreadingConfig 文档
- [ ] 添加更多线程安全测试

---

**生成时间**: 2025-10-25  
**审查人**: GitHub Copilot  
**版本**: v1.0
