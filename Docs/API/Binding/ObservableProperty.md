# ObservableProperty\<T, Owner\>

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding`（`fk` 命名空间下有别名） |
| **头文件** | `fk/binding/ObservableProperty.h` |
| **源文件** | 纯头文件模板类，无源文件 |

## 描述

`ObservableProperty<T, Owner>` 是一个可观察属性模板类，提供自动属性变更通知和属性访问器注册功能。它是 ViewModel 宏系统的现代替代方案，具有更好的类型安全性、更清晰的代码结构和更强的编译期检查。

**核心功能**：
- 自动注册到 `PropertyAccessorRegistry`，支持字符串路径绑定
- 支持自定义 getter、setter、validator
- 类型安全的属性访问
- 隐式转换和赋值运算符支持
- 链式调用配置

**相比 ViewModel 宏的优势**：
- 无需在类外使用注册宏
- 代码更清晰，更易调试
- 编译期类型检查
- 更好的 IDE 支持（自动补全、跳转定义等）

## 类定义

```cpp
namespace fk::binding {

template<typename T, typename Owner>
class ObservableProperty {
public:
    // 构造函数
    ObservableProperty(Owner* owner, std::string_view name);
    
    // 配置方法（链式调用）
    ObservableProperty& Getter(std::function<const T&()> func);
    ObservableProperty& Setter(std::function<void(const T&)> func);
    ObservableProperty& Validator(std::function<bool(const T&)> func);
    
    // 值访问
    const T& get() const;
    void set(const T& val);
    
    // 运算符重载
    operator const T&() const;
    ObservableProperty& operator=(const T& val);

private:
    T value_;                                    // 属性值存储
    Owner* owner_;                               // 所属对象指针
    std::string_view name_;                      // 属性名称
    
    std::function<const T&()> getter_;           // 可自定义的 getter
    std::function<void(const T&)> setter_;       // 可自定义的 setter
    std::function<bool(const T&)> validator_;    // 可自定义的 validator
};

} // namespace fk::binding

namespace fk {
    using binding::ObservableProperty;  // 命名空间别名
}
```

## 模板参数

### T

属性值的类型。

可以是任何支持 `!=` 运算符比较的类型。常见类型包括：
- 基本类型：`int`、`double`、`bool` 等
- 字符串类型：`std::string`
- 自定义值类型

### Owner

属性所属的类类型。

**约束**：必须继承自 `ObservableObject`（或实现 `INotifyPropertyChanged` 接口并提供 `RaisePropertyChanged` 方法）。

---

## 构造函数

### ObservableProperty(Owner* owner, std::string_view name)

```cpp
ObservableProperty(Owner* owner, std::string_view name);
```

创建一个可观察属性实例。

**参数**：
- `owner` - 属性所属对象指针（通常传入 `this`）
- `name` - 属性名称（用于绑定路径和变更通知）

**行为**：
1. 设置默认的 getter、setter 和 validator
2. 默认 getter 返回内部存储的值
3. 默认 setter 在值变更时调用 `owner->RaisePropertyChanged(name)`
4. 默认 validator 总是返回 `true`
5. 自动将属性注册到 `PropertyAccessorRegistry`

**示例**：
```cpp
class Person : public fk::ObservableObject {
public:
    // 基本用法：传入 this 和属性名
    fk::ObservableProperty<std::string, Person> name{this, "Name"};
    fk::ObservableProperty<int, Person> age{this, "Age"};
};
```

---

## 配置方法

### Getter

```cpp
ObservableProperty& Getter(std::function<const T&()> func);
```

设置自定义的 getter 函数。

**参数**：
- `func` - 自定义 getter 函数，签名：`const T&()`

**返回值**：
返回 `*this`，支持链式调用。

**说明**：
完全替换默认的 getter 行为。自定义 getter 可以用于计算属性、延迟加载等场景。

**示例**：
```cpp
class Rectangle : public fk::ObservableObject {
public:
    fk::ObservableProperty<double, Rectangle> width{this, "Width"};
    fk::ObservableProperty<double, Rectangle> height{this, "Height"};
    
    // 计算属性：面积 = 宽 × 高
    fk::ObservableProperty<double, Rectangle> area{this, "Area"}
        .Getter([this]() -> const double& {
            static double cachedArea;
            cachedArea = width.get() * height.get();
            return cachedArea;
        });
};
```

---

### Setter

```cpp
ObservableProperty& Setter(std::function<void(const T&)> func);
```

设置自定义的 setter 函数。

**参数**：
- `func` - 自定义 setter 函数，签名：`void(const T&)`

**返回值**：
返回 `*this`，支持链式调用。

**说明**：
完全替换默认的 setter 行为（包括变更通知）。如果需要变更通知，需要在自定义 setter 中手动调用 `owner_->RaisePropertyChanged()`。

**示例**：
```cpp
class Temperature : public fk::ObservableObject {
public:
    // 自定义 setter：同时更新摄氏度和华氏度
    fk::ObservableProperty<double, Temperature> celsius{this, "Celsius"}
        .Setter([this](const double& val) {
            if (celsius_ != val) {
                celsius_ = val;
                fahrenheit_ = val * 9.0 / 5.0 + 32.0;
                RaisePropertyChanged("Celsius");
                RaisePropertyChanged("Fahrenheit");
            }
        });

private:
    double celsius_ = 0.0;
    double fahrenheit_ = 32.0;
};
```

---

### Validator

```cpp
ObservableProperty& Validator(std::function<bool(const T&)> func);
```

设置验证函数。

**参数**：
- `func` - 验证函数，签名：`bool(const T&)`

**返回值**：
返回 `*this`，支持链式调用。

**说明**：
在调用 setter 前验证值。如果验证函数返回 `false`，则不执行 setter，属性值保持不变。

**示例**：
```cpp
class Student : public fk::ObservableObject {
public:
    // 验证成绩在 0-100 之间
    fk::ObservableProperty<int, Student> score{this, "Score"}
        .Validator([](int val) { return val >= 0 && val <= 100; });
    
    // 验证年龄为正数
    fk::ObservableProperty<int, Student> age{this, "Age"}
        .Validator([](int val) { return val > 0; });
    
    // 验证姓名非空
    fk::ObservableProperty<std::string, Student> name{this, "Name"}
        .Validator([](const std::string& val) { return !val.empty(); });
};
```

---

## 值访问方法

### get

```cpp
const T& get() const;
```

获取属性值。

**返回值**：
属性值的常量引用。

**说明**：
调用 getter 函数返回属性值。默认实现返回内部存储的值。

**示例**：
```cpp
Person person;
person.name = "Alice";

// 使用 get() 方法
std::string personName = person.name.get();  // "Alice"
```

---

### set

```cpp
void set(const T& val);
```

设置属性值。

**参数**：
- `val` - 新值

**说明**：
执行流程：
1. 调用验证函数验证新值
2. 如果验证失败（返回 `false`），不执行 setter，直接返回
3. 如果验证通过，调用 setter 函数设置新值

默认的 setter 实现会在值变更时自动触发 `PropertyChanged` 事件。

**示例**：
```cpp
Student student;

// 使用 set() 方法
student.score.set(95);   // 验证通过，设置成功
student.score.set(150);  // 验证失败（>100），不会设置
student.score.set(-10);  // 验证失败（<0），不会设置
```

---

## 运算符重载

### operator const T&() const

```cpp
operator const T&() const;
```

隐式转换为属性值类型。

**返回值**：
属性值的常量引用（等同于调用 `get()`）。

**示例**：
```cpp
Person person;
person.name = "Bob";

// 隐式转换
std::string s = person.name;  // 隐式调用 operator const T&()
std::cout << person.name;     // 隐式转换为 std::string

// 在条件表达式中使用
if (person.age > 18) {        // age 隐式转换为 int
    // ...
}
```

---

### operator=(const T& val)

```cpp
ObservableProperty& operator=(const T& val);
```

赋值运算符。

**参数**：
- `val` - 新值

**返回值**：
返回 `*this`。

**说明**：
等同于调用 `set(val)`。

**示例**：
```cpp
Person person;

// 使用赋值运算符
person.name = "Charlie";  // 等同于 person.name.set("Charlie")
person.age = 25;          // 等同于 person.age.set(25)
```

---

## 工作原理

### 自动注册流程

```
┌─────────────────────────────────────────────────────────────┐
│  ObservableProperty 构造时自动注册                            │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  PropertyAccessorRegistry::RegisterProperty<Base>()         │
│  - Base = INotifyPropertyChanged                            │
│  - 注册 getter 和 setter lambda                              │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  BindingPath 可以通过字符串路径访问属性                        │
│  例如: "Name", "Age", "Score"                                │
└─────────────────────────────────────────────────────────────┘
```

### 值设置流程

```
set(newValue)
      │
      ▼
┌─────────────┐    false    ┌─────────────────┐
│ validator_  │ ──────────► │ 返回，不设置值   │
│ (newValue)  │             └─────────────────┘
└─────────────┘
      │ true
      ▼
┌─────────────┐
│ setter_     │
│ (newValue)  │
└─────────────┘
      │ (默认实现)
      ▼
┌─────────────────────────────────────┐
│ if (value_ != newValue) {           │
│     value_ = newValue;              │
│     owner_->RaisePropertyChanged(); │
│ }                                   │
└─────────────────────────────────────┘
```

---

## 使用示例

### 示例 1：基本 ViewModel

```cpp
#include "fk/binding/ObservableProperty.h"
#include "fk/binding/ObservableObject.h"

class UserViewModel : public fk::ObservableObject {
public:
    // 定义可观察属性
    fk::ObservableProperty<std::string, UserViewModel> firstName{this, "FirstName"};
    fk::ObservableProperty<std::string, UserViewModel> lastName{this, "LastName"};
    fk::ObservableProperty<int, UserViewModel> age{this, "Age"};
    fk::ObservableProperty<std::string, UserViewModel> email{this, "Email"};
};

// 使用
UserViewModel user;
user.firstName = "John";
user.lastName = "Doe";
user.age = 30;
user.email = "john.doe@example.com";

// 读取属性
std::cout << user.firstName.get() << " " << user.lastName.get() << std::endl;
// 或使用隐式转换
std::cout << static_cast<std::string>(user.firstName) << std::endl;
```

### 示例 2：带验证的属性

```cpp
class Product : public fk::ObservableObject {
public:
    // 产品名称不能为空
    fk::ObservableProperty<std::string, Product> name{this, "Name"}
        .Validator([](const std::string& val) { return !val.empty(); });
    
    // 价格必须为正数
    fk::ObservableProperty<double, Product> price{this, "Price"}
        .Validator([](double val) { return val > 0.0; });
    
    // 库存数量不能为负
    fk::ObservableProperty<int, Product> stock{this, "Stock"}
        .Validator([](int val) { return val >= 0; });
};

Product product;
product.name = "";        // 验证失败，不会设置
product.name = "Laptop";  // 验证通过

product.price = -100.0;   // 验证失败
product.price = 999.99;   // 验证通过

product.stock = -5;       // 验证失败
product.stock = 100;      // 验证通过
```

### 示例 3：自定义 getter/setter

```cpp
class PasswordViewModel : public fk::ObservableObject {
public:
    PasswordViewModel() {
        // 自定义 getter：返回星号遮罩
        password
            .Getter([this]() -> const std::string& {
                static std::string masked;
                masked = std::string(actualPassword_.size(), '*');
                return masked;
            })
            .Setter([this](const std::string& val) {
                actualPassword_ = val;
                RaisePropertyChanged("Password");
            });
    }
    
    fk::ObservableProperty<std::string, PasswordViewModel> password{this, "Password"};
    
    // 获取实际密码（内部使用）
    const std::string& GetActualPassword() const { return actualPassword_; }

private:
    std::string actualPassword_;
};

PasswordViewModel vm;
vm.password = "secret123";
std::cout << vm.password.get();  // 输出 "*********"
```

### 示例 4：链式调用配置

```cpp
class ConfigViewModel : public fk::ObservableObject {
public:
    // 组合使用 getter、setter 和 validator
    fk::ObservableProperty<int, ConfigViewModel> volume{this, "Volume"}
        .Validator([](int val) { return val >= 0 && val <= 100; })
        .Setter([this](const int& val) {
            if (volume_ != val) {
                volume_ = val;
                RaisePropertyChanged("Volume");
                // 同时更新静音状态
                if (val == 0 && !isMuted_) {
                    isMuted_ = true;
                    RaisePropertyChanged("IsMuted");
                }
            }
        })
        .Getter([this]() -> const int& { return volume_; });
    
private:
    int volume_ = 50;
    bool isMuted_ = false;
};
```

### 示例 5：与数据绑定集成

```cpp
// ViewModel
class PersonViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, PersonViewModel> name{this, "Name"};
    fk::ObservableProperty<int, PersonViewModel> age{this, "Age"};
};

// 绑定到 UI 元素
auto viewModel = std::make_shared<PersonViewModel>();
viewModel->name = "Alice";
viewModel->age = 25;

// 设置数据上下文
textBox->SetDataContext(viewModel);

// 创建绑定（可以使用字符串路径）
textBox->SetBinding(TextBox::TextProperty(), 
    fk::Binding().Path("Name").Mode(fk::BindingMode::TwoWay));

// 当 viewModel->name 变更时，TextBox 会自动更新
viewModel->name = "Bob";  // TextBox 显示 "Bob"

// 当用户在 TextBox 中输入时，viewModel->name 也会更新
```

---

## 与 ViewModel 宏对比

### 旧方式（ViewModel 宏）

```cpp
// 已弃用的方式
class OldViewModel : public fk::ObservableObject {
    OBSERVABLE_PROPERTY(std::string, Name)
    OBSERVABLE_PROPERTY(int, Age)
};

// 需要在类外注册
REGISTER_OBSERVABLE_PROPERTY(OldViewModel, Name)
REGISTER_OBSERVABLE_PROPERTY(OldViewModel, Age)
```

### 新方式（ObservableProperty）

```cpp
// 推荐的方式
class NewViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, NewViewModel> name{this, "Name"};
    fk::ObservableProperty<int, NewViewModel> age{this, "Age"};
};

// 无需额外注册！
```

| 特性 | ViewModel 宏 | ObservableProperty |
|------|-------------|-------------------|
| 注册方式 | 需要在类外使用宏 | 构造时自动注册 |
| 类型安全 | 依赖宏展开 | 编译期类型检查 |
| IDE 支持 | 有限 | 完整（自动补全、跳转） |
| 可调试性 | 困难 | 容易 |
| 自定义能力 | 有限 | 支持自定义 getter/setter/validator |
| 代码清晰度 | 宏隐藏细节 | 显式声明 |

---

## 私有成员说明

| 成员 | 类型 | 描述 |
|------|------|------|
| `value_` | `T` | 属性值存储 |
| `owner_` | `Owner*` | 所属对象指针，用于触发变更通知 |
| `name_` | `std::string_view` | 属性名称，用于绑定路径和变更通知 |
| `getter_` | `std::function<const T&()>` | getter 函数，默认返回 `value_` |
| `setter_` | `std::function<void(const T&)>` | setter 函数，默认更新 `value_` 并触发通知 |
| `validator_` | `std::function<bool(const T&)>` | 验证函数，默认总是返回 `true` |

---

## 注意事项

1. **Owner 类型约束**：Owner 必须继承自 `ObservableObject` 或提供 `RaisePropertyChanged(std::string_view)` 方法。

2. **生命周期**：`ObservableProperty` 存储 `owner_` 的原始指针，必须确保 owner 的生命周期覆盖属性的使用期。

3. **线程安全**：默认实现不是线程安全的。如果需要多线程访问，请在自定义 setter 中添加同步机制。

4. **自定义 setter 注意**：自定义 setter 会完全替换默认实现，需要手动调用 `RaisePropertyChanged` 来触发变更通知。

5. **验证失败静默处理**：验证失败时不会抛出异常，只是不执行 setter。如果需要用户反馈，请在 UI 层处理验证逻辑。

6. **属性名称唯一性**：在同一个类中，属性名称应该唯一，以确保绑定路径解析正确。

---

## 相关类

- [ObservableObject](ObservableObject.md) - 可观察对象基类
- [INotifyPropertyChanged](INotifyPropertyChanged.md) - 属性变更通知接口
- [BindingPath](BindingPath.md) - 属性路径解析器
- [PropertyAccessorRegistry](BindingPath.md#propertyaccessorregistry-类) - 属性访问器注册表
- [Binding](Binding.md) - 数据绑定配置类
