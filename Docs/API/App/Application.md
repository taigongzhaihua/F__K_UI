# Application - 应用程序

## 概述

`Application` 是应用程序的入口类,负责管理应用程序的生命周期、窗口集合和消息循环。

**命名空间**: `fk`  
**头文件**: `fk/app/Application.h`

## 核心功能

- **生命周期管理**: 启动、运行、关闭应用程序
- **窗口管理**: 管理多个窗口,包括主窗口
- **消息循环**: 运行事件循环直到主窗口关闭
- **全局访问**: 通过 `Current()` 单例访问
- **生命周期事件**: Startup、Exit、Activated、Deactivated

## API 参考

### 构造/析构

```cpp
Application();
virtual ~Application();
```

### 静态方法

```cpp
static Application* Current();
```
获取当前应用程序实例(单例)。

### 核心方法

```cpp
void Run(ui::WindowPtr mainWindow);
```
运行消息循环,直到主窗口关闭。
- **参数**: `mainWindow` - 主窗口
- **阻塞**: 此方法阻塞直到应用程序退出

```cpp
void Shutdown();
```
关闭应用程序,停止消息循环。

```cpp
bool IsRunning() const;
```
检查应用程序是否正在运行。

### 窗口管理

```cpp
void AddWindow(const ui::WindowPtr& window, const std::string& name);
```
添加命名窗口到应用程序。

```cpp
void RemoveWindow(const ui::WindowPtr& window);
void RemoveWindow(const std::string& name);
```
移除窗口(按指针或名称)。

```cpp
ui::WindowPtr GetWindow(const std::string& name) const;
```
按名称获取窗口。

```cpp
ui::WindowPtr GetMainWindow() const;
```
获取主窗口。

```cpp
const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;
```
获取所有窗口集合。

### 事件

```cpp
core::Event<> Startup;      // 应用程序启动时触发
core::Event<> Exit;         // 应用程序退出时触发
core::Event<> Activated;    // 应用程序激活时触发
core::Event<> Deactivated;  // 应用程序失去焦点时触发
```

## 使用示例

### 1. 基本应用程序

```cpp
#include <fk/app/Application.h>
#include <fk/ui/Window.h>

int main() {
    fk::Application app;
    
    // 创建主窗口
    auto window = fk::ui::Window::Create();
    window->Title("Hello F__K UI");
    window->Width(800);
    window->Height(600);
    
    // 运行应用程序
    app.Run(window);  // 阻塞直到窗口关闭
    
    return 0;
}
```

### 2. 监听生命周期事件

```cpp
#include <fk/app/Application.h>
#include <iostream>

int main() {
    fk::Application app;
    
    // 订阅生命周期事件
    app.Startup.Subscribe([]() {
        std::cout << "应用程序启动\n";
    });
    
    app.Exit.Subscribe([]() {
        std::cout << "应用程序退出\n";
        // 清理资源
    });
    
    app.Activated.Subscribe([]() {
        std::cout << "应用程序激活\n";
    });
    
    auto window = fk::ui::Window::Create();
    app.Run(window);
    
    return 0;
}
```

### 3. 多窗口应用程序

```cpp
#include <fk/app/Application.h>
#include <fk/ui/Window.h>
#include <fk/ui/Button.h>

int main() {
    fk::Application app;
    
    // 主窗口
    auto mainWindow = fk::ui::Window::Create();
    mainWindow->Title("主窗口");
    mainWindow->Width(600)->Height(400);
    
    // 添加按钮打开第二个窗口
    auto button = fk::ui::Button::Create();
    button->Content("打开设置窗口");
    button->Click.Subscribe([&app]() {
        // 创建设置窗口
        auto settingsWindow = fk::ui::Window::Create();
        settingsWindow->Title("设置");
        settingsWindow->Width(400)->Height(300);
        
        // 添加到应用程序
        app.AddWindow(settingsWindow, "settings");
        
        settingsWindow->Show();
    });
    
    mainWindow->Content(button);
    
    app.Run(mainWindow);
    
    return 0;
}
```

### 4. 全局访问应用程序

```cpp
#include <fk/app/Application.h>

// 在任何地方访问应用程序实例
void SomeFunction() {
    auto* app = fk::Application::Current();
    
    if (app && app->IsRunning()) {
        // 获取主窗口
        auto mainWindow = app->GetMainWindow();
        
        // 遍历所有窗口
        for (const auto& [name, window] : app->Windows()) {
            std::cout << "窗口: " << name << "\n";
        }
    }
}
```

### 5. 优雅关闭应用程序

```cpp
#include <fk/app/Application.h>
#include <fk/ui/Window.h>
#include <fk/ui/Button.h>

int main() {
    fk::Application app;
    
    auto window = fk::ui::Window::Create();
    window->Title("可关闭应用");
    
    // 添加退出按钮
    auto exitButton = fk::ui::Button::Create();
    exitButton->Content("退出应用");
    exitButton->Click.Subscribe([&app]() {
        // 优雅关闭
        app.Shutdown();
    });
    
    window->Content(exitButton);
    
    // 窗口关闭时自动退出
    window->Closing.Subscribe([&app](bool& cancel) {
        std::cout << "正在关闭应用程序...\n";
        app.Shutdown();
    });
    
    app.Run(window);
    
    return 0;
}
```

### 6. 初始化配置

```cpp
#include <fk/app/Application.h>

class MyApp : public fk::Application {
public:
    MyApp() {
        // 订阅启动事件
        Startup.Subscribe([this]() {
            InitializeServices();
            LoadConfiguration();
        });
        
        // 订阅退出事件
        Exit.Subscribe([this]() {
            SaveConfiguration();
            CleanupServices();
        });
    }
    
private:
    void InitializeServices() {
        // 初始化服务(如数据库、网络等)
    }
    
    void LoadConfiguration() {
        // 加载配置文件
    }
    
    void SaveConfiguration() {
        // 保存配置
    }
    
    void CleanupServices() {
        // 清理资源
    }
};

int main() {
    MyApp app;
    auto window = fk::ui::Window::Create();
    app.Run(window);
    return 0;
}
```

## 最佳实践

### 1. 使用单例模式
```cpp
// 推荐:通过 Current() 访问
auto* app = fk::Application::Current();

// 避免:保存多个 Application 实例指针
```

### 2. 主窗口管理
```cpp
// 推荐:通过 Run() 设置主窗口
app.Run(mainWindow);

// 主窗口关闭时,应用程序自动退出
```

### 3. 窗口命名
```cpp
// 推荐:为重要窗口命名
app.AddWindow(settingsWindow, "settings");
app.AddWindow(editorWindow, "editor");

// 方便后续访问
auto settings = app.GetWindow("settings");
```

### 4. 生命周期处理
```cpp
// 推荐:在 Startup 事件中初始化
app.Startup.Subscribe([]() {
    InitializeServices();
});

// 推荐:在 Exit 事件中清理
app.Exit.Subscribe([]() {
    CleanupResources();
});
```

### 5. 资源管理
```cpp
// 推荐:使用智能指针
auto window = fk::ui::Window::Create();  // 返回 shared_ptr

// 避免:手动管理窗口生命周期
```

## 常见问题

### Q1: 如何在不同模块中访问应用程序?
```cpp
// 使用 Current() 单例
auto* app = fk::Application::Current();
if (app) {
    // 安全访问
}
```

### Q2: 如何创建多窗口应用?
```cpp
// 1. 创建并命名窗口
auto window2 = fk::ui::Window::Create();
app.AddWindow(window2, "window2");

// 2. 显示窗口
window2->Show();

// 3. 按需移除
app.RemoveWindow("window2");
```

### Q3: 主窗口关闭后会发生什么?
主窗口关闭时,`Run()` 方法返回,应用程序退出。其他窗口会自动关闭。

### Q4: 如何实现启动画面(Splash Screen)?
```cpp
auto splashWindow = fk::ui::Window::Create();
splashWindow->Title("加载中...");
splashWindow->Show();

app.Startup.Subscribe([&]() {
    // 加载完成后关闭启动画面
    splashWindow->Close();
    
    // 显示主窗口
    auto mainWindow = app.GetMainWindow();
    mainWindow->Show();
});

app.Run(mainWindow);
```

### Q5: 如何处理应用程序崩溃?
```cpp
int main() {
    try {
        fk::Application app;
        auto window = fk::ui::Window::Create();
        app.Run(window);
    } catch (const std::exception& e) {
        std::cerr << "应用程序崩溃: " << e.what() << "\n";
        // 记录日志、显示错误对话框等
        return 1;
    }
    return 0;
}
```

## 线程安全

- **UI 线程**: Application 必须在主线程(UI 线程)上创建和运行
- **事件触发**: 所有生命周期事件在 UI 线程上触发
- **跨线程访问**: 使用 `Dispatcher` 从其他线程访问 UI

```cpp
// 从工作线程更新 UI
std::thread worker([&app]() {
    // 完成工作...
    
    // 切换到 UI 线程
    if (auto mainWindow = app.GetMainWindow()) {
        mainWindow->Dispatcher().InvokeAsync([]() {
            // 安全的 UI 更新
        });
    }
});
```

## 性能考虑

1. **窗口数量**: 保持合理的窗口数量,每个窗口都有渲染开销
2. **事件订阅**: 及时取消不再需要的事件订阅
3. **主窗口**: 避免频繁改变主窗口

## 相关类

- **Window**: 窗口类
- **Dispatcher**: 线程调度器
- **Event**: 事件系统

## 参考

- [Window.md](../UI/Window.md) - 窗口文档
- [Dispatcher.md](../Core/Dispatcher.md) - 调度器文档
- [Event.md](../Core/Event.md) - 事件系统
