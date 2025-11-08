# BindingExpression - 绑定表达式

## 概述

`BindingExpression` 是绑定系统的执行引擎，负责激活绑定、维护源和目标之间的数据同步、处理类型转换和数据验证。每个激活的 Binding 都对应一个 BindingExpression 实例。

## 核心概念

### 绑定的生命周期

```
创建 Binding → 附加到属性 → 创建 BindingExpression → Activate() → 监听源变更
                                                      ↑
                                                  维护同步
                                                      ↓
                                                  UpdateTarget/UpdateSource
                                                      ↓
                                                   Detach()
```

## 核心 API

### 构造函数

```cpp
BindingExpression(Binding definition, DependencyObject* target, const DependencyProperty& property);
```

通常由框架在附加 Binding 时创建，不需要手动构造。

参数：
- `definition`: 绑定定义（来源、路径、模式等）
- `target`: 目标 UI 元素
- `property`: 目标依赖属性

### 激活和停用

```cpp
void Activate();
void Detach();
```

- `Activate()`: 启动绑定，订阅源属性变更、监听 DataContext 变更
- `Detach()`: 停用绑定，取消订阅所有事件，清理资源

**示例：**
```cpp
auto binding = std::make_shared<fk::binding::Binding>();
binding->SetPath("UserName");

auto expr = std::make_shared<fk::binding::BindingExpression>(
    *binding, textBox.get(), fk::ui::TextBox::TextProperty
);

expr->Activate();   // 开始同步

// ... 使用绑定

expr->Detach();     // 停止同步
```

### 查询绑定状态

```cpp
bool IsActive() const noexcept;
DependencyObject* Target() const noexcept;
const DependencyProperty& Property() const noexcept;
const Binding& Definition() const noexcept;
```

**示例：**
```cpp
if (expr->IsActive()) {
    auto target = expr->Target();
    auto& prop = expr->Property();
}
```

### 同步方向

```cpp
void UpdateTarget();  // 从源更新目标
void UpdateSource();  // 从目标更新源
```

- `UpdateTarget()`: 获取源值，应用转换器，设置目标属性
- `UpdateSource()`: 获取目标值，应用反向转换器，设置源属性

通常自动触发，很少需要手动调用。

**示例：**
```cpp
// 手动强制更新
expr->UpdateTarget();  // 目标属性更新为源的最新值

expr->UpdateSource();  // 源属性更新为目标的最新值
```

### 应用目标值

```cpp
void ApplyTargetValue(std::any value);
```

直接应用一个值到目标属性，触发绑定验证。

**示例：**
```cpp
expr->ApplyTargetValue(std::string("New Value"));
```

### 验证错误

```cpp
bool HasValidationErrors() const noexcept;
const std::vector<ValidationResult>& GetValidationErrors() const noexcept;

using ValidationErrorsChangedEvent = core::Event<const std::vector<ValidationResult>&>;
ValidationErrorsChangedEvent ValidationErrorsChanged;
```

获取和监听验证错误。

**示例：**
```cpp
if (expr->HasValidationErrors()) {
    for (const auto& error : expr->GetValidationErrors()) {
        std::cout << "验证错误: " << error.ErrorMessage << "\n";
    }
}

expr->ValidationErrorsChanged.Subscribe(
    [](const auto& errors) {
        // 验证错误已更新
    }
);
```

## 常见模式

### 1. 基本绑定流程

```cpp
#include "fk/binding/Binding.h"
#include "fk/binding/BindingExpression.h"
#include "fk/ui/TextBox.h"

class LoginViewModel {
public:
    std::string GetUsername() const { return username_; }
    void SetUsername(const std::string& value) {
        username_ = value;
    }
private:
    std::string username_;
};

int main() {
    auto vm = std::make_shared<LoginViewModel>();
    auto textBox = fk::ui::TextBox::Create();
    textBox->SetDataContext(vm);
    
    // 创建绑定
    auto binding = std::make_shared<fk::binding::Binding>();
    binding->SetPath("Username");
    binding->SetMode(fk::binding::BindingMode::TwoWay);
    
    // 创建表达式
    auto expr = std::make_shared<fk::binding::BindingExpression>(
        *binding, textBox.get(), fk::ui::TextBox::TextProperty
    );
    
    // 激活
    expr->Activate();
    
    // 现在文本框和 ViewModel.Username 同步
}
```

### 2. 监听验证错误

```cpp
class FormViewModel {
public:
    std::string GetEmail() const { return email_; }
    void SetEmail(const std::string& value) {
        if (IsValidEmail(value)) {
            email_ = value;
        } else {
            throw std::invalid_argument("Invalid email");
        }
    }
private:
    std::string email_;
    bool IsValidEmail(const std::string& email) const {
        // 简化的验证
        return email.find('@') != std::string::npos;
    }
};

void SetupEmailBinding() {
    auto vm = std::make_shared<FormViewModel>();
    auto emailBox = fk::ui::TextBox::Create();
    emailBox->SetDataContext(vm);
    
    auto binding = std::make_shared<fk::binding::Binding>();
    binding->SetPath("Email");
    
    auto expr = std::make_shared<fk::binding::BindingExpression>(
        *binding, emailBox.get(), fk::ui::TextBox::TextProperty
    );
    
    // 监听验证错误
    expr->ValidationErrorsChanged.Subscribe(
        [emailBox](const auto& errors) {
            if (!errors.empty()) {
                emailBox->SetBackground("red");
            } else {
                emailBox->SetBackground("white");
            }
        }
    );
    
    expr->Activate();
}
```

### 3. 手动同步

```cpp
void ManualSyncExample() {
    auto vm = std::make_shared<MyViewModel>();
    auto control = fk::ui::Control::Create();
    
    auto binding = std::make_shared<fk::binding::Binding>();
    auto expr = std::make_shared<fk::binding::BindingExpression>(
        *binding, control.get(), fk::ui::Control::NameProperty
    );
    
    expr->Activate();
    
    // 用户修改了源
    vm->SetName("Alice");
    
    // 确保目标同步（通常自动发生）
    expr->UpdateTarget();
    
    // 用户修改了目标（UI）
    // ... 用户输入
    
    // 更新源
    expr->UpdateSource();
}
```

### 4. 绑定模式选择

```cpp
void ConfigureBindingMode() {
    auto expr = std::make_shared<fk::binding::BindingExpression>(...);
    
    // OneWay: 源 → 目标（只读）
    auto readOnlyBinding = std::make_shared<fk::binding::Binding>();
    readOnlyBinding->SetMode(fk::binding::BindingMode::OneWay);
    
    // OneWayToSource: 目标 → 源
    auto sourceBinding = std::make_shared<fk::binding::Binding>();
    sourceBinding->SetMode(fk::binding::BindingMode::OneWayToSource);
    
    // TwoWay: 双向同步
    auto twoWayBinding = std::make_shared<fk::binding::Binding>();
    twoWayBinding->SetMode(fk::binding::BindingMode::TwoWay);
}
```

### 5. 状态检查

```cpp
void CheckBindingState() {
    auto expr = std::make_shared<fk::binding::BindingExpression>(...);
    
    std::cout << "Active: " << expr->IsActive() << "\n";
    std::cout << "Has errors: " << expr->HasValidationErrors() << "\n";
    
    if (expr->HasValidationErrors()) {
        for (const auto& error : expr->GetValidationErrors()) {
            std::cout << "  - " << error.ErrorMessage << "\n";
        }
    }
}
```

## 实现细节

### 更新流程

1. **UpdateTarget**:
   - 解析源路径
   - 获取源值
   - 应用转换器
   - 设置目标属性

2. **UpdateSource**:
   - 获取目标属性值
   - 应用反向转换器
   - 验证值
   - 设置源属性

### 事件订阅

BindingExpression 自动订阅：
- DataContext 变更（重新查找源）
- 源属性变更（UpdateTarget）
- 目标属性变更（UpdateSource，取决于绑定模式）

### 线程安全性

BindingExpression 在 UI 线程中使用，不是线程安全的。

## 最佳实践

### 1. 确保正确的生命周期管理

```cpp
// ✅ 使用共享指针
auto expr = std::make_shared<fk::binding::BindingExpression>(...);
expr->Activate();

// ❌ 避免栈上对象
{
    fk::binding::BindingExpression expr(...);
    expr.Activate();
}  // 作用域结束，对象销毁
```

### 2. 在激活前配置 Binding

```cpp
// ✅ 推荐顺序
auto binding = std::make_shared<fk::binding::Binding>();
binding->SetPath("PropertyName");
binding->SetMode(fk::binding::BindingMode::TwoWay);
binding->SetConverter(converter);

auto expr = std::make_shared<fk::binding::BindingExpression>(...);
expr->Activate();

// ❌ 避免先激活再配置
auto expr = std::make_shared<fk::binding::BindingExpression>(...);
expr->Activate();
binding->SetPath("PropertyName");  // 太晚了
```

### 3. 正确处理错误

```cpp
// ✅ 监听验证错误
expr->ValidationErrorsChanged.Subscribe(
    [](const auto& errors) {
        if (!errors.empty()) {
            // 处理错误
        }
    }
);

// ❌ 忽视错误
// 绑定出错时不采取任何行动
```

## 常见问题

### Q: UpdateTarget() 和 UpdateSource() 何时自动调用？
**A:** 
- UpdateTarget: 当源属性变更或 DataContext 变更时
- UpdateSource: 当目标属性变更且绑定模式支持时（取决于 UpdateSourceTrigger）

### Q: 如何调试绑定问题？
**A:** 检查 `HasValidationErrors()` 并打印 `GetValidationErrors()`，确保源和目标类型兼容。

### Q: 可以更改激活中的绑定定义吗？
**A:** 不建议。应该调用 `Detach()`、修改定义、再 `Activate()`。

## 相关文档

- [Binding.md](./Binding.md) - 绑定系统
- [BindingContext.md](./BindingContext.md) - 数据上下文
- [BindingPath.md](./BindingPath.md) - 绑定路径解析
- [ValueConverters.md](./ValueConverters.md) - 类型转换器
