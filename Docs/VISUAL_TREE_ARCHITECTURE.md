# 视觉树架构完整说明

## 目录

1. [核心概念](#核心概念)
2. [类层次结构](#类层次结构)
3. [视觉树 vs 逻辑树](#视觉树-vs-逻辑树)
4. [树的构建与维护](#树的构建与维护)
5. [遍历机制](#遍历机制)
6. [模板系统与树结构](#模板系统与树结构)
7. [实际示例](#实际示例)
8. [最佳实践](#最佳实践)

---

## 核心概念

### 什么是视觉树？

**视觉树（Visual Tree）**是UI框架中所有可视化元素的层次结构，代表了实际渲染到屏幕上的元素组织方式。

### 双树架构

F_K_UI 采用**双树架构**，类似于 WPF：

```
┌─────────────────────┐
│   逻辑树 (Logical)   │  ← 开发者直接操作的元素
│   Panel, Button等    │
└──────────┬──────────┘
           │ 展开模板
           ↓
┌─────────────────────┐
│   视觉树 (Visual)    │  ← 实际渲染的完整元素树
│   包含模板内容       │
└─────────────────────┘
```

**关键区别：**

| 特性 | 逻辑树 | 视觉树 |
|------|--------|--------|
| **定义** | 开发者定义的元素结构 | 包含模板展开后的完整结构 |
| **粒度** | 粗粒度（高层控件） | 细粒度（所有渲染元素） |
| **模板** | 不包含模板内部 | 包含完整模板内容 |
| **用途** | 数据绑定、事件路由 | 渲染、命中测试 |

---

## 类层次结构

### 继承链

```
DependencyObject (依赖属性系统)
    ↓
Visual (视觉树基类)
    ├─ 视觉树父子关系
    ├─ 变换矩阵
    ├─ 命中测试
    └─ CollectDrawCommands()
    ↓
UIElement (UI元素基类)
    ├─ 布局系统 (Measure/Arrange)
    ├─ 输入事件
    ├─ 路由事件
    └─ GetLogicalChildren()
    ↓
FrameworkElement<Derived> (CRTP 框架元素)
    ├─ 数据上下文
    ├─ 样式和资源
    ├─ Margin/Padding
    └─ 尺寸约束
    ↓
Panel<Derived> / Control<Derived> / ...
```

### 关键类职责

#### 1. Visual - 视觉树基础

```cpp
class Visual : public DependencyObject {
private:
    Visual* visualParent_;              // 视觉树父节点
    std::vector<Visual*> visualChildren_; // 视觉树子节点
    Matrix3x2 transform_;                // 局部变换

public:
    // 视觉树操作
    void AddVisualChild(Visual* child);
    void RemoveVisualChild(Visual* child);
    Visual* GetVisualParent() const;
    Visual* GetVisualChild(size_t index) const;
    
    // 渲染
    virtual void CollectDrawCommands(RenderContext& context);
    
    // 变换
    Matrix3x2 GetAbsoluteTransform() const;
};
```

**职责：**
- 管理视觉树的父子关系
- 提供变换和命中测试
- 收集绘制命令

#### 2. UIElement - UI元素基础

```cpp
class UIElement : public Visual {
private:
    Size desiredSize_;        // 期望尺寸
    Size renderSize_;         // 渲染尺寸
    Rect layoutRect_;         // 布局矩形
    UIElement* templatedParent_; // 模板化父元素
    std::string name_;        // 元素名称 (用于 FindName)

public:
    // 布局
    void Measure(const Size& availableSize);
    void Arrange(const Rect& finalRect);
    
    // 逻辑树
    virtual std::vector<UIElement*> GetLogicalChildren() const;
    
    // 模板支持
    UIElement* GetTemplatedParent() const;
    void SetTemplatedParent(UIElement* parent);
    
    // 事件
    void RaiseEvent(RoutedEventArgs& args);
};
```

**职责：**
- 布局系统（测量和排列）
- 输入事件处理
- 路由事件机制
- 逻辑树遍历

#### 3. Panel - 容器基类

```cpp
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
private:
    std::vector<UIElement*> children_; // 子元素集合

public:
    // 子元素管理
    Derived* AddChild(UIElement* child) {
        children_.push_back(child);
        this->AddVisualChild(child);      // 添加到视觉树
        this->TakeOwnership(child);       // 获取所有权
        this->InvalidateMeasure();
        return static_cast<Derived*>(this);
    }
    
    // 逻辑树（覆写）
    std::vector<UIElement*> GetLogicalChildren() const override {
        return children_; // 返回子元素集合
    }
};
```

**职责：**
- 管理子元素集合
- 维护逻辑树结构
- 提供布局逻辑

---

## 视觉树 vs 逻辑树

### 视觉树（Visual Tree）

**定义：** 所有实际渲染到屏幕的元素的完整层次结构。

**特点：**
- 包含模板展开后的所有元素
- 包含所有视觉装饰（边框、背景等）
- 用于渲染和命中测试

**访问方式：**
```cpp
Visual* parent = element->GetVisualParent();
Visual* child = element->GetVisualChild(index);
size_t count = element->GetVisualChildrenCount();
```

### 逻辑树（Logical Tree）

**定义：** 开发者定义的高层元素结构。

**特点：**
- 只包含逻辑上的父子关系
- 不包含模板内部元素
- 用于数据绑定和事件路由

**访问方式：**
```cpp
std::vector<UIElement*> children = element->GetLogicalChildren();
```

### 对比示例

**代码：**
```cpp
auto button = new Button();
button->SetContent(new TextBlock()->Text("Click Me"));
```

**逻辑树：**
```
Button
  └─ TextBlock "Click Me"
```

**视觉树（应用模板后）：**
```
Button
  └─ Border (from template)
      └─ ContentPresenter (from template)
          └─ TextBlock "Click Me" (content)
```

---

## 树的构建与维护

### 1. 添加子元素到视觉树

```cpp
// Panel 添加子元素
void Panel::AddChild(UIElement* child) {
    // 1. 添加到逻辑树（子元素集合）
    children_.push_back(child);
    
    // 2. 添加到视觉树
    this->AddVisualChild(child);
    
    // 3. 获取所有权（内存管理）
    this->TakeOwnership(child);
    
    // 4. 标记需要重新布局
    this->InvalidateMeasure();
}
```

**`AddVisualChild()` 的实现：**
```cpp
void Visual::AddVisualChild(Visual* child) {
    if (child) {
        // 从旧父节点移除
        if (child->visualParent_) {
            child->visualParent_->RemoveVisualChild(child);
        }
        
        // 添加到新父节点
        visualChildren_.push_back(child);
        child->visualParent_ = this;
    }
}
```

### 2. 移除子元素

```cpp
void Panel::RemoveChild(UIElement* child) {
    // 1. 从逻辑树移除
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        children_.erase(it);
    }
    
    // 2. 从视觉树移除
    this->RemoveVisualChild(child);
    
    // 3. 标记需要重新布局
    this->InvalidateMeasure();
}
```

### 3. 模板应用时的树构建

```cpp
// Button 应用模板
void Button::ApplyTemplate() {
    if (template_) {
        // 1. 从模板工厂创建视觉树
        UIElement* templateRoot = template_->LoadContent();
        
        // 2. 设置模板化父元素
        SetTemplatedParent(templateRoot, this);
        
        // 3. 添加到视觉树（不添加到逻辑树！）
        this->AddVisualChild(templateRoot);
        
        // 4. 查找模板部件
        FindTemplateChild("PART_ContentPresenter");
    }
}

// 递归设置模板化父元素
void SetTemplatedParent(UIElement* element, UIElement* parent) {
    element->SetTemplatedParent(parent);
    for (auto* child : element->GetLogicalChildren()) {
        SetTemplatedParent(child, parent);
    }
}
```

---

## 遍历机制

### 1. 视觉树遍历（深度优先）

```cpp
void TraverseVisualTree(Visual* root, std::function<void(Visual*)> visitor) {
    if (!root) return;
    
    // 访问当前节点
    visitor(root);
    
    // 递归访问子节点
    size_t count = root->GetVisualChildrenCount();
    for (size_t i = 0; i < count; ++i) {
        TraverseVisualTree(root->GetVisualChild(i), visitor);
    }
}
```

**使用场景：**
- 渲染（CollectDrawCommands）
- 命中测试
- 查找元素（FindVisualChild）

### 2. 逻辑树遍历

```cpp
void TraverseLogicalTree(UIElement* root, std::function<void(UIElement*)> visitor) {
    if (!root) return;
    
    // 访问当前节点
    visitor(root);
    
    // 递归访问逻辑子节点
    for (auto* child : root->GetLogicalChildren()) {
        TraverseLogicalTree(child, visitor);
    }
}
```

**使用场景：**
- FindName（按名称查找元素）
- 数据上下文传播
- 路由事件

### 3. 向上遍历（查找祖先）

```cpp
template<typename T>
T* FindAncestor(UIElement* element) {
    Visual* current = element->GetVisualParent();
    while (current) {
        if (auto* typed = dynamic_cast<T*>(current)) {
            return typed;
        }
        current = current->GetVisualParent();
    }
    return nullptr;
}
```

**使用场景：**
- 查找父容器
- 查找 TemplatedParent
- 事件冒泡

### 4. FindName 实现

```cpp
UIElement* FindName(UIElement* root, const std::string& name) {
    // 检查当前元素
    if (root->GetName() == name) {
        return root;
    }
    
    // 递归搜索逻辑子节点
    for (auto* child : root->GetLogicalChildren()) {
        if (auto* found = FindName(child, name)) {
            return found;
        }
    }
    
    return nullptr;
}
```

---

## 模板系统与树结构

### 模板如何影响树结构

#### 不使用模板（简单控件）

```cpp
// TextBlock 直接渲染
auto text = new TextBlock()->Text("Hello");

// 树结构：
TextBlock "Hello"
```

#### 使用模板（复杂控件）

```cpp
// Button 使用模板
auto button = new Button();
button->SetContent(new TextBlock()->Text("Click"));

// 逻辑树：
Button
  └─ TextBlock "Click" (Content)

// 视觉树（应用默认模板后）：
Button
  └─ Border (from template)
      └─ ContentPresenter (from template)
          └─ TextBlock "Click" (content)
```

### 模板部件（Template Parts）

```cpp
// Button 的默认模板
auto template = new ControlTemplate();
template->SetFactory([]() {
    auto border = new Border();
    border->SetName("PART_Border");  // 命名部件
    
    auto presenter = new ContentPresenter();
    presenter->SetName("PART_ContentPresenter");
    
    border->SetChild(presenter);
    return border;
});

button->SetTemplate(template);
button->ApplyTemplate();

// 查找模板部件
auto* border = button->GetTemplateChild<Border>("PART_Border");
auto* presenter = button->GetTemplateChild<ContentPresenter>("PART_ContentPresenter");
```

### TemplatedParent 机制

```cpp
// 设置模板化父元素
void Button::ApplyTemplate() {
    UIElement* root = template_->LoadContent();
    SetTemplatedParentRecursive(root, this);
    AddVisualChild(root);
}

void SetTemplatedParentRecursive(UIElement* element, UIElement* parent) {
    element->SetTemplatedParent(parent);
    
    // 递归设置所有逻辑子节点
    for (auto* child : element->GetLogicalChildren()) {
        SetTemplatedParentRecursive(child, parent);
    }
}

// 查询：从模板内部访问控件
UIElement* GetTemplatedParent() const {
    return templatedParent_;
}
```

---

## 实际示例

### 示例 1：StackPanel 与 TextBlock

**代码：**
```cpp
auto panel = new StackPanel();
panel->AddChild(new TextBlock()->Text("First"));
panel->AddChild(new TextBlock()->Text("Second"));
```

**逻辑树：**
```
StackPanel
  ├─ TextBlock "First"
  └─ TextBlock "Second"
```

**视觉树（相同）：**
```
StackPanel
  ├─ TextBlock "First"
  └─ TextBlock "Second"
```

### 示例 2：带 Border 的 Button

**代码：**
```cpp
auto button = new Button();
button->SetContent(new TextBlock()->Text("OK"));
```

**逻辑树：**
```
Button
  └─ TextBlock "OK" (Content)
```

**视觉树（应用模板后）：**
```
Button
  └─ Border (template)  ← templatedParent = Button
      └─ ContentPresenter (template)  ← templatedParent = Button
          └─ TextBlock "OK" (content)
```

### 示例 3：嵌套容器

**代码：**
```cpp
auto panel = new StackPanel();
panel->AddChild(
    new Border()
        ->SetChild(
            new TextBlock()->Text("Nested")
        )
);
```

**逻辑树：**
```
StackPanel
  └─ Border
      └─ TextBlock "Nested"
```

**视觉树（相同）：**
```
StackPanel
  └─ Border
      └─ TextBlock "Nested"
```

### 示例 4：ComboBox（复杂控件）

**代码：**
```cpp
auto combo = new ComboBox();
combo->AddItem(new ComboBoxItem()->SetContent("Item 1"));
combo->AddItem(new ComboBoxItem()->SetContent("Item 2"));
```

**逻辑树：**
```
ComboBox
  ├─ ComboBoxItem "Item 1"
  └─ ComboBoxItem "Item 2"
```

**视觉树（应用模板后）：**
```
ComboBox
  ├─ Border (template)
  │   └─ Grid (template)
  │       ├─ ContentPresenter (显示选中项)
  │       └─ ToggleButton (下拉按钮)
  └─ Popup (template)
      └─ ItemsPresenter (template)
          ├─ ComboBoxItem "Item 1"
          └─ ComboBoxItem "Item 2"
```

---

## 渲染流程与树的关系

### 完整渲染流程

```
Window::RenderFrame()
    ↓
rootElement_->Measure(windowSize)  ← 遍历逻辑树
    ↓
rootElement_->Arrange(rect)        ← 遍历逻辑树
    ↓
RenderContext context;
rootElement_->CollectDrawCommands(context)  ← 遍历视觉树
    ↓
    递归调用每个 Visual 的：
    1. PushTransform(layoutRect.offset)
    2. OnRender(context) [基础控件]
    3. 递归子节点的 CollectDrawCommands()
    4. PopTransform()
    ↓
GlRenderer->Render(commands)       ← OpenGL 渲染
```

### CollectDrawCommands 的递归

```cpp
void UIElement::CollectDrawCommands(RenderContext& context) {
    // 1. 推入当前元素的布局偏移
    context.PushTransform(layoutRect_.x, layoutRect_.y);
    
    // 2. 绘制自身（如果是基础控件）
    OnRender(context);
    
    // 3. 递归绘制视觉子节点
    for (size_t i = 0; i < GetVisualChildrenCount(); ++i) {
        GetVisualChild(i)->CollectDrawCommands(context);
    }
    
    // 4. 弹出变换
    context.PopTransform();
}
```

**关键点：**
- 使用**视觉树**进行遍历（不是逻辑树）
- 累积坐标变换（从父到子）
- 基础控件调用 `OnRender()`，复杂控件递归子节点

---

## 最佳实践

### 1. 选择正确的树进行遍历

```cpp
// ✅ 使用逻辑树 - 数据绑定、FindName
UIElement* FindByName(UIElement* root, const std::string& name) {
    for (auto* child : root->GetLogicalChildren()) {
        // ...
    }
}

// ✅ 使用视觉树 - 渲染、命中测试
void Render(Visual* root) {
    for (size_t i = 0; i < root->GetVisualChildrenCount(); ++i) {
        root->GetVisualChild(i)->CollectDrawCommands(...);
    }
}
```

### 2. 内存管理

```cpp
// ✅ Panel 自动管理子元素所有权
panel->AddChild(new TextBlock());  // Panel 拥有 TextBlock

// ❌ 不要手动 delete
auto* text = new TextBlock();
panel->AddChild(text);
delete text;  // 错误！Panel 已拥有所有权
```

### 3. 模板部件访问

```cpp
// ✅ 安全访问模板部件
void Button::OnApplyTemplate() {
    if (auto* border = GetTemplateChild<Border>("PART_Border")) {
        // 使用 border
    }
}

// ❌ 不要假设模板结构
void Button::OnApplyTemplate() {
    auto* border = (Border*)GetVisualChild(0);  // 危险！
}
```

### 4. 避免混淆两种树

```cpp
// ❌ 错误：在视觉树中查找逻辑子节点
for (size_t i = 0; i < button->GetVisualChildrenCount(); ++i) {
    // 这会包含模板内部元素！
}

// ✅ 正确：在逻辑树中查找
for (auto* child : button->GetLogicalChildren()) {
    // 只包含逻辑子节点
}
```

---

## 总结

### 关键要点

1. **双树架构**
   - 逻辑树：开发者定义的结构
   - 视觉树：实际渲染的完整结构

2. **类职责**
   - `Visual`：视觉树基础、渲染
   - `UIElement`：布局、事件、逻辑树
   - `Panel`：子元素管理

3. **遍历规则**
   - 逻辑树：`GetLogicalChildren()`
   - 视觉树：`GetVisualChild(index)`

4. **模板机制**
   - 模板展开时创建额外的视觉节点
   - `TemplatedParent` 连接模板内容和控件

5. **渲染流程**
   - 布局：遍历逻辑树
   - 渲染：遍历视觉树

### 架构图

```
┌────────────────────────────────────────────┐
│           应用程序代码                      │
│   panel->AddChild(button)                  │
└──────────────┬─────────────────────────────┘
               ↓
┌────────────────────────────────────────────┐
│           逻辑树 (Logical Tree)             │
│   Panel                                    │
│     └─ Button                              │
└──────────────┬─────────────────────────────┘
               ↓ ApplyTemplate()
┌────────────────────────────────────────────┐
│           视觉树 (Visual Tree)              │
│   Panel                                    │
│     └─ Button                              │
│         └─ Border (template)               │
│             └─ ContentPresenter (template) │
└──────────────┬─────────────────────────────┘
               ↓ CollectDrawCommands()
┌────────────────────────────────────────────┐
│           渲染命令 (Draw Commands)          │
│   DrawRect, DrawText, ...                  │
└──────────────┬─────────────────────────────┘
               ↓
┌────────────────────────────────────────────┐
│           OpenGL 渲染                       │
│   GlRenderer                               │
└────────────────────────────────────────────┘
```

这个架构确保了：
- **灵活性**：模板可完全自定义控件外观
- **性能**：渲染只遍历视觉树
- **简洁性**：开发者只需关注逻辑树
- **一致性**：所有复杂控件使用相同机制
