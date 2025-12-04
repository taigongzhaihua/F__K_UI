# F__K UI Framework - 模块参考

F__K UI 提供了模块化的头文件组织方式,类似 Qt 的设计。您可以根据需要只包含所需的模块,以减少编译时间和依赖关系。

## 使用方式

### 完整框架 (推荐用于快速开始)

```cpp
#include <fk/FK_UI.h>
```

包含所有模块,适合快速开发和原型设计。

### 模块化引用 (推荐用于生产环境)

```cpp
#include <fk/Application>  // 只包含应用程序相关
#include <fk/Controls>     // 只包含 UI 控件
#include <fk/DataBinding>  // 只包含数据绑定
```

按需包含,减少编译时间和代码体积。

---

## 模块列表

### 核心模块

#### `<fk/Core>`
核心基础设施组件
- Event - 事件系统
- Timer - 计时器
- Clock - 时钟
- Dispatcher - 调度器
- Logger - 日志系统

**使用场景:** 所有应用的基础,通常会间接包含

```cpp
#include <fk/Core>

fk::Logger::Info("Application started");
fk::Timer timer;
```

---

#### `<fk/Application>`
应用程序入口和窗口管理
- Application - 应用程序主类
- Window - 主窗口

**使用场景:** 创建应用程序入口点

```cpp
#include <fk/Application>

int main() {
    fk::Application app;
    auto window = std::make_shared<fk::ui::Window>();
    window->Title("My App")->Width(800)->Height(600);
    return app.Run(window);
}
```

---

### UI 模块

#### `<fk/Controls>`
所有 UI 控件
- Control - 控件基类
- ContentControl - 内容控件
- ItemsControl - 列表控件基类
- Button - 按钮
- TextBlock - 文本显示
- TextBox - 文本输入
- Border - 边框容器
- Image - 图像
- ScrollBar - 滚动条
- ScrollViewer - 滚动视图
- ListBox - 列表框

**使用场景:** 构建用户界面

```cpp
#include <fk/Controls>

auto button = std::make_shared<fk::ui::Button>();
button->Content("Click Me");

auto text = std::make_shared<fk::ui::TextBlock>();
text->Text("Hello World");
```

---

#### `<fk/Layouts>`
布局容器
- Panel - 面板基类
- StackPanel - 堆叠布局
- Grid - 网格布局
- GridCellAttacher - Grid 单元格附加属性

**使用场景:** 组织控件布局

```cpp
#include <fk/Layouts>

auto stack = std::make_shared<fk::ui::StackPanel>();
stack->Orientation(fk::ui::Orientation::Vertical);

auto grid = std::make_shared<fk::ui::Grid>();
grid->RowDefinitions("Auto,*,Auto");
grid->ColumnDefinitions("200,*");
```

---

#### `<fk/Graphics>`
图形绘制
- Shape - 形状基类
- Brush - 画刷
- Primitives - 基础几何类型 (Point, Size, Rect)
- Thickness - 厚度 (边距/填充)
- CornerRadius - 圆角半径
- Transform - 变换
- PointCollection - 点集合

**使用场景:** 图形绘制和几何计算

```cpp
#include <fk/Graphics>

fk::ui::Point p{100, 200};
fk::ui::Thickness margin{10, 5, 10, 5};
fk::ui::CornerRadius radius{5};

auto brush = std::make_shared<fk::ui::SolidColorBrush>(Colors::Blue);
```

---

#### `<fk/Styling>`
样式和主题
- Style - 样式
- Setter - 属性设置器
- ControlTemplate - 控件模板
- DataTemplate - 数据模板
- FrameworkTemplate - 框架模板基类
- ContentPresenter - 内容呈现器
- ResourceDictionary - 资源字典
- Theme - 主题
- ThemeManager - 主题管理器
- DynamicResource - 动态资源

**使用场景:** 定义和应用样式主题

```cpp
#include <fk/Styling>

auto style = std::make_shared<fk::ui::Style>();
style->AddSetter("Background", fk::ui::Colors::LightGray);
style->AddSetter("Foreground", fk::ui::Colors::Black);

auto theme = std::make_shared<fk::resources::Theme>("Light");
fk::resources::ThemeManager::Instance().SetCurrentTheme(theme);
```

---

#### `<fk/Collections>`
集合类型
- VisualCollection - 视觉元素集合
- ObservableCollection - 可观察集合
- PointCollection - 点集合

**使用场景:** 管理元素集合

```cpp
#include <fk/Collections>

auto items = std::make_shared<fk::ui::ObservableCollection<std::string>>();
items->Add("Item 1");
items->Add("Item 2");
```

---

#### `<fk/Input>`
输入管理
- InputManager - 输入管理器
- FocusManager - 焦点管理器

**使用场景:** 处理用户输入和焦点

```cpp
#include <fk/Input>

fk::ui::FocusManager::SetFocusedElement(element);
```

---

### 功能模块

#### `<fk/DataBinding>`
数据绑定系统
- Binding - 绑定
- BindingExpression - 绑定表达式
- BindingContext - 绑定上下文
- BindingPath - 绑定路径
- MultiBinding - 多重绑定
- TemplateBinding - 模板绑定
- DependencyObject - 依赖对象
- DependencyProperty - 依赖属性
- PropertyStore - 属性存储
- INotifyPropertyChanged - 属性变更通知接口
- INotifyDataErrorInfo - 数据错误通知接口
- ObservableObject - 可观察对象
- ValueConverters - 值转换器
- ValidationRule - 验证规则

**使用场景:** 实现 MVVM 模式

```cpp
#include <fk/DataBinding>

class ViewModel : public fk::binding::ObservableObject {
    OBSERVABLE_PROPERTY(std::string, Name);
};

auto binding = std::make_shared<fk::binding::Binding>();
binding->Path("Name")->Source(viewModel);
textBlock->SetBinding(fk::ui::TextBlock::TextProperty(), binding);
```

---

#### `<fk/Animation>`
动画系统
- Timeline - 时间线基类
- Animation - 动画基类
- Storyboard - 故事板
- AnimationClock - 动画时钟
- AnimationManager - 动画管理器
- EasingFunction - 缓动函数

**基础动画:**
- ColorAnimation - 颜色动画
- DoubleAnimation - 数值动画
- PointAnimation - 点动画
- ThicknessAnimation - 厚度动画

**关键帧动画:**
- KeyFrame - 关键帧
- KeyFrameAnimation - 关键帧动画基类
- ColorAnimationUsingKeyFrames - 颜色关键帧动画
- DoubleAnimationUsingKeyFrames - 数值关键帧动画
- PointAnimationUsingKeyFrames - 点关键帧动画
- ThicknessAnimationUsingKeyFrames - 厚度关键帧动画

**视觉状态:**
- VisualStateManager - 视觉状态管理器
- VisualState - 视觉状态
- VisualStateGroup - 视觉状态组
- VisualStateBuilder - 视觉状态构建器
- VisualTransition - 视觉转换

**使用场景:** 创建流畅的 UI 动画

```cpp
#include <fk/Animation>

auto anim = std::make_shared<fk::animation::DoubleAnimation>();
anim->From(0.0)->To(1.0)->Duration(std::chrono::milliseconds(300));

auto storyboard = std::make_shared<fk::animation::Storyboard>();
storyboard->AddAnimation(anim, element, "Opacity");
storyboard->Begin();
```

---

#### `<fk/Rendering>`
渲染系统
- RenderContext - 渲染上下文
- DrawCommand - 绘制命令
- RenderCommand - 渲染命令
- RenderCommandBuffer - 渲染命令缓冲区
- RenderList - 渲染列表
- IRenderer - 渲染器接口
- GlRenderer - OpenGL 渲染器
- TextRenderer - 文本渲染器
- ColorUtils - 颜色工具

**使用场景:** 自定义渲染逻辑 (高级用法)

```cpp
#include <fk/Rendering>

void OnRender(fk::render::RenderContext& ctx) {
    ctx.DrawRect(rect, brush);
    ctx.DrawText(text, position, font);
}
```

---

## 常用组合

### 基础应用
```cpp
#include <fk/Application>
#include <fk/Controls>
#include <fk/Layouts>
```

### MVVM 应用
```cpp
#include <fk/Application>
#include <fk/Controls>
#include <fk/Layouts>
#include <fk/DataBinding>
```

### 动画应用
```cpp
#include <fk/Application>
#include <fk/Controls>
#include <fk/Animation>
```

### 自定义渲染
```cpp
#include <fk/Application>
#include <fk/Graphics>
#include <fk/Rendering>
```

### 完整功能
```cpp
#include <fk/FK_UI.h>
```

---

## 依赖关系

各模块之间的依赖关系:

```
Core (无依赖)
  ↓
Application (依赖 Core)
  ↓
Controls (依赖 Application, Graphics)
  ↓
Layouts (依赖 Controls)
  
DataBinding (依赖 Core)
Animation (依赖 Core, DataBinding)
Styling (依赖 DataBinding)
Rendering (依赖 Core, Graphics)
```

---

## 最佳实践

1. **开发阶段:** 使用 `<fk/FK_UI.h>` 快速开发
2. **优化阶段:** 切换到模块化引用,只包含需要的模块
3. **库开发:** 优先使用最小依赖的模块组合
4. **性能关键:** 避免包含 Animation 和 Rendering 如果不需要

---

## 示例项目结构

```cpp
// main.cpp - 入口文件
#include <fk/Application>

// view.h - 视图定义
#include <fk/Controls>
#include <fk/Layouts>

// viewmodel.h - 视图模型
#include <fk/DataBinding>
#include <fk/Collections>

// theme.h - 主题定义
#include <fk/Styling>

// custom_control.h - 自定义控件
#include <fk/Controls>
#include <fk/Graphics>
#include <fk/Rendering>
```
