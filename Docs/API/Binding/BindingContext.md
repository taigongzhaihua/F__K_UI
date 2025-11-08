# BindingContext - 绑定上下文

## 概述

`BindingContext` 管理单个 UI 元素的数据上下文（DataContext），实现 MVVM 模式中数据绑定的关键功能。它支持本地数据上下文和继承自父元素的数据上下文，并提供数据上下文变更通知。

## 核心概念

### 什么是 DataContext？

`DataContext` 是与 UI 元素关联的任意对象（通过 `std::any` 存储），通常是 ViewModel 实例。绑定系统通过 DataContext 来查找要绑定的数据。

**示例：**
```cpp
// 创建 ViewModel
class LoginViewModel {
public:
    std::string username;
    std::string password;
};

// 创建 UI 元素并设置 DataContext
auto viewModel = std::make_shared<LoginViewModel>();
auto textBox = std::make_shared<TextBox>();
textBox->SetDataContext(viewModel);

// 现在可以通过绑定访问 username 属性
// 绑定路径: "username" 会从 DataContext 中查找
```

### 继承机制

DataContext 可以从父元素继承：

```
Window (DataContext = MainViewModel)
  └─ Grid (继承 MainViewModel)
      └─ TextBox (继承 MainViewModel)
          └─ Button (继承 MainViewModel)
```

## 核心 API

### 构造函数

```cpp
explicit BindingContext(DependencyObject& owner);
```

每个 `DependencyObject`（UI 元素的基类）都有一个关联的 `BindingContext`。通常由框架自动创建，不需要手动构建。

### 获取数据上下文

```cpp
const std::any& GetDataContext() const noexcept;
```

获取有效的数据上下文（可能是本地设置的或继承的）。

**示例：**
```cpp
auto context = bindingContext.GetDataContext();
if (!context.has_value()) {
    std::cout << "未设置 DataContext\n";
    return;
}

// 类型转换
auto viewModel = std::any_cast<std::shared_ptr<MyViewModel>>(context);
```

### 检查数据上下文状态

```cpp
bool HasDataContext() const noexcept;      // 是否有有效的 DataContext（本地或继承）
bool HasLocalDataContext() const noexcept; // 是否有本地设置的 DataContext
```

**示例：**
```cpp
if (bindingContext.HasDataContext()) {
    std::cout << "有有效的 DataContext\n";
}

if (bindingContext.HasLocalDataContext()) {
    std::cout << "本地设置了 DataContext\n";
} else if (bindingContext.HasDataContext()) {
    std::cout << "从父元素继承了 DataContext\n";
}
```

### 设置本地数据上下文

```cpp
void SetLocalDataContext(std::any value);
```

设置此元素的本地 DataContext，会触发 `DataContextChanged` 事件。

**特点：**
- 覆盖继承的 DataContext
- 会通知所有订阅者
- 支持任意类型（通过 `std::any`）

**示例：**
```cpp
// 方式 1: 共享指针
auto viewModel = std::make_shared<LoginViewModel>();
bindingContext.SetLocalDataContext(viewModel);

// 方式 2: 引用语义
MyViewModel vm;
bindingContext.SetLocalDataContext(std::ref(vm));

// 方式 3: 栈对象指针
bindingContext.SetLocalDataContext(&vm);
```

### 清除本地数据上下文

```cpp
void ClearLocalDataContext();
```

移除本地设置的 DataContext，之后会继承父元素的 DataContext。

**示例：**
```cpp
// 设置本地 DataContext
bindingContext.SetLocalDataContext(vm1);

// 清除，恢复继承
bindingContext.ClearLocalDataContext();

// 现在 GetDataContext() 返回父元素的 DataContext
```

### 管理父元素关系

```cpp
void SetParent(BindingContext* parent);
BindingContext* Parent() const noexcept;
```

建立与父元素 BindingContext 的关系，实现数据上下文继承。

通常由框架在建立父子关系时自动调用。

**示例：**
```cpp
// 框架内部使用
class FrameworkElement {
public:
    void AddChild(std::shared_ptr<UIElement> child) {
        children_.push_back(child);
        
        // 建立 BindingContext 关系
        child->GetBindingContext().SetParent(&this->GetBindingContext());
    }
};
```

### 数据上下文变更事件

```cpp
using DataContextChangedEvent = core::Event<const std::any&, const std::any&>;
DataContextChangedEvent DataContextChanged;
```

当数据上下文变更时触发此事件，传递旧值和新值。

**订阅示例：**
```cpp
auto connection = bindingContext.DataContextChanged.Subscribe(
    [](const std::any& oldValue, const std::any& newValue) {
        std::cout << "DataContext 已更改\n";
    }
);
```

## 常见模式

### 1. 基本 MVVM 设置

```cpp
#include "fk/ui/Window.h"
#include "fk/ui/TextBox.h"
#include "fk/binding/ObservableObject.h"

class LoginViewModel : public fk::binding::ObservableObject {
public:
    std::string GetUsername() const { return username_; }
    void SetUsername(const std::string& value) {
        if (username_ != value) {
            username_ = value;
            OnPropertyChanged("Username");
        }
    }
    
private:
    std::string username_;
};

int main() {
    // 创建 ViewModel
    auto viewModel = std::make_shared<LoginViewModel>();
    
    // 创建 UI 并设置 DataContext
    auto window = fk::ui::Window::Create();
    window->SetDataContext(viewModel);
    
    // 子元素自动继承 DataContext
    auto usernameBox = fk::ui::TextBox::Create();
    window->AddChild(usernameBox);
    
    // 现在可以绑定 "Username" 属性
    auto binding = std::make_shared<fk::binding::Binding>();
    binding->SetPath("Username");
    usernameBox->SetBinding(fk::ui::TextBox::TextProperty, binding);
}
```

### 2. 动态切换 ViewModel

```cpp
class UIView {
private:
    std::shared_ptr<UIElement> view_;
    
public:
    template<typename ViewModel>
    void SetViewModel(std::shared_ptr<ViewModel> viewModel) {
        auto& ctx = view_->GetBindingContext();
        
        // 连接变更事件
        ctx.DataContextChanged.Subscribe(
            [this](const std::any& oldValue, const std::any& newValue) {
                OnViewModelChanged();
            }
        );
        
        // 设置新 ViewModel
        ctx.SetLocalDataContext(viewModel);
    }
    
    void OnViewModelChanged() {
        // 重新绑定或更新 UI
    }
};
```

### 3. 分层 DataContext

```cpp
// 主窗口: 全应用级 ViewModel
auto mainWindow = Window::Create();
auto appVM = std::make_shared<AppViewModel>();
mainWindow->SetDataContext(appVM);

// 侧边栏: 使用应用级 ViewModel 中的属性
auto sidebar = Panel::Create();
mainWindow->AddChild(sidebar);
// sidebar 继承 appVM

// 内容区域: 切换到特定页面的 ViewModel
auto content = Panel::Create();
mainWindow->AddChild(content);

auto page1VM = std::make_shared<Page1ViewModel>();
content->SetDataContext(page1VM);  // 覆盖继承的 DataContext
```

### 4. 监听 DataContext 变更

```cpp
class DataBoundControl : public Control {
public:
    DataBoundControl() {
        auto& ctx = GetBindingContext();
        ctx.DataContextChanged.Subscribe(
            [this](const std::any& oldValue, const std::any& newValue) {
                OnDataContextChanged(oldValue, newValue);
            }
        );
    }
    
private:
    void OnDataContextChanged(const std::any& oldValue, const std::any& newValue) {
        // 更新绑定或进行其他响应式操作
        std::cout << "ViewModel 已切换\n";
    }
};
```

### 5. 条件性 DataContext 继承

```cpp
class ItemsControl {
private:
    std::vector<std::shared_ptr<UIElement>> items_;
    
public:
    void GenerateItems(int count) {
        for (int i = 0; i < count; ++i) {
            auto item = CreateItemTemplate();
            
            // 每个项目继承 ItemsControl 的 DataContext
            item->GetBindingContext().SetParent(
                &this->GetBindingContext()
            );
            
            items_.push_back(item);
        }
    }
};
```

### 6. 实现 ViewModel 提供者

```cpp
class ViewModelProvider {
public:
    template<typename ViewModel>
    static void SetViewModel(
        UIElement& element,
        std::shared_ptr<ViewModel> viewModel) {
        
        element.GetBindingContext().SetLocalDataContext(viewModel);
    }
    
    static std::any GetViewModel(const UIElement& element) {
        return element.GetBindingContext().GetDataContext();
    }
    
    static bool HasViewModel(const UIElement& element) {
        return element.GetBindingContext().HasDataContext();
    }
};

// 使用
ViewModelProvider::SetViewModel(*window, appVM);
auto vm = ViewModelProvider::GetViewModel(*window);
```

## 实现细节

### 数据上下文解析顺序

1. 检查本地 DataContext（已设置）
2. 如果未设置，向上查询父元素
3. 递归直到找到有设置的 DataContext
4. 如果整个层级都未设置，返回空

### 等价性判断

```cpp
static bool AreEquivalent(const std::any& lhs, const std::any& rhs);
```

检查两个 DataContext 值是否相等。用于避免不必要的事件触发。

## 最佳实践

### 1. 在层级顶部设置 DataContext

```cpp
// ✅ 推荐
auto window = Window::Create();
window->SetDataContext(mainVM);  // 在顶部设置
// 所有子元素自动继承

// ❌ 避免
auto window = Window::Create();
auto grid = Grid::Create();
auto button = Button::Create();
button->SetDataContext(mainVM);  // 在底部设置不够优雅
```

### 2. 保持 ViewModel 生命周期管理

```cpp
// ✅ 使用共享指针管理生命周期
auto viewModel = std::make_shared<MyViewModel>();
element->SetDataContext(viewModel);

// ❌ 避免栈上对象
void BadExample() {
    MyViewModel viewModel;  // 栈上对象
    element->SetDataContext(std::ref(viewModel));
    // 函数返回时 viewModel 被销毁，引用悬空
}
```

### 3. 订阅 DataContext 变更

```cpp
// ✅ 需要时才订阅
control->GetBindingContext().DataContextChanged.Subscribe(
    [this](const auto& oldVal, const auto& newVal) {
        RefreshBindings();
    }
);

// ❌ 不要频繁创建/销毁订阅
for (int i = 0; i < 1000; ++i) {
    auto conn = ctx.DataContextChanged.Subscribe(...);
    // conn 立即销毁
}
```

### 4. 适当时机清除本地 DataContext

```cpp
void ResetToInherited() {
    // 清除本地设置，恢复继承
    element->GetBindingContext().ClearLocalDataContext();
}
```

## 常见问题

### Q: 如何检查 DataContext 是否是特定类型？
**A:** 使用 `std::any_cast` 或 `type_info`：
```cpp
auto context = bindingContext.GetDataContext();
if (context.type() == typeid(std::shared_ptr<MyViewModel>)) {
    auto vm = std::any_cast<std::shared_ptr<MyViewModel>>(context);
}
```

### Q: 设置相同的 DataContext 会触发事件吗？
**A:** 否，框架会进行等价性判断（`AreEquivalent`），只有值真正改变才触发事件。

### Q: 子元素可以覆盖父元素的 DataContext 吗？
**A:** 可以，使用 `SetLocalDataContext()`。子元素的本地 DataContext 优先级更高。

### Q: 如何实现多级继承？
**A:** 自动支持，通过 `SetParent()` 建立链式关系：
```
GrandParent → Parent → Child
  VM1         VM1       VM1 (继承)
              ↓         ↓
          (可设置VM2)  (继承VM2或VM1)
```

## 相关文档

- [Binding.md](./Binding.md) - 绑定系统
- [BindingExpression.md](./BindingExpression.md) - 绑定表达式
- [DependencyObject.md](./DependencyObject.md) - 依赖对象基类
- [ObservableObject.md](./ObservableObject.md) - 可观察对象
