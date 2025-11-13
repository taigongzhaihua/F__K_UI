# 开发指南

## 项目结构

```
F__K_UI/
├── include/fk/          # 公共头文件
│   ├── app/            # 应用程序模块（Application、Window）
│   ├── binding/        # 数据绑定系统（DependencyProperty、Binding）
│   ├── core/           # 核心工具（Dispatcher、Event、Logger）
│   ├── render/         # 渲染系统（Renderer、RenderBackend）
│   └── ui/             # UI 控件和元素
├── src/                # 实现文件
│   ├── app/
│   ├── binding/
│   ├── core/
│   ├── render/
│   └── ui/
├── examples/           # 示例应用程序
├── samples/            # 示例项目
├── third_party/        # 第三方依赖
│   ├── glfw/          # 窗口管理
│   ├── freetype/      # 字体渲染
│   └── stb/           # 图像加载（stb_image）
├── Docs/              # 文档
│   ├── API/           # API 参考
│   ├── Designs/       # 架构和设计文档
│   ├── GettingStarted.md
│   ├── Development.md（本文件）
│   └── Implementation-Status.md
└── CMakeLists.txt     # 构建配置
```

## 架构概览

### 模块层次结构

```
┌─────────────────────────────────────────┐
│          应用程序层                      │
│     (app::Application, Window)          │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│           UI 层                          │
│  (Controls, Panels, Visual Tree)        │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│        绑定层                            │
│  (DependencyObject, Binding System)     │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│        渲染层                            │
│   (Renderer, RenderBackend, OpenGL)     │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│         核心层                           │
│  (Dispatcher, Events, Utilities)        │
└─────────────────────────────────────────┘
```

## 关键组件

### 1. Core 模块（`core/`）

**目的**：基础工具和基础设施

**核心类**：
- `Dispatcher`：线程安全的事件调度
- `DispatcherObject`：线程关联对象的基类
- `Event<T>`：类型安全的事件系统
- `Logger`：日志基础设施
- `Clock`：时间管理

### 2. Binding 模块（`binding/`）

**目的**：依赖属性和数据绑定系统

**核心类**：
- `DependencyObject`：具有依赖属性的基类
- `DependencyProperty`：属性元数据和管理
- `Binding`：数据绑定配置
- `BindingExpression`：活动绑定连接
- `ObservableObject`：带 INPC 的 ViewModel 基类

**核心特性**：
- 属性变更通知
- 值传播
- 双向绑定支持
- 值转换器

### 3. UI 模块（`ui/`）

**目的**：可视元素和控件

**核心类**：
- `Visual`：基础视觉树节点
- `UIElement`：交互元素基类
- `FrameworkElement`：布局感知元素
- `Control`：可模板化控件基类
- `Panel`：容器基类

**控件**：
- `Button`：按钮控件
- `TextBlock`：文本显示
- `Border`：边框装饰器
- `Image`：图像显示
- `StackPanel`：堆栈布局
- `Grid`：网格布局

### 4. Render 模块（`render/`）

**目的**：渲染管线和图形

**核心类**：
- `Renderer`：主渲染器
- `GlRenderer`：OpenGL 实现
- `RenderBackend`：平台抽象
- `TextRenderer`：文本渲染

### 5. App 模块（`app/`）

**目的**：应用程序生命周期

**核心类**：
- `Application`：应用程序单例
- `Window`：顶级窗口

## 构建系统

### CMake 配置

框架使用 CMake 作为构建系统。主要目标：

```cmake
# 库目标
add_library(fk_ui STATIC ...)

# 示例可执行文件
add_executable(hello_world examples/hello_world.cpp)
target_link_libraries(hello_world fk_ui)
```

### 构建步骤

```bash
# 配置
mkdir build && cd build
cmake ..

# 构建
cmake --build . -j8

# 运行测试
ctest
```

### 构建选项

```cmake
# 调试/发布
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=Release

# 启用示例
cmake .. -DBUILD_EXAMPLES=ON

# 启用测试
cmake .. -DBUILD_TESTS=ON
```

## 代码风格

### 命名约定

- **类名**：PascalCase（`Button`、`DependencyObject`）
- **方法名**：PascalCase（`GetValue`、`SetValue`）
- **变量名**：camelCase（`fontSize`、`isEnabled`）
- **私有成员**：camelCase_ 带下划线（`width_`、`parent_`）
- **常量**：PascalCase 或 UPPER_CASE

### 文件组织

- **头文件**：`include/fk/module/ClassName.h`
- **实现文件**：`src/module/ClassName.cpp`
- **一个类一个文件**
- **头文件使用保护宏**：`FK_MODULE_CLASSNAME_H`

### 代码格式

```cpp
// 命名空间
namespace fk::ui {

// 类定义
class Button : public ContentControl<Button> {
public:
    // 公共方法
    Button();
    ~Button();
    
    // 流畅 API
    Button* Content(const std::string& text);
    Button* Width(double width);
    
protected:
    // 保护方法
    void OnRender(const RenderContext& context) override;
    
private:
    // 私有成员
    std::string content_;
    double width_;
};

} // namespace fk::ui
```

### 最佳实践

1. **使用智能指针**：
   ```cpp
   auto button = std::make_shared<Button>();
   ```

2. **RAII 资源管理**：
   ```cpp
   class Resource {
   public:
       Resource() { /* 获取资源 */ }
       ~Resource() { /* 释放资源 */ }
   };
   ```

3. **Const 正确性**：
   ```cpp
   const std::string& GetText() const;
   void SetText(const std::string& text);
   ```

4. **避免原始指针所有权**：
   ```cpp
   // 好
   std::shared_ptr<Control> control;
   
   // 不好
   Control* control = new Control();
   ```

## 添加新控件

### 步骤 1：创建头文件

`include/fk/ui/MyControl.h`：

```cpp
#pragma once
#include "fk/ui/Control.h"

namespace fk::ui {

class MyControl : public Control<MyControl> {
public:
    MyControl();
    virtual ~MyControl() = default;
    
    // 依赖属性
    static const DependencyProperty& MyPropertyProperty();
    
    // 流畅 API
    MyControl* MyProperty(int value);
    
protected:
    void OnRender(const RenderContext& context) override;
    void MeasureOverride(const Size& availableSize) override;
    void ArrangeOverride(const Size& finalSize) override;
    
private:
    int myProperty_;
};

} // namespace fk::ui
```

### 步骤 2：创建实现文件

`src/ui/MyControl.cpp`：

```cpp
#include "fk/ui/MyControl.h"

namespace fk::ui {

// 依赖属性注册
const DependencyProperty& MyControl::MyPropertyProperty() {
    static auto prop = DependencyProperty::Register<MyControl, int>(
        "MyProperty",
        PropertyMetadata::Create(0)
    );
    return prop;
}

MyControl::MyControl() : myProperty_(0) {
    // 初始化
}

MyControl* MyControl::MyProperty(int value) {
    SetValue(MyPropertyProperty(), value);
    return this;
}

void MyControl::OnRender(const RenderContext& context) {
    // 自定义渲染
}

Size MyControl::MeasureOverride(const Size& availableSize) {
    // 自定义测量逻辑
    return Size(100, 50);
}

Size MyControl::ArrangeOverride(const Size& finalSize) {
    // 自定义排列逻辑
    return finalSize;
}

} // namespace fk::ui
```

### 步骤 3：更新 CMakeLists.txt

```cmake
target_sources(fk_ui PRIVATE
    src/ui/MyControl.cpp
)
```

### 步骤 4：创建示例

`examples/my_control_example.cpp`：

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/MyControl.h"

int main() {
    auto app = app::Application::Create();
    auto window = app->CreateWindow();
    
    auto control = window->SetContent<MyControl>();
    control->MyProperty(42);
    
    return app->Run();
}
```

## 依赖属性实现

### 注册依赖属性

```cpp
static const DependencyProperty& WidthProperty() {
    static auto prop = DependencyProperty::Register<UIElement, double>(
        "Width",
        PropertyMetadata::Create(
            NAN,  // 默认值
            [](DependencyObject* d, const DependencyPropertyChangedEventArgs& e) {
                // 变更回调
                auto element = static_cast<UIElement*>(d);
                element->InvalidateMeasure();
            }
        )
    );
    return prop;
}
```

### 属性包装器

```cpp
UIElement* Width(double width) {
    SetValue(WidthProperty(), width);
    return this;
}

double GetWidth() const {
    return GetValue<double>(WidthProperty());
}
```

## 数据绑定实现

### 创建可观察 ViewModel

```cpp
class MyViewModel : public ObservableObject {
public:
    // 定义属性
    PROPERTY(std::string, Username, "")
    PROPERTY(int, Age, 0)
    
    void UpdateUsername(const std::string& name) {
        SetUsername(name);
        // 自动触发 PropertyChanged
    }
};
```

### 绑定到 UI

```cpp
auto viewModel = std::make_shared<MyViewModel>();

textBox->SetValue(
    TextBox::TextProperty(),
    Binding("Username")
        .Source(viewModel)
        .Mode(BindingMode::TwoWay)
);
```

## 调试技巧

### 1. 启用日志

```cpp
core::LoggerManager::Instance().SetLogger(
    std::make_shared<core::ConsoleLogger>()
);
```

### 2. 视觉树检查

```cpp
void PrintVisualTree(Visual* element, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << typeid(*element).name() << std::endl;
    
    for (auto child : element->GetChildren()) {
        PrintVisualTree(child, depth + 1);
    }
}
```

### 3. 属性调试

```cpp
// 获取值来源
auto source = element->GetValueSource(UIElement::WidthProperty());
switch (source) {
    case ValueSource::Local: 
        std::cout << "本地设置" << std::endl;
        break;
    case ValueSource::Binding:
        std::cout << "来自绑定" << std::endl;
        break;
    // ...
}
```

### 4. 断点调试

建议的断点位置：
- `DependencyObject::SetValue`：属性变更
- `UIElement::Measure`/`Arrange`：布局
- `Renderer::Render`：渲染
- `InputManager::ProcessInput`：输入处理

## 测试

### 单元测试

使用 Google Test 框架：

```cpp
#include <gtest/gtest.h>
#include "fk/ui/Button.h"

TEST(ButtonTest, ContentProperty) {
    auto button = std::make_shared<Button>();
    button->Content("Test");
    
    EXPECT_EQ(button->GetContent(), "Test");
}
```

### 运行测试

```bash
cd build
ctest --output-on-failure
```

## 性能优化

### 1. 避免不必要的布局

```cpp
// 批量更新
element->BeginInit();
element->Width(100);
element->Height(50);
element->Background(Colors::Blue);
element->EndInit();  // 只触发一次布局
```

### 2. 使用对象池

```cpp
// 重用对象而不是创建新对象
class ObjectPool {
    std::vector<std::shared_ptr<Object>> pool_;
    
public:
    std::shared_ptr<Object> Acquire() {
        if (!pool_.empty()) {
            auto obj = pool_.back();
            pool_.pop_back();
            return obj;
        }
        return std::make_shared<Object>();
    }
    
    void Release(std::shared_ptr<Object> obj) {
        pool_.push_back(obj);
    }
};
```

### 3. 延迟计算

```cpp
class LazyValue {
    mutable std::optional<int> cached_;
    
public:
    int GetValue() const {
        if (!cached_) {
            cached_ = ComputeExpensiveValue();
        }
        return *cached_;
    }
};
```

## 贡献指南

### 提交代码

1. Fork 仓库
2. 创建特性分支
3. 提交变更
4. 推送到分支
5. 创建 Pull Request

### 提交消息格式

```
[模块] 简短描述

详细描述变更内容和原因。

- 变更点 1
- 变更点 2
```

示例：
```
[UI] 添加 ListBox 控件

实现 ListBox 控件，支持项目选择和键盘导航。

- 添加 ListBox 类
- 实现选择逻辑
- 添加键盘导航支持
```

### 代码审查检查清单

- [ ] 代码遵循风格指南
- [ ] 添加了适当的注释
- [ ] 更新了文档
- [ ] 添加了单元测试
- [ ] 所有测试通过
- [ ] 没有编译警告
- [ ] 性能已考虑

## 常见问题

### Q: 如何处理线程安全？

A: 所有 UI 对象必须在 UI 线程上访问。使用 `Dispatcher` 跨线程调用：

```cpp
dispatcher->InvokeAsync([element]() {
    element->SetValue(Property, value);
});
```

### Q: 如何实现自定义布局？

A: 继承 `Panel` 并重写 `MeasureOverride` 和 `ArrangeOverride`：

```cpp
class MyPanel : public Panel<MyPanel> {
protected:
    Size MeasureOverride(const Size& availableSize) override {
        // 测量逻辑
    }
    
    Size ArrangeOverride(const Size& finalSize) override {
        // 排列逻辑
    }
};
```

### Q: 如何添加新的依赖属性？

A: 使用 `DependencyProperty::Register`：

```cpp
static const DependencyProperty& MyPropertyProperty() {
    static auto prop = DependencyProperty::Register<MyClass, int>(
        "MyProperty",
        PropertyMetadata::Create(defaultValue)
    );
    return prop;
}
```

## 资源

- [入门指南](GettingStarted.md)
- [API 文档](API/README.md)
- [架构文档](Architecture.md)
- [实现状态](Implementation-Status.md)

## 许可证

本项目采用 MIT 许可证。详见 LICENSE 文件。

---

**快乐编码！🚀**

*最后更新：2025年11月*
