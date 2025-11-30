# BindingPath::Segment

## 概览

| 属性 | 值 |
|------|-----|
| **命名空间** | `fk::binding` |
| **头文件** | `fk/binding/BindingPath.h` |
| **外部类** | `BindingPath` |

## 描述

`BindingPath::Segment` 是 `BindingPath` 的内部结构体，表示属性路径中的单个段。每个段可以是：

- **属性段**：通过属性名访问对象成员（如 `User`、`Name`）
- **索引段**：通过数字索引访问集合元素（如 `[0]`、`[5]`）

路径 `"Users[0].Address.City"` 会被解析为四个段：
1. 属性段 `Users`
2. 索引段 `0`
3. 属性段 `Address`
4. 属性段 `City`

---

## 结构体定义

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
    
    // ... BindingPath 其他成员
};

} // namespace fk::binding
```

---

## Kind 枚举

```cpp
enum class Kind { Property, Index };
```

定义段的类型。

| 枚举值 | 描述 | 示例路径 |
|--------|------|---------|
| `Property` | 属性访问段，通过名称访问 | `"User"`, `"Name"` |
| `Index` | 索引访问段，通过数字索引访问 | `"[0]"`, `"[10]"` |

**示例**：
```cpp
fk::BindingPath path("Items[0].Name");

for (const auto& seg : path.Segments()) {
    if (seg.kind == fk::BindingPath::Segment::Kind::Property) {
        std::cout << "属性段: " << seg.name << std::endl;
    } else if (seg.kind == fk::BindingPath::Segment::Kind::Index) {
        std::cout << "索引段: [" << seg.index << "]" << std::endl;
    }
}
// 输出:
// 属性段: Items
// 索引段: [0]
// 属性段: Name
```

---

## 数据成员

### kind

```cpp
Kind kind{Kind::Property};
```

段的类型，默认为 `Kind::Property`。

| 值 | 描述 |
|----|------|
| `Kind::Property` | 属性段，使用 `name` 成员 |
| `Kind::Index` | 索引段，使用 `index` 成员 |

### name

```cpp
std::string name{};
```

属性名称，仅当 `kind == Kind::Property` 时有效。

**示例**：
```cpp
fk::BindingPath path("User.Name");
const auto& firstSegment = path.Segments()[0];

if (firstSegment.kind == fk::BindingPath::Segment::Kind::Property) {
    std::cout << firstSegment.name << std::endl;  // "User"
}
```

### index

```cpp
std::size_t index{0};
```

索引值，仅当 `kind == Kind::Index` 时有效。

**示例**：
```cpp
fk::BindingPath path("Items[5]");
const auto& secondSegment = path.Segments()[1];

if (secondSegment.kind == fk::BindingPath::Segment::Kind::Index) {
    std::cout << secondSegment.index << std::endl;  // 5
}
```

---

## 静态工厂方法

### Property

```cpp
static Segment Property(std::string segmentName);
```

创建属性段。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `segmentName` | `std::string` | 属性名称 |

**返回值**：配置好的 `Segment` 对象，`kind` 为 `Kind::Property`

**示例**：
```cpp
auto segment = fk::BindingPath::Segment::Property("Username");

std::cout << (segment.kind == fk::BindingPath::Segment::Kind::Property) << std::endl;  // true
std::cout << segment.name << std::endl;  // "Username"
```

### Index

```cpp
static Segment Index(std::size_t value);
```

创建索引段。

**参数**：
| 参数 | 类型 | 描述 |
|------|------|------|
| `value` | `std::size_t` | 索引值 |

**返回值**：配置好的 `Segment` 对象，`kind` 为 `Kind::Index`

**示例**：
```cpp
auto segment = fk::BindingPath::Segment::Index(3);

std::cout << (segment.kind == fk::BindingPath::Segment::Kind::Index) << std::endl;  // true
std::cout << segment.index << std::endl;  // 3
```

---

## 路径解析示例

### 简单属性路径

```cpp
fk::BindingPath path("Name");

// 解析结果：1 个段
// [0] Kind::Property, name="Name"
```

### 嵌套属性路径

```cpp
fk::BindingPath path("User.Address.City");

// 解析结果：3 个段
// [0] Kind::Property, name="User"
// [1] Kind::Property, name="Address"
// [2] Kind::Property, name="City"
```

### 索引访问路径

```cpp
fk::BindingPath path("Items[0]");

// 解析结果：2 个段
// [0] Kind::Property, name="Items"
// [1] Kind::Index, index=0
```

### 复杂混合路径

```cpp
fk::BindingPath path("Orders[2].Items[0].Product.Name");

// 解析结果：6 个段
// [0] Kind::Property, name="Orders"
// [1] Kind::Index, index=2
// [2] Kind::Property, name="Items"
// [3] Kind::Index, index=0
// [4] Kind::Property, name="Product"
// [5] Kind::Property, name="Name"
```

---

## 使用示例

### 示例 1：遍历路径段

```cpp
void PrintPathStructure(const fk::BindingPath& path) {
    std::cout << "路径: " << path.Raw() << std::endl;
    std::cout << "段数: " << path.Segments().size() << std::endl;
    
    int i = 0;
    for (const auto& segment : path.Segments()) {
        std::cout << "[" << i++ << "] ";
        
        switch (segment.kind) {
            case fk::BindingPath::Segment::Kind::Property:
                std::cout << "属性: \"" << segment.name << "\"" << std::endl;
                break;
            case fk::BindingPath::Segment::Kind::Index:
                std::cout << "索引: [" << segment.index << "]" << std::endl;
                break;
        }
    }
}

// 使用
fk::BindingPath path("Users[0].Profile.Avatar");
PrintPathStructure(path);

// 输出:
// 路径: Users[0].Profile.Avatar
// 段数: 4
// [0] 属性: "Users"
// [1] 索引: [0]
// [2] 属性: "Profile"
// [3] 属性: "Avatar"
```

### 示例 2：手动构建路径段

```cpp
// 虽然通常通过字符串构造 BindingPath，
// 但可以使用工厂方法创建独立的 Segment
auto propSeg = fk::BindingPath::Segment::Property("Items");
auto idxSeg = fk::BindingPath::Segment::Index(0);

std::cout << "属性段名: " << propSeg.name << std::endl;  // "Items"
std::cout << "索引段值: " << idxSeg.index << std::endl;  // 0
```

### 示例 3：根据段类型执行不同逻辑

```cpp
void ProcessSegment(const fk::BindingPath::Segment& segment) {
    using Kind = fk::BindingPath::Segment::Kind;
    
    switch (segment.kind) {
        case Kind::Property:
            // 通过 PropertyAccessorRegistry 获取属性值
            std::cout << "访问属性: " << segment.name << std::endl;
            break;
            
        case Kind::Index:
            // 通过索引访问集合元素
            std::cout << "访问索引: " << segment.index << std::endl;
            break;
    }
}
```

### 示例 4：检查路径是否包含索引访问

```cpp
bool HasIndexAccess(const fk::BindingPath& path) {
    for (const auto& segment : path.Segments()) {
        if (segment.kind == fk::BindingPath::Segment::Kind::Index) {
            return true;
        }
    }
    return false;
}

// 使用
fk::BindingPath path1("User.Name");
fk::BindingPath path2("Items[0].Name");

std::cout << HasIndexAccess(path1) << std::endl;  // false
std::cout << HasIndexAccess(path2) << std::endl;  // true
```

---

## 路径解析流程

```
输入: "Users[0].Address.City"
        │
        ▼
┌─────────────────────────────────┐
│      路径字符串解析器            │
│   (BindingPath 构造函数)         │
└─────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────┐
│ 分割为 token:                    │
│ "Users" → "[0]" → "Address" →   │
│ "City"                           │
└─────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────┐
│ 为每个 token 创建 Segment:       │
│                                  │
│ "Users"   → Property("Users")   │
│ "[0]"     → Index(0)            │
│ "Address" → Property("Address") │
│ "City"    → Property("City")    │
└─────────────────────────────────┘
        │
        ▼
输出: std::vector<Segment> 
      包含 4 个段
```

---

## 段类型对比

| 特性 | Property 段 | Index 段 |
|------|-------------|----------|
| **Kind 值** | `Kind::Property` | `Kind::Index` |
| **有效成员** | `name` | `index` |
| **路径语法** | `PropertyName` | `[n]` |
| **访问方式** | PropertyAccessorRegistry | std::vector 索引 |
| **示例** | `"User"`, `"Name"` | `"[0]"`, `"[10]"` |

---

## 注意事项

1. **成员有效性**：根据 `kind` 的值，只有对应的成员是有效的
   - `Kind::Property` → 使用 `name`
   - `Kind::Index` → 使用 `index`

2. **默认值**：新创建的 Segment 默认为 Property 类型，`name` 为空字符串

3. **工厂方法优先**：建议使用 `Property()` 和 `Index()` 工厂方法创建 Segment，而非直接构造

4. **不可变性**：虽然 Segment 的成员是公开的，但通常作为只读数据使用

---

## 相关类

- [BindingPath](BindingPath.md) - 包含 Segment 的外部类
- [PropertyAccessorRegistry](PropertyAccessorRegistry.md) - 用于解析 Property 段
- [Binding](Binding.md) - 使用 BindingPath 配置绑定

## 另请参阅

- [数据绑定概述](../BINDING_MODULE_OVERVIEW.md)
