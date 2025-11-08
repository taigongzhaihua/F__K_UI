# Visual - 可视元素接口

## 概述

`Visual` 是一个抽象接口，定义了所有参与渲染的 UI 元素必须实现的契约。它提供了渲染系统获取元素视觉信息（边界、不透明度、可见性等）所需的接口。

## 核心设计

Visual 接口采用模板方法模式，确保渲染系统能够统一地查询和处理所有 UI 元素的渲染属性。

## 核心 API

### 获取渲染边界

```cpp
virtual Rect GetRenderBounds() const = 0;
```

返回元素的最终渲染矩形（布局后的位置和大小）。

**返回值：** `Rect` 结构体包含：
- `X`, `Y`: 相对于父元素的位置
- `Width`, `Height`: 元素的渲染尺寸

**特点：**
- 只读操作，不应改变元素状态
- 返回的是最终布局后的矩形
- 包括所有内部变换（如缩放、旋转）

**示例：**
```cpp
auto visual = element.get();  // UIElement 实现了 Visual
auto bounds = visual->GetRenderBounds();

std::cout << "位置: (" << bounds.X << ", " << bounds.Y << ")\n";
std::cout << "尺寸: " << bounds.Width << "x" << bounds.Height << "\n";
```

### 获取不透明度

```cpp
virtual float GetOpacity() const = 0;
```

返回元素的不透明度值，范围 [0.0, 1.0]。

**说明：**
- `0.0`: 完全透明
- `1.0`: 完全不透明
- `0.5`: 半透明

**示例：**
```cpp
auto opacity = visual->GetOpacity();

if (opacity < 0.5f) {
    std::cout << "元素是半透明的\n";
}
```

### 获取可见性

```cpp
virtual Visibility GetVisibility() const = 0;
```

返回元素的可见性状态。

**返回值：**
```cpp
enum class Visibility {
    Visible,    // 可见
    Hidden,     // 隐藏但占用空间
    Collapsed   // 隐藏且不占用空间
};
```

**示例：**
```cpp
if (visual->GetVisibility() == fk::ui::Visibility::Visible) {
    // 渲染元素
}
```

### 获取可视子元素

```cpp
virtual std::vector<Visual*> GetVisualChildren() const = 0;
```

返回此元素的所有可视子元素列表。

**说明：**
- 返回的是原始指针（非所有权）
- 应遍历此列表进行递归渲染
- 可以过滤已隐藏的子元素

**示例：**
```cpp
void RenderVisualTree(const fk::ui::Visual* visual) {
    if (visual->GetVisibility() == fk::ui::Visibility::Collapsed) {
        return;  // 不需要渲染隐藏元素
    }
    
    // 渲染此元素
    RenderElement(visual);
    
    // 递归渲染子元素
    for (auto child : visual->GetVisualChildren()) {
        RenderVisualTree(child);
    }
}
```

### 是否有渲染内容

```cpp
virtual bool HasRenderContent() const = 0;
```

检查元素是否有实际的渲染内容（背景、边框、文本等）。

**用途：**
- 优化渲染（跳过纯布局容器）
- 判断是否需要刷新
- 确定交互检测范围

**示例：**
```cpp
if (visual->HasRenderContent()) {
    // 需要调用渲染命令
    renderList.AddCommand(CreateRenderCommand(visual));
} else {
    // 仅是容器，只需处理子元素
}
```

## 实现约定

### UIElement 实现

框架中的 `UIElement` 是 Visual 接口的主要实现。任何 UI 元素最终都继承自 UIElement 并实现 Visual 接口。

```cpp
class UIElement : public Visual {
public:
    Rect GetRenderBounds() const override;
    float GetOpacity() const override;
    Visibility GetVisibility() const override;
    std::vector<Visual*> GetVisualChildren() const override;
    bool HasRenderContent() const override;
};
```

## 常见模式

### 1. 递归遍历可视树

```cpp
void TraverseVisualTree(const fk::ui::Visual* root, std::function<void(const fk::ui::Visual*)> visitor) {
    if (!root || root->GetVisibility() == fk::ui::Visibility::Collapsed) {
        return;
    }
    
    visitor(root);
    
    for (auto child : root->GetVisualChildren()) {
        TraverseVisualTree(child, visitor);
    }
}

// 使用：计算元素总数
int elementCount = 0;
TraverseVisualTree(window.get(), [&](const auto*) {
    elementCount++;
});
```

### 2. 寻找特定元素

```cpp
const fk::ui::Visual* FindVisualAt(const fk::ui::Visual* root, float x, float y) {
    if (!root || root->GetVisibility() == fk::ui::Visibility::Collapsed) {
        return nullptr;
    }
    
    auto bounds = root->GetRenderBounds();
    if (!bounds.Contains(x, y)) {
        return nullptr;
    }
    
    // 先检查子元素（深度优先）
    for (auto child : root->GetVisualChildren()) {
        if (auto found = FindVisualAt(child, x, y)) {
            return found;
        }
    }
    
    // 检查当前元素
    if (root->HasRenderContent()) {
        return root;
    }
    
    return nullptr;
}
```

### 3. 收集可见元素

```cpp
void CollectVisibleElements(
    const fk::ui::Visual* root,
    std::vector<const fk::ui::Visual*>& outVisuals) {
    
    if (!root) return;
    
    if (root->GetVisibility() != fk::ui::Visibility::Visible) {
        return;  // 非可见元素跳过
    }
    
    if (root->GetOpacity() > 0.0f) {
        outVisuals.push_back(root);
    }
    
    for (auto child : root->GetVisualChildren()) {
        CollectVisibleElements(child, outVisuals);
    }
}
```

### 4. 计算有效边界

```cpp
fk::Rect GetEffectiveBounds(const fk::ui::Visual* visual) {
    auto bounds = visual->GetRenderBounds();
    
    // 根据不透明度调整
    if (visual->GetOpacity() < 1.0f) {
        // 可能需要扩展边界以容纳半透明效果
    }
    
    return bounds;
}
```

### 5. 渲染集合生成

```cpp
class RenderListBuilder {
public:
    void BuildFromVisualTree(const fk::ui::Visual* root) {
        TraverseVisualTree(root, [this](const auto* visual) {
            if (visual->HasRenderContent() && 
                visual->GetVisibility() == fk::ui::Visibility::Visible &&
                visual->GetOpacity() > 0.0f) {
                
                AddToRenderList(visual);
            }
        });
    }
    
private:
    void AddToRenderList(const fk::ui::Visual* visual) {
        // 生成渲染命令
        auto cmd = CreateRenderCommand(visual);
        renderList_.AddCommand(cmd);
    }
};
```

## 最佳实践

### 1. 总是检查可见性

```cpp
// ✅ 推荐
if (visual->GetVisibility() == fk::ui::Visibility::Visible) {
    // 仅处理可见元素
}

// ❌ 避免
// 处理所有元素，忽视可见性
```

### 2. 使用边界进行裁剪

```cpp
// ✅ 推荐：使用 GetRenderBounds 进行裁剪检测
auto bounds = visual->GetRenderBounds();
if (IsInClipRect(bounds, clipRect)) {
    Render(visual);
}
```

### 3. 缓存不透明度计算

```cpp
// ✅ 推荐
float opacity = visual->GetOpacity();
if (opacity > 0.0f) {
    Render(visual, opacity);
}

// ❌ 避免
for (int i = 0; i < 100; ++i) {
    if (visual->GetOpacity() > 0.0f) {  // 重复调用
        // ...
    }
}
```

## 常见问题

### Q: Visual 接口为什么是必要的？
**A:** 它为渲染系统提供了统一的接口来查询任何 UI 元素的渲染信息，实现了渲染逻辑与 UI 结构的解耦。

### Q: GetRenderBounds() 会改变元素吗？
**A:** 不会。它是只读操作，返回已经计算的布局结果。

### Q: 如何处理 GetVisualChildren() 返回空列表？
**A:** 这很正常，表示元素没有子元素（如 Button、TextBlock）。

### Q: HasRenderContent() 的返回值影响渲染吗？
**A:** 不直接影响，但可以用于优化。比如纯容器元素可以跳过绘制背景。

## 相关文档

- [UIElement.md](./UIElement.md) - UI 元素基类
- [FrameworkElement.md](./FrameworkElement.md) - 布局框架
- [RenderHost.md](../Render/RenderHost.md) - 渲染主机
