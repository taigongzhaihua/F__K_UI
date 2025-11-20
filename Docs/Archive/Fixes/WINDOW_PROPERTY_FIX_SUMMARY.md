# Window 依赖属性修复总结

## 问题描述

Window的依赖属性存在以下问题：

### 问题1: Show()后修改属性不同步
```cpp
auto window = app->CreateWindow();
window->Show();

// 这些修改不会反映到原生窗口！
window->SetTitle("新标题");     // ❌ 原生窗口标题不变
window->SetWidth(1024);         // ❌ 窗口大小不变
window->SetLeft(100);           // ❌ 窗口位置不变
```

### 问题2: 部分属性从未应用
```cpp
window->SetTopmost(true);           // ❌ 从未应用到GLFW窗口
window->SetShowInTaskbar(false);    // ❌ 从未应用到GLFW窗口
```

### 问题3: 缺少属性变更回调
依赖属性注册时没有设置PropertyChangedCallback，导致：
- 属性值虽然保存了，但不会触发任何更新
- 无法自动同步到原生窗口
- 与WPF的依赖属性行为不一致

## 根本原因

Window的依赖属性注册使用了简单的元数据：
```cpp
// 之前的代码 - 没有回调
const binding::DependencyProperty& Window::TitleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        {std::string("")}  // 只有默认值，没有回调
    );
    return property;
}
```

这导致：
1. Show()方法只在创建窗口时读取一次属性值
2. 后续修改属性时，没有任何机制通知原生窗口
3. 某些属性（如Topmost）在Show()中根本没有被读取

## 解决方案

### 1. 添加属性变更回调

为每个需要同步的属性添加PropertyChangedCallback：

```cpp
// 修复后的代码 - 带有回调
const binding::DependencyProperty& Window::TitleProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Title",
        typeid(std::string),
        typeid(Window),
        binding::PropertyMetadata{
            std::any(std::string("")),
            &Window::OnTitleChanged  // ✅ 添加回调
        }
    );
    return property;
}
```

### 2. 实现回调函数

每个回调函数负责将属性变更同步到原生窗口：

```cpp
void Window::OnTitleChanged(
    binding::DependencyObject& d,
    const binding::DependencyProperty& prop,
    const std::any& oldValue,
    const std::any& newValue
) {
    auto* window = dynamic_cast<Window*>(&d);
    if (!window || !window->nativeHandle_) {
        return;  // 窗口还未创建，跳过
    }
    
    try {
        std::string title = std::any_cast<std::string>(newValue);
        
#ifdef FK_HAS_GLFW
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window->nativeHandle_);
        glfwSetWindowTitle(glfwWindow, title.c_str());  // ✅ 同步到GLFW
#else
        SimulatedWindow* simWindow = static_cast<SimulatedWindow*>(window->nativeHandle_);
        simWindow->title = title;  // ✅ 同步到模拟窗口
#endif
    } catch (const std::bad_any_cast&) {
        // 忽略类型转换错误
    }
}
```

### 3. 修复的属性列表

| 属性 | 回调函数 | GLFW API | 说明 |
|------|---------|----------|------|
| **Title** | OnTitleChanged | glfwSetWindowTitle | 窗口标题 |
| **Width** | OnWidthChanged | glfwSetWindowSize | 窗口宽度 |
| **Height** | OnHeightChanged | glfwSetWindowSize | 窗口高度 |
| **Left** | OnLeftChanged | glfwSetWindowPos | 窗口X坐标 |
| **Top** | OnTopChanged | glfwSetWindowPos | 窗口Y坐标 |
| **Topmost** | OnTopmostChanged | glfwSetWindowAttrib(GLFW_FLOATING) | 窗口置顶 |

注意：
- Width和Height共享同一个GLFW API，所以两个回调都会调用glfwSetWindowSize
- Left和Top共享同一个GLFW API，所以两个回调都会调用glfwSetWindowPos
- ShowInTaskbar在GLFW中没有直接对应的API，暂不支持运行时修改

### 4. 在Show()中应用初始属性

```cpp
void Window::Show() {
    // ... 创建窗口代码 ...
    
    // ✅ 应用 Topmost 属性
    ApplyTopmostToNativeWindow();
    
    // ... 其他代码 ...
}
```

## 修复效果

### Before (修复前) ❌

```cpp
auto window = app->CreateWindow();
window->Title("初始标题");
window->Show();

// 这些修改都不会生效
window->SetTitle("新标题");     // ❌ 窗口标题仍是"初始标题"
window->SetWidth(1024);         // ❌ 窗口仍是默认大小
window->SetTopmost(true);       // ❌ 窗口不会置顶
```

### After (修复后) ✅

```cpp
auto window = app->CreateWindow();
window->Title("初始标题");
window->Show();

// 这些修改都会立即生效
window->SetTitle("新标题");     // ✅ 窗口标题立即更新
window->SetWidth(1024);         // ✅ 窗口大小立即改变
window->SetTopmost(true);       // ✅ 窗口立即置顶

// 流式API也能正常工作
window->Title("链式更新")
      ->Width(800)
      ->Height(600);            // ✅ 所有修改都会同步
```

## 测试验证

### 测试场景1: 动态修改标题
```cpp
window->Show();
window->SetTitle("动态标题");  // ✅ 原生窗口标题立即更新
```

### 测试场景2: 动态调整大小
```cpp
window->Show();
window->SetWidth(1024);
window->SetHeight(768);         // ✅ 窗口大小立即改变
```

### 测试场景3: 动态移动位置
```cpp
window->Show();
window->SetLeft(200);
window->SetTop(100);            // ✅ 窗口位置立即移动
```

### 测试场景4: 动态切换置顶
```cpp
window->Show();
window->SetTopmost(true);       // ✅ 窗口置顶
window->SetTopmost(false);      // ✅ 取消置顶
```

## 技术细节

### 属性变更回调机制

1. **触发时机**: 当调用SetValue()修改依赖属性时
2. **回调参数**: 提供旧值和新值
3. **执行顺序**: 先更新属性值，再调用回调
4. **异常处理**: 回调中捕获异常，避免影响属性系统

### GLFW支持情况

| 功能 | GLFW版本 | API |
|------|---------|-----|
| 标题 | 3.0+ | glfwSetWindowTitle |
| 大小 | 3.0+ | glfwSetWindowSize |
| 位置 | 3.0+ | glfwSetWindowPos |
| 置顶 | 3.2+ | glfwSetWindowAttrib(GLFW_FLOATING) |

### 模拟窗口支持

对于没有GLFW的环境（如CI），使用SimulatedWindow：
- 支持Title、Width、Height的同步
- Left、Top不支持（模拟窗口没有位置概念）
- Topmost不支持（模拟窗口没有置顶概念）

## 兼容性

### 向后兼容
- ✅ 所有现有代码无需修改
- ✅ SetXxx()方法行为不变
- ✅ GetXxx()方法行为不变
- ✅ 流式API完全兼容

### 新增功能
- ✅ Show()后修改属性会自动同步
- ✅ Topmost属性现在生效
- ✅ 属性变更立即反映到UI

## 影响的文件

### 修改的文件
1. `include/fk/ui/Window.h`
   - 添加Width和Height属性声明（重写FrameworkElement）
   - 添加属性变更回调函数声明
   - 添加ApplyTopmostToNativeWindow()辅助方法

2. `src/ui/Window.cpp`
   - 更新所有依赖属性注册，添加回调
   - 实现6个属性变更回调函数
   - 在Show()中应用Topmost属性

### 测试验证
- ✅ window_test - 所有测试通过
- ✅ findname_demo - 所有测试通过
- ✅ 自定义测试 - 验证属性同步功能

## 总结

这个修复解决了Window依赖属性的核心问题：
1. **实时同步**: 属性修改立即反映到原生窗口
2. **完整支持**: Topmost等属性现在能正常工作
3. **标准行为**: 与WPF的依赖属性行为一致
4. **向后兼容**: 不破坏任何现有代码

修复使Window的依赖属性系统更加健壮和可用。

## 未来改进

可能的改进方向：
1. 为ShowInTaskbar添加平台特定的支持
2. 添加更多窗口属性（如Icon、ResizeMode等）
3. 支持属性变更动画效果
4. 添加属性验证和强制转换
