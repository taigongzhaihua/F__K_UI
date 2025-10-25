# F__K_UI 指针使用规范

**日期**: 2025年10月26日  
**状态**: ✅ 已验证 - 当前设计符合最佳实践

---

## 一、指针使用原则

### 核心原则

1. **所有权转移**: 使用 `std::shared_ptr<T>`
2. **观察关系**: 使用裸指针 `T*`
3. **返回容器**: 使用 `std::span<const std::shared_ptr<T>>` 或 `std::vector<T*>`

---

## 二、API 分类与规范

### 2.1 子元素管理 API

#### Panel - 多子元素容器

```cpp
class Panel {
public:
    // ✅ 添加子元素 - 转移所有权
    void AddChild(std::shared_ptr<UIElement> child);
    
    // ✅ 移除子元素 - 用裸指针识别对象
    // 注意: 不能用 const UIElement*,因为内部需要调用 DetachChild(UIElement&)
    void RemoveChild(UIElement* child);
    
    // ✅ 获取子元素列表 - 返回观察视图
    [[nodiscard]] std::span<const std::shared_ptr<UIElement>> GetChildren() const noexcept;
    
    // ✅ 检查是否有子元素
    [[nodiscard]] bool HasChildren() const noexcept;
    
    // ✅ 获取子元素数量
    [[nodiscard]] std::size_t GetChildCount() const noexcept;
};
```

**设计理由：**
- `AddChild(shared_ptr)`: 接受所有权，存储到 `vector<shared_ptr<UIElement>> children_`
- `RemoveChild(UIElement*)`: 只需要识别对象，用 `.get()` 比较指针
- `GetChildren()`: 返回 span 避免复制，const 保护不被修改

#### Decorator - 单子元素容器

```cpp
class Decorator {
public:
    // ✅ 设置子元素 - 转移所有权
    void SetChild(std::shared_ptr<UIElement> child);
    
    // ✅ 获取子元素 - 返回 shared_ptr (允许外部持有)
    [[nodiscard]] std::shared_ptr<UIElement> GetChild() const noexcept;
    
    // ✅ 检查是否有子元素
    [[nodiscard]] bool HasChild() const noexcept;
    
    void ClearChild();
};
```

**设计理由：**
- `GetChild()` 返回 `shared_ptr` 而不是裸指针，因为调用者可能需要延长对象生命周期

#### ContentControl - 内容容器

```cpp
class ControlBase {
public:
    // ✅ 设置内容 - 转移所有权
    void SetContent(std::shared_ptr<UIElement> content);
    
    // ✅ 获取内容 - 返回 shared_ptr
    [[nodiscard]] std::shared_ptr<UIElement> GetContent() const;
    
    // ✅ 检查是否有内容
    [[nodiscard]] bool HasContent() const;
    
    void ClearContent();
};
```

---

### 2.2 逻辑树 API

#### DependencyObject - 逻辑树管理

```cpp
class DependencyObject {
public:
    // ✅ 添加逻辑子节点 - 不拥有，只建立关系
    void AddLogicalChild(DependencyObject* child);
    
    // ✅ 移除逻辑子节点
    void RemoveLogicalChild(DependencyObject* child);
    
    // ✅ 获取逻辑父节点 - 观察指针
    [[nodiscard]] DependencyObject* GetLogicalParent() const noexcept;
    
    // ✅ 获取逻辑子节点列表 - 裸指针向量
    [[nodiscard]] const std::vector<DependencyObject*>& GetLogicalChildren() const noexcept;
};
```

**设计理由：**
- 逻辑树是**观察关系**，不拥有对象
- 实际所有权由 UI 树（children_/child_/content_）管理
- 使用裸指针避免循环引用和不必要的引用计数

---

## 三、设计模式总结

### 所有权模型

```
UI 树 (拥有对象)
├─ Panel::children_ : vector<shared_ptr<UIElement>>    // 拥有
├─ Decorator::child_ : shared_ptr<UIElement>            // 拥有
└─ Control::content_ (DP存储) : shared_ptr<UIElement>   // 拥有

逻辑树 (观察关系)
├─ logicalParent_ : DependencyObject*                   // 不拥有
└─ logicalChildren_ : vector<DependencyObject*>         // 不拥有
```

### 方法签名规范

| 操作类型 | 参数类型 | 返回类型 | 示例 |
|---------|---------|---------|------|
| 添加子元素 | `shared_ptr<T>` | `void` 或 `Ptr` (流式) | `AddChild(shared_ptr)` |
| 移除子元素 | `T*` | `void` 或 `Ptr` (流式) | `RemoveChild(UIElement*)` |
| 设置子元素 | `shared_ptr<T>` | `void` | `SetChild(shared_ptr)` |
| 获取子元素 | - | `shared_ptr<T>` | `GetChild()` |
| 获取子元素列表 | - | `span<const shared_ptr<T>>` | `GetChildren()` |
| 获取父节点 | - | `T*` | `GetLogicalParent()` |
| 建立逻辑关系 | `T*` | `void` | `AddLogicalChild(T*)` |

---

## 四、常见使用模式

### 添加子元素

```cpp
auto panel = std::make_shared<StackPanel>();
auto child = std::make_shared<ContentControl>();

// ✅ 正确：转移所有权
panel->AddChild(child);  // child 仍然有效，shared_ptr 共享所有权

// ✅ 也可以：移动语义
panel->AddChild(std::move(child));  // child 变为 nullptr
```

### 移除子元素

```cpp
auto panel = std::make_shared<StackPanel>();
auto child = std::make_shared<ContentControl>();
panel->AddChild(child);

// ✅ 正确：使用裸指针识别
panel->RemoveChild(child.get());

// ✅ 也可以：直接传递
UIElement* rawPtr = child.get();
panel->RemoveChild(rawPtr);
```

### 访问子元素

```cpp
auto panel = std::make_shared<StackPanel>();

// ✅ 遍历所有子元素
for (const auto& child : panel->GetChildren()) {
    // child 是 const shared_ptr<UIElement>&
    if (child) {
        child->Measure(availableSize);
    }
}

// ✅ 获取数量
std::size_t count = panel->GetChildCount();

// ✅ 检查是否为空
if (panel->HasChildren()) {
    // ...
}
```

### 逻辑树遍历

```cpp
void PrintLogicalTree(DependencyObject* obj, int depth = 0) {
    if (!obj) return;
    
    std::cout << std::string(depth * 2, ' ') << obj->GetElementName() << '\n';
    
    // ✅ 遍历逻辑子节点（裸指针）
    for (DependencyObject* child : obj->GetLogicalChildren()) {
        PrintLogicalTree(child, depth + 1);
    }
}
```

---

## 五、为什么不使用其他方案

### ❌ 方案A: RemoveChild 使用 shared_ptr

```cpp
// ❌ 不推荐
void RemoveChild(const std::shared_ptr<UIElement>& child);

// 问题：
// 1. 不必要的引用计数操作
// 2. 调用者可能只有裸指针
// 3. 语义不清晰（不需要所有权）
```

### ❌ 方案B: GetChildren 返回 vector 副本

```cpp
// ❌ 不推荐
std::vector<std::shared_ptr<UIElement>> GetChildren() const;

// 问题：
// 1. 复制整个 vector，性能差
// 2. 不必要的内存分配
```

### ❌ 方案C: 逻辑树使用 weak_ptr

```cpp
// ❌ 不推荐
class DependencyObject {
    std::vector<std::weak_ptr<DependencyObject>> logicalChildren_;
};

// 问题：
// 1. weak_ptr 有性能开销（线程安全的引用计数）
// 2. 需要 lock() 才能访问，复杂且低效
// 3. UI 树已经管理生命周期，不需要 weak_ptr
```

---

## 六、内存安全性分析

### 生命周期保证

1. **UI 树拥有对象**
   - `children_`, `child_`, `content_` 使用 `shared_ptr`
   - 只要在 UI 树中，对象就存活

2. **逻辑树是观察者**
   - 只在对象存活期间有效
   - 对象从 UI 树移除时，会自动调用 `RemoveLogicalChild`
   - 通过 `DetachChild` / `DetachContent` 同步

3. **访问安全性**
   - 所有 UI 操作必须在正确线程（通过 `VerifyAccess()`）
   - 逻辑树指针只在同一线程访问，无竞态

### 悬空指针预防

```cpp
// Panel::RemoveChild 实现
void Panel::RemoveChild(UIElement* child) {
    // 1. 先从逻辑树移除（断开观察关系）
    DetachChild(*child);
    
    // 2. 再从 UI 树移除（释放所有权）
    children_.erase(it);
    
    // 顺序很重要！确保逻辑树指针在对象销毁前清理
}
```

---

## 七、总结

### ✅ 当前设计的优点

1. **清晰的所有权语义**
   - `shared_ptr` = 拥有
   - 裸指针 = 观察

2. **高性能**
   - 避免不必要的引用计数
   - 避免不必要的内存复制

3. **类型安全**
   - 编译期保证类型正确
   - span 提供边界检查

4. **符合 C++ 最佳实践**
   - 遵循 C++ Core Guidelines
   - 与 STL 风格一致

### 📋 规范检查清单

- [x] AddChild 使用 `shared_ptr` ✅
- [x] RemoveChild 使用裸指针 ✅
- [x] GetChildren 返回 `span<const shared_ptr>` ✅
- [x] GetChild 返回 `shared_ptr` ✅
- [x] 逻辑树使用裸指针 ✅
- [x] 所有 Getter 都有 `[[nodiscard]]` ✅
- [x] 所有观察指针都有 `noexcept` ✅

### 结论

**当前的指针使用规范已经是最佳实践，无需修改！**

API-Design-Issues.md 中关于"指针类型使用不统一"的问题是**误报**。当前设计：
- 语义清晰
- 性能最优
- 内存安全
- 符合 C++ 最佳实践

---

**编写时间**: 2025-10-26  
**审查人**: GitHub Copilot  
**状态**: ✅ 已验证通过
