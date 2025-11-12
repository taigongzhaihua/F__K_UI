# Application

## 概览

**目的**：应用程序单例，管理应用程序生命周期和窗口

**命名空间**：`fk::app`

**头文件**：`fk/app/Application.h`

## 描述

`Application` 是应用程序的入口点和单例对象。它管理窗口创建、消息循环和应用程序生命周期事件。

## 公共接口

### 创建应用程序

#### Create
```cpp
static std::shared_ptr<Application> Create();
```

创建应用程序单例。

**示例**：
```cpp
auto app = app::Application::Create();
```

### 窗口管理

#### CreateWindow
```cpp
std::shared_ptr<Window> CreateWindow();
```

创建新窗口。

**示例**：
```cpp
auto window = app->CreateWindow();
window->Title("我的应用")->Width(800)->Height(600);
```

### 运行应用程序

#### Run
```cpp
int Run();
```

启动应用程序消息循环。阻塞直到应用程序退出。

**返回值**：应用程序退出代码

**示例**：
```cpp
return app->Run();
```

### 退出应用程序

#### Shutdown
```cpp
void Shutdown(int exitCode = 0);
```

关闭应用程序。

**示例**：
```cpp
app->Shutdown(0);
```

### 应用程序事件

#### Startup / Exit
```cpp
core::Event<> Startup;
core::Event<int> Exit;
```

应用程序启动和退出事件。

**示例**：
```cpp
app->Startup += []() {
    std::cout << "应用程序启动" << std::endl;
};

app->Exit += [](int exitCode) {
    std::cout << "应用程序退出，代码: " << exitCode << std::endl;
};
```

### Dispatcher 访问

#### GetDispatcher
```cpp
std::shared_ptr<core::Dispatcher> GetDispatcher() const;
```

获取主UI线程的Dispatcher。

## 使用示例

### 最小应用程序
```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"

int main() {
    auto app = app::Application::Create();
    auto window = app->CreateWindow();
    window->Title("Hello F__K_UI")->Width(800)->Height(600);
    
    return app->Run();
}
```

### 带内容的应用程序
```cpp
int main() {
    auto app = app::Application::Create();
    
    auto window = app->CreateWindow();
    window->Title("我的应用")
          ->Width(1024)
          ->Height(768);
    
    // 创建主界面
    auto mainPanel = window->SetContent<StackPanel>();
    mainPanel->Orientation(Orientation::Vertical)
             ->Spacing(10)
             ->Margin(Thickness(20));
    
    auto title = mainPanel->AddChild<TextBlock>();
    title->Text("欢迎使用我的应用")
         ->FontSize(24);
    
    auto button = mainPanel->AddChild<Button>();
    button->Content("开始")
          ->Width(120)
          ->Height(40);
    
    button->Click += [app]() {
        std::cout << "开始按钮被点击" << std::endl;
    };
    
    return app->Run();
}
```

### 处理应用程序事件
```cpp
int main() {
    auto app = app::Application::Create();
    
    // 启动事件
    app->Startup += []() {
        std::cout << "应用程序正在启动..." << std::endl;
        // 执行初始化任务
    };
    
    // 退出事件
    app->Exit += [](int exitCode) {
        std::cout << "应用程序退出，代码: " << exitCode << std::endl;
        // 执行清理任务
    };
    
    auto window = app->CreateWindow();
    window->Title("我的应用")->Width(800)->Height(600);
    
    return app->Run();
}
```

## 相关类

- [Window](../UI/Window.md) - 顶级窗口
- [Dispatcher](../Core/Dispatcher.md) - UI线程调度

## 另请参阅

- [设计文档](../../Design/App/Application.md)
- [入门指南](../../GettingStarted.md)
