# BindingPath

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/BindingPath.h` |
| **源文件** | `src/binding/BindingPath.cpp` |

## 描述

`BindingPath` 负责解析和遍历属性路径字符串，实现：

- 解析属性路径字符串为路径段序列
- 支持简单属性、嵌套属性和索引访问
- 从源对象解析属性值
- 向源对象设置属性值
- 与 `PropertyAccessorRegistry` 协作实现属性访问

---

## BindingPath 类定义

```cpp
namespace fk::binding {

class BindingPath {
public:
    struct Segment {
        enum class Kind { Property, Index };

        Kind kind{Kind::Property};
        std::string name{};
        std::size_t index{0};

        static Segment Property(std::string segmentName);
        static Segment Index(std::size_t value);
    };

    BindingPath() = default;
    explicit BindingPath(std::string path);

    [[nodiscard]] bool IsEmpty() const noexcept;
    [[nodiscard]] const std::vector<Segment>& Segments() const noexcept;
    [[nodiscard]] const std::string& Raw() const noexcept;

    bool Resolve(const std::any& source, std::any& result) const;
    bool SetValue(std::any& source, const std::any& value) const;

private:
    std::string raw_;
    std::vector<Segment> segments_;
};

} // namespace fk::binding
```

## 构造函数

### BindingPath()

```cpp
BindingPath() = default;
```

默认构造函数，创建一个空的路径。

### BindingPath(std::string)

```cpp
explicit BindingPath(std::string path);
```

从路径字符串构造。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `path` | `std::string` | 属性路径字符串 |

**异常**：
- 如果路径中缺少闭合括号 `]`，抛出 `std::invalid_argument`
- 如果属性名为空，抛出 `std::invalid_argument`
- 如果索引不是有效数字，抛出 `std::invalid_argument`

**示例**：
```cpp
fk::BindingPath path1("Username");           // 简单属性
fk::BindingPath path2("User.Name");          // 嵌套属性
fk::BindingPath path3("Items[0]");           // 索引访问
fk::BindingPath path4("Users[0].Address.City"); // 组合路径
```

---

## 支持的路径语法

| 语法 | 描述 | 示例 |
|------|------|------|
| `PropertyName` | 简单属性访问 | `"Username"` |
| `Object.Property` | 嵌套属性访问 | `"User.Name"` |
| `Collection[n]` | 索引访问 | `"Items[0]"` |
| 组合 | 混合使用 | `"Users[0].Address.City"` |

---

## 查询方法

### IsEmpty

```cpp
[[nodiscard]] bool IsEmpty() const noexcept;
```

检查路径是否为空。

**返回值**：如果路径没有任何段返回 `true`

**示例**：
```cpp
fk::BindingPath emptyPath;
fk::BindingPath path("Name");

std::cout << emptyPath.IsEmpty() << std::endl;  // true
std::cout << path.IsEmpty() << std::endl;       // false
```

### Segments

```cpp
[[nodiscard]] const std::vector<Segment>& Segments() const noexcept;
```

获取解析后的路径段序列。

**返回值**：`Segment` 向量的常量引用

**示例**：
```cpp
fk::BindingPath path("User.Items[0].Name");

for (const auto& segment : path.Segments()) {
    if (segment.kind == fk::BindingPath::Segment::Kind::Property) {
        std::cout << "属性: " << segment.name << std::endl;
    } else {
        std::cout << "索引: " << segment.index << std::endl;
    }
}
// 输出:
// 属性: User
// 属性: Items
// 索引: 0
// 属性: Name
```

### Raw

```cpp
[[nodiscard]] const std::string& Raw() const noexcept;
```

获取原始路径字符串。

**返回值**：原始路径字符串的常量引用

**示例**：
```cpp
fk::BindingPath path("User.Name");
std::cout << path.Raw() << std::endl;  // "User.Name"
```

---

## 值操作方法

### Resolve

```cpp
bool Resolve(const std::any& source, std::any& result) const;
```

从源对象解析属性值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `source` | `const std::any&` | 源对象 |
| `result` | `std::any&` | 输出参数，解析的值 |

**返回值**：如果成功解析返回 `true`，否则返回 `false`

**说明**：
- 如果路径为空，返回源对象本身
- 逐段遍历路径，使用 `PropertyAccessorRegistry` 获取属性值
- 支持 `std::vector<std::any>` 的索引访问

**示例**：
```cpp
// 假设已注册 Person 类的属性访问器
Person person;
person.name = "张三";

std::any source = person;
std::any result;

fk::BindingPath path("name");
if (path.Resolve(source, result)) {
    std::cout << std::any_cast<std::string>(result) << std::endl;  // "张三"
}
```

### SetValue

```cpp
bool SetValue(std::any& source, const std::any& value) const;
```

向源对象设置属性值。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `source` | `std::any&` | 源对象（可修改） |
| `value` | `const std::any&` | 要设置的值 |

**返回值**：如果成功设置返回 `true`，否则返回 `false`

**说明**：
- 如果路径为空，直接替换源对象
- 逐段遍历路径，在最后一段使用 `PropertyAccessorRegistry` 设置值
- 支持 `std::vector<std::any>` 的索引写入

**示例**：
```cpp
std::any source = person;
fk::BindingPath path("name");

if (path.SetValue(source, std::string("李四"))) {
    std::cout << "设置成功" << std::endl;
}
```

---

## Segment 内部结构

### Kind 枚举

```cpp
enum class Kind { Property, Index };
```

| 值 | 描述 |
|----|------|
| `Property` | 属性访问段 |
| `Index` | 索引访问段 |

### 静态工厂方法

```cpp
static Segment Property(std::string segmentName);
static Segment Index(std::size_t value);
```

创建路径段的便捷方法。

**示例**：
```cpp
auto propSegment = fk::BindingPath::Segment::Property("Name");
auto indexSegment = fk::BindingPath::Segment::Index(0);
```

---

## 私有成员

| 成员 | 类型 | 描述 |
|------|------|------|
| `raw_` | `std::string` | 原始路径字符串 |
| `segments_` | `std::vector<Segment>` | 解析后的路径段序列 |

---

## 相关类

- [PropertyAccessorRegistry](PropertyAccessorRegistry.md) - 属性访问器注册表，为 BindingPath 提供属性解析支持
- [Binding](Binding.md) - 使用 BindingPath 配置属性路径
- [BindingExpression](BindingExpression.md) - 使用 BindingPath 解析绑定值
- [ObservableObject](ObservableObject.md) - ViewModel 基类

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
- [入门指南](../../Guides/GettingStarted.md)
