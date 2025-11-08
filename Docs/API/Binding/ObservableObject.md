# ObservableObject 文档

## 概述

`ObservableObject` 是 MVVM 模式中 ViewModel 的基类。它提供了属性变化通知机制，使得 View 能够自动响应 ViewModel 属性的变化。

**命名空间：** `fk::binding`

**关键特性：**
- ✅ 属性变化自动通知 View
- ✅ 支持数据绑定
- ✅ 简化 FK_PROPERTY 宏的使用
- ✅ 提供链式调用 API

---

## 核心概念

### MVVM 中的 ViewModel

在 MVVM 架构中：
- **Model** - 业务数据和逻辑
- **View** - UI 界面（XAML/代码）
- **ViewModel** - 中介层，暴露属性和命令给 View

```
Model ←→ ViewModel ←→ View (UI)
              ↓
         ObservableObject
       （属性变化通知）
```

### 属性通知流程

```
1. ViewModel 属性值改变
2. 触发 PropertyChanged 事件
3. View 的绑定感知变化
4. View 自动更新显示
```

---

## 继承 ObservableObject

### 基本用法

```cpp
class MyViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Name);
    FK_PROPERTY(int, Age);
    FK_PROPERTY(bool, IsActive);
    
    MyViewModel() {
        SetName("John");
        SetAge(30);
        SetIsActive(true);
    }
};
```

#### FK_PROPERTY 宏

`FK_PROPERTY(Type, PropertyName)` 自动生成：
- `Ptr PropertyName(Type value)` - 设置属性，触发通知，返回 this
- `Type PropertyName() const` - 获取属性值
- `void SetPropertyName(Type value)` - 设置属性（内部使用）
- `PropertyNameProperty()` - 获取属性定义

### 链式调用

```cpp
auto vm = std::make_shared<MyViewModel>();
vm->Name("Alice")
  ->Age(25)
  ->IsActive(true);

printf("Name: %s, Age: %d\n", vm->Name().c_str(), vm->Age());
```

---

## 属性绑定

### 单向绑定（ViewModel → View）

ViewModel 属性变化时自动更新 View：

```cpp
// ViewModel
class PersonViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, FirstName);
    FK_PROPERTY(std::string, LastName);
    FK_PROPERTY(std::string, FullName);
    
    PersonViewModel() {
        SetFirstName("John");
        SetLastName("Doe");
        UpdateFullName();
    }
    
    Ptr FirstName(std::string value) {
        // FK_PROPERTY 自动调用基类的 Set 方法
        SetFirstName(value);
        UpdateFullName();
        return shared_from_this();
    }
    
private:
    void UpdateFullName() {
        SetFullName(FirstName() + " " + LastName());
    }
};

// View
auto vm = std::make_shared<PersonViewModel>();
auto textBlock = ui::textBlock()
    ->Bind(binding::Bind(vm->FullNameProperty()));

// 修改 ViewModel
vm->FirstName("Jane");  // TextBlock 自动更新为 "Jane Doe"
```

### 双向绑定（View ↔ ViewModel）

View 和 ViewModel 互相同步：

```cpp
// ViewModel
class AgeViewModel : public ObservableObject {
public:
    FK_PROPERTY(int, Age);
    
    AgeViewModel() {
        SetAge(20);
    }
};

// View
auto vm = std::make_shared<AgeViewModel>();
auto textBox = ui::textBox()
    ->Bind(binding::Bind(vm->AgeProperty())
               ->TwoWay());  // 双向绑定

// 用户在 TextBox 中输入 → ViewModel 属性更新
// ViewModel 属性更新 → TextBox 显示改变
```

---

## 命令与 ViewModel

### 暴露命令

```cpp
class CommandViewModel : public ObservableObject {
private:
    ICommand::Ptr saveCommand_;
    ICommand::Ptr deleteCommand_;
    
public:
    CommandViewModel() {
        saveCommand_ = binding::relayCommand(
            [this](const std::any&) { DoSave(); },
            [this](const std::any&) { return CanSave(); }
        );
        
        deleteCommand_ = binding::relayCommand(
            [this](const std::any&) { DoDelete(); },
            [this](const std::any&) { return CanDelete(); }
        );
    }
    
    ICommand::Ptr GetSaveCommand() const { 
        return saveCommand_; 
    }
    
    ICommand::Ptr GetDeleteCommand() const { 
        return deleteCommand_; 
    }
    
private:
    void DoSave() { /* 保存逻辑 */ }
    void DoDelete() { /* 删除逻辑 */ }
    bool CanSave() { /* 检查是否可以保存 */ }
    bool CanDelete() { /* 检查是否可以删除 */ }
};

// View
auto vm = std::make_shared<CommandViewModel>();
auto saveBtn = ui::button()
    ->Content("Save")
    ->Command(vm->GetSaveCommand());

auto delBtn = ui::button()
    ->Content("Delete")
    ->Command(vm->GetDeleteCommand());
```

---

## 完整 MVVM 示例

### Model
```cpp
struct User {
    std::string id;
    std::string name;
    std::string email;
    int age;
};

class UserService {
public:
    std::vector<User> GetAllUsers() { /* ... */ }
    bool SaveUser(const User& user) { /* ... */ }
    bool DeleteUser(const std::string& id) { /* ... */ }
};
```

### ViewModel
```cpp
class UserViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Name);
    FK_PROPERTY(std::string, Email);
    FK_PROPERTY(int, Age);
    FK_PROPERTY(bool, IsLoading);
    
    UserViewModel(std::shared_ptr<UserService> service)
        : service_(service),
          saveCommand_(CreateSaveCommand()),
          deleteCommand_(CreateDeleteCommand())
    {}
    
    ICommand::Ptr GetSaveCommand() { 
        return saveCommand_; 
    }
    
    ICommand::Ptr GetDeleteCommand() { 
        return deleteCommand_; 
    }
    
    Ptr LoadUser(const std::string& userId) {
        SetIsLoading(true);
        // 异步加载用户
        std::thread([this, userId] {
            auto users = service_->GetAllUsers();
            // 找到用户并更新属性
            auto it = std::find_if(users.begin(), users.end(),
                [&](const User& u) { return u.id == userId; });
            if (it != users.end()) {
                SetName(it->name);
                SetEmail(it->email);
                SetAge(it->age);
            }
            SetIsLoading(false);
        }).detach();
        return shared_from_this();
    }
    
private:
    std::shared_ptr<UserService> service_;
    ICommand::Ptr saveCommand_;
    ICommand::Ptr deleteCommand_;
    
    ICommand::Ptr CreateSaveCommand() {
        return binding::relayCommand(
            [this](const std::any&) {
                User user{/* ... */};
                if (service_->SaveUser(user)) {
                    // 保存成功
                }
            },
            [this](const std::any&) { 
                return !IsLoading() && !Name().empty(); 
            }
        );
    }
    
    ICommand::Ptr CreateDeleteCommand() {
        return binding::relayCommand(
            [this](const std::any&) {
                // 删除逻辑
            }
        );
    }
};
```

### View
```cpp
auto vm = std::make_shared<UserViewModel>(userService);
vm->LoadUser("user123");

auto view = ui::grid()
    ->ColumnDefinitions({
        ColumnDefinition{GridLength::Auto()},
        ColumnDefinition{GridLength::Star()}
    })
    ->RowDefinitions({
        RowDefinition{GridLength::Auto()},
        RowDefinition{GridLength::Auto()},
        RowDefinition{GridLength::Auto()},
        RowDefinition{GridLength::Auto()}
    })
    ->Children({
        ui::textBlock()->Text("Name:") | cell(0, 0),
        ui::textBox()
            ->Bind(binding::Bind(vm->NameProperty())->TwoWay())
            | cell(0, 1),
        
        ui::textBlock()->Text("Email:") | cell(1, 0),
        ui::textBox()
            ->Bind(binding::Bind(vm->EmailProperty())->TwoWay())
            | cell(1, 1),
        
        ui::textBlock()->Text("Age:") | cell(2, 0),
        ui::textBox()
            ->Bind(binding::Bind(vm->AgeProperty())->TwoWay())
            | cell(2, 1),
        
        ui::stackPanel()
            ->Orientation(Orientation::Horizontal)
            ->Spacing(5.0f)
            ->Children({
                ui::button()
                    ->Content("Save")
                    ->Command(vm->GetSaveCommand()),
                ui::button()
                    ->Content("Delete")
                    ->Command(vm->GetDeleteCommand())
            })
            | cell(3, 1)
    });
```

---

## 属性变化通知

### 手动触发通知

虽然 FK_PROPERTY 会自动触发，但有时需要手动通知：

```cpp
class CalculationViewModel : public ObservableObject {
public:
    FK_PROPERTY(int, A);
    FK_PROPERTY(int, B);
    FK_PROPERTY(int, Result);
    
    Ptr A(int value) {
        SetA(value);
        UpdateResult();
        return shared_from_this();
    }
    
    Ptr B(int value) {
        SetB(value);
        UpdateResult();
        return shared_from_this();
    }
    
private:
    void UpdateResult() {
        SetResult(A() + B());
    }
};
```

---

## 常见问题

### Q1：ObservableObject 如何使用？
**A：** 继承 ObservableObject，使用 FK_PROPERTY 宏定义属性，然后绑定到 View。

### Q2：FK_PROPERTY 和普通属性有何区别？
**A：** FK_PROPERTY 自动管理属性变化通知，支持数据绑定。修改 FK_PROPERTY 会自动通知 View 更新。

### Q3：如何监听属性变化？
**A：** 使用属性的 PropertyChanged 事件或绑定到 View 控件。

### Q4：ViewModel 中可以有普通方法吗？
**A：** 可以。ViewModel 可以混合使用属性和普通方法。

### Q5：如何处理 ViewModel 与 Model 的数据转换？
**A：** 在 ViewModel 中进行转换：
```cpp
auto user = service_->GetUser(id);  // Model
SetName(user.name);  // 存储到 ViewModel 属性
SetEmail(user.email);
```

---

## 最佳实践

1. **ViewModel 中只放置 UI 相关逻辑：**
   ```cpp
   // ✅ 推荐
   FK_PROPERTY(bool, IsLoading);
   
   // ⚠️ 避免
   void PerformComplexCalculation();  // 应在 Model/Service 中
   ```

2. **使用 ICommand 处理用户操作：**
   ```cpp
   // ✅ 推荐
   auto cmd = binding::relayCommand([this](const std::any&) {
       DoSomething();
   });
   
   // ⚠️ 避免
   void OnButtonClick();  // 耦合到 View
   ```

3. **确保线程安全：**
   ```cpp
   // 从后台线程更新 ViewModel 时小心
   std::thread([this] {
       auto data = FetchDataFromNetwork();
       SetResult(data);  // 确保主线程安全
   }).detach();
   ```

4. **避免 ViewModel 持有 View 引用：**
   ```cpp
   // ❌ 不要这样做
   class MyViewModel : public ObservableObject {
       UIElement* view_;  // 耦合！
   };
   ```

---

## 性能考虑

- **属性变化通知：** 每次属性改变都会触发事件，频繁改变可能有性能影响
- **绑定数量：** 大量绑定会增加事件处理开销
- **复杂 ViewModel：** 过大的 ViewModel 可能导致初始化缓慢

---

## 另见

- [ICommand.md](../Core/ICommand.md) - 命令系统
- [DependencyProperty.md](DependencyProperty.md) - 属性系统
- [Binding.md](Binding.md) - 数据绑定机制（待完成）
- [Button.md](../UI/Button.md) - Command 绑定示例
