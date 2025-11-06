# App 模块 API 文档

应用程序框架模块,提供应用程序生命周期管理。

命名空间: `fk`

---

## 目录

- [Application](#application) - 应用程序类

---

## Application

**应用程序类** - 管理应用程序的生命周期、窗口和消息循环。

### 类定义

```cpp
class Application {
public:
    Application();
    virtual ~Application();
    
    static Application* Current();
    
    void Run(ui::WindowPtr mainWindow);
    void Shutdown();
    
    bool IsRunning() const;
    
    // 窗口管理
    void AddWindow(const ui::WindowPtr& window, const std::string& name);
    void RemoveWindow(const ui::WindowPtr& window);
    void RemoveWindow(const std::string& name);
    ui::WindowPtr GetWindow(const std::string& name) const;
    ui::WindowPtr GetMainWindow() const;
    const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;
    
    // 事件
    core::Event<> Startup;
    core::Event<> Exit;
    core::Event<> Activated;
    core::Event<> Deactivated;
};
```

### 核心方法

#### Current()

获取当前应用程序实例

```cpp
static Application* Current();
```

**返回:** 当前应用程序实例指针

**线程安全:** 是

**注意:** 必须先创建 Application 对象,否则返回 nullptr

**示例:**
```cpp
Application app;
// ...
Application* current = Application::Current();  // 获取全局实例
```

#### Run()

运行主消息循环

```cpp
void Run(ui::WindowPtr mainWindow);
```

**参数:**
- `mainWindow` - 主窗口

**线程安全:** 否(必须在主线程调用)

**作用:** 
- 显示主窗口
- 触发 Startup 事件
- 进入消息循环
- 主窗口关闭时退出循环
- 触发 Exit 事件

**注意:** 此方法会阻塞直到应用程序退出

**示例:**
```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

int main() {
    Application app;
    
    // 创建主窗口 (推荐使用工厂函数)
    auto mainWindow = ui::window()
        ->Title("My Application")
        ->Width(800)
        ->Height(600);
    
    // 设置窗口内容
    auto button = ui::button()
        ->Content("Click Me");
    mainWindow->Content(button);
    
    // 订阅启动事件 (推荐使用 += 运算符)
    app.Startup += [] {
        std::cout << "Application started" << std::endl;
    };
    
    // 订阅退出事件
    app.Exit += [] {
        std::cout << "Application exiting" << std::endl;
    };
    
    // 运行应用程序
    app.Run(mainWindow);
    
    return 0;
}
```

#### Shutdown()

关闭应用程序

```cpp
void Shutdown();
```

**线程安全:** 是

**作用:** 关闭所有窗口,退出消息循环

**示例:**
```cpp
// 在按钮点击事件中关闭应用
exitButton->Click += [](ui::detail::ButtonBase&) {
    Application::Current()->Shutdown();
};
```

#### IsRunning()

检查应用程序是否正在运行

```cpp
bool IsRunning() const;
```

**返回:** 如果应用程序正在运行返回 true

**线程安全:** 是

**示例:**
```cpp
while (app.IsRunning()) {
    // 自定义处理...
}
```

### 窗口管理

#### AddWindow()

添加窗口到应用程序

```cpp
void AddWindow(const ui::WindowPtr& window, const std::string& name);
```

**参数:**
- `window` - 窗口智能指针
- `name` - 窗口名称(用于查找)

**线程安全:** 否

**使用场景:** 管理多个窗口

**示例:**
```cpp
auto settingsWindow = ui::window()
    ->Title("Settings");

app.AddWindow(settingsWindow, "Settings");

// 稍后可以通过名称获取
auto window = app.GetWindow("Settings");
window->Show();
```

#### RemoveWindow() - 通过窗口指针

移除指定窗口

```cpp
void RemoveWindow(const ui::WindowPtr& window);
```

**参数:**
- `window` - 要移除的窗口

**线程安全:** 否

**示例:**
```cpp
app.RemoveWindow(settingsWindow);
```

#### RemoveWindow() - 通过名称

移除指定名称的窗口

```cpp
void RemoveWindow(const std::string& name);
```

**参数:**
- `name` - 窗口名称

**线程安全:** 否

**示例:**
```cpp
app.RemoveWindow("Settings");
```

#### GetWindow()

通过名称获取窗口

```cpp
ui::WindowPtr GetWindow(const std::string& name) const;
```

**参数:**
- `name` - 窗口名称

**返回:** 窗口智能指针,未找到返回 nullptr

**线程安全:** 否

**示例:**
```cpp
auto window = app.GetWindow("Settings");
if (window) {
    window->Show();
} else {
    std::cout << "Window not found" << std::endl;
}
```

#### GetMainWindow()

获取主窗口

```cpp
ui::WindowPtr GetMainWindow() const;
```

**返回:** 主窗口指针(传递给 Run() 的窗口)

**线程安全:** 否

**示例:**
```cpp
auto mainWindow = app.GetMainWindow();
mainWindow->Title("New Title");
```

#### Windows()

获取所有窗口

```cpp
const std::unordered_map<std::string, ui::WindowPtr>& Windows() const;
```

**返回:** 窗口映射表(名称 -> 窗口)

**线程安全:** 否

**示例:**
```cpp
for (const auto& [name, window] : app.Windows()) {
    std::cout << "Window: " << name << std::endl;
}
```

### 事件

#### Startup

应用程序启动事件

```cpp
core::Event<> Startup;
```

**触发时机:** Run() 方法开始时,主窗口显示前

**使用场景:** 
- 初始化全局资源
- 加载配置
- 建立数据库连接

**示例:**
```cpp
app.Startup += [] {
    std::cout << "Application starting..." << std::endl;
    // 初始化代码...
};
```

#### Exit

应用程序退出事件

```cpp
core::Event<> Exit;
```

**触发时机:** 主窗口关闭后,Run() 方法返回前

**使用场景:**
- 保存应用程序状态
- 清理资源
- 关闭数据库连接

**示例:**
```cpp
app.Exit += [] {
    std::cout << "Application exiting..." << std::endl;
    // 清理代码...
};
```

#### Activated

应用程序激活事件

```cpp
core::Event<> Activated;
```

**触发时机:** 应用程序获得焦点时

**使用场景:** 
- 恢复活动状态
- 刷新数据

**示例:**
```cpp
app.Activated += [] {
    std::cout << "Application activated" << std::endl;
};
```

#### Deactivated

应用程序失活事件

```cpp
core::Event<> Deactivated;
```

**触发时机:** 应用程序失去焦点时

**使用场景:**
- 暂停活动
- 保存当前状态

**示例:**
```cpp
app.Deactivated += [] {
    std::cout << "Application deactivated" << std::endl;
};
```

---

## 完整应用示例

### 单窗口应用

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"

int main() {
    using namespace fk;
    
    Application app;
    
    // 创建主窗口
    auto mainWindow = ui::window()
        ->Title("Single Window App")
        ->Width(400)
        ->Height(300);
    
    // 创建 UI
    auto panel = ui::stackPanel()
        ->Spacing(10.0f);
    
    auto titleText = ui::textBlock()
        ->Text("Welcome to F__K_UI!")
        ->FontSize(24.0f);
    
    auto button = ui::button()
        ->Content("Exit");
    
    // 点击按钮退出
    button->Click += [](ui::detail::ButtonBase&) {
        Application::Current()->Shutdown();
    };
    
    panel->AddChild(titleText);
    panel->AddChild(button);
    
    mainWindow->Content(panel);
    
    // 运行
    app.Run(mainWindow);
    
    return 0;
}
```

### 多窗口应用

```cpp
#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"

class MyApplication {
public:
    void Run() {
        Application app;
        
        // 创建主窗口
        auto mainWindow = ui::window()
            ->Title("Main Window")
            ->Width(600)
            ->Height(400);
        
        // 创建"打开设置"按钮
        auto openSettingsBtn = ui::button()
            ->Content("Open Settings");
        
        openSettingsBtn->Click += [this, &app](ui::detail::ButtonBase&) {
            OpenSettingsWindow(app);
        };
        
        mainWindow->Content(openSettingsBtn);
        
        // 订阅事件
        app.Startup += [] {
            std::cout << "Application started" << std::endl;
        };
        
        app.Exit += [] {
            std::cout << "Application closing" << std::endl;
        });
        
        // 运行
        app.Run(mainWindow);
    }
    
private:
    void OpenSettingsWindow(Application& app) {
        // 检查设置窗口是否已存在
        auto existing = app.GetWindow("Settings");
        if (existing) {
            existing->Show();
            return;
        }
        
        // 创建新的设置窗口
        auto settingsWindow = ui::window()
            ->Title("Settings")
            ->Width(400)
            ->Height(300);
        
        auto closeBtn = ui::button()
            ->Content("Close");
        
        closeBtn->Click += [settingsWindow](ui::detail::ButtonBase&) {
            settingsWindow->Close();
        };
        
        settingsWindow->Content(closeBtn);
        settingsWindow->Closed += [&app] {
            app.RemoveWindow("Settings");
        };
        
        // 添加到应用程序
        app.AddWindow(settingsWindow, "Settings");
        settingsWindow->Show();
    }
};

int main() {
    MyApplication app;
    app.Run();
    return 0;
}
```

### MVVM 应用

```cpp
#include "fk/app/Application.h"
#include "fk/binding/ObservableObject.h"

// ViewModel
class MainViewModel : public fk::binding::ObservableObject {
public:
    void SetUserName(const std::string& value) {
        if (SetProperty(userName_, value, "UserName")) {
            OnPropertyChanged("Greeting");
        }
    }
    
    const std::string& GetUserName() const { return userName_; }
    
    std::string GetGreeting() const {
        return userName_.empty() ? "Hello, Guest!" : "Hello, " + userName_ + "!";
    }
    
    void UpdateGreeting() {
        OnPropertyChanged("Greeting");
    }
    
private:
    std::string userName_;
};

int main() {
    using namespace fk;
    
    Application app;
    
    // 创建 ViewModel
    auto viewModel = std::make_shared<MainViewModel>();
    
    // 创建 View
    auto mainWindow = ui::window()
        ->Title("MVVM Example")
        ->Width(400)
        ->Height(200)
        ->SetDataContext(viewModel);
    
    auto panel = ui::stackPanel()
        ->Spacing(10.0f);
    
    // 显示问候语
    auto greetingText = ui::textBlock()
        ->FontSize(18.0f);
    greetingText->SetBinding(
        ui::detail::TextBlockBase::TextProperty(),
        binding::Binding().Path("Greeting")
    );
    
    // 输入框(假设有 TextBox 控件)
    // auto nameInput = ui::textBox();
    // nameInput->SetBinding(
    //     ui::detail::TextBoxBase::TextProperty(),
    //     binding::Binding().Path("UserName").Mode(binding::BindingMode::TwoWay)
    // );
    
    // 更新按钮
    auto updateBtn = ui::button()
        ->Content("Update");
    
    updateBtn->Click += [viewModel](ui::detail::ButtonBase&) {
        viewModel->SetUserName("John Doe");
    };
    
    panel->AddChild(greetingText);
    // panel->AddChild(nameInput);
    panel->AddChild(updateBtn);
    
    mainWindow->Content(panel);
    
    // 运行
    app.Run(mainWindow);
    
    return 0;
}
```

---

## 生命周期

应用程序的生命周期流程:

```
1. 创建 Application 对象
   ↓
2. 配置应用程序(订阅事件、设置全局状态)
   ↓
3. 创建主窗口和 UI
   ↓
4. 调用 app.Run(mainWindow)
   ↓
5. 触发 Startup 事件
   ↓
6. 显示主窗口
   ↓
7. 进入消息循环
   │ - 处理窗口事件
   │ - 渲染帧
   │ - 重复...
   ↓
8. 主窗口关闭或调用 Shutdown()
   ↓
9. 触发 Exit 事件
   ↓
10. Run() 方法返回
    ↓
11. Application 对象销毁
```

---

## 线程安全说明

| 方法 | 线程安全性 |
|---|---|
| Current() | 是 |
| Run() | 否(必须在主线程) |
| Shutdown() | 是 |
| IsRunning() | 是 |
| 窗口管理方法 | 否(必须在主线程) |
| 事件订阅 | 是 |

---

## 最佳实践

### 应用程序结构

1. **使用单一 Application 实例**
   ```cpp
   int main() {
       Application app;  // 栈上创建
       // ...
       app.Run(mainWindow);
       return 0;
   }
   ```

2. **正确的初始化顺序**
   ```cpp
   // 1. 创建 Application
   Application app;
   
   // 2. 订阅事件 (推荐使用 += 运算符)
   app.Startup += [] { /* 初始化 */ };
   app.Exit += [] { /* 清理 */ };
   
   // 3. 创建 UI
   auto window = CreateWindow();
   
   // 4. 运行
   app.Run(window);
   ```

3. **使用 Startup 事件初始化**
   ```cpp
   app.Startup += [] {
       // 加载配置
       LoadConfiguration();
       
       // 初始化服务
       InitializeServices();
       
       // 连接数据库
       ConnectDatabase();
   });
   ```

### 窗口管理

1. **为重要窗口命名**
   ```cpp
   app.AddWindow(mainWindow, "Main");
   app.AddWindow(settingsWindow, "Settings");
   app.AddWindow(aboutWindow, "About");
   ```

2. **检查窗口是否已存在**
   ```cpp
   void ShowWindow(Application& app, const std::string& name) {
       auto window = app.GetWindow(name);
       if (window) {
           window->Show();  // 显示已存在的窗口
       } else {
           // 创建新窗口...
       }
   }
   ```

3. **清理关闭的窗口**
   ```cpp
   window->Closed += [&app, name = windowName] {
       app.RemoveWindow(name);
   };
   ```

### 应用程序退出

1. **提供退出确认**
   ```cpp
   mainWindow->Closed += [&app] {
       if (ConfirmExit()) {
           app.Shutdown();
       } else {
           // 取消关闭(需要额外实现)
       }
   };
   ```

2. **保存状态后退出**
   ```cpp
   app.Exit += [] {
       SaveApplicationState();
       CloseConnections();
       FlushLogs();
   };
   ```

3. **优雅地关闭所有窗口**
   ```cpp
   void GracefulShutdown(Application& app) {
       // 关闭所有次要窗口
       for (const auto& [name, window] : app.Windows()) {
           if (window != app.GetMainWindow()) {
               window->Close();
           }
       }
       
       // 最后关闭主窗口
       app.GetMainWindow()->Close();
   }
   ```

### 错误处理

1. **使用 try-catch 保护 Run()**
   ```cpp
   try {
       app.Run(mainWindow);
   } catch (const std::exception& e) {
       std::cerr << "Application error: " << e.what() << std::endl;
       return 1;
   }
   ```

2. **事件处理器中的异常**
   ```cpp
   app.Startup += [] {
       try {
           InitializeApplication();
       } catch (const std::exception& e) {
           ShowErrorDialog("Initialization failed: " + std::string(e.what()));
           Application::Current()->Shutdown();
       }
   };
   ```
