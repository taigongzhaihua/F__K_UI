# 流式API重构总结

## 概述

完成对 F__K_UI 框架所有UI元素的流式API统一,确保所有派生类都能使用链式调用风格。

## 重构原则

### 1. 分层设计模式

```
UIElement (基类,Get/Set方法)
  ↓
FrameworkElement (基类,Get/Set方法)
  ↓
  ├─ Panel (基类,Get/Set方法,不使用View模板)
  │    ↓
  │    └─ StackPanel : View<StackPanel, Panel> (流式API包装)
  │
  └─ ControlBase
       ↓
       Control<Derived> : View<Derived, ControlBase> (流式API)
         ↓
         ├─ ContentControl
         ├─ ItemsControl
         └─ Window
```

### 2. API设计规则

#### 基类 (UIElement, FrameworkElement, Panel)
- 使用 **Get/Set 前缀** 的标准方法
- 不使用 View 模板(避免多重继承冲突)
- 提供基础功能实现

```cpp
// 基类风格
class Panel : public FrameworkElement {
    void AddChild(std::shared_ptr<UIElement> child);
    std::span<const std::shared_ptr<UIElement>> GetChildren() const;
};
```

#### 派生类 (通过 View 模板)
- 继承 `View<Derived, Base>` 模板自动获得流式API
- **方法重载模式**: 无参getter + 有参setter返回Ptr
- 包装基类方法提供流式API

```cpp
// 派生类风格 - 完整的 Getter/Setter 重载
class StackPanel : public View<StackPanel, Panel> {
    // Getter: 无参重载,返回属性值
    ui::Orientation Orientation() const;
    
    // Setter: 有参重载,返回 shared_ptr<StackPanel> 支持链式调用
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);
    
    // 流式API: 包装基类方法
    std::shared_ptr<StackPanel> AddChild(std::shared_ptr<UIElement> child) {
        Panel::AddChild(std::move(child));
        return Self();  // View模板提供的Self()方法
    }
};

// View 模板自动提供的 Getter/Setter 重载
auto panel = std::make_shared<StackPanel>();

// Getter 调用 (无参)
float w = panel->Width();           // 返回 float
float h = panel->Height();          // 返回 float  
float op = panel->Opacity();        // 返回 float
auto orient = panel->Orientation(); // 返回 ui::Orientation

// Setter 调用 (有参,返回 Ptr)
panel->Width(800)                   // 返回 shared_ptr<StackPanel>
     ->Height(600)                  // 返回 shared_ptr<StackPanel>
     ->Opacity(0.9)                 // 返回 shared_ptr<StackPanel>
     ->Orientation(Orientation::Vertical);  // 返回 shared_ptr<StackPanel>
```

### 3. View 模板的作用

View 模板为所有派生类自动提供完整的 **Getter/Setter 重载**:

#### 自动提供的流式API属性
- **布局属性**: `Width()`, `Height()`, `MinWidth()`, `MinHeight()`, `MaxWidth()`, `MaxHeight()`
- **对齐属性**: `HorizontalAlignment()`, `VerticalAlignment()`, `Margin()`
- **视觉属性**: `Opacity()`, `Visibility()`, `IsEnabled()`
- **数据属性**: `DataContext()`
- **辅助方法**: `Self()` - 返回正确类型的 shared_ptr

#### Getter/Setter 重载机制
```cpp
// View 模板中的实现模式
template <typename Derived, typename Base>
class View : public Base, public std::enable_shared_from_this<Derived> {
    // Getter: 无参重载,返回属性值
    [[nodiscard]] float Width() const { 
        return Base::GetWidth(); 
    }
    
    // Setter: 有参重载,返回 Ptr 支持链式调用
    Ptr Width(float value) {
        Base::SetWidth(value);
        return Self();  // 返回 shared_ptr<Derived>
    }
    
    // 所有属性都遵循相同模式...
};
```

#### 使用示例
```cpp
// 创建派生类实例
auto panel = std::make_shared<StackPanel>();

// Getter 调用 - 编译器根据无参选择getter重载
float currentWidth = panel->Width();     // 返回 float
float currentHeight = panel->Height();   // 返回 float
auto visibility = panel->Visibility();   // 返回 ui::Visibility

// Setter 调用 - 编译器根据有参选择setter重载,支持链式调用
panel->Width(800)          // 返回 shared_ptr<StackPanel>
     ->Height(600)         // 返回 shared_ptr<StackPanel>
     ->Opacity(0.8)        // 返回 shared_ptr<StackPanel>
     ->Visibility(ui::Visibility::Visible);  // 返回 shared_ptr<StackPanel>

// 混合使用 getter 和 setter
if (panel->Width() < 1024) {           // getter: 读取当前值
    panel->Width(1024)->Height(768);   // setter: 设置新值并链式调用
}
```

## 实现细节

### StackPanel 流式API包装

#### 头文件 (StackPanel.h)
```cpp
class StackPanel : public View<StackPanel, Panel> {
public:
    using View::View;  // 继承View构造函数
    
    // Getter/Setter 重载: 自有属性
    [[nodiscard]] ui::Orientation Orientation() const noexcept { 
        return orientation_; 
    }
    std::shared_ptr<StackPanel> Orientation(ui::Orientation orientation);
    
    // 流式API: 包装基类方法
    std::shared_ptr<StackPanel> AddChild(std::shared_ptr<UIElement> child) {
        Panel::AddChild(std::move(child));
        return Self();  // View模板的辅助方法
    }
    
    std::shared_ptr<StackPanel> RemoveChild(UIElement* child) {
        Panel::RemoveChild(child);
        return Self();
    }
    
    std::shared_ptr<StackPanel> ClearChildren() {
        Panel::ClearChildren();
        return Self();
    }
    
    // View 模板自动提供:
    // - float Width() const
    // - Ptr Width(float)
    // - float Height() const  
    // - Ptr Height(float)
    // - float Opacity() const
    // - Ptr Opacity(float)
    // - ui::Visibility Visibility() const
    // - Ptr Visibility(ui::Visibility)
    // ... 等等所有 FrameworkElement 属性
    
protected:
    Size MeasureOverride(const Size& availableSize) override;
    Size ArrangeOverride(const Size& finalSize) override;

private:
    ui::Orientation orientation_{ui::Orientation::Vertical};
};
```

#### 实现文件 (StackPanel.cpp)
```cpp
std::shared_ptr<StackPanel> StackPanel::Orientation(ui::Orientation orientation) {
    if (orientation_ == orientation) {
        return Self();  // View模板提供,返回shared_ptr<StackPanel>
    }
    orientation_ = orientation;
    InvalidateMeasure();
    InvalidateArrange();
    return Self();
}
```

### 为什么 Panel 不使用 View 模板?

**问题**: 如果 Panel 也使用 View<Panel, FrameworkElement>,会导致:
```cpp
Panel : View<Panel, FrameworkElement>
  ↓ (enable_shared_from_this<Panel>)
StackPanel : View<StackPanel, Panel>
  ↓ (enable_shared_from_this<StackPanel>)
```

这会造成 **多重继承歧义** - StackPanel 同时继承两个 `enable_shared_from_this`,编译器无法确定使用哪个。

**解决方案**: 基类不使用 View 模板,只有最终派生类使用:
```cpp
Panel : FrameworkElement  // 不使用View,无enable_shared_from_this
  ↓
StackPanel : View<StackPanel, Panel>  // 只有这一层有enable_shared_from_this
```

## 使用示例

### 基本 Getter/Setter 重载
```cpp
auto panel = std::make_shared<StackPanel>();

// Setter: 有参重载,支持链式调用
panel->Orientation(ui::Orientation::Vertical)
     ->Width(800)
     ->Height(600)
     ->Opacity(0.9);

// Getter: 无参重载,读取属性值
auto orient = panel->Orientation();  // 返回 ui::Orientation
float w = panel->Width();            // 返回 float
float h = panel->Height();           // 返回 float
float op = panel->Opacity();         // 返回 float

std::cout << "Panel: " << w << "x" << h 
          << ", Opacity: " << op << std::endl;
```

### 混合 Getter/Setter 使用
```cpp
// 根据当前值条件设置
if (panel->Width() < 1024) {
    panel->Width(1024)->Height(768);
}

// 基于现有值修改
panel->Opacity(panel->Opacity() * 0.5);  // 半透明

// 切换状态
auto currentOrientation = panel->Orientation();
panel->Orientation(
    currentOrientation == Orientation::Vertical 
        ? Orientation::Horizontal 
        : Orientation::Vertical
);
```

### 完整链式调用示例
```cpp
auto panel = std::make_shared<StackPanel>();
panel->Orientation(ui::Orientation::Vertical)  // StackPanel自有方法
     ->Width(800)                               // View模板提供
     ->Height(600)                              // View模板提供
     ->Opacity(0.9)                             // View模板提供
     ->Visibility(ui::Visibility::Visible)      // View模板提供
     ->AddChild(child1)                         // StackPanel包装的Panel方法
     ->AddChild(child2)
     ->AddChild(child3);

// 验证设置
std::cout << "Panel configured: " 
          << panel->Width() << "x" << panel->Height()
          << ", Children: " << panel->GetChildCount()
          << std::endl;
```

### 与Window集成
```cpp
auto window = ui::window()
    ->Title("Fluent API Demo")
    ->Width(800)
    ->Height(600);

auto panel = std::make_shared<StackPanel>();
panel->Orientation(ui::Orientation::Vertical)
     ->AddChild(std::make_shared<ui::ContentControl>())
     ->AddChild(std::make_shared<ui::ContentControl>());

window->Content(panel);
```

## 技术要点

### Self() 方法的重要性
View 模板提供的 `Self()` 方法解决类型转换问题:
```cpp
protected:
    Ptr Self() {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }
```

- 自动转换为正确的派生类类型
- 支持链式调用的类型安全
- 避免手动 `static_pointer_cast` 的重复代码

### shared_from_this 使用条件
必须满足:
1. 类继承 `std::enable_shared_from_this<T>`
2. 对象通过 `std::make_shared` 或 `std::shared_ptr` 创建
3. 不能在构造函数中调用(对象未完全构造)

**正确**:
```cpp
auto panel = std::make_shared<StackPanel>();  // shared_ptr管理
panel->AddChild(child);  // OK: 在方法中调用shared_from_this()
```

**错误**:
```cpp
StackPanel::StackPanel() {
    Self();  // ❌ 构造函数中调用 - 抛出bad_weak_ptr
}
```

## 修改的文件

### 头文件
1. `include/fk/ui/Panel.h` - 保持基类风格,Get/Set方法
2. `include/fk/ui/StackPanel.h` - 继承View,添加流式API包装

### 实现文件
1. `src/ui/Panel.cpp` - 标准setter实现,不返回this
2. `src/ui/StackPanel.cpp` - 流式API实现,使用Self()返回

### 示例文件
1. `examples/main.cpp` - 展示流式API链式调用
2. `examples/main_new.cpp` - 更新为流式API
3. `examples/test_window.cpp` - 更新为流式API
4. `examples/phase2_demo.cpp` - 更新为流式API链式调用

## 编译和测试

### 编译结果
```bash
cmake --build build --config Debug
# ✅ 所有目标编译成功
```

### 测试结果
```bash
# 测试1: 流式API基本功能
.\build\fk_example.exe
# ✅ 成功: 链式调用正常,窗口创建成功

# 测试2: 自动失效跟踪
.\build\phase2_demo.exe
# ✅ 成功: 流式API + 自动渲染失效工作正常
```

## API一致性总结

### 当前架构
| 类 | 继承 | API风格 | 流式API来源 |
|---|---|---|---|
| UIElement | FrameworkElement | Get/Set | - |
| FrameworkElement | UIElement | Get/Set | - |
| Panel | FrameworkElement | Get/Set | - |
| StackPanel | View<StackPanel, Panel> | **流式API** | 继承View + 自定义包装 |
| ContentControl | Control<ContentControl> | **流式API** | 继承View (通过Control) |
| Window | ContentControl | **流式API + 重载** | 继承View + 自定义重载 |

### 设计哲学

1. **基类简单**: 基础类(UIElement, Panel)使用标准Get/Set,保持简洁
2. **派生类强大**: 通过View模板,派生类自动获得流式API能力
3. **按需包装**: 派生类可以选择性地包装基类方法为流式API
4. **类型安全**: Self()方法确保链式调用的类型正确性

### 优势

✅ **一致性**: 所有终端类(用户直接使用的类)都有流式API  
✅ **灵活性**: 基类保持简单,派生类按需扩展  
✅ **可扩展**: 添加新类只需继承View模板即可获得完整流式API  
✅ **类型安全**: 编译期检查,无运行时类型转换错误  
✅ **向后兼容**: 基类Get/Set方法仍可独立使用  

## 未来扩展

### 添加新Panel派生类
```cpp
class GridPanel : public View<GridPanel, Panel> {
public:
    using View::View;
    
    // 自有流式API
    std::shared_ptr<GridPanel> Rows(int count) {
        rows_ = count;
        return Self();
    }
    
    // 包装基类方法
    std::shared_ptr<GridPanel> AddChild(std::shared_ptr<UIElement> child) {
        Panel::AddChild(std::move(child));
        return Self();
    }
    
    // 自动继承View提供的: Width(), Height(), Opacity()等
};
```

### 使用模式
```cpp
auto grid = std::make_shared<GridPanel>();
grid->Rows(3)           // GridPanel自有方法
    ->Width(800)        // View模板提供
    ->AddChild(child1)  // GridPanel包装的Panel方法
    ->AddChild(child2);
```

## 结论

通过分层设计和View模板,成功实现了:
- ✅ 统一的流式API风格
- ✅ 类型安全的链式调用
- ✅ 避免多重继承冲突
- ✅ 保持代码简洁和可维护性

所有UI元素及其派生类现在都支持一致的流式API风格! 🎉
