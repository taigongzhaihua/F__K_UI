# F__K_UI 入门指南

## 简介

F__K_UI 是一个受 WPF 启发的现代 C++ UI 框架，专注于简洁的 API 和强大的功能。本指南将帮助您开始构建第一个应用程序。

## 前置条件

- **C++17 或更高版本**编译器（MSVC、GCC 或 Clang）
- **CMake 3.20+**
- **OpenGL 3.3+** 支持
- **Windows、Linux 或 macOS**（主要支持 Windows）

## 安装

### 1. 克隆仓库

```bash
git clone https://github.com/taigongzhaihua/F__K_UI.git
cd F__K_UI
```

### 2. 构建项目

#### Windows (PowerShell)

```powershell
mkdir build
cd build
cmake ..
cmake --build . -j 8
```

#### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j8
```

## 第一个应用程序

### Hello World 示例

创建一个带按钮的简单窗口：

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

int main() {
    // 创建应用程序
    auto app = app::Application::Create();
    
    // 创建主窗口
    auto window = app->CreateWindow();
    window->Title("我的第一个 F__K_UI 应用")
          ->Width(800)
          ->Height(600);
    
    // 添加按钮
    auto button = window->SetContent<Button>();
    button->Content("点击我！")
          ->Width(200)
          ->Height(50);
    
    // 运行应用程序
    return app->Run();
}
```

### 理解代码

1. **应用程序创建**：`Application::Create()` 初始化框架
2. **窗口设置**：使用流畅 API 创建和配置窗口
3. **内容添加**：使用 `SetContent<>()` 设置窗口内容
4. **运行循环**：`app->Run()` 启动消息循环

## 核心概念

### 1. 流畅 API（方法链）

F__K_UI 使用流畅、可链式调用的 API 实现简洁的代码：

```cpp
button->Content("提交")
      ->Width(100)
      ->Height(40)
      ->Background(Colors::Blue)
      ->Foreground(Colors::White);
```

### 2. 视觉树

UI 元素形成父子层次结构：

```cpp
Window
  └─ StackPanel
       ├─ Button
       ├─ TextBlock
       └─ Image
```

### 3. 依赖属性

属性支持绑定、验证和变更通知：

```cpp
// 获取属性
double width = element->GetValue<double>(UIElement::WidthProperty());

// 设置属性
element->SetValue(UIElement::WidthProperty(), 200.0);
```

### 4. 数据绑定

自动同步 UI 和数据：

```cpp
// 创建 ViewModel
class MyViewModel : public ObservableObject {
    PROPERTY(std::string, Username, "")
};

auto viewModel = std::make_shared<MyViewModel>();

// 绑定到 UI
textBox->SetValue(TextBox::TextProperty(), 
                  Binding("Username").Source(viewModel));
```

## 布局系统

### StackPanel - 堆栈布局

垂直或水平排列子元素：

```cpp
auto stack = std::make_shared<StackPanel>();
stack->Orientation(Orientation::Vertical)
     ->Spacing(10);

stack->AddChild<Button>()->Content("按钮 1");
stack->AddChild<Button>()->Content("按钮 2");
stack->AddChild<TextBlock>()->Text("一些文本");
```

### Grid - 网格布局

基于行和列的灵活布局：

```cpp
auto grid = std::make_shared<Grid>();

// 定义行和列
grid->AddRowDefinition()->Height(GridLength::Auto());
grid->AddRowDefinition()->Height(GridLength(1, GridUnitType::Star));
grid->AddColumnDefinition()->Width(GridLength(200));
grid->AddColumnDefinition()->Width(GridLength(1, GridUnitType::Star));

// 添加元素
auto button = grid->AddChild<Button>();
button->Content("按钮")
      ->Row(0)
      ->Column(0);
```

## 常用控件

### Button - 按钮

交互式按钮控件：

```cpp
auto button = std::make_shared<Button>();
button->Content("点击我")
      ->Width(120)
      ->Height(40);

// 处理点击事件
button->Click += []() {
    std::cout << "按钮被点击！" << std::endl;
};
```

### TextBlock - 文本显示

显示只读文本：

```cpp
auto text = std::make_shared<TextBlock>();
text->Text("欢迎使用 F__K_UI")
    ->FontSize(24)
    ->Foreground(Colors::Blue);
```

### Border - 边框装饰

为元素添加边框和背景：

```cpp
auto border = std::make_shared<Border>();
border->Background(Colors::LightGray)
      ->BorderBrush(Colors::Black)
      ->BorderThickness(Thickness(2))
      ->CornerRadius(CornerRadius(5));

// 设置子内容
border->SetChild<TextBlock>()->Text("带边框的内容");
```

### Image - 图像显示

显示位图图像：

```cpp
auto image = std::make_shared<Image>();
image->Source("path/to/image.png")
     ->Width(300)
     ->Height(200);
```

## 事件处理

### 订阅事件

```cpp
// 订阅按钮点击
button->Click += []() {
    std::cout << "已点击" << std::endl;
};

// 订阅鼠标事件
button->MouseEnter += [](const MouseEventArgs& args) {
    std::cout << "鼠标进入" << std::endl;
};
```

### 取消订阅事件

```cpp
// 保存连接
auto connection = button->Click += []() {
    std::cout << "已点击" << std::endl;
};

// 稍后断开连接
connection.Disconnect();
```

## 数据绑定

### 单向绑定

数据从源流向目标：

```cpp
// 创建源对象
auto viewModel = std::make_shared<MyViewModel>();
viewModel->SetUsername("John");

// 绑定到 TextBlock
textBlock->SetValue(TextBlock::TextProperty(),
                    Binding("Username")
                        .Source(viewModel)
                        .Mode(BindingMode::OneWay));
```

### 双向绑定

UI 和数据双向同步：

```cpp
textBox->SetValue(TextBox::TextProperty(),
                  Binding("Username")
                      .Source(viewModel)
                      .Mode(BindingMode::TwoWay));
```

### 值转换器

在绑定时转换值：

```cpp
class BoolToVisibilityConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, ...) override {
        bool boolValue = std::any_cast<bool>(value);
        return boolValue ? Visibility::Visible : Visibility::Collapsed;
    }
};

// 使用转换器
element->SetValue(UIElement::VisibilityProperty(),
                  Binding("IsEnabled")
                      .Source(viewModel)
                      .Converter(std::make_shared<BoolToVisibilityConverter>()));
```

## 变换

### 旋转变换

```cpp
auto rotateTransform = std::make_shared<RotateTransform>(
    45.0f,      // 角度
    100.0f,     // 中心 X
    100.0f      // 中心 Y
);
element->SetRenderTransform(rotateTransform);
```

### 缩放变换

```cpp
auto scaleTransform = std::make_shared<ScaleTransform>(
    1.5f,       // X 缩放
    1.5f,       // Y 缩放
    100.0f,     // 中心 X
    100.0f      // 中心 Y
);
element->SetRenderTransform(scaleTransform);
```

### 平移变换

```cpp
auto translateTransform = std::make_shared<TranslateTransform>(
    50.0f,      // X 偏移
    30.0f       // Y 偏移
);
element->SetRenderTransform(translateTransform);
```

### 组合变换

```cpp
auto transformGroup = std::make_shared<TransformGroup>();
transformGroup->AddChild(std::make_shared<ScaleTransform>(2.0f, 2.0f));
transformGroup->AddChild(std::make_shared<RotateTransform>(45.0f));
transformGroup->AddChild(std::make_shared<TranslateTransform>(100.0f, 50.0f));

element->SetRenderTransform(transformGroup);
```

## 完整示例

### 完整应用程序

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Border.h"

int main() {
    auto app = app::Application::Create();
    auto window = app->CreateWindow();
    
    window->Title("完整示例")
          ->Width(400)
          ->Height(300);
    
    // 创建主面板
    auto mainPanel = window->SetContent<StackPanel>();
    mainPanel->Orientation(Orientation::Vertical)
             ->Spacing(10)
             ->Margin(Thickness(20));
    
    // 添加标题
    auto title = mainPanel->AddChild<TextBlock>();
    title->Text("欢迎使用 F__K_UI")
         ->FontSize(24)
         ->HorizontalAlignment(HorizontalAlignment::Center);
    
    // 添加带边框的内容
    auto border = mainPanel->AddChild<Border>();
    border->Background(Colors::LightBlue)
          ->BorderBrush(Colors::Blue)
          ->BorderThickness(Thickness(2))
          ->CornerRadius(CornerRadius(5))
          ->Padding(Thickness(10));
    
    auto contentText = border->SetChild<TextBlock>();
    contentText->Text("这是一个完整的示例应用程序");
    
    // 添加按钮
    auto button = mainPanel->AddChild<Button>();
    button->Content("点击我")
          ->HorizontalAlignment(HorizontalAlignment::Center)
          ->Width(120)
          ->Height(40);
    
    button->Click += []() {
        std::cout << "按钮被点击！" << std::endl;
    };
    
    return app->Run();
}
```

## 最佳实践

### 1. 使用 shared_ptr 管理对象

```cpp
auto button = std::make_shared<Button>();
```

### 2. 使用方法链进行配置

```cpp
button->Content("提交")
      ->Width(100)
      ->Height(40);
```

### 3. 将 UI 与业务逻辑分离

使用 MVVM 模式：
- **Model**：业务逻辑和数据
- **View**：UI 元素
- **ViewModel**：绑定层

### 4. 处理事件连接

始终管理事件连接以避免内存泄漏：

```cpp
// 好的做法
auto connection = button->Click += handler;
// 稍后...
connection.Disconnect();
```

### 5. 使用依赖属性进行可绑定属性

```cpp
class MyControl : public Control<MyControl> {
public:
    static const DependencyProperty& MyPropertyProperty() {
        static auto prop = DependencyProperty::Register<MyControl, int>(
            "MyProperty", 
            PropertyMetadata::Create(0)
        );
        return prop;
    }
};
```

## 常见模式

### 1. 创建自定义控件

```cpp
class CustomButton : public Button {
public:
    CustomButton() {
        // 自定义初始化
        Background(Colors::Blue);
        Foreground(Colors::White);
    }
    
protected:
    void OnRender(const RenderContext& context) override {
        // 自定义渲染
        Button::OnRender(context);
        // 添加额外的绘制
    }
};
```

### 2. 响应式布局

```cpp
auto grid = std::make_shared<Grid>();

// 使用 Star 大小进行比例调整
grid->AddColumnDefinition()->Width(GridLength(1, GridUnitType::Star));
grid->AddColumnDefinition()->Width(GridLength(2, GridUnitType::Star));

// 第二列将是第一列宽度的两倍
```

### 3. 动态内容

```cpp
// 动态添加子元素
for (int i = 0; i < 5; i++) {
    auto button = stack->AddChild<Button>();
    button->Content(std::format("按钮 {}", i));
}

// 动态移除子元素
stack->RemoveChild(button);
stack->ClearChildren();
```

## 调试技巧

### 1. 启用日志

```cpp
// 在 main() 中启用日志
core::LoggerManager::Instance().SetLogger(
    std::make_shared<core::ConsoleLogger>()
);
```

### 2. 检查视觉树

```cpp
// 打印视觉树层次结构
void PrintVisualTree(Visual* element, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << typeid(*element).name() << std::endl;
    
    for (auto child : element->GetChildren()) {
        PrintVisualTree(child, depth + 1);
    }
}
```

### 3. 属性值检查

```cpp
// 检查当前值
auto width = element->GetValue<double>(UIElement::WidthProperty());
auto source = element->GetValueSource(UIElement::WidthProperty());
std::cout << "宽度：" << width << "，来源：" << (int)source << std::endl;
```

## 下一步

- 探索 `/examples` 目录中的更多示例
- 查看 [API 文档](API/README.md) 了解详细参考
- 阅读[开发指南](Development.md) 了解贡献
- 查看[实现状态](Implementation-Status.md) 了解当前进度

## 获取帮助

- **文档**：参见 [文档目录](README.md)
- **示例**：查看 `/examples` 文件夹
- **问题**：在 GitHub 上开启 issue
- **架构**：查看[架构文档](Designs/Architecture-Refactoring.md)

---

**开始构建精彩的应用程序！🚀**

*最后更新：2025年11月*
