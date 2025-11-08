# PropertyAccessors - 属性访问器

## 概述

`PropertyAccessors` 模块提供了一套机制，允许绑定系统访问自定义类型的属性，实现动态属性绑定。通过注册 getter 和 setter，任何类型都可以参与数据绑定。

## 核心概念

属性访问器是一对函数（getter 和 setter），允许绑定系统以统一的方式访问类型的属性：

```cpp
struct Accessor {
    using Getter = std::function<bool(const std::any&, std::any&)>;
    using Setter = std::function<bool(std::any&, const std::any&)>;
};
```

## 注册方式

### 方式 1: 使用宏注册

```cpp
class Person {
public:
    std::string GetName() const { return name_; }
    void SetName(const std::string& value) { name_ = value; }
private:
    std::string name_;
};

// 在某处注册
FK_BINDING_REGISTER_PROPERTY(
    Person,
    "Name",
    &Person::GetName,
    &Person::SetName
)
```

### 方式 2: 程序化注册

```cpp
fk::binding::PropertyAccessorRegistry::RegisterProperty<Person>(
    "Name",
    &Person::GetName,
    &Person::SetName
);
```

### 方式 3: 仅注册 Getter

```cpp
FK_BINDING_REGISTER_GETTER(Person, "Name", &Person::GetName)
```

## 工作流程

1. 在应用启动时注册所有需要绑定的类型
2. 当绑定路径需要访问属性时，查询注册的访问器
3. 使用访问器的 getter/setter 进行值访问

## 相关文档

- [BindingPath.md](./BindingPath.md) - 绑定路径解析
- [Binding.md](./Binding.md) - 绑定系统
