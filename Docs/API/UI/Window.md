# Window 文档

## 概述

`Window` 是应用程序的顶级窗口容器，通常是应用 UI 的根元素。Window 管理窗口生命周期、事件处理、显示/隐藏、以及与操作系统的交互。

**继承链：** `UIElement` → `FrameworkElement` → `ContentControl` → `Window`

**命名空间：** `fk::ui`

**职责：**
- ✅ 管理应用窗口
- ✅ 处理窗口事件（Loaded、Closed、SizeChanged 等）
- ✅ 显示/隐藏窗口
- ✅ 管理窗口内容（通常是一个布局容器）

---

## 核心属性

### Title
- **类型：** `std::string`
- **默认值：** `""`
- **说明：** 窗口标题栏显示的文本
- **可绑定：** ✅ 是

```cpp
auto window = ui::window()
    ->Title("My Application");
```

### Width / Height
- **类型：** `float`
- **默认值：** 依赖于内容或系统默认值
- **说明：** 窗口的宽度和高度（单位：像素）
- **可绑定：** ✅ 是

```cpp
ui::window()
    ->Title("Application")
    ->Width(800.0f)
    ->Height(600.0f);
```

### MinWidth / MaxWidth / MinHeight / MaxHeight
- **类型：** `float`
- **默认值：** 系统默认值
- **说明：** 窗口尺寸约束

```cpp
ui::window()
    ->MinWidth(400.0f)
    ->MinHeight(300.0f)
    ->MaxWidth(1600.0f)
    ->MaxHeight(1200.0f);
```

### Content
- **类型：** `std::shared_ptr<UIElement>`
- **默认值：** `nullptr`
- **说明：** 窗口内的内容（通常是一个 Panel 或其他容器）
- **可绑定：** ✅ 是

```cpp
auto window = ui::window()
    ->Title("Main Window")
    ->Content(
        ui::stackPanel()
            ->Orientation(Orientation::Vertical)
            ->Children({
                ui::textBlock()->Text("欢迎"),
                ui::button()->Content("点击")
            })
    );
```

---

## 窗口状态属性

### IsActive
- **类型：** `bool` (只读)
- **说明：** 窗口是否为活动窗口（具有焦点）

```cpp
if (window->IsActive()) {
    // 窗口有焦点
}
```

### WindowState
- **类型：** `WindowState` (枚举)
- **默认值：** `WindowState::Normal`
- **说明：** 窗口的显示状态
- **可选值：**
  - `WindowState::Normal` - 正常显示
  - `WindowState::Minimized` - 最小化
  - `WindowState::Maximized` - 最大化
- **可绑定：** ✅ 是

```cpp
window->WindowState(WindowState::Maximized);  // 最大化

window->WindowState(WindowState::Minimized);  // 最小化

window->WindowState(WindowState::Normal);     // 恢复正常
```

### Resizable
- **类型：** `bool`
- **默认值：** `true`
- **说明：** 窗口是否可调整大小

```cpp
ui::window()
    ->Resizable(false);  // 固定尺寸窗口
```

### Topmost
- **类型：** `bool`
- **默认值：** `false`
- **说明：** 窗口是否始终在其他窗口上方

```cpp
ui::window()
    ->Topmost(true);  // 始终在最前面
```

### ShowInTaskbar
- **类型：** `bool`
- **默认值：** `true`
- **说明：** 窗口是否在任务栏显示

```cpp
ui::window()
    ->ShowInTaskbar(false);  // 隐藏任务栏图标
```

---

## 窗口事件

### Loaded
在窗口初始化完成后触发。

```cpp
window->OnLoaded([](Window* w) {
    printf("窗口已加载\n");
});
```

### Closed
在窗口关闭时触发。

```cpp
window->OnClosed([](Window* w) {
    printf("窗口已关闭\n");
});
```

### SizeChanged
当窗口尺寸改变时触发。

```cpp
window->OnSizeChanged([](SizeChangedEventArgs& args) {
    printf("新尺寸：%.0f x %.0f\n", 
           args.newSize.width, args.newSize.height);
});
```

### LocationChanged
当窗口位置改变时触发。

```cpp
window->OnLocationChanged([](PointChangedEventArgs& args) {
    printf("新位置：(%.0f, %.0f)\n", 
           args.newPoint.x, args.newPoint.y);
});
```

### StateChanged
当窗口状态改变时触发。

```cpp
window->OnStateChanged([](Window* w) {
    if (w->WindowState() == WindowState::Maximized) {
        printf("窗口已最大化\n");
    }
});
```

### Activated
窗口获得焦点时触发。

```cpp
window->OnActivated([](Window* w) {
    printf("窗口获得焦点\n");
});
```

### Deactivated
窗口失去焦点时触发。

```cpp
window->OnDeactivated([](Window* w) {
    printf("窗口失去焦点\n");
});
```

---

## 窗口方法

### void Show()
显示窗口并使其成为活动窗口。

```cpp
window->Show();
```

### void Hide()
隐藏窗口。

```cpp
window->Hide();
```

### void Close()
关闭窗口。

```cpp
window->Close();
```

### void Minimize()
最小化窗口。

```cpp
window->Minimize();
```

### void Maximize()
最大化窗口。

```cpp
window->Maximize();
```

### void Restore()
恢复窗口到正常大小。

```cpp
window->Restore();
```

### void Activate()
激活窗口（使其获得焦点）。

```cpp
window->Activate();
```

### void SetPosition(float x, float y)
设置窗口位置。

```cpp
window->SetPosition(100.0f, 100.0f);  // 移动到 (100, 100)
```

### PointF GetPosition() const
获取窗口的当前位置。

```cpp
auto pos = window->GetPosition();
printf("窗口位置：(%.0f, %.0f)\n", pos.x, pos.y);
```

---

## 完整使用示例

### 示例 1：简单应用窗口

```cpp
auto window = ui::window()
    ->Title("Simple Application")
    ->Width(500.0f)
    ->Height(400.0f)
    ->Content(
        ui::stackPanel()
            ->Orientation(Orientation::Vertical)
            ->Padding({20, 20, 20, 20})
            ->Spacing(10.0f)
            ->Children({
                ui::textBlock()
                    ->Text("欢迎使用我的应用")
                    ->FontSize(18.0f)
                    ->FontWeight(FontWeight::Bold),
                
                ui::textBlock()
                    ->Text("这是一个简单的演示窗口"),
                
                ui::button()
                    ->Content("关闭")
                    ->OnClick([window](Button*) {
                        window->Close();
                    })
            })
    )
    ->OnLoaded([](Window* w) {
        printf("应用已启动\n");
    })
    ->OnClosed([](Window* w) {
        printf("应用已退出\n");
    });

window->Show();
```

### 示例 2：可调整尺寸的窗口

```cpp
auto window = ui::window()
    ->Title("Resizable Window")
    ->MinWidth(300.0f)
    ->MinHeight(200.0f)
    ->MaxWidth(1600.0f)
    ->MaxHeight(1200.0f)
    ->Width(800.0f)
    ->Height(600.0f)
    ->Resizable(true)
    ->OnSizeChanged([](SizeChangedEventArgs& args) {
        printf("新尺寸：%.0f x %.0f\n", 
               args.newSize.width, args.newSize.height);
    });
```

### 示例 3：多窗口应用

```cpp
auto mainWindow = ui::window()
    ->Title("Main Window")
    ->Width(400.0f)
    ->Height(300.0f)
    ->Content(
        ui::button()
            ->Content("打开对话框")
            ->OnClick([](Button*) {
                auto dialog = ui::window()
                    ->Title("Dialog")
                    ->Width(300.0f)
                    ->Height(200.0f)
                    ->Topmost(true)  // 始终在主窗口上方
                    ->Content(
                        ui::textBlock()->Text("这是一个对话框")
                    );
                dialog->Show();
            })
    );

mainWindow->Show();
```

### 示例 4：响应窗口事件

```cpp
auto window = ui::window()
    ->Title("Event Demo")
    ->Width(600.0f)
    ->Height(400.0f)
    ->OnLoaded([](Window* w) {
        printf("✓ 窗口已加载\n");
    })
    ->OnActivated([](Window* w) {
        printf("✓ 窗口获得焦点\n");
    })
    ->OnDeactivated([](Window* w) {
        printf("✓ 窗口失去焦点\n");
    })
    ->OnStateChanged([](Window* w) {
        switch (w->WindowState()) {
            case WindowState::Normal:
                printf("✓ 窗口状态：正常\n");
                break;
            case WindowState::Minimized:
                printf("✓ 窗口状态：最小化\n");
                break;
            case WindowState::Maximized:
                printf("✓ 窗口状态：最大化\n");
                break;
        }
    })
    ->OnSizeChanged([](SizeChangedEventArgs& args) {
        printf("✓ 窗口尺寸变化：%.0f x %.0f\n",
               args.newSize.width, args.newSize.height);
    })
    ->OnClosed([](Window* w) {
        printf("✓ 窗口已关闭\n");
    });

window->Show();
```

---

## 从 ContentControl 继承的属性

### Content
窗口的主要内容（详见 [Control.md](Control.md)）。

---

## 从 FrameworkElement 继承的属性

### Margin / Padding / MinWidth / MaxWidth 等
详见 [FrameworkElement.md](FrameworkElement.md)

---

## 从 UIElement 继承的属性

### Visibility / IsEnabled / Opacity 等
详见 [UIElement.md](UIElement.md)

---

## 常见问题

### Q1：如何检测窗口是否还是活动的？
**A：** 使用 `IsActive` 属性或订阅 `Activated` / `Deactivated` 事件：
```cpp
if (window->IsActive()) {
    // 窗口有焦点
}
```

### Q2：如何保存和恢复窗口的位置与尺寸？
**A：** 在关闭前保存，重新打开时恢复：
```cpp
// 保存
float x = window->GetPosition().x;
float y = window->GetPosition().y;
float w = window->ActualWidth();
float h = window->ActualHeight();

// 恢复
window->SetPosition(x, y);
window->Width(w)->Height(h);
```

### Q3：如何创建模态对话框？
**A：** 设置 `Topmost(true)` 并禁用主窗口的交互：
```cpp
auto dialog = ui::window()
    ->Topmost(true)
    ->Title("Confirmation")
    ->Content(...);

mainWindow->IsEnabled(false);
dialog->Show();

dialog->OnClosed([mainWindow](Window*) {
    mainWindow->IsEnabled(true);
});
```

### Q4：窗口最小化时是否仍在处理事件？
**A：** 是的。最小化不影响事件处理，但可能会暂停渲染以节省资源。

### Q5：如何禁止关闭窗口？
**A：** 需要在关闭前拦截（具体方式取决于框架是否支持 `Closing` 事件）。当前可在 `Closed` 后重新打开。

---

## 最佳实践

1. **设置合理的尺寸约束：**
   ```cpp
   window->MinWidth(400.0f)->MinHeight(300.0f);
   ```

2. **使用内容容器：**
   ```cpp
   // ✅ 推荐
   auto content = ui::stackPanel()->Children({...});
   window->Content(content);
   
   // ⚠️ 避免在 Content 中直接放置多个元素
   ```

3. **保存用户偏好的窗口状态：**
   ```cpp
   auto settings = LoadSettings();
   window->Width(settings.windowWidth)
         ->Height(settings.windowHeight)
         ->SetPosition(settings.posX, settings.posY);
   ```

4. **正确处理关闭事件：**
   ```cpp
   window->OnClosed([](Window*) {
       SaveState();
       // 执行清理操作
   });
   ```

---

## 性能建议

- **多个窗口：** 每个窗口都有独立的渲染上下文，过多窗口会增加内存开销
- **频繁显示/隐藏：** 考虑缓存窗口而不是频繁创建销毁
- **大型内容：** 如果窗口内容复杂，使用虚拟化容器提高性能

---

## 另见

- [Control.md](Control.md) - 控件基类（Content 属性）
- [FrameworkElement.md](FrameworkElement.md) - 布局基础
- [UIElement.md](UIElement.md) - 基础元素
- [Application.md](../App/Application.md) - 应用框架（待完成）
