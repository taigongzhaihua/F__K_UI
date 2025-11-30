# PropertyAccessorRegistry

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/BindingPath.h` |
| **源文件** | `src/binding/BindingPath.cpp` |

## 描述

`PropertyAccessorRegistry` 是属性访问器的全局注册表，是数据绑定系统中属性访问的核心基础设施。它实现：

- 🔧 **属性访问器注册** - 注册类型的属性 getter 和 setter 函数
- 📦 **多种持有类型支持** - 自动支持值、指针、智能指针、引用包装
- 🔍 **运行时属性解析** - 通过属性名称在运行时访问属性
- 🔗 **BindingPath 集成** - 为 `BindingPath` 提供属性解析支持
- 🔒 **线程安全** - 内部使用互斥锁保护注册表

---

## 类定义

```cpp
namespace fk::binding {

class PropertyAccessorRegistry {
public:
    // 访问器结构体
    struct Accessor {
        using Getter = std::function<bool(const std::any&, std::any&)>;
        using Setter = std::function<bool(std::any&, const std::any&)>;

        Getter getter{};
        Setter setter{};
    };

    // 底层注册方法
    static void RegisterAccessor(std::type_index ownerType, std::string name, const Accessor& accessor);
    static const Accessor* FindAccessor(std::type_index ownerType, std::string_view name);

    // 模板便捷方法 - 注册 getter
    template<typename Owner, typename Getter>
    static void RegisterPropertyGetter(std::string name, Getter getter);

    // 模板便捷方法 - 注册 setter
    template<typename Owner, typename Setter>
    static void RegisterPropertySetter(std::string name, Setter setter);

    // 模板便捷方法 - 注册读写属性
    template<typename Owner, typename Getter, typename Setter>
    static void RegisterProperty(std::string name, Getter getter, Setter setter);

    // 模板便捷方法 - 注册只读属性
    template<typename Owner, typename Getter>
    static void RegisterProperty(std::string name, Getter getter);
};

} // namespace fk::binding
```

---

## Accessor 内部结构

### 类型别名

```cpp
using Getter = std::function<bool(const std::any&, std::any&)>;
using Setter = std::function<bool(std::any&, const std::any&)>;
```

| 类型 | 签名 | 描述 |
|------|------|------|
| `Getter` | `bool(const std::any& source, std::any& result)` | 从源对象读取属性值 |
| `Setter` | `bool(std::any& source, const std::any& value)` | 向源对象设置属性值 |

### 成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `getter` | `Getter` | 属性读取函数，可为空 |
| `setter` | `Setter` | 属性写入函数，可为空 |

---

## 底层注册方法

### RegisterAccessor

```cpp
static void RegisterAccessor(std::type_index ownerType, std::string name, const Accessor& accessor);
```

直接注册属性访问器到全局注册表。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `ownerType` | `std::type_index` | 属性所属类型的类型索引 |
| `name` | `std::string` | 属性名称 |
| `accessor` | `const Accessor&` | 包含 getter/setter 的访问器 |

**说明**：
- 此方法是底层 API，通常应使用模板便捷方法
- 如果同一类型的同名属性已注册，将被覆盖
- 线程安全

**示例**：
```cpp
PropertyAccessorRegistry::Accessor accessor;
accessor.getter = [](const std::any& source, std::any& result) -> bool {
    try {
        const auto& person = std::any_cast<const Person&>(source);
        result = person.name;
        return true;
    } catch (...) {
        return false;
    }
};
accessor.setter = [](std::any& source, const std::any& value) -> bool {
    try {
        auto& person = std::any_cast<Person&>(source);
        person.name = std::any_cast<std::string>(value);
        return true;
    } catch (...) {
        return false;
    }
};

fk::PropertyAccessorRegistry::RegisterAccessor(typeid(Person), "name", accessor);
```

### FindAccessor

```cpp
static const Accessor* FindAccessor(std::type_index ownerType, std::string_view name);
```

查找已注册的属性访问器。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `ownerType` | `std::type_index` | 属性所属类型的类型索引 |
| `name` | `std::string_view` | 属性名称 |

**返回值**：找到的访问器指针，如果未注册返回 `nullptr`

**说明**：
- 线程安全
- 返回的指针在注册表生命周期内有效

**示例**：
```cpp
auto* accessor = fk::PropertyAccessorRegistry::FindAccessor(typeid(Person), "name");

if (accessor && accessor->getter) {
    std::any source = person;
    std::any result;
    
    if (accessor->getter(source, result)) {
        std::cout << "姓名: " << std::any_cast<std::string>(result) << std::endl;
    }
}

if (accessor && accessor->setter) {
    std::any source = person;
    std::any value = std::string("李四");
    
    if (accessor->setter(source, value)) {
        std::cout << "设置成功" << std::endl;
    }
}
```

---

## 模板便捷方法

### RegisterPropertyGetter

```cpp
template<typename Owner, typename Getter>
static void RegisterPropertyGetter(std::string name, Getter getter);
```

注册只读属性访问器。

**模板参数**：
| 参数 | 描述 |
|------|------|
| `Owner` | 属性所属的类类型 |
| `Getter` | getter 函数类型（lambda、函数指针或成员函数指针） |

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 属性名称 |
| `getter` | `Getter` | getter 函数 |

**支持的 Getter 签名**：
- `PropertyType(const Owner&)` - 返回值
- `const PropertyType&(const Owner&)` - 返回常量引用
- `PropertyType Owner::*` - 成员指针
- `PropertyType (Owner::*)() const` - 常量成员函数

**示例**：
```cpp
class Person {
public:
    std::string name;
    int age;
    
    int GetAge() const { return age; }
};

// 使用 lambda 访问成员变量
fk::PropertyAccessorRegistry::RegisterPropertyGetter<Person>(
    "name",
    [](const Person& p) -> const std::string& { return p.name; }
);

// 使用成员函数指针
fk::PropertyAccessorRegistry::RegisterPropertyGetter<Person>(
    "age",
    &Person::GetAge
);

// 使用 lambda 返回计算值
fk::PropertyAccessorRegistry::RegisterPropertyGetter<Person>(
    "isAdult",
    [](const Person& p) { return p.age >= 18; }
);
```

### RegisterPropertySetter

```cpp
template<typename Owner, typename Setter>
static void RegisterPropertySetter(std::string name, Setter setter);
```

注册只写属性访问器。

**模板参数**：
| 参数 | 描述 |
|------|------|
| `Owner` | 属性所属的类类型 |
| `Setter` | setter 函数类型 |

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 属性名称 |
| `setter` | `Setter` | setter 函数 |

**支持的 Setter 签名**：
- `void(Owner&, const std::any&)` - 接受 std::any 值
- `void(Owner&, PropertyType)` - 接受具体类型值
- `void (Owner::*)(PropertyType)` - 成员函数

**示例**：
```cpp
class Person {
public:
    std::string name;
    int age;
    
    void SetAge(int newAge) { age = newAge; }
};

// 使用 lambda 处理 std::any
fk::PropertyAccessorRegistry::RegisterPropertySetter<Person>(
    "name",
    [](Person& p, const std::any& value) {
        p.name = std::any_cast<std::string>(value);
    }
);

// 使用成员函数指针
fk::PropertyAccessorRegistry::RegisterPropertySetter<Person>(
    "age",
    &Person::SetAge
);
```

### RegisterProperty（读写）

```cpp
template<typename Owner, typename Getter, typename Setter>
static void RegisterProperty(std::string name, Getter getter, Setter setter);
```

注册可读写属性访问器。

**模板参数**：
| 参数 | 描述 |
|------|------|
| `Owner` | 属性所属的类类型 |
| `Getter` | getter 函数类型 |
| `Setter` | setter 函数类型 |

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `name` | `std::string` | 属性名称 |
| `getter` | `Getter` | getter 函数 |
| `setter` | `Setter` | setter 函数 |

**示例**：
```cpp
fk::PropertyAccessorRegistry::RegisterProperty<Person>(
    "name",
    [](const Person& p) -> const std::string& { return p.name; },
    [](Person& p, const std::any& value) { p.name = std::any_cast<std::string>(value); }
);

fk::PropertyAccessorRegistry::RegisterProperty<Person>(
    "age",
    [](const Person& p) { return p.age; },
    [](Person& p, const std::any& value) { p.age = std::any_cast<int>(value); }
);
```

### RegisterProperty（只读）

```cpp
template<typename Owner, typename Getter>
static void RegisterProperty(std::string name, Getter getter);
```

注册只读属性访问器（`RegisterPropertyGetter` 的别名）。

**示例**：
```cpp
// 只读计算属性
fk::PropertyAccessorRegistry::RegisterProperty<Person>(
    "fullName",
    [](const Person& p) { return p.firstName + " " + p.lastName; }
);
```

---

## 支持的持有类型

`PropertyAccessorRegistry` 的模板方法会自动为以下类型变体注册访问器：

| 类型 | 描述 | 示例 |
|------|------|------|
| `Owner` | 值类型 | `Person` |
| `const Owner` | 常量值类型 | `const Person` |
| `Owner*` | 原始指针 | `Person*` |
| `const Owner*` | 常量原始指针 | `const Person*` |
| `std::shared_ptr<Owner>` | 共享指针 | `std::shared_ptr<Person>` |
| `std::shared_ptr<const Owner>` | 常量共享指针 | `std::shared_ptr<const Person>` |
| `std::weak_ptr<Owner>` | 弱指针 | `std::weak_ptr<Person>` |
| `std::weak_ptr<const Owner>` | 常量弱指针 | `std::weak_ptr<const Person>` |
| `std::reference_wrapper<Owner>` | 引用包装 | `std::ref(person)` |
| `std::reference_wrapper<const Owner>` | 常量引用包装 | `std::cref(person)` |

### 自动类型转换示例

```cpp
// 注册一次
fk::PropertyAccessorRegistry::RegisterProperty<Person>(
    "name",
    [](const Person& p) -> const std::string& { return p.name; },
    [](Person& p, const std::any& v) { p.name = std::any_cast<std::string>(v); }
);

// 以下所有类型都可以正常工作
Person person;
Person* ptr = &person;
std::shared_ptr<Person> sharedPtr = std::make_shared<Person>();
std::weak_ptr<Person> weakPtr = sharedPtr;
auto ref = std::ref(person);

std::any source1 = person;           // 值类型
std::any source2 = ptr;              // 指针
std::any source3 = sharedPtr;        // 共享指针
std::any source4 = weakPtr;          // 弱指针
std::any source5 = ref;              // 引用包装

// 所有这些都可以通过 BindingPath 解析
fk::BindingPath path("name");
std::any result;

path.Resolve(source1, result);  // ✓
path.Resolve(source2, result);  // ✓
path.Resolve(source3, result);  // ✓
path.Resolve(source4, result);  // ✓
path.Resolve(source5, result);  // ✓
```

---

## 属性访问流程

```
┌─────────────────────────────────────────────────────────────────┐
│                    PropertyAccessorRegistry                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │            RegisterProperty             │
        │  ┌───────────────────────────────────┐  │
        │  │ 1. 提取属性类型和所有者类型       │  │
        │  │ 2. 创建 Accessor 包装             │  │
        │  │ 3. 为所有持有类型变体注册         │  │
        │  └───────────────────────────────────┘  │
        └─────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                        内部注册表                                │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │  std::unordered_map<                                      │  │
│  │      std::type_index,                                     │  │
│  │      std::unordered_map<std::string, Accessor>            │  │
│  │  >                                                        │  │
│  └───────────────────────────────────────────────────────────┘  │
│                                                                  │
│  typeid(Person)      → { "name" → Accessor, "age" → Accessor }  │
│  typeid(Person*)     → { "name" → Accessor, "age" → Accessor }  │
│  typeid(shared_ptr)  → { "name" → Accessor, "age" → Accessor }  │
│  ...                                                             │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │            FindAccessor                 │
        │  ┌───────────────────────────────────┐  │
        │  │ 1. 获取 std::any 的实际类型       │  │
        │  │ 2. 在注册表中查找访问器           │  │
        │  │ 3. 返回 Accessor* 或 nullptr      │  │
        │  └───────────────────────────────────┘  │
        └─────────────────────────────────────────┘
                              │
                              ▼
        ┌─────────────────────────────────────────┐
        │           BindingPath::Resolve          │
        │  ┌───────────────────────────────────┐  │
        │  │ 1. 遍历路径段                     │  │
        │  │ 2. 对每段调用 FindAccessor        │  │
        │  │ 3. 调用 getter 获取中间值         │  │
        │  │ 4. 返回最终解析结果               │  │
        │  └───────────────────────────────────┘  │
        └─────────────────────────────────────────┘
```

---

## 使用示例

### 示例 1：基本属性注册

```cpp
#include "fk/binding/BindingPath.h"

// 定义数据类
class User {
public:
    std::string username;
    std::string email;
    int level;
    bool isActive;
};

// 注册所有属性
void RegisterUserProperties() {
    using namespace fk::binding;
    
    PropertyAccessorRegistry::RegisterProperty<User>(
        "username",
        [](const User& u) -> const std::string& { return u.username; },
        [](User& u, const std::any& v) { u.username = std::any_cast<std::string>(v); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<User>(
        "email",
        [](const User& u) -> const std::string& { return u.email; },
        [](User& u, const std::any& v) { u.email = std::any_cast<std::string>(v); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<User>(
        "level",
        [](const User& u) { return u.level; },
        [](User& u, const std::any& v) { u.level = std::any_cast<int>(v); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<User>(
        "isActive",
        [](const User& u) { return u.isActive; },
        [](User& u, const std::any& v) { u.isActive = std::any_cast<bool>(v); }
    );
}
```

### 示例 2：嵌套对象属性

```cpp
class Address {
public:
    std::string country;
    std::string city;
    std::string street;
    std::string zipCode;
};

class Company {
public:
    std::string name;
    Address headquarters;
    std::vector<std::string> departments;
};

void RegisterCompanyProperties() {
    using namespace fk::binding;
    
    // Address 属性
    PropertyAccessorRegistry::RegisterProperty<Address>(
        "country",
        [](const Address& a) -> const std::string& { return a.country; },
        [](Address& a, const std::any& v) { a.country = std::any_cast<std::string>(v); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<Address>(
        "city",
        [](const Address& a) -> const std::string& { return a.city; },
        [](Address& a, const std::any& v) { a.city = std::any_cast<std::string>(v); }
    );
    
    // Company 属性
    PropertyAccessorRegistry::RegisterProperty<Company>(
        "name",
        [](const Company& c) -> const std::string& { return c.name; },
        [](Company& c, const std::any& v) { c.name = std::any_cast<std::string>(v); }
    );
    
    // 嵌套对象属性
    PropertyAccessorRegistry::RegisterProperty<Company>(
        "headquarters",
        [](const Company& c) -> const Address& { return c.headquarters; },
        [](Company& c, const std::any& v) { c.headquarters = std::any_cast<Address>(v); }
    );
}

// 使用嵌套路径
void UseNestedPath() {
    Company company;
    company.name = "Acme Corp";
    company.headquarters.city = "New York";
    
    std::any source = company;
    std::any result;
    
    // 解析嵌套属性
    fk::BindingPath path("headquarters.city");
    if (path.Resolve(source, result)) {
        std::cout << "城市: " << std::any_cast<std::string>(result) << std::endl;
        // 输出: 城市: New York
    }
}
```

### 示例 3：计算属性和只读属性

```cpp
class Rectangle {
public:
    double width;
    double height;
};

void RegisterRectangleProperties() {
    using namespace fk::binding;
    
    // 可读写属性
    PropertyAccessorRegistry::RegisterProperty<Rectangle>(
        "width",
        [](const Rectangle& r) { return r.width; },
        [](Rectangle& r, const std::any& v) { r.width = std::any_cast<double>(v); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<Rectangle>(
        "height",
        [](const Rectangle& r) { return r.height; },
        [](Rectangle& r, const std::any& v) { r.height = std::any_cast<double>(v); }
    );
    
    // 只读计算属性
    PropertyAccessorRegistry::RegisterProperty<Rectangle>(
        "area",
        [](const Rectangle& r) { return r.width * r.height; }
    );
    
    PropertyAccessorRegistry::RegisterProperty<Rectangle>(
        "perimeter",
        [](const Rectangle& r) { return 2 * (r.width + r.height); }
    );
    
    PropertyAccessorRegistry::RegisterProperty<Rectangle>(
        "isSquare",
        [](const Rectangle& r) { return r.width == r.height; }
    );
}
```

### 示例 4：与数据绑定系统集成

```cpp
class PersonViewModel : public fk::ObservableObject {
public:
    fk::ObservableProperty<std::string, PersonViewModel> Name{this, "Name"};
    fk::ObservableProperty<int, PersonViewModel> Age{this, "Age"};
};

void SetupBinding() {
    // 注册 PersonViewModel 属性（ObservableProperty 自动注册）
    
    auto viewModel = std::make_shared<PersonViewModel>();
    viewModel->Name = "张三";
    viewModel->Age = 30;
    
    // 创建绑定
    auto binding = fk::Binding()
        .Path("Name")
        .Mode(fk::BindingMode::TwoWay);
    
    // 将绑定应用到 UI 元素
    textBox->SetBinding(fk::TextBox::TextProperty(), binding);
    
    // BindingExpression 内部使用 PropertyAccessorRegistry 解析路径
}
```

### 示例 5：使用宏简化注册

```cpp
// 可以创建辅助宏简化注册
#define REGISTER_PROPERTY(Owner, PropertyName, Type) \
    fk::PropertyAccessorRegistry::RegisterProperty<Owner>( \
        #PropertyName, \
        [](const Owner& o) -> const Type& { return o.PropertyName; }, \
        [](Owner& o, const std::any& v) { o.PropertyName = std::any_cast<Type>(v); } \
    )

#define REGISTER_READONLY_PROPERTY(Owner, PropertyName, Type) \
    fk::PropertyAccessorRegistry::RegisterProperty<Owner>( \
        #PropertyName, \
        [](const Owner& o) -> const Type& { return o.PropertyName; } \
    )

// 使用宏
class Product {
public:
    std::string name;
    double price;
    int quantity;
};

void RegisterProductProperties() {
    REGISTER_PROPERTY(Product, name, std::string);
    REGISTER_PROPERTY(Product, price, double);
    REGISTER_PROPERTY(Product, quantity, int);
}
```

---

## 线程安全

`PropertyAccessorRegistry` 是线程安全的：

- 所有注册和查找操作都使用互斥锁保护
- 多线程可以同时查找访问器
- 注册操作会独占锁

### 最佳实践

```cpp
// ✓ 推荐：在程序初始化时注册所有属性
int main() {
    // 初始化阶段注册属性
    RegisterAllProperties();
    
    // 然后启动 UI 和业务逻辑
    StartApplication();
}

// ✗ 避免：在运行时频繁注册
void OnButtonClick() {
    // 不推荐在事件处理中注册属性
    PropertyAccessorRegistry::RegisterProperty<...>(...);
}
```

---

## 性能考虑

### 查找性能

| 操作 | 复杂度 | 说明 |
|------|--------|------|
| `FindAccessor` | O(1) 平均 | 使用 `std::unordered_map` |
| `RegisterAccessor` | O(1) 平均 | 哈希表插入 |

### 优化建议

1. **批量注册**：在初始化时一次性注册所有属性
2. **避免重复注册**：相同属性不需要多次注册
3. **缓存访问器**：如果频繁访问同一属性，可以缓存 `Accessor*`

```cpp
// 缓存访问器示例
class OptimizedPropertyAccess {
    const PropertyAccessorRegistry::Accessor* nameAccessor_ = nullptr;
    
public:
    void Initialize() {
        nameAccessor_ = PropertyAccessorRegistry::FindAccessor(typeid(Person), "name");
    }
    
    std::string GetName(const std::any& person) {
        if (nameAccessor_ && nameAccessor_->getter) {
            std::any result;
            if (nameAccessor_->getter(person, result)) {
                return std::any_cast<std::string>(result);
            }
        }
        return "";
    }
};
```

---

## 私有成员

### 内部存储

| 存储 | 类型 | 描述 |
|------|------|------|
| `accessors` | `std::unordered_map<std::type_index, std::unordered_map<std::string, Accessor>>` | 类型到属性名到访问器的二级映射 |
| `mutex` | `std::mutex` | 保护注册表的互斥锁 |

### 内部辅助方法

| 方法 | 描述 |
|------|------|
| `UnwrapPointer` | 从 `std::any` 中解包指针类型 |
| `UnwrapSharedPtr` | 从 `std::any` 中解包共享指针 |
| `UnwrapWeakPtr` | 从 `std::any` 中解包弱指针 |
| `UnwrapRefWrapper` | 从 `std::any` 中解包引用包装 |

---

## 注意事项

### 类型匹配

```cpp
// ⚠️ 确保 getter 返回类型与实际使用匹配
PropertyAccessorRegistry::RegisterProperty<Person>(
    "age",
    [](const Person& p) { return p.age; },  // 返回 int
    [](Person& p, const std::any& v) {
        // 必须用 int 接收
        p.age = std::any_cast<int>(v);  // ✓
        // p.age = std::any_cast<double>(v);  // ✗ 类型不匹配
    }
);
```

### 异常处理

```cpp
// Accessor 的 getter/setter 应该捕获异常并返回 false
PropertyAccessorRegistry::RegisterProperty<Person>(
    "name",
    [](const Person& p) -> const std::string& { return p.name; },
    [](Person& p, const std::any& v) {
        try {
            p.name = std::any_cast<std::string>(v);
        } catch (const std::bad_any_cast&) {
            // 类型转换失败，setter 返回 false
            return;
        }
    }
);
```

### 生命周期

```cpp
// ⚠️ 确保源对象在访问期间有效
std::any source;
{
    Person person;
    source = &person;  // 存储指针
}
// person 已销毁，source 中的指针悬空

// ✓ 使用智能指针管理生命周期
auto person = std::make_shared<Person>();
std::any source = person;  // 共享所有权
```

---

## 相关类

- [BindingPath](BindingPath.md) - 使用 PropertyAccessorRegistry 解析属性路径
- [Binding](Binding.md) - 通过属性路径配置绑定
- [BindingExpression](BindingExpression.md) - 在运行时使用路径解析
- [ObservableProperty](ObservableProperty.md) - 自动注册属性访问器

## 另请参阅

- [数据绑定概述](BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
