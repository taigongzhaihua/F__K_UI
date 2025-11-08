# ICommand 接口文档

## 概述

`ICommand` 是 MVVM 模式中的核心接口，用于将 UI 元素的用户操作（如点击、提交）与 ViewModel 逻辑解耦。Button、MenuItem 等交互控件通过绑定 Command 来执行 ViewModel 中定义的操作，并根据 `CanExecute` 状态自动启用/禁用。

**命名空间：** `fk::binding`

**关键特性：**
- ✅ 分离 UI 与业务逻辑
- ✅ 支持参数传递（CommandParameter）
- ✅ 自动启用/禁用控件状态
- ✅ 通过事件通知状态变化

---

## 核心接口

### ICommand

```cpp
class ICommand {
public:
    using Ptr = std::shared_ptr<ICommand>;
    
    // 检查命令是否可执行
    virtual bool CanExecute(const std::any& parameter) = 0;
    
    // 执行命令
    virtual void Execute(const std::any& parameter) = 0;
    
    // 当 CanExecute 状态变化时触发的事件
    virtual Event<ICommand*>& CanExecuteChanged() = 0;
    
    virtual ~ICommand() = default;
};
```

---

## 核心方法

### bool CanExecute(const std::any& parameter)
- **参数：** `parameter` - 命令参数（可选）
- **返回值：** 
  - `true` - 命令可执行，关联控件启用
  - `false` - 命令不可执行，关联控件禁用
- **说明：** 控件使用此方法确定是否启用

**用法示例：**
```cpp
if (command->CanExecute(parameter)) {
    command->Execute(parameter);
}
```

### void Execute(const std::any& parameter)
- **参数：** `parameter` - 传递给命令的参数
- **说明：** 执行命令的具体逻辑，由 ViewModel 实现

**约定：** 仅当 `CanExecute` 返回 `true` 时调用此方法

### Event\<ICommand*\>& CanExecuteChanged()
- **返回值：** 事件对象引用
- **触发条件：** 当 `CanExecute` 的返回值可能发生变化时触发
- **监听者：** 通常是 Button 等控件，监听此事件以更新 IsEnabled 状态

**用法：**
```cpp
command->CanExecuteChanged() += 
    [this](ICommand* cmd) { UpdateButtonState(); };
```

---

## CommandBase 基类

框架提供的默认实现基类：

```cpp
class CommandBase : public ICommand {
protected:
    Event<ICommand*> canExecuteChanged_;
    
public:
    Event<ICommand*>& CanExecuteChanged() override {
        return canExecuteChanged_;
    }
    
    // 派生类调用此方法通知状态变化
    void RaiseCanExecuteChanged() {
        canExecuteChanged_.Raise(this);
    }
};
```

---

## RelayCommand 实现

最常用的 ICommand 实现，用于绑定 ViewModel 中的方法。

```cpp
class RelayCommand : public CommandBase {
public:
    // 执行函数类型
    using ExecuteHandler = std::function<void(const std::any&)>;
    
    // 可执行检查函数类型
    using CanExecuteHandler = std::function<bool(const std::any&)>;
    
    // 创建仅包含 Execute 的命令
    static Ptr Create(ExecuteHandler executeFunc);
    
    // 创建包含 Execute 和 CanExecute 的命令
    static Ptr Create(
        ExecuteHandler executeFunc,
        CanExecuteHandler canExecuteFunc
    );
    
    bool CanExecute(const std::any& parameter) override;
    void Execute(const std::any& parameter) override;
};
```

---

## 便捷工厂函数

### relayCommand(execute_func)
```cpp
// 不需要 CanExecute 检查的命令
auto saveCommand = binding::relayCommand([this](const std::any& param) {
    Save();
});
```

### relayCommand(execute_func, canExecute_func)
```cpp
// 需要状态检查的命令
auto deleteCommand = binding::relayCommand(
    [this](const std::any& param) { Delete(); },
    [this](const std::any& param) { return HasSelection(); }
);
```

---

## 在 Button 中的使用

### 命令自动绑定

```cpp
class MyViewModel : public ObservableObject {
public:
    auto GetSaveCommand() {
        return binding::relayCommand(
            [this](const std::any&) { DoSave(); },
            [this](const std::any&) { return !IsLoading(); }
        );
    }
    
private:
    void DoSave() { /* ... */ }
    bool IsLoading() const { /* ... */ }
};

// 在 View 中
auto vm = std::make_shared<MyViewModel>();
auto button = ui::button()
    ->Content("保存")
    ->Command(vm->GetSaveCommand());

// Button 会：
// 1. 调用 command->CanExecute() 确定是否启用
// 2. 订阅 CanExecuteChanged 事件
// 3. 状态变化时自动更新 IsEnabled
// 4. 点击时调用 command->Execute(CommandParameter)
```

### CommandParameter 的使用

```cpp
// ViewModel
class ItemViewModel : public ObservableObject {
public:
    auto GetDeleteCommand() {
        return binding::relayCommand(
            [this](const std::any& param) {
                if (auto id = std::any_cast<int>(param)) {
                    DeleteItem(id);
                }
            }
        );
    }
};

// View
auto vm = std::make_shared<ItemViewModel>();
auto button = ui::button()
    ->Content("删除")
    ->Command(vm->GetDeleteCommand())
    ->CommandParameter(std::any(itemId));

// 点击时，command->Execute(itemId) 被调用
```

---

## 完整 MVVM 示例

### ViewModel 定义

```cpp
class TodoViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, TodoText);
    FK_PROPERTY(std::vector<std::string>, TodoList);
    FK_PROPERTY(std::string, SelectedTodo);
    
    TodoViewModel() 
        : addCommand_(CreateAddCommand())
        , deleteCommand_(CreateDeleteCommand())
        , clearCommand_(CreateClearCommand())
    {}
    
    ICommand::Ptr GetAddCommand() const { return addCommand_; }
    ICommand::Ptr GetDeleteCommand() const { return deleteCommand_; }
    ICommand::Ptr GetClearCommand() const { return clearCommand_; }
    
private:
    ICommand::Ptr addCommand_;
    ICommand::Ptr deleteCommand_;
    ICommand::Ptr clearCommand_;
    
    ICommand::Ptr CreateAddCommand() {
        return binding::relayCommand(
            [this](const std::any&) {
                if (!TodoText().empty()) {
                    auto list = TodoList();
                    list.push_back(TodoText());
                    SetTodoList(list);
                    SetTodoText("");  // 清空输入
                }
            },
            [this](const std::any&) {
                return !TodoText().empty();
            }
        );
    }
    
    ICommand::Ptr CreateDeleteCommand() {
        return binding::relayCommand(
            [this](const std::any&) {
                auto list = TodoList();
                auto it = std::find(list.begin(), list.end(), SelectedTodo());
                if (it != list.end()) {
                    list.erase(it);
                    SetTodoList(list);
                }
            },
            [this](const std::any&) {
                return !SelectedTodo().empty();
            }
        );
    }
    
    ICommand::Ptr CreateClearCommand() {
        return binding::relayCommand(
            [this](const std::any&) { SetTodoList({}); },
            [this](const std::any&) { return !TodoList().empty(); }
        );
    }
};
```

### View 与绑定

```cpp
auto vm = std::make_shared<TodoViewModel>();

auto view = ui::stackPanel()
    ->Orientation(Orientation::Vertical)
    ->Children({
        // 输入框
        ui::textBox()
            ->Bind(ui::binding::Bind(vm->TodoTextProperty())),
        
        // 添加按钮 - 自动启用/禁用
        ui::button()
            ->Content("添加")
            ->Command(vm->GetAddCommand()),
        
        // 项目列表
        ui::itemsControl()
            ->Bind(ui::binding::Bind(vm->TodoListProperty())),
        
        // 删除按钮 - 仅当选中项目时启用
        ui::button()
            ->Content("删除")
            ->Command(vm->GetDeleteCommand()),
        
        // 清空按钮 - 仅当列表非空时启用
        ui::button()
            ->Content("全部清空")
            ->Command(vm->GetClearCommand())
    });
```

---

## 自定义 Command 实现

### 创建复杂业务逻辑的命令

```cpp
class AsyncCommand : public CommandBase {
private:
    std::function<void(const std::any&)> executeFunc_;
    std::function<bool(const std::any&)> canExecuteFunc_;
    bool isExecuting_ = false;
    
public:
    AsyncCommand(
        std::function<void(const std::any&)> exec,
        std::function<bool(const std::any&)> canExec
    ) : executeFunc_(exec), canExecuteFunc_(canExec) {}
    
    bool CanExecute(const std::any& parameter) override {
        return !isExecuting_ && canExecuteFunc_(parameter);
    }
    
    void Execute(const std::any& parameter) override {
        if (!CanExecute(parameter)) return;
        
        isExecuting_ = true;
        RaiseCanExecuteChanged();  // 禁用按钮
        
        // 异步执行
        std::thread([this, parameter]() {
            try {
                executeFunc_(parameter);
            } catch (const std::exception& e) {
                // 错误处理
            }
            isExecuting_ = false;
            RaiseCanExecuteChanged();  // 重新启用按钮
        }).detach();
    }
};

// 使用
auto uploadCommand = std::make_shared<AsyncCommand>(
    [this](const std::any&) { DoUpload(); },
    [this](const std::any&) { return HasFile(); }
);
```

---

## 带参数的命令链

### 多参数传递

```cpp
struct CommandParams {
    int id;
    std::string action;
};

auto command = binding::relayCommand(
    [this](const std::any& param) {
        if (auto p = std::any_cast<CommandParams>(param)) {
            HandleAction(p.id, p.action);
        }
    }
);

// 使用
button->CommandParameter(std::any(CommandParams{123, "delete"}));
```

---

## 常见问题

### Q1：Button 如何知道何时更新 IsEnabled？
**A：** Button 在初始化 Command 时：
1. 调用 `command->CanExecute()` 初始化 IsEnabled
2. 订阅 `command->CanExecuteChanged()` 事件
3. 事件触发时重新调用 `CanExecute()` 更新状态

### Q2：如何在 ViewModel 中手动触发 CanExecute 检查？
**A：** 调用 `RelayCommand` 的 `RaiseCanExecuteChanged()` 方法：
```cpp
// 状态改变后
SetIsLoading(false);
if (auto relay = std::dynamic_pointer_cast<RelayCommand>(saveCommand_)) {
    relay->RaiseCanExecuteChanged();
}
```

### Q3：CommandParameter 如何传递？
**A：** 通过 Button 的 `CommandParameter` 属性：
```cpp
button->CommandParameter(std::any(42));  // 任何类型
// 执行时：command->Execute(std::any(42))
```

### Q4：是否可以在命令执行中更改 Command 本身？
**A：** 不建议。应该让 Execute 改变 ViewModel 状态，由状态驱动 CanExecute 返回值，然后触发 `RaiseCanExecuteChanged()`。

---

## 最佳实践

1. **分离职责：** Command 只负责执行操作，状态检查由 CanExecute 负责
2. **及时通知：** 状态改变后立即调用 `RaiseCanExecuteChanged()`
3. **避免阻塞：** 长耗时操作应该异步执行
4. **参数验证：** 在 Execute 中验证 `std::any_cast` 结果，防止异常
5. **命令复用：** 一个 ViewModel 的命令可被多个 View 共享

---

## 另见

- [Button.md](../UI/Button.md) - Button 的 Command 属性
- [ObservableObject.md](../Binding/ObservableObject.md) - 属性通知系统
- [Binding.md](../Binding/Binding.md) - 数据绑定机制
