# F__K_UI 架构优化方案

## 当前架构问题分析

### 1. **Window 类职责过重**

**问题**：
- Window 直接管理 GLFW 窗口创建和销毁
- Window 直接处理消息循环
- Window 同时负责内容管理和渲染协调
- 缺少与 Application 的有效整合

**WPF 参考**：
- WPF 的 `Window` 继承自 `ContentControl`，复用布局和内容系统
- `Application` 负责消息循环和窗口生命周期管理
- `HwndSource` 封装原生窗口句柄

### 2. **Application 与 Window 的关系松散**

**问题**：
```cpp
// 当前：Application 只是一个窗口容器
app.AddWindow(window, "name");
window->Show();  // Window 自己管理生命周期
```

**应该**：
```cpp
// Application 主导窗口生命周期
app.Run(mainWindow);  // Application 控制消息循环
```

### 3. **RenderHost 没有与 Visual 树充分整合**

**问题**：
- RenderHost 需要手动传入 Visual 根节点
- 没有自动监听 Visual 树变化
- 渲染触发机制不够自动化

**应该**：
- RenderHost 自动订阅 Visual 树的失效通知
- 布局变化自动触发渲染
- 支持脏矩形优化

### 4. **缺少 CompositionTarget**

**WPF 有**：
- `CompositionTarget.Rendering` 事件（每帧渲染前触发）
- 统一的渲染时钟
- 动画系统集成

### 5. **Dispatcher 与 UI 元素的整合不足**

**问题**：
- UIElement 有 `DispatcherObject` 基类，但很少使用
- 跨线程属性设置没有自动调度
- 缺少 `VerifyAccess()` 检查

### 6. **Visual 树和逻辑树分离不清晰**

**WPF 设计**：
- **逻辑树**：FrameworkElement 层次（业务逻辑）
- **Visual 树**：Visual 层次（渲染优化）
- 模板可以扩展 Visual 树但不改变逻辑树

**当前问题**：
- 只有一棵树，既是逻辑树又是 Visual 树
- 无法优化渲染（如虚拟化）

---

## 优化方案

### 方案 1：Window 重构（高优先级）

#### 1.1 让 Window 继承 ContentControl

```cpp
// 修改前
class Window : public std::enable_shared_from_this<Window> {
    std::shared_ptr<UIElement> content_;
};

// 修改后
class Window : public ContentControl {
    // Window 本身就是一个 ContentControl
    // 自动拥有 Content 属性和布局能力
};
```

**优势**：
- 复用 ContentControl 的所有功能
- Content 变更自动触发布局
- 统一的属性系统

#### 1.2 引入 WindowInteropHelper

```cpp
// 封装原生窗口操作
class WindowInteropHelper {
public:
    explicit WindowInteropHelper(Window* window);
    
    void* GetHandle() const;  // 返回 GLFWwindow*
    void EnsureHandle();      // 延迟创建窗口
    
private:
    Window* owner_;
    GLFWwindow* handle_{nullptr};
};
```

**优势**：
- Window 类更纯粹（专注 UI）
- 原生窗口操作集中管理
- 便于平台抽象

#### 1.3 消息循环移到 Application

```cpp
class Application {
public:
    void Run(std::shared_ptr<ui::Window> mainWindow);
    
private:
    void DoEvents();  // 处理一次消息
    void RunMessageLoop();
    
    std::shared_ptr<ui::Window> mainWindow_;
};
```

---

### 方案 2：引入 CompositionTarget（中优先级）

```cpp
// 渲染时钟和合成目标
class CompositionTarget {
public:
    // 每帧渲染前触发
    static Event<RenderingEventArgs> Rendering;
    
    // 获取渲染帧率
    static int GetTargetFrameRate();
    static void SetTargetFrameRate(int fps);
    
private:
    static void OnRenderingTimer();
};

// 使用
CompositionTarget::Rendering += [](const RenderingEventArgs& e) {
    // 更新动画
    // 计算时间差
};
```

**优势**：
- 统一的渲染时钟
- 动画系统基础
- 性能监控支持

---

### 方案 3：RenderHost 自动化（高优先级）

#### 3.1 自动失效追踪

```cpp
class UIElement {
    void InvalidateVisual() {
        if (auto host = GetRenderHost()) {
            host->InvalidateElement(this);
        }
    }
    
private:
    RenderHost* GetRenderHost() const;
};

class RenderHost {
public:
    void InvalidateElement(UIElement* element) {
        dirtyElements_.insert(element);
        RequestRender();
    }
    
private:
    std::unordered_set<UIElement*> dirtyElements_;
};
```

#### 3.2 自动连接到 Window

```cpp
class Window : public ContentControl {
protected:
    void OnContentChanged(UIElement* oldContent, UIElement* newContent) override {
        if (renderHost_) {
            renderHost_->SetRootVisual(newContent);
        }
    }
};
```

---

### 方案 4：Dispatcher 自动检查（中优先级）

```cpp
class UIElement {
    void SetValue(DependencyProperty* dp, std::any value) {
        VerifyAccess();  // 自动检查线程
        
        // 或者自动调度
        if (!CheckAccess()) {
            GetDispatcher()->InvokeAsync([=]() {
                SetValue(dp, value);
            });
            return;
        }
        
        // 实际设置
        DependencyObject::SetValue(dp, value);
    }
    
private:
    void VerifyAccess() const {
        if (!CheckAccess()) {
            throw std::runtime_error("Cross-thread operation not allowed");
        }
    }
    
    bool CheckAccess() const {
        return GetDispatcher()->CheckAccess();
    }
};
```

---

### 方案 5：分离 Visual 树和逻辑树（低优先级，未来考虑）

```cpp
// 逻辑树节点
class LogicalTreeHelper {
public:
    static UIElement* GetParent(UIElement* element);
    static std::vector<UIElement*> GetChildren(UIElement* element);
};

// Visual 树节点（用于渲染优化）
class VisualTreeHelper {
public:
    static Visual* GetParent(Visual* visual);
    static std::vector<Visual*> GetChildren(Visual* visual);
    static int GetChildrenCount(Visual* visual);
};

// 支持虚拟化
class VirtualizingPanel : public Panel {
    // 只创建可见的 Visual
    // 滚动时动态创建/销毁
};
```

---

## 重构步骤（渐进式）

### 阶段 1：Window 重构（1-2周）

1. ✅ 创建 `WindowInteropHelper` 类
2. ✅ Window 继承 `ContentControl`
3. ✅ 移除 Window 的 `SetContent/GetContent`（使用继承的）
4. ✅ 消息循环移到 `Application::Run()`

**影响**：
- 破坏性变更（API 变化）
- 需要更新所有示例代码

### 阶段 2：RenderHost 自动化（1周）

1. ✅ 实现 `InvalidateVisual()` 追踪
2. ✅ RenderHost 自动连接到 Window
3. ✅ 布局变化自动触发渲染

**影响**：
- 非破坏性（内部优化）
- 性能提升

### 阶段 3：Dispatcher 增强（3-5天）

1. ✅ 添加 `VerifyAccess()` 检查
2. ✅ DependencyObject 属性设置自动调度
3. ✅ 可配置是否强制检查（调试模式）

**影响**：
- 非破坏性
- 提高线程安全性

### 阶段 4：CompositionTarget（1周）

1. ✅ 实现渲染时钟
2. ✅ `Rendering` 事件
3. ✅ 集成到 Application 消息循环

**影响**：
- 非破坏性（新增功能）
- 为动画系统铺路

---

## 优化后的架构图

```
Application (消息循环主导者)
    ├── Dispatcher (UI线程调度)
    ├── CompositionTarget (渲染时钟)
    └── Windows (窗口集合)
        └── Window : ContentControl
            ├── WindowInteropHelper (GLFW封装)
            ├── Content (UIElement树)
            │   └── StackPanel
            │       ├── Control
            │       └── Control
            └── RenderHost (自动失效追踪)
                ├── RenderTreeBuilder
                ├── RenderScene
                └── GlRenderer
                    └── GLFW Context
```

---

## 对比：优化前后

### Window 使用对比

**优化前**：
```cpp
auto window = ui::window()
    ->Title("App")
    ->Width(800)
    ->Height(600);

window->SetContent(panel);  // 手动设置内容
window->Show();
window->Run();  // Window 自己管理消息循环
```

**优化后**：
```cpp
auto window = ui::window()
    ->Title("App")
    ->Width(800)
    ->Height(600);

window->Content(panel);  // 继承自 ContentControl
app.Run(window);  // Application 管理消息循环
```

### RenderHost 使用对比

**优化前**：
```cpp
renderHost->RequestRender();
renderHost->RenderFrame(ctx, *rootElement);  // 手动传入根节点
```

**优化后**：
```cpp
// 什么都不用做！
// 布局变化自动触发渲染
panel->Width(200);  // 自动 InvalidateVisual -> RenderHost
```

### 跨线程调用对比

**优化前**：
```cpp
std::thread([element]() {
    element->Width(100);  // 可能崩溃！
}).detach();
```

**优化后**：
```cpp
std::thread([element]() {
    element->Width(100);  // 自动调度到UI线程
    // 或者抛出异常（调试模式）
}).detach();
```

---

## 实施建议

### 立即执行（本周）

1. **WindowInteropHelper** - 封装 GLFW，降低 Window 复杂度
2. **Application::Run()** - 消息循环集中管理

### 近期执行（本月）

3. **Window 继承 ContentControl** - 统一内容模型
4. **RenderHost 自动失效追踪** - 提升渲染效率

### 中期规划（下个月）

5. **CompositionTarget** - 渲染时钟
6. **Dispatcher 自动检查** - 线程安全

### 长期规划（按需）

7. **Visual 树 / 逻辑树分离** - 支持虚拟化
8. **样式和模板系统** - 完整的 WPF 风格

---

## 风险评估

### 高风险

- **Window 继承 ContentControl**：破坏现有 API
  - **缓解**：提供兼容层，逐步迁移

### 中风险

- **Application::Run()** 改动：影响示例代码
  - **缓解**：更新文档和示例

### 低风险

- **RenderHost 自动化**：内部实现，不影响外部
- **Dispatcher 检查**：可选开关，逐步启用

---

## 总结

这套优化方案将使 F__K_UI 的架构更接近 WPF 的成熟设计：

✅ **职责清晰**：Application（生命周期）、Window（UI容器）、RenderHost（渲染）各司其职  
✅ **自动化**：失效追踪、渲染触发、线程调度自动化  
✅ **可扩展**：为动画、样式、模板系统打下基础  
✅ **线程安全**：Dispatcher 集成，防止跨线程错误  

建议采用**渐进式重构**，先实现高优先级项目，验证效果后再推进。
