# 所有控件绑定支持总览

## 已支持绑定的控件

### ✅ TextBlock - 文本显示控件

**所有 6 个属性支持绑定：**

```cpp
auto text = ui::textBlock()
    ->Text(bind("Name"))                    // 文本内容
    ->FontSize(bind("TitleSize"))           // 字体大小
    ->Foreground(bind("ThemeColor"))        // 前景色
    ->FontFamily(bind("Font"))              // 字体族
    ->TextWrapping(bind("WrapMode"))        // 换行模式
    ->TextTrimming(bind("TrimMode"));       // 截断模式
```

---

### ✅ Button - 按钮控件

**所有 7 个可配置属性支持绑定：**

```cpp
auto button = ui::button()
    ->Content("点击我")
    ->CornerRadius(bind("ButtonRadius"))         // 圆角半径
    ->Background(bind("ButtonColor"))            // 背景色
    ->Foreground(bind("TextColor"))              // 前景色（文字）
    ->HoveredBackground(bind("HoverColor"))      // 悬停背景色
    ->PressedBackground(bind("PressColor"))      // 按下背景色
    ->BorderBrush(bind("BorderColor"))           // 边框颜色
    ->BorderThickness(bind("BorderSize"));       // 边框粗细
```

**使用示例：**
```cpp
class ButtonViewModel : public fk::ObservableObject {
public:
    FK_PROPERTY(std::string, ButtonColor)
    FK_PROPERTY(std::string, TextColor)
    FK_PROPERTY(bool, IsEnabled)
};
FK_VIEWMODEL_AUTO(ButtonViewModel, ButtonColor, TextColor, IsEnabled)

// UI
auto button = ui::button()
    ->Content("动态按钮")
    ->Background(bind("ButtonColor"))    // ✅ 绑定背景色
    ->Foreground(bind("TextColor"));     // ✅ 绑定文字颜色
```

---

### ✅ StackPanel - 堆叠布局面板

**2 个布局属性支持绑定：**

```cpp
auto panel = ui::stackPanel()
    ->Orientation(bind("LayoutDirection"))  // 方向（水平/垂直）
    ->Spacing(bind("ItemSpacing"));         // 子元素间距
```

**使用示例：**
```cpp
class LayoutViewModel : public fk::ObservableObject {
public:
    FK_PROPERTY(ui::Orientation, Direction)
    FK_PROPERTY(float, Spacing)
};
FK_VIEWMODEL_AUTO(LayoutViewModel, Direction, Spacing)

auto panel = ui::stackPanel()
    ->Orientation(bind("Direction"))  // ✅ 动态切换横向/纵向
    ->Spacing(bind("Spacing"));       // ✅ 动态调整间距
```

---

### ✅ ScrollBar - 滚动条控件

**所有 8 个属性支持绑定：**

```cpp
auto scrollBar = ui::scrollBar()
    ->Minimum(bind("MinValue"))           // 最小值
    ->Maximum(bind("MaxValue"))           // 最大值
    ->Value(bind("CurrentValue"))         // 当前值
    ->ViewportSize(bind("ViewSize"))      // 可见区域大小
    ->Orientation(bind("Direction"))      // 方向
    ->ThumbBrush(bind("ThumbColor"))      // 滑块颜色
    ->TrackBrush(bind("TrackColor"))      // 轨道颜色
    ->Thickness(bind("BarThickness"));    // 滚动条厚度
```

**使用示例：**
```cpp
class ScrollViewModel : public fk::ObservableObject {
public:
    FK_PROPERTY(double, Minimum)
    FK_PROPERTY(double, Maximum)
    FK_PROPERTY(double, Value)
    FK_PROPERTY(double, ViewportSize)
};
FK_VIEWMODEL_AUTO(ScrollViewModel, Minimum, Maximum, Value, ViewportSize)

auto scrollBar = ui::scrollBar()
    ->Minimum(bind("Minimum"))        // ✅ 绑定范围
    ->Maximum(bind("Maximum"))        // ✅ 绑定范围
    ->Value(bind("Value"))            // ✅ 双向绑定滚动位置
    ->ViewportSize(bind("ViewportSize"));
```

---

### ✅ ScrollViewer - 滚动视图控件

**4 个可配置属性支持绑定：**

```cpp
auto scrollViewer = ui::scrollViewer()
    ->HorizontalOffset(bind("ScrollX"))                       // 水平偏移
    ->VerticalOffset(bind("ScrollY"))                         // 垂直偏移
    ->HorizontalScrollBarVisibility(bind("ShowHScroll"))      // 水平滚动条可见性
    ->VerticalScrollBarVisibility(bind("ShowVScroll"));       // 垂直滚动条可见性
```

**使用示例：**
```cpp
class ScrollViewModel : public fk::ObservableObject {
public:
    FK_PROPERTY(double, ScrollX)
    FK_PROPERTY(double, ScrollY)
    FK_PROPERTY(ui::ScrollBarVisibility, HScrollVis)
    FK_PROPERTY(ui::ScrollBarVisibility, VScrollVis)
};
FK_VIEWMODEL_AUTO(ScrollViewModel, ScrollX, ScrollY, HScrollVis, VScrollVis)

auto scrollViewer = ui::scrollViewer()
    ->HorizontalOffset(bind("ScrollX"))           // ✅ 绑定滚动位置
    ->VerticalOffset(bind("ScrollY"))             // ✅ 绑定滚动位置
    ->HorizontalScrollBarVisibility(bind("HScrollVis"))  // ✅ 动态显示/隐藏
    ->VerticalScrollBarVisibility(bind("VScrollVis"));   // ✅ 动态显示/隐藏
```

---

## 统计

### 已实现控件：5 个

| 控件 | 绑定属性数 | 状态 |
|------|-----------|------|
| TextBlock | 6 | ✅ 完整支持 |
| Button | 7 | ✅ 完整支持 |
| StackPanel | 2 | ✅ 完整支持 |
| ScrollBar | 8 | ✅ 完整支持 |
| ScrollViewer | 4 | ✅ 完整支持 |
| **总计** | **27** | **✅ 全部完成** |

---

## 使用模式

### 模式 1：直接值设置（传统方式）

```cpp
auto text = ui::textBlock()
    ->Text("固定文本")
    ->FontSize(24.0f);
```

### 模式 2：数据绑定（新方式）

```cpp
auto text = ui::textBlock()
    ->Text(bind("Name"))           // ✅ 绑定到 ViewModel
    ->FontSize(bind("TitleSize")); // ✅ 绑定到 ViewModel
```

### 模式 3：混合使用

```cpp
auto text = ui::textBlock()
    ->Text(bind("Name"))           // ✅ 绑定（动态）
    ->FontSize(24.0f)              // 固定值（静态）
    ->Foreground(bind("Color"));   // ✅ 绑定（动态）
```

---

## 完整示例

### 多控件绑定演示

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Button.h"
#include "fk/ui/ScrollBar.h"
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"

// ViewModel
class AppViewModel : public fk::ObservableObject {
public:
    AppViewModel() {
        propertyName_Title = "多控件绑定演示";
        propertyName_TitleSize = 28.0f;
        propertyName_SubTitle = "所有控件都支持数据绑定";
        propertyName_ButtonText = "点击我";
        propertyName_ButtonColor = "#FF4CAF50";
        propertyName_ScrollValue = 50.0;
        propertyName_PanelSpacing = 20.0f;
    }

    FK_PROPERTY(std::string, Title)
    FK_PROPERTY(float, TitleSize)
    FK_PROPERTY(std::string, SubTitle)
    FK_PROPERTY(std::string, ButtonText)
    FK_PROPERTY(std::string, ButtonColor)
    FK_PROPERTY(double, ScrollValue)
    FK_PROPERTY(float, PanelSpacing)
};
FK_VIEWMODEL_AUTO(AppViewModel, Title, TitleSize, SubTitle, ButtonText, ButtonColor, ScrollValue, PanelSpacing)

int main() {
    using namespace fk;
    
    Application app;
    auto viewModel = std::make_shared<AppViewModel>();
    
    // 创建 UI
    auto window = ui::window()
        ->Title("全控件绑定演示")
        ->Width(800)
        ->Height(600);
    
    auto panel = ui::stackPanel()
        ->Orientation(ui::Orientation::Vertical)
        ->Spacing(bind("PanelSpacing"));  // 🎯 绑定间距
    
    // 标题 - TextBlock 绑定
    auto titleText = ui::textBlock()
        ->Text(bind("Title"))             // 🎯 绑定文本
        ->FontSize(bind("TitleSize"))     // 🎯 绑定字体大小
        ->Foreground("#FF2E86DE");
    
    // 副标题 - TextBlock 绑定
    auto subTitle = ui::textBlock()
        ->Text(bind("SubTitle"))          // 🎯 绑定文本
        ->FontSize(16.0f)
        ->Foreground("#FF666666");
    
    // 按钮 - Button 绑定
    auto button = ui::button()
        ->Content(bind("ButtonText"))     // 🎯 绑定按钮文本
        ->Background(bind("ButtonColor"))  // 🎯 绑定背景色
        ->Width(150)
        ->Height(40);
    
    button->Click += [viewModel](auto&) {
        static int counter = 0;
        viewModel->SetTitle("标题已更新 " + std::to_string(++counter));
        viewModel->SetButtonColor(counter % 2 ? "#FFFF5722" : "#FF4CAF50");
        viewModel->SetScrollValue(counter * 10.0);
    };
    
    // 滚动条 - ScrollBar 绑定
    auto scrollBar = ui::scrollBar()
        ->Minimum(0.0)
        ->Maximum(100.0)
        ->Value(bind("ScrollValue"))      // 🎯 绑定滚动值
        ->Orientation(ui::Orientation::Horizontal)
        ->Width(400)
        ->Height(20);
    
    // 显示滚动值的文本
    auto valueText = ui::textBlock()
        ->Text("滚动值: ")
        ->FontSize(14.0f);
    
    // 组装 UI
    panel->AddChild(titleText);
    panel->AddChild(subTitle);
    panel->AddChild(button);
    panel->AddChild(scrollBar);
    panel->AddChild(valueText);
    
    window->SetContent(panel);
    
    // ⚠️ 关键：转换为基类指针
    std::shared_ptr<binding::INotifyPropertyChanged> baseViewModel = viewModel;
    window->SetDataContext(baseViewModel);
    
    app.AddWindow(window, "MainWindow");
    app.Run(window);
    
    return 0;
}
```

**运行效果：**
- 点击按钮 → 标题更新、按钮颜色切换、滚动条位置改变
- 所有绑定自动同步到 UI
- 无需手动更新界面

---

## 技术实现

### 使用的宏

所有绑定支持通过以下宏实现：

```cpp
// 引用类型（std::string 等）
FK_BINDING_PROPERTY(PropertyName, std::string)

// 值类型（float, int, double 等）
FK_BINDING_PROPERTY_VALUE(PropertyName, float)

// 枚举类型
FK_BINDING_PROPERTY_ENUM(PropertyName, EnumType)
```

### 生成的代码

每个宏生成 3 个函数：

```cpp
// Getter
[[nodiscard]] const ValueType& PropertyName() const {
    return GetPropertyName();
}

// Setter（普通值）
Ptr PropertyName(const ValueType& value) {
    SetPropertyName(value);
    return Self();
}

// Setter（绑定）
Ptr PropertyName(binding::Binding binding) {
    SetBinding(PropertyNameProperty(), std::move(binding));
    return Self();
}
```

---

## 优势

### ✅ 统一性
所有控件使用相同的绑定语法：`Property(bind("..."))`

### ✅ 简洁性
```cpp
// 传统方式：3 行代码
Binding binding;
binding.Path("Name");
textBlock->SetBinding(TextBlock::TextProperty(), binding);

// 新方式：1 行代码
textBlock->Text(bind("Name"));
```

### ✅ 可维护性
使用宏减少重复代码，易于添加新控件

### ✅ 类型安全
编译时检查，避免运行时错误

### ✅ 性能
- 内联函数无开销
- `std::move` 语义避免拷贝

---

## 下一步扩展

### 计划添加的控件

#### Window
```cpp
window->Title(bind("WindowTitle"))
      ->Width(bind("WindowWidth"))
      ->Height(bind("WindowHeight"));
```

#### Panel 基类
```cpp
panel->Background(bind("PanelColor"))
     ->Padding(bind("PanelPadding"));
```

#### ContentControl
```cpp
contentControl->Content(bind("ContentVM"));
```

#### ItemsControl
```cpp
itemsControl->ItemsSource(bind("ItemsCollection"))
            ->DisplayMemberPath(bind("DisplayPath"));
```

---

## 总结

🎉 **已完成 5 个主要控件的完整绑定支持！**

- **TextBlock** - 文本显示（6 个属性）
- **Button** - 按钮交互（7 个属性）
- **StackPanel** - 布局控制（2 个属性）
- **ScrollBar** - 滚动控制（8 个属性）
- **ScrollViewer** - 滚动视图（4 个属性）

**总计：27 个可绑定属性，全部支持链式调用！**

所有控件现在都可以：
- ✅ 使用 `Property(value)` 设置固定值
- ✅ 使用 `Property(bind("..."))` 绑定到 ViewModel
- ✅ 混合使用固定值和绑定
- ✅ 链式调用所有方法
