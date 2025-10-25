# 依赖属性重构方案

## 问题分析

### 当前状态

#### Window 类
```cpp
class Window : public ContentControl {
private:
    std::string title_;      // ❌ 普通成员变量
    int width_;              // ❌ 普通成员变量
    int height_;             // ❌ 普通成员变量
    bool visible_;           // ❌ 普通成员变量
};
```

#### StackPanel 类
```cpp
class StackPanel : public View<StackPanel, Panel> {
private:
    ui::Orientation orientation_{ui::Orientation::Vertical};  // ❌ 普通成员变量
};
```

#### Panel 类
```cpp
class Panel : public FrameworkElement {
private:
    std::vector<std::shared_ptr<UIElement>> children_;  // ✅ 子元素集合,不需要是依赖属性
};
```

### 问题

1. **不一致性**: FrameworkElement 的 Width/Height 是 DependencyProperty,但 Window 的 Width/Height 是普通变量
2. **无法绑定**: 这些属性不支持数据绑定
3. **无变更通知**: 属性改变不会触发依赖属性系统的通知机制
4. **无样式支持**: 不能通过样式系统设置这些属性

---

## 重构方案

### 方案1: 所有独有属性改为 DependencyProperty (推荐)

#### StackPanel - Orientation 属性

**修改前**:
```cpp
class StackPanel : public View<StackPanel, Panel> {
private:
    ui::Orientation orientation_{ui::Orientation::Vertical};
public:
    ui::Orientation Orientation() const noexcept { return orientation_; }
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);
};
```

**修改后**:
```cpp
class StackPanel : public View<StackPanel, Panel> {
public:
    // 依赖属性声明
    static const binding::DependencyProperty& OrientationProperty();
    
    // Getter/Setter (委托给依赖属性系统)
    ui::Orientation Orientation() const {
        return std::any_cast<ui::Orientation>(GetValue(OrientationProperty()));
    }
    
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation) {
        SetValue(OrientationProperty(), orientation);
        return Self();
    }
    
private:
    // 移除成员变量
    // ui::Orientation orientation_;  // 删除
    
    // 属性元数据构建
    static binding::PropertyMetadata BuildOrientationMetadata();
    
    // 属性变更回调
    static void OrientationPropertyChanged(
        binding::DependencyObject& sender,
        const binding::DependencyProperty& property,
        const std::any& oldValue,
        const std::any& newValue
    );
};
```

**实现文件** (StackPanel.cpp):
```cpp
#include "fk/ui/StackPanel.h"

namespace fk::ui {

// 依赖属性定义
const binding::DependencyProperty& StackPanel::OrientationProperty() {
    static const auto prop = binding::DependencyProperty::Register(
        "Orientation",
        typeid(ui::Orientation),
        typeid(StackPanel),
        BuildOrientationMetadata()
    );
    return prop;
}

binding::PropertyMetadata StackPanel::BuildOrientationMetadata() {
    return binding::PropertyMetadata(
        ui::Orientation::Vertical,  // 默认值
        &StackPanel::OrientationPropertyChanged
    );
}

void StackPanel::OrientationPropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* panel = dynamic_cast<StackPanel*>(&sender);
    if (!panel) return;
    
    // 触发布局更新
    panel->InvalidateMeasure();
    panel->InvalidateArrange();
}

// 旧的 Orientation setter 实现改为委托
std::shared_ptr<StackPanel> StackPanel::Orientation(ui::Orientation orientation) {
    SetValue(OrientationProperty(), orientation);
    return Self();
}

} // namespace fk::ui
```

---

#### Window - Title/Width/Height 属性

**修改前**:
```cpp
class Window : public ContentControl {
private:
    std::string title_;
    int width_;
    int height_;
public:
    std::string Title() const;
    Ptr Title(const std::string& title);
    int Width() const;
    Ptr Width(int w);
    int Height() const;
    Ptr Height(int h);
};
```

**修改后**:
```cpp
class Window : public ContentControl {
public:
    // 依赖属性声明
    static const binding::DependencyProperty& TitleProperty();
    static const binding::DependencyProperty& WindowWidthProperty();   // 注意命名
    static const binding::DependencyProperty& WindowHeightProperty();  // 注意命名
    
    // Getter/Setter (委托给依赖属性系统)
    std::string Title() const {
        return std::any_cast<std::string>(GetValue(TitleProperty()));
    }
    
    Ptr Title(const std::string& title) {
        SetValue(TitleProperty(), title);
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
    // 窗口大小 (区别于布局 Width/Height)
    int WindowWidth() const {
        return std::any_cast<int>(GetValue(WindowWidthProperty()));
    }
    
    Ptr WindowWidth(int w) {
        SetValue(WindowWidthProperty(), w);
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
    int WindowHeight() const {
        return std::any_cast<int>(GetValue(WindowHeightProperty()));
    }
    
    Ptr WindowHeight(int h) {
        SetValue(WindowHeightProperty(), h);
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
private:
    // 移除成员变量
    // std::string title_;  // 删除
    // int width_;          // 删除
    // int height_;         // 删除
    
    static binding::PropertyMetadata BuildTitleMetadata();
    static binding::PropertyMetadata BuildWindowWidthMetadata();
    static binding::PropertyMetadata BuildWindowHeightMetadata();
    
    static void TitlePropertyChanged(/*...*/);
    static void WindowSizePropertyChanged(/*...*/);
};
```

**实现文件** (Window.cpp):
```cpp
const binding::DependencyProperty& Window::TitleProperty() {
    static const auto prop = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        BuildTitleMetadata()
    );
    return prop;
}

binding::PropertyMetadata Window::BuildTitleMetadata() {
    return binding::PropertyMetadata(
        std::string("Window"),  // 默认值
        &Window::TitlePropertyChanged
    );
}

void Window::TitlePropertyChanged(
    binding::DependencyObject& sender,
    const binding::DependencyProperty& property,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&sender);
    if (!window || !window->interopHelper_) return;
    
    const auto& title = std::any_cast<const std::string&>(newValue);
    window->interopHelper_->SetTitle(title);
}

// WindowWidth/WindowHeight 类似实现...
```

---

### 方案2: 混合模式 (部分属性改为 DependencyProperty)

仅将**需要绑定支持**的属性改为 DependencyProperty:

**需要改为 DP**:
- ✅ StackPanel.Orientation - 需要样式/模板支持
- ✅ Window.Title - 可能需要绑定到应用标题
- ⚠️ Window.Width/Height - 窗口大小可能不需要绑定

**保持普通变量**:
- ❌ Window.visible_ - 内部状态
- ❌ Panel.children_ - 集合管理

---

## 命名约定调整

### Window Width/Height 命名冲突解决

**问题**: Window 需要两种 Width/Height:
1. **窗口大小** (int,像素) - 操作系统窗口尺寸
2. **布局大小** (float) - 继承自 FrameworkElement

**解决方案A**: 明确命名区分
```cpp
class Window : public ContentControl {
    // 窗口大小 (操作系统级别)
    static const binding::DependencyProperty& WindowWidthProperty();
    static const binding::DependencyProperty& WindowHeightProperty();
    
    int WindowWidth() const;
    Ptr WindowWidth(int);
    int WindowHeight() const;
    Ptr WindowHeight(int);
    
    // 布局大小继承自 FrameworkElement::Width/Height (float)
    // 通过 ContentControl -> Control -> View 模板访问
};
```

**解决方案B**: 使用 Width/Height 作为窗口大小,隐藏布局属性
```cpp
class Window : public ContentControl {
    // Width/Height = 窗口大小 (隐藏 FrameworkElement 的版本)
    static const binding::DependencyProperty& WidthProperty();  // 新定义,覆盖基类
    int Width() const;
    Ptr Width(int);
    
    // 访问布局宽度需要显式
    float LayoutWidth() const { return FrameworkElement::GetWidth(); }
};
```

**推荐**: 方案A,更清晰

---

## 实现步骤

### 步骤1: 重构 StackPanel.Orientation

1. 添加 `OrientationProperty()` 静态方法
2. 实现属性元数据和变更回调
3. 修改 Getter 从依赖属性读取
4. 修改 Setter 写入依赖属性
5. 移除 `orientation_` 成员变量
6. 更新测试代码

**影响范围**: 低,StackPanel 使用较少

---

### 步骤2: 重构 Window 属性

1. 重命名方法:
   - `Width()` → `WindowWidth()`
   - `Height()` → `WindowHeight()`
2. 添加依赖属性定义
3. 修改 Getter/Setter 委托给依赖属性
4. 更新 WindowInteropHelper 集成
5. 移除成员变量
6. 更新所有示例代码

**影响范围**: 高,需要更新所有使用 Window 的代码

---

### 步骤3: 更新 View 模板 (可选)

如果需要 View 模板自动包装依赖属性:
```cpp
template <typename Derived, typename Base>
class View : public Base, public std::enable_shared_from_this<Derived> {
    // 当前实现: 直接调用 Base::SetXxx
    Ptr Width(float value) {
        Base::SetWidth(value);  // 内部调用 SetValue
        return Self();
    }
    
    // 已经支持依赖属性,无需修改
};
```

---

## 优势对比

### 使用 DependencyProperty

**优势**:
- ✅ 统一的属性系统
- ✅ 支持数据绑定
- ✅ 属性变更通知
- ✅ 样式和模板支持
- ✅ 属性元数据和验证
- ✅ 属性继承

**劣势**:
- ❌ 实现复杂度增加
- ❌ 轻微性能开销 (any_cast)
- ❌ 更多样板代码

### 使用普通成员变量

**优势**:
- ✅ 简单直接
- ✅ 性能最优
- ✅ 类型安全(编译期)

**劣势**:
- ❌ 无法绑定
- ❌ 手动通知
- ❌ 与框架不一致

---

## 建议优先级

### 🔴 高优先级
1. **StackPanel.Orientation** → DependencyProperty
   - 影响小
   - 典型示例
   - 需要样式支持

### 🟡 中优先级
2. **Window.Title** → DependencyProperty
   - 可能需要绑定
   - API 简单

### 🟢 低优先级
3. **Window.Width/Height** → DependencyProperty
   - 需要重命名 (WindowWidth/WindowHeight)
   - 影响范围大
   - 可以延后

---

## 迁移路径

### 向后兼容策略

**方案**: 保留旧方法,标记为 deprecated
```cpp
class StackPanel : public View<StackPanel, Panel> {
public:
    // 新 API (依赖属性)
    static const binding::DependencyProperty& OrientationProperty();
    ui::Orientation Orientation() const;
    std::shared_ptr<StackPanel> Orientation(ui::Orientation);
    
    // 旧 API (deprecated,调用新 API)
    [[deprecated("Use Orientation() property instead")]]
    void SetOrientation(ui::Orientation orientation) {
        Orientation(orientation);
    }
    
    [[deprecated("Use Orientation() property instead")]]
    ui::Orientation GetOrientation() const {
        return Orientation();
    }
};
```

---

## 示例对比

### 修改前
```cpp
auto panel = std::make_shared<StackPanel>();
panel->Orientation(Orientation::Vertical)  // 设置成员变量
     ->Width(800);                          // 设置依赖属性

// 无法绑定
```

### 修改后
```cpp
auto panel = std::make_shared<StackPanel>();
panel->Orientation(Orientation::Vertical)  // 设置依赖属性
     ->Width(800);                          // 设置依赖属性

// 支持绑定
panel->SetBinding(StackPanel::OrientationProperty(), myBinding);

// 支持样式
style->SetSetter(StackPanel::OrientationProperty(), Orientation::Horizontal);
```

---

## 结论

将 Window、StackPanel 等类的独有属性改为 **DependencyProperty** 是架构上更一致、更强大的方案。

**推荐实施**:
1. 先重构 **StackPanel.Orientation** (低风险,高收益)
2. 再重构 **Window.Title** (中等收益)
3. 最后考虑 **Window.Width/Height** (需要谨慎设计命名)

这样可以逐步提升框架的一致性和功能完整性。
