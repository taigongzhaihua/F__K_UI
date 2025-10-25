# F__K_UI 命名规范

**版本**: 1.0  
**生效日期**: 2025-10-26

---

## 一、属性访问器命名规则

### 1.1 Getter 方法

| 返回类型 | 命名规则 | 示例 |
|---------|---------|------|
| 普通属性 | `Get{PropertyName}()` | `GetWidth()`, `GetOpacity()`, `GetVisibility()` |
| 布尔状态 | `Is{PropertyName}()` | `IsEnabled()`, `IsVisible()`, `IsLoaded()` |
| 布尔存在性 | `Has{PropertyName}()` | `HasContent()`, `HasChildren()`, `HasDataContext()` |
| 容器访问 | `Get{CollectionName}()` | `GetChildren()`, `GetLogicalChildren()` |

### 1.2 Setter 方法

**规则**: 所有 Setter 统一使用 `Set{PropertyName}()` 格式

```cpp
void SetWidth(float value);
void SetOpacity(float value);
void SetVisibility(Visibility value);
void SetIsEnabled(bool value);  // 即使 Getter 是 IsEnabled()
```

### 1.3 流式 API (方法重载)

**规则**: 对于支持流式编程的类,使用无参/有参方法重载

```cpp
// 无参版本 - Getter (返回值)
std::string Title() const;
int Width() const;

// 有参版本 - Setter (返回 Ptr 支持链式调用)
Ptr Title(const std::string& title);
Ptr Width(int w);

// 使用示例
auto window = std::make_shared<Window>();
window->Title("My Window")      // Setter,返回 Ptr
      ->Width(800)              // Setter,返回 Ptr
      ->Height(600);            // Setter,返回 Ptr

std::cout << window->Title();   // Getter,返回 string
std::cout << window->Width();   // Getter,返回 int
```

**适用场景**:
- 顶层对象 (Window, Application)
- 构建器模式对象
- 需要链式配置的类

**不适用场景**:
- 普通UI元素 (UIElement, FrameworkElement) - 使用 Get/Set 前缀
- 只读属性 - 只提供 Getter

---

## 二、子元素管理命名

### 2.1 单子元素容器 (Decorator, ContentControl)

```cpp
// Setter
void SetChild(std::shared_ptr<UIElement> child);
void SetContent(std::shared_ptr<UIElement> content);  // ContentControl 特有

// Getter
std::shared_ptr<UIElement> GetChild() const;
std::shared_ptr<UIElement> GetContent() const;

// 检查
bool HasChild() const;
bool HasContent() const;

// 清除
void ClearChild();
void ClearContent();
```

**注意**: 这些类不使用流式API,因为它们是普通UI元素,不是顶层对象。

### 2.2 多子元素容器 (Panel)

```cpp
// 添加/移除
void AddChild(std::shared_ptr<UIElement> child);
bool RemoveChild(UIElement* child);  // 使用裸指针比较

// Getter
std::span<const std::shared_ptr<UIElement>> GetChildren() const;
std::size_t GetChildCount() const;

// 检查
bool HasChildren() const;

// 清除
void ClearChildren();
```

---

## 三、事件命名规则

### 3.1 状态变化事件

**格式**: 过去时,表示已发生

```cpp
Event<> Loaded;
Event<> Initialized;
Event<> Closed;
Event<> Opened;
```

### 3.2 属性变化事件

**格式**: `{PropertyName}Changed`

```cpp
Event<UIElement*, UIElement*> ContentChanged;
Event<Visibility, Visibility> VisibilityChanged;
Event<float, float> OpacityChanged;
```

### 3.3 操作事件

**格式**: `{Action}{Tense}` + 可选对象

```cpp
Event<UIElement&> MeasureInvalidated;
Event<UIElement&> ArrangeInvalidated;
Event<DependencyObject&> AttachedToLogicalTree;
Event<DependencyObject&> DetachedFromLogicalTree;
```

---

## 四、类型命名规则

### 4.1 类名

**规则**: PascalCase,名词或名词短语

```cpp
class UIElement { };
class FrameworkElement { };
class DependencyObject { };
class RenderHost { };
```

### 4.2 枚举

**规则**: PascalCase 类名 + PascalCase 枚举值

```cpp
enum class Visibility {
    Visible,    // 不用 VISIBLE 或 kVisible
    Hidden,
    Collapsed
};

enum class ThreadCheckMode {
    Disabled,
    WarnOnly,
    ThrowException
};
```

### 4.3 结构体

**规则**: 简单数据类型使用 PascalCase

```cpp
struct Size {
    float width{0};
    float height{0};
};

struct Rect {
    float x{0}, y{0}, width{0}, height{0};
};
```

---

## 五、成员变量命名

### 5.1 私有成员

**规则**: camelCase + `_` 后缀

```cpp
private:
    float opacity_{1.0f};
    bool isEnabled_{true};
    std::vector<std::shared_ptr<UIElement>> children_;
    std::shared_ptr<Dispatcher> dispatcher_;
```

### 5.2 保护成员

**规则**: 同私有成员,camelCase + `_` 后缀

```cpp
protected:
    Size desiredSize_;
    bool isMeasureValid_{false};
```

---

## 六、指针使用规范

### 6.1 所有权

| 场景 | 类型 | 说明 |
|------|------|------|
| 拥有子对象 | `std::shared_ptr<T>` | 共享所有权 |
| 传递所有权 | `std::shared_ptr<T>` | 参数/返回值 |
| 观察父对象 | `T*` | 不拥有,仅观察 |
| 临时访问 | `T*` 或 `const T*` | 函数参数 |

### 6.2 示例

```cpp
// ✅ 正确
void AddChild(std::shared_ptr<UIElement> child);  // 接收所有权
UIElement* GetLogicalParent() const;              // 观察,不拥有
bool RemoveChild(UIElement* child);               // 比较用裸指针

// ❌ 错误
void AddChild(UIElement* child);                  // 所有权不明确
std::shared_ptr<UIElement> GetLogicalParent();    // 不应共享父对象
```

---

## 七、命名空间规则

### 7.1 顶层命名空间

```cpp
namespace fk { }              // 框架根命名空间
namespace fk::ui { }          // UI元素
namespace fk::core { }        // 核心功能
namespace fk::binding { }     // 数据绑定
namespace fk::render { }      // 渲染系统
namespace fk::app { }         // 应用程序框架
```

### 7.2 导出到根命名空间

**规则**: 常用类型可以用 `using` 导出到 `fk` 命名空间

```cpp
namespace fk::ui {
    class UIElement { };
}

namespace fk {
    using ui::UIElement;  // 允许 fk::UIElement
}
```

---

## 八、重构检查清单

重构现有代码时,按以下顺序检查:

- [ ] Getter 是否使用正确前缀 (Get/Is/Has)
- [ ] Setter 是否都用 Set 前缀
- [ ] 布尔方法是否用 Is/Has 前缀
- [ ] 子元素管理是否统一命名
- [ ] 指针类型是否符合所有权规范
- [ ] 事件名称是否符合格式
- [ ] 成员变量是否有 `_` 后缀

---

## 九、迁移指南

### 9.1 常见重命名映射

| 旧名称 | 新名称 | 类 | 说明 |
|--------|--------|-----|------|
| `Visibility()` | `GetVisibility()` | UIElement | 普通元素用Get前缀 |
| `Opacity()` | `GetOpacity()` | UIElement | 普通元素用Get前缀 |
| `Children()` | `GetChildren()` | Panel | 普通元素用Get前缀 |
| `ChildCount()` | `GetChildCount()` | Panel | 普通元素用Get前缀 |
| `Title()` (getter) | `Title()` | Window | ⚠️ 保持不变 - 流式API重载 |
| `Width()` (getter) | `Width()` | Window | ⚠️ 保持不变 - 流式API重载 |
| `Height()` (getter) | `Height()` | Window | ⚠️ 保持不变 - 流式API重载 |

**重要**: Window 类使用方法重载实现流式API,不使用 Get/Set 前缀!
| `Width(int)` (setter) | `SetWidth(int)` | Window |

### 9.2 不变的名称

以下名称已符合规范,保持不变:

**布尔值 Getter** (使用 Is/Has 前缀):
- `IsEnabled()` ✅
- `IsMeasureValid()` ✅
- `IsLoaded()` ✅
- `HasThreadAccess()` ✅

**普通 Getter** (使用 Get 前缀):
- `GetWidth()` ✅ (FrameworkElement)
- `GetChild()` ✅ (Decorator)
- `GetContent()` ✅ (Control)

**流式API** (方法重载,不用前缀):
- `Title()` / `Title(string)` ✅ (Window)
- `Width()` / `Width(int)` ✅ (Window)
- `Height()` / `Height(int)` ✅ (Window)

---

**制定人**: GitHub Copilot  
**审批日期**: 2025-10-26  
**状态**: 已生效
