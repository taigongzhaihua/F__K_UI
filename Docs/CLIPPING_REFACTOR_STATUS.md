# 裁剪系统重构说明

## 当前实现状态

### 1. 裁剪区域交集 ✅ 已实现

`RenderContext::PushClip` 已经正确实现了裁剪区域交集：

```cpp
void RenderContext::PushClip(const ui::Rect& clipRect) {
    // 保存当前状态
    clipStack_.push(currentClip_);
    
    // 变换裁剪矩形到全局坐标
    ui::Rect globalClip = TransformRect(clipRect);
    
    // 与当前裁剪求交集 ✅
    if (currentClip_.enabled) {
        float x1 = std::max(currentClip_.clipRect.x, globalClip.x);
        float y1 = std::max(currentClip_.clipRect.y, globalClip.y);
        float x2 = std::min(
            currentClip_.clipRect.x + currentClip_.clipRect.width,
            globalClip.x + globalClip.width
        );
        float y2 = std::min(
            currentClip_.clipRect.y + currentClip_.clipRect.height,
            globalClip.y + globalClip.height
        );
        
        currentClip_.clipRect = ui::Rect{x1, y1, std::max(0.0f, x2 - x1), std::max(0.0f, y2 - y1)};
    } else {
        currentClip_.clipRect = globalClip;
    }
    
    currentClip_.enabled = true;
    ApplyCurrentClip();
}
```

**工作流程**：
1. 父元素调用`PushClip(parentClipRect)` → 设置父裁剪区域
2. 子元素调用`PushClip(childClipRect)` → 自动与父裁剪区域求交集
3. 孙元素调用`PushClip(grandChildClipRect)` → 再次与当前裁剪区域（已经是父+子的交集）求交集

### 2. Panel宽度约束问题 ⚠️ 需要讨论

#### 当前行为

```cpp
// StackPanel.cpp MeasureOverride
Size childAvailable = availableSize;

if (orientation == Orientation::Vertical) {
    // 垂直堆叠：高度无限，宽度受限
    childAvailable.height = std::numeric_limits<float>::infinity();
} else {
    // 水平堆叠：宽度无限，高度受限
    childAvailable.width = std::numeric_limits<float>::infinity();
}

// 子元素会收到这个约束
child->Measure(childAvailable);
```

**问题场景**：

```cpp
auto panel = std::make_shared<StackPanel>();
panel->SetWidth(300);  // 父元素宽度300

auto border = std::make_shared<Border>();
border->SetWidth(400);  // 子元素显式设置宽度400
```

#### WPF行为

在WPF中：
- **显式Width/Height会覆盖父元素的约束**
- 但是在**Arrange阶段**，子元素仍然只能在父元素提供的finalSize内布局
- 如果父元素设置了`ClipToBounds=true`，超出部分会被裁剪

#### 当前F__K_UI行为

- Measure阶段：子元素的显式Width/Height会被使用
- Arrange阶段：子元素会被arrange到其DesiredSize
- 如果父元素`ClipToBounds=true`：超出部分**应该**被裁剪（需要测试验证）

### 3. 需要验证的场景

创建测试用例验证以下场景：

#### 场景A：父元素ClipToBounds=true，子元素超出
```cpp
panel->SetWidth(300);
panel->SetClipToBounds(true);

border->SetWidth(400);  // 超出父元素
panel->AddChild(border);
```

**期望结果**：
- border测量时desiredSize.width = 400
- border arrange时layoutRect.width = 400（或者被限制到300？）
- 渲染时，超出300的部分被裁剪

#### 场景B：父元素ClipToBounds=false，子元素超出
```cpp
panel->SetWidth(300);
// panel->SetClipToBounds(false);  // 默认

border->SetWidth(400);
panel->AddChild(border);
```

**期望结果**：
- border测量时desiredSize.width = 400
- border arrange时layoutRect.width = 400
- 渲染时，超出300的部分**不被裁剪**，显示溢出

#### 场景C：嵌套裁剪区域交集
```cpp
panel1->SetWidth(500);
panel1->SetClipToBounds(true);

  panel2->SetWidth(400);
  panel2->SetClipToBounds(true);
  
    border->SetWidth(450);
```

**期望结果**：
- border的有效显示区域 = min(500, 400) = 400

## 建议的修改方案

### 方案1：保持当前设计（推荐）

**理由**：
- 裁剪交集已经正确实现
- 布局约束传递符合WPF行为（显式大小优先）
- 只需要确保ClipToBounds工作正常

**需要做的**：
- 验证ClipToBounds是否正确裁剪超出内容
- 添加测试用例

### 方案2：强制子元素不能超出父元素

**修改点**：在Panel的ArrangeOverride中限制子元素大小

```cpp
Size StackPanel::ArrangeOverride(const Size& finalSize) {
    for (auto* child : children_) {
        Size childDesired = child->GetDesiredSize();
        
        // 强制限制子元素不能超出容器
        float childWidth = std::min(childDesired.width, finalSize.width);
        float childHeight = std::min(childDesired.height, finalSize.height);
        
        child->Arrange(Rect(x, y, childWidth, childHeight));
    }
}
```

**缺点**：
- 违背WPF行为（WPF允许子元素超出arrange bounds）
- 与显式大小的语义冲突
- 不建议采用

## 结论

**当前实现是正确的**！

1. ✅ 裁剪区域交集：已经在`RenderContext::PushClip`中实现
2. ✅ 布局约束传递：遵循WPF语义
3. ⚠️ 需要验证：ClipToBounds是否正确工作

建议运行测试程序验证ClipToBounds功能是否正常。
