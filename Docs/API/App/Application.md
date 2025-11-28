# Application

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk` |
| **头文件** | `fk/app/Application.h` |
| **源文件** | `src/app/Application.cpp` |

## 描述

`Application` 是应用程序的入口点和单例对象。它负责：

- 管理应用程序生命周期（启动、运行、关闭）
- 管理窗口集合和主窗口
- 运行消息循环和处理事件
- 协调动画更新和窗口渲染

## 类定义

```cpp
namespace fk {

class Application {
public:
    Application();
    virtual ~Application();

    // 单例访问
    static Application* Current();

    // 窗口管理
    ui::Window* CreateWindow();
    void AddWindow(const ui::WindowPtr& window, const std::string& name);
    void RemoveWindow(const ui::WindowPtr& window);
    void RemoveWindow(const std::string& name);
    const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;
    ui::WindowPtr GetWindow(const std::string& name) const;
    ui::WindowPtr GetMainWindow() const;

    // 生命周期管理
    void Run(ui::WindowPtr mainWindow);
    void Shutdown();
    bool IsRunning() const;

    // 事件
    core::Event<> Startup;
    core::Event<> Exit;
    core::Event<> Activated;
    core::Event<> Deactivated;

private:
    static Application* instance_;
    std::unordered_map<std::string, ui::WindowPtr> windows_;
    ui::WindowPtr mainWindow_;
    bool isRunning_;
};

} // namespace fk
```

## 构造函数与析构函数

### Application()

```cpp
Application();
```

构造一个 Application 实例。构造时会自动将自身设置为当前单例实例。

**示例**：
```cpp
fk::Application app;
```

### ~Application()

```cpp
virtual ~Application();
```

析构函数。如果当前实例是单例，则清除单例指针。

---

## 静态方法

### Current

```cpp
static Application* Current();
```

获取当前应用程序单例实例。

**返回值**：当前 Application 实例的指针，如果没有实例则返回 `nullptr`

**示例**：
```cpp
auto* app = fk::Application::Current();
if (app && app->IsRunning()) {
    // 应用程序正在运行
}
```

---

## 窗口管理方法

### CreateWindow

```cpp
ui::Window* CreateWindow();
```

创建一个新窗口。窗口由 Application 管理生命周期，自动添加到窗口集合中。

**返回值**：新创建窗口的原始指针

**说明**：
- 窗口使用指针地址作为唯一名称自动注册
- 窗口所有权由 Application 管理，无需手动释放

**示例**：
```cpp
fk::Application app;
auto* window = app.CreateWindow();
window->SetTitle("我的窗口");
window->SetSize(800, 600);
```

### AddWindow

```cpp
void AddWindow(const ui::WindowPtr& window, const std::string& name);
```

将窗口添加到应用程序管理的窗口集合中。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `window` | `const ui::WindowPtr&` | 要添加的窗口智能指针 |
| `name` | `const std::string&` | 窗口的唯一标识名称 |

**说明**：
- 如果窗口指针为空，将输出错误并返回
- 如果名称已存在，将替换现有窗口
- 如果窗口不可见，将自动调用 `Show()` 显示窗口

**示例**：
```cpp
auto window = std::make_shared<fk::ui::Window>();
app.AddWindow(window, "settings");
```

### RemoveWindow (按指针)

```cpp
void RemoveWindow(const ui::WindowPtr& window);
```

从窗口集合中移除指定窗口。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `window` | `const ui::WindowPtr&` | 要移除的窗口智能指针 |

**说明**：
- 如果窗口可见，将自动调用 `Hide()` 隐藏窗口
- 如果窗口不在集合中，不执行任何操作

### RemoveWindow (按名称)

```cpp
void RemoveWindow(const std::string& name);
```

根据名称从窗口集合中移除窗口。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `const std::string&` | 窗口的标识名称 |

**说明**：
- 如果窗口可见，将自动调用 `Hide()` 隐藏窗口
- 如果名称不存在，不执行任何操作

**示例**：
```cpp
app.RemoveWindow("settings");
```

### Windows

```cpp
const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;
```

获取所有托管窗口的集合。

**返回值**：窗口名称到窗口指针的映射表引用

**示例**：
```cpp
for (const auto& [name, window] : app.Windows()) {
    std::cout << "窗口: " << name << std::endl;
}
```

### GetWindow

```cpp
ui::WindowPtr GetWindow(const std::string& name) const;
```

根据名称获取窗口。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `const std::string&` | 窗口的标识名称 |

**返回值**：找到的窗口智能指针，如果不存在则返回 `nullptr`

**示例**：
```cpp
auto settingsWindow = app.GetWindow("settings");
if (settingsWindow) {
    settingsWindow->Show();
}
```

### GetMainWindow

```cpp
ui::WindowPtr GetMainWindow() const;
```

获取主窗口。

**返回值**：主窗口智能指针

**示例**：
```cpp
auto mainWindow = app.GetMainWindow();
if (mainWindow) {
    mainWindow->SetTitle("新标题");
}
```

---

## 生命周期方法

### Run

```cpp
void Run(ui::WindowPtr mainWindow);
```

启动应用程序消息循环。此方法会阻塞直到应用程序退出。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `mainWindow` | `ui::WindowPtr` | 主窗口智能指针 |

**说明**：
- 如果应用程序已在运行，立即返回
- 如果主窗口为空，输出错误并返回
- 触发 `Startup` 事件
- 自动显示主窗口
- 进入消息循环，处理事件、更新动画、渲染窗口
- 主窗口关闭时退出循环，调用 `Shutdown()`

**示例**：
```cpp
fk::Application app;
auto* window = app.CreateWindow();
window->SetTitle("我的应用");

auto mainWindow = std::make_shared<fk::ui::Window>(*window);
app.Run(mainWindow);
```

### Shutdown

```cpp
void Shutdown();
```

关闭应用程序。

**说明**：
- 如果应用程序未运行，不执行任何操作
- 设置运行状态为 `false`
- 隐藏并清除所有窗口
- 触发 `Exit` 事件

**示例**：
```cpp
// 在某个事件处理中关闭应用
button->OnClick([&app]() {
    app.Shutdown();
});
```

### IsRunning

```cpp
bool IsRunning() const;
```

检查应用程序是否正在运行。

**返回值**：`true` 表示正在运行，`false` 表示未运行

**示例**：
```cpp
if (app.IsRunning()) {
    std::cout << "应用程序正在运行" << std::endl;
}
```

---

## 事件

### Startup

```cpp
core::Event<> Startup;
```

应用程序启动时触发的事件。在 `Run()` 方法中主窗口显示前触发。

**示例**：
```cpp
app.Startup += []() {
    std::cout << "应用程序正在启动..." << std::endl;
    // 执行初始化任务
};
```

### Exit

```cpp
core::Event<> Exit;
```

应用程序退出时触发的事件。在 `Shutdown()` 方法中所有窗口关闭后触发。

**示例**：
```cpp
app.Exit += []() {
    std::cout << "应用程序正在退出..." << std::endl;
    // 执行清理任务
};
```

### Activated

```cpp
core::Event<> Activated;
```

应用程序被激活时触发的事件。

**示例**：
```cpp
app.Activated += []() {
    std::cout << "应用程序已激活" << std::endl;
};
```

### Deactivated

```cpp
core::Event<> Deactivated;
```

应用程序失去焦点时触发的事件。

**示例**：
```cpp
app.Deactivated += []() {
    std::cout << "应用程序已失去焦点" << std::endl;
};
```

---

## 使用示例

### 最小应用程序

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"

int main() {
    fk::Application app;
    
    // 创建并配置窗口
    auto* window = app.CreateWindow();
    window->SetTitle("Hello F__K_UI");
    window->SetSize(800, 600);
    
    // 运行应用程序
    auto mainWindow = app.GetWindow("Window_" + std::to_string(reinterpret_cast<uintptr_t>(window)));
    app.Run(mainWindow);
    
    return 0;
}
```

### 带事件处理的应用程序

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include <iostream>

int main() {
    fk::Application app;
    
    // 注册启动事件
    app.Startup += []() {
        std::cout << "应用程序正在启动..." << std::endl;
    };
    
    // 注册退出事件
    app.Exit += []() {
        std::cout << "应用程序正在退出..." << std::endl;
    };
    
    // 创建窗口
    auto* window = app.CreateWindow();
    window->SetTitle("事件示例");
    window->SetSize(1024, 768);
    
    // 获取主窗口并运行
    for (const auto& [name, win] : app.Windows()) {
        app.Run(win);
        break;
    }
    
    return 0;
}
```

### 多窗口管理

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"

int main() {
    fk::Application app;
    
    // 创建主窗口
    auto mainWindow = std::make_shared<fk::ui::Window>();
    mainWindow->SetTitle("主窗口");
    mainWindow->SetSize(800, 600);
    app.AddWindow(mainWindow, "main");
    
    // 创建设置窗口
    auto settingsWindow = std::make_shared<fk::ui::Window>();
    settingsWindow->SetTitle("设置");
    settingsWindow->SetSize(400, 300);
    app.AddWindow(settingsWindow, "settings");
    
    // 获取窗口
    auto settings = app.GetWindow("settings");
    if (settings) {
        // 配置设置窗口
    }
    
    // 移除窗口
    app.RemoveWindow("settings");
    
    // 运行主窗口
    app.Run(app.GetWindow("main"));
    
    return 0;
}
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `instance_` | `static Application*` | 单例实例指针 |
| `windows_` | `std::unordered_map<std::string, ui::WindowPtr>` | 托管窗口集合 |
| `mainWindow_` | `ui::WindowPtr` | 主窗口指针 |
| `isRunning_` | `bool` | 应用程序运行状态 |

---

## 相关类

- [Window](../UI/Window.md) - 顶级窗口类
- [Event](../Core/Event.md) - 事件系统

## 另请参阅

- [入门指南](../../Guides/GettingStarted.md)
