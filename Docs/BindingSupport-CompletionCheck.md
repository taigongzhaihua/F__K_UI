# 绑定支持完整性检查报告

## 已完成的控件 ✅

### 1. **View 基类** - 12 个属性 ✅
- Width, Height, MinWidth, MaxWidth, MinHeight, MaxHeight
- HorizontalAlignment, VerticalAlignment, Margin
- Visibility, IsEnabled, Opacity
- **状态**：使用宏优化，所有属性支持绑定

### 2. **Control 基类** - 5 个属性 ✅
- IsFocused, TabIndex, Cursor, Content, Padding
- **状态**：使用宏，所有属性支持绑定

### 3. **Panel 基类** - 1 个属性 ✅
- Children
- **状态**：手动实现绑定支持

### 4. **TextBlock** - 6 个属性 ✅
- Text, FontSize, Foreground, FontFamily, TextWrapping, TextTrimming
- **状态**：使用宏，所有属性支持绑定

### 5. **Button** - 7 个属性 ✅
- CornerRadius, Background, Foreground, HoveredBackground, PressedBackground, BorderBrush, BorderThickness
- **状态**：使用宏，所有属性支持绑定

### 6. **StackPanel** - 2 个属性 ✅
- Orientation, Spacing
- **状态**：手动实现绑定支持

### 7. **ScrollBar** - 8 个属性 ✅
- Minimum, Maximum, Value, ViewportSize, Orientation, ThumbBrush, TrackBrush, Thickness
- **状态**：使用宏，所有属性支持绑定

### 8. **ScrollViewer** - 4 个属性 ✅
- HorizontalOffset, VerticalOffset, HorizontalScrollBarVisibility, VerticalScrollBarVisibility
- **状态**：使用宏，所有属性支持绑定

---

## 需要添加绑定支持的控件 ⚠️

### 1. **ItemsControl** - 4 个属性 ⚠️

**继承关系**：`ItemsControl : public Control<ItemsControl>`

**依赖属性**：
- ✅ **已有基类支持**：继承 Control 基类的 5 个属性 + View 基类的 12 个属性
- ❌ **缺少绑定支持**：
  1. `ItemsSource` - `std::vector<std::any>` - 数据源
  2. `ItemTemplate` - `ItemTemplateFunc` - 项模板函数
  3. `DisplayMemberPath` - `std::string` - 显示成员路径
  4. `AlternationCount` - `int` - 交替计数

**需要的操作**：
- 添加 4 个属性的绑定支持
- ItemsSource 和 DisplayMemberPath 可以使用宏
- ItemTemplate 是函数类型，需要特殊处理

**优先级**：⭐⭐⭐ 高（常用控件）

---

### 2. **Window** - 3 个属性 ⚠️

**继承关系**：`Window : public ContentControl`

**依赖属性**：
- ✅ **已有基类支持**：继承 Control 基类的 5 个属性 + View 基类的 12 个属性
- ❌ **缺少绑定支持**：
  1. `Title` - `std::string` - 窗口标题
  2. `Width` - `int` - 窗口宽度（注意：与 FrameworkElement 的 float Width 不同）
  3. `Height` - `int` - 窗口高度（注意：与 FrameworkElement 的 float Height 不同）

**特殊情况**：
- Window 的 Width/Height 是 `int` 类型（窗口像素）
- FrameworkElement 的 Width/Height 是 `float` 类型（布局大小）
- 这是两个不同的属性，需要小心处理命名冲突

**需要的操作**：
- 添加 Title 绑定支持（可以使用宏）
- Width/Height 需要手动实现（类型冲突）

**优先级**：⭐⭐ 中（顶级窗口，但通常不需要动态绑定）

---

### 3. **ContentControl** - 无特有属性 ✅

**继承关系**：`ContentControl : public Control<ContentControl>`

**状态**：
- ✅ 继承 Control 基类的所有属性（包括 Content）
- ✅ 无需额外工作

---

### 4. **Decorator** - 1 个属性 ⚠️

**继承关系**：`Decorator : public FrameworkElement`（非模板类）

**属性**：
- `Child` - `std::shared_ptr<UIElement>` - 子元素

**状态**：
- ❌ 不继承 View 基类（直接继承 FrameworkElement）
- ❌ 不是模板类，无法使用 View 的链式 API
- ❌ 没有绑定支持

**需要的操作**：
- 要么：将 Decorator 改为模板类 `Decorator : public View<Decorator>`
- 要么：手动添加绑定支持
- 优先级较低，可能不常用

**优先级**：⭐ 低（不常用）

---

## 总结

### 当前状态

| 类别 | 支持绑定 | 不支持绑定 | 总计 |
|------|----------|------------|------|
| **基类** | 3 | 0 | 3 |
| **控件** | 6 | 3 | 9 |
| **总计** | 9 | 3 | 12 |

**绑定属性统计**：
- ✅ 已支持：45 个属性
- ⚠️ 待添加：8 个属性（ItemsControl 4 + Window 3 + Decorator 1）
- 📊 完成度：**45 / 53 = 85%**

---

## 推荐优先级

### 高优先级 ⭐⭐⭐

**ItemsControl** - 常用控件，需要绑定 ItemsSource

```cpp
// 典型使用场景
class MyViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::vector<std::any>, Items)
    FK_PROPERTY(std::string, DisplayPath)
};

auto itemsControl = ui::itemsControl()
    ->ItemsSource(bind("Items"))           // ❌ 当前不支持
    ->DisplayMemberPath(bind("DisplayPath")); // ❌ 当前不支持
```

### 中优先级 ⭐⭐

**Window** - 窗口属性，但通常在构造时设置，不常动态绑定

```cpp
auto window = ui::window()
    ->Title(bind("WindowTitle"))  // ❌ 当前不支持
    ->Width(bind("WindowWidth"))  // ❌ 当前不支持（命名冲突）
    ->Height(bind("WindowHeight")); // ❌ 当前不支持（命名冲突）
```

### 低优先级 ⭐

**Decorator** - 不常用，且结构需要重构

---

## 建议的实现方案

### 方案 1：优先实现 ItemsControl

```cpp
// 在 ItemsControl.h 中添加：
#include "fk/ui/BindingMacros.h"

class ItemsControl : public Control<ItemsControl> {
public:
    // ... 现有代码 ...
    
    // 🎯 添加绑定支持
    
    // ItemsSource - 手动实现（std::vector<std::any> 特殊类型）
    [[nodiscard]] const std::vector<std::any>& ItemsSource() const {
        return GetItemsSource();
    }
    
    Ptr ItemsSource(std::vector<std::any> source) {
        SetItemsSource(std::move(source));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Ptr ItemsSource(binding::Binding binding) {
        SetBinding(ItemsSourceProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // DisplayMemberPath - 手动实现（string_view -> string）
    [[nodiscard]] std::string DisplayMemberPath() const {
        return GetDisplayMemberPath();
    }
    
    Ptr DisplayMemberPath(std::string_view path) {
        SetDisplayMemberPath(path);
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Ptr DisplayMemberPath(binding::Binding binding) {
        SetBinding(DisplayMemberPathProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // AlternationCount - 可以考虑使用宏（但需要添加 getter）
    [[nodiscard]] int AlternationCount() const {
        return GetAlternationCount();
    }
    
    Ptr AlternationCount(int count) {
        SetAlternationCount(count);
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Ptr AlternationCount(binding::Binding binding) {
        SetBinding(AlternationCountProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    // ItemTemplate - 手动实现（函数类型）
    [[nodiscard]] ItemTemplateFunc ItemTemplate() const {
        return GetItemTemplate();
    }
    
    Ptr ItemTemplate(ItemTemplateFunc templateFunc) {
        SetItemTemplate(std::move(templateFunc));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
    
    Ptr ItemTemplate(binding::Binding binding) {
        SetBinding(ItemTemplateProperty(), std::move(binding));
        return std::static_pointer_cast<ItemsControl>(shared_from_this());
    }
};
```

### 方案 2：Window 的特殊处理

Window 的 Width/Height 与基类冲突，需要特殊命名：

```cpp
class Window : public ContentControl {
public:
    // Title - 可以使用绑定
    Ptr Title(binding::Binding binding) {
        SetBinding(TitleProperty(), std::move(binding));
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
    // WindowWidth / WindowHeight - 避免与基类 Width/Height 冲突
    Ptr WindowWidth(binding::Binding binding) {
        SetBinding(WidthProperty(), std::move(binding));
        return std::static_pointer_cast<Window>(shared_from_this());
    }
    
    Ptr WindowHeight(binding::Binding binding) {
        SetBinding(HeightProperty(), std::move(binding));
        return std::static_pointer_cast<Window>(shared_from_this());
    }
};
```

或者干脆不为 Window 的 Width/Height 添加绑定支持，因为：
1. 窗口大小通常在创建时固定
2. 动态改变窗口大小不是常见需求
3. 与基类属性名冲突

---

## 结论

### ✅ 当前完成度：85%

- 已支持 45 个属性的绑定
- 还有 8 个属性待添加

### 🎯 建议行动

1. **立即实现**：ItemsControl 的 4 个属性（高优先级）
2. **可选实现**：Window 的 Title（中优先级）
3. **暂缓实现**：Window 的 Width/Height（命名冲突）、Decorator（低优先级）

### 📊 完成后统计

实现 ItemsControl 后：
- 支持属性：49 个
- 完成度：49 / 53 = **92.5%**

实现 Window.Title 后：
- 支持属性：50 个
- 完成度：50 / 53 = **94.3%**

**基本可以认为绑定系统已经完成！** 🎉
