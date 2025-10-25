# API一致性检查报告

生成时间: 2025-10-26

## 类继承层次结构

```
UIElement (基类)
  ├─ FrameworkElement
  │   ├─ Panel (基类,不使用View)
  │   │   └─ StackPanel : View<StackPanel, Panel> ✅ 有流式API
  │   │
  │   ├─ Decorator ❌ 直接继承,无流式API
  │   │
  │   └─ ControlBase
  │       └─ Control<Derived> : View<Derived, ControlBase> ✅ 有流式API
  │           ├─ ContentControl ✅ 继承流式API
  │           ├─ ItemsControl ✅ 继承流式API
  │           └─ Window ⚠️ 有流式API,但部分方法重复定义
```

## API风格对比

### 1. UIElement (基类)
**API风格**: Get/Set 前缀

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `GetVisibility()` | `Visibility` | Getter |
| `SetVisibility(Visibility)` | `void` | Setter |
| `GetOpacity()` | `float` | Getter |
| `SetOpacity(float)` | `void` | Setter |

**状态**: ✅ 符合基类设计,使用标准 Get/Set

---

### 2. FrameworkElement (基类)
**API风格**: Get/Set 前缀

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `GetWidth()` | `float` | Getter |
| `SetWidth(float)` | `void` | Setter |
| `GetHeight()` | `float` | Getter |
| `SetHeight(float)` | `void` | Setter |
| `GetMargin()` | `const Thickness&` | Getter |
| `SetMargin(const Thickness&)` | `void` | Setter |

**状态**: ✅ 符合基类设计,使用标准 Get/Set

---

### 3. Panel (基类)
**API风格**: Get/Set 前缀 + void setter

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `AddChild(shared_ptr<UIElement>)` | `void` | Setter |
| `RemoveChild(UIElement*)` | `void` | Setter |
| `ClearChildren()` | `void` | Setter |
| `GetChildren()` | `span<const shared_ptr<UIElement>>` | Getter |
| `GetChildCount()` | `size_t` | Getter |
| `HasChildren()` | `bool` | Getter |

**状态**: ✅ 符合基类设计,不使用流式API

---

### 4. StackPanel : View<StackPanel, Panel>
**API风格**: Getter/Setter 重载 + 流式API

**自有属性**:
| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `Orientation()` const | `ui::Orientation` | Getter (无参) |
| `Orientation(ui::Orientation)` | `shared_ptr<StackPanel>` | Setter (有参) |

**包装 Panel 方法** (流式API):
| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `AddChild(shared_ptr<UIElement>)` | `shared_ptr<StackPanel>` | 包装基类 |
| `RemoveChild(UIElement*)` | `shared_ptr<StackPanel>` | 包装基类 |
| `ClearChildren()` | `shared_ptr<StackPanel>` | 包装基类 |

**继承 View 模板**:
- `Width()` const → `float`
- `Width(float)` → `shared_ptr<StackPanel>`
- `Height()` const → `float`
- `Height(float)` → `shared_ptr<StackPanel>`
- ... 等所有 FrameworkElement 属性

**状态**: ✅ 完整流式API,符合设计

---

### 5. Decorator : FrameworkElement
**API风格**: Get/Set 前缀 + void setter

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `SetChild(shared_ptr<UIElement>)` | `void` | Setter |
| `GetChild()` | `shared_ptr<UIElement>` | Getter |
| `ClearChild()` | `void` | Setter |

**问题**: ❌ 直接继承 FrameworkElement,没有流式API
**建议**: 改为 `Decorator : View<Decorator, FrameworkElement>`

---

### 6. Control<Derived> : View<Derived, ControlBase>
**API风格**: 继承 View 的完整流式API

**状态**: ✅ 通过 View 模板获得完整流式API

---

### 7. ContentControl : Control<ContentControl>
**API风格**: Get/Set 前缀 + 继承流式API

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `SetContent(shared_ptr<UIElement>)` | `void` | Setter |
| `GetContent()` | `shared_ptr<UIElement>` | Getter |

**继承的流式API**:
- 通过 Control -> View 继承所有流式API方法

**问题**: ⚠️ SetContent 是 void,没有流式包装
**建议**: 添加 `Content(shared_ptr<UIElement>)` 返回 `shared_ptr<ContentControl>`

---

### 8. Window : ContentControl
**API风格**: 混合 - 部分重载,部分继承

**自有方法重载**:
| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `Title()` const | `string` | Getter |
| `Title(const string&)` | `Ptr` | Setter |
| `Width()` const | `int` | Getter ⚠️ |
| `Width(int)` | `Ptr` | Setter ⚠️ |
| `Height()` const | `int` | Getter ⚠️ |
| `Height(int)` | `Ptr` | Setter ⚠️ |

**问题**: 
1. ⚠️ Width/Height 返回 `int`,但 View 模板的 Width/Height 返回 `float`
2. ⚠️ 这会导致重载冲突或名称隐藏

**Window 特有成员变量**:
```cpp
std::string title_;
int width_;      // ⚠️ 与 FrameworkElement 的布局宽度重复?
int height_;     // ⚠️ 与 FrameworkElement 的布局高度重复?
bool visible_;
```

**建议**: 
- Window 的 Width/Height 应该是窗口大小 (int)
- View 模板的 Width/Height 是布局大小 (float)
- 需要区分命名,或者使用 `WindowWidth()`/`WindowHeight()`

---

### 9. ItemsControl : Control<ItemsControl>
**API风格**: Get/Set 前缀 + 继承流式API

| 方法 | 返回类型 | 说明 |
|------|---------|------|
| `GetItems()` | `ObservableCollection<UIElement>&` | Getter |

**状态**: ✅ 继承流式API,Items 通过引用访问

---

## 发现的问题

### 问题1: Decorator 缺少流式API ❌

**当前状态**:
```cpp
class Decorator : public FrameworkElement {
    void SetChild(std::shared_ptr<UIElement> child);
    std::shared_ptr<UIElement> GetChild() const;
};
```

**建议修改**:
```cpp
class Decorator : public View<Decorator, FrameworkElement> {
    // Getter/Setter 重载
    std::shared_ptr<UIElement> Child() const;
    std::shared_ptr<Decorator> Child(std::shared_ptr<UIElement> child);
    
    // 包装基类方法 (可选,保持兼容性)
    void SetChild(std::shared_ptr<UIElement> child);
    std::shared_ptr<UIElement> GetChild() const;
};
```

---

### 问题2: ContentControl 的 Content 方法无流式API ⚠️

**当前状态**:
```cpp
class ContentControl : public Control<ContentControl> {
    void SetContent(std::shared_ptr<UIElement> content);
    std::shared_ptr<UIElement> GetContent() const;
};
```

**建议添加**:
```cpp
class ContentControl : public Control<ContentControl> {
    // Getter/Setter 重载
    std::shared_ptr<UIElement> Content() const { return GetContent(); }
    std::shared_ptr<ContentControl> Content(std::shared_ptr<UIElement> content) {
        SetContent(std::move(content));
        return std::static_pointer_cast<ContentControl>(shared_from_this());
    }
    
    // 保留原有方法 (向后兼容)
    void SetContent(std::shared_ptr<UIElement> content);
    std::shared_ptr<UIElement> GetContent() const;
};
```

---

### 问题3: Window 的 Width/Height 与 View 模板冲突 ⚠️

**当前冲突**:
```cpp
// Window 自己的方法
int Width() const;                    // 返回 int
Ptr Width(int);                       // 参数是 int

// 从 View<T> 继承的方法
float Width() const;                  // 返回 float (来自View模板)
Ptr Width(float);                     // 参数是 float (来自View模板)
```

**问题**: 
- 方法签名不同,会导致名称隐藏或重载歧义
- Window 的窗口大小 (int) vs 布局大小 (float) 概念混淆

**解决方案A**: 重命名 Window 特定方法
```cpp
class Window : public ContentControl {
    // 窗口大小 (原生像素)
    int WindowWidth() const;
    Ptr WindowWidth(int);
    int WindowHeight() const;
    Ptr WindowHeight(int);
    
    // 继承 View 模板的 Width()/Height() 用于布局
};
```

**解决方案B**: 使用 using 声明隐藏基类方法
```cpp
class Window : public ContentControl {
    // 明确使用 Window 的版本
    int Width() const;          // 隐藏 View 的 float Width()
    Ptr Width(int);             // 隐藏 View 的 Ptr Width(float)
    
    // 如需访问布局宽度,显式调用
    float LayoutWidth() const { return ContentControl::Width(); }
};
```

---

## 统一性评分

| 类 | 流式API | Getter/Setter重载 | 评分 | 说明 |
|----|---------|------------------|------|------|
| UIElement | ❌ | ❌ | 5/5 | 基类,设计正确 |
| FrameworkElement | ❌ | ❌ | 5/5 | 基类,设计正确 |
| Panel | ❌ | ❌ | 5/5 | 基类,设计正确 |
| StackPanel | ✅ | ✅ | 5/5 | 完美流式API |
| Decorator | ❌ | ❌ | 2/5 | **缺少流式API** |
| Control<T> | ✅ | ✅ | 5/5 | 模板提供流式API |
| ContentControl | ✅ (部分) | ❌ | 3/5 | **Content方法无重载** |
| ItemsControl | ✅ | ✅ | 5/5 | 完整流式API |
| Window | ✅ | ⚠️ | 3/5 | **Width/Height冲突** |

**总体评分**: 38/45 (84%)

---

## 优先级建议

### 🔴 高优先级
1. **修复 Window 的 Width/Height 冲突**
   - 重命名为 WindowWidth/WindowHeight
   - 或使用 using 声明明确版本
   - 避免编译器歧义

### 🟡 中优先级
2. **为 Decorator 添加流式API**
   - 改为继承 View<Decorator, FrameworkElement>
   - 添加 Child() getter/setter 重载

3. **为 ContentControl 添加 Content() 重载**
   - 提供流式API的 Content() 方法
   - 保持 SetContent/GetContent 向后兼容

### 🟢 低优先级
4. **文档更新**
   - 明确区分窗口大小 vs 布局大小
   - 更新API使用指南

---

## 命名约定总结

### 当前统一规则

1. **基类** (UIElement, FrameworkElement, Panel):
   - Getter: `GetXxx()` 前缀
   - Setter: `SetXxx()` 前缀,返回 `void`
   - Boolean: `IsXxx()`, `HasXxx()`

2. **派生类** (通过 View 模板):
   - Getter: `Xxx()` 无参重载,返回值类型
   - Setter: `Xxx(value)` 有参重载,返回 `shared_ptr<Derived>`
   - 支持完整链式调用

3. **特殊情况**:
   - Window: 需要区分窗口大小 vs 布局大小
   - ContentControl: Content 方法需要重载
   - Decorator: 需要改为使用 View 模板

---

## 示例对比

### ✅ 良好设计 - StackPanel
```cpp
auto panel = std::make_shared<StackPanel>();

// 流式API
panel->Orientation(Orientation::Vertical)  // setter
     ->Width(800)                           // setter (View模板)
     ->Height(600)                          // setter (View模板)
     ->AddChild(child);                     // setter (包装)

// Getter
auto orient = panel->Orientation();         // getter
float w = panel->Width();                   // getter (View模板)
```

### ❌ 需要改进 - ContentControl
```cpp
auto control = std::make_shared<ContentControl>();

// 当前: 无法链式调用
control->SetContent(child);  // void,断开链式

// 期望: 流式API
control->Content(child)      // 返回 shared_ptr<ContentControl>
       ->Width(200)
       ->Height(100);
```

### ⚠️ 有冲突 - Window
```cpp
auto window = ui::window();

// 当前: 可能有歧义
window->Width(800);   // 调用哪个? Window::Width(int) 还是 View::Width(float)?

// 建议: 明确命名
window->WindowWidth(800)   // 窗口大小
      ->Width(800.0f);     // 布局大小 (通常一致,但类型明确)
```

---

## 结论

F__K_UI 框架的 API 一致性已经达到 **84%**,主要成就:
- ✅ View 模板成功提供统一的流式API
- ✅ StackPanel 完美实现 getter/setter 重载
- ✅ 基类保持简洁的 Get/Set 风格

需要改进的地方:
- ❌ Decorator 缺少流式API包装
- ⚠️ ContentControl 的 Content 方法需要重载
- ⚠️ Window 的 Width/Height 与 View 模板冲突

建议优先解决 Window 的命名冲突,这是最容易导致编译错误的问题。
