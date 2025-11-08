# BindingPath - 绑定路径解析

## 概述

`BindingPath` 负责解析和执行绑定路径表达式，支持属性链式访问和索引访问。绑定系统使用它来定位源数据并获取/设置属性值。

## 核心概念

### 路径格式

绑定路径是点分隔的属性路径或带索引的混合路径：

```
"PropertyName"              // 简单属性
"Parent.Child"              // 嵌套属性
"Items[0]"                  // 索引访问
"Items[0].Name"             // 混合路径
"Data.Tags[2].Description"  // 复杂路径
```

## 路径段（Path Segment）

```cpp
struct Segment {
    enum class Kind { Property, Index };
    
    Kind kind{Kind::Property};
    std::string name{};
    std::size_t index{0};
    
    static Segment Property(std::string segmentName);
    static Segment Index(std::size_t value);
};
```

路径由若干段组成，每段代表一个属性或索引。

**示例：**
```cpp
auto seg1 = fk::binding::BindingPath::Segment::Property("Data");
auto seg2 = fk::binding::BindingPath::Segment::Index(0);
auto seg3 = fk::binding::BindingPath::Segment::Property("Name");
// 路径: "Data[0].Name"
```

## 核心 API

### 构造函数

```cpp
BindingPath() = default;
explicit BindingPath(std::string path);
```

**示例：**
```cpp
fk::binding::BindingPath path1;  // 空路径

fk::binding::BindingPath path2("User.Profile.Name");  // 解析路径

// 检查是否成功
if (path2.IsEmpty()) {
    std::cout << "路径为空\n";
}
```

### 查询路径

```cpp
bool IsEmpty() const noexcept;
const std::vector<Segment>& Segments() const noexcept;
const std::string& Raw() const noexcept;
```

**示例：**
```cpp
fk::binding::BindingPath path("Data.Items[0].Name");

std::cout << "原始路径: " << path.Raw() << "\n";  // "Data.Items[0].Name"

for (const auto& seg : path.Segments()) {
    if (seg.kind == fk::binding::BindingPath::Segment::Kind::Property) {
        std::cout << "属性: " << seg.name << "\n";
    } else {
        std::cout << "索引: " << seg.index << "\n";
    }
}
```

### 解析路径值

```cpp
bool Resolve(const std::any& source, std::any& result) const;
```

从源对象按路径解析得到目标值。

**示例：**
```cpp
struct Person {
    std::string name;
    std::vector<std::string> tags;
};

Person person;
person.name = "Alice";
person.tags.push_back("developer");

fk::binding::BindingPath namePath("name");
std::any result;

if (namePath.Resolve(std::ref(person), result)) {
    auto name = std::any_cast<std::string>(result);
    std::cout << "名称: " << name << "\n";  // "Alice"
}

// 索引访问
fk::binding::BindingPath tagPath("tags[0]");
if (tagPath.Resolve(std::ref(person), result)) {
    auto tag = std::any_cast<std::string>(result);
    std::cout << "标签: " << tag << "\n";  // "developer"
}
```

### 设置路径值

```cpp
bool SetValue(std::any& source, const std::any& value) const;
```

按路径设置源对象的属性值。

**示例：**
```cpp
Person person;
fk::binding::BindingPath path("name");

if (path.SetValue(std::ref(person), std::string("Bob"))) {
    std::cout << person.name << "\n";  // "Bob"
}
```

## 属性访问器注册

为了让 BindingPath 能访问自定义类型的属性，需要注册访问器：

```cpp
class PropertyAccessorRegistry {
public:
    struct Accessor {
        using Getter = std::function<bool(const std::any&, std::any&)>;
        using Setter = std::function<bool(std::any&, const std::any&)>;
        
        Getter getter{};
        Setter setter{};
    };
    
    // 注册访问器
    static void RegisterAccessor(
        std::type_index ownerType,
        std::string name,
        const Accessor& accessor
    );
    
    // 查找访问器
    static const Accessor* FindAccessor(
        std::type_index ownerType,
        std::string_view name
    );
    
    // 模板注册方法
    template<typename Owner, typename Getter>
    static void RegisterPropertyGetter(std::string name, Getter getter);
    
    template<typename Owner, typename Setter>
    static void RegisterPropertySetter(std::string name, Setter setter);
    
    template<typename Owner, typename Getter, typename Setter>
    static void RegisterProperty(std::string name, Getter getter, Setter setter);
};
```

### 注册自定义类型

```cpp
// 示例：注册 Person 类型

class Person {
public:
    std::string GetName() const { return name_; }
    void SetName(const std::string& value) { name_ = value; }
    
private:
    std::string name_;
};

// 在某个地方进行注册
void RegisterPersonAccessors() {
    using Registry = fk::binding::PropertyAccessorRegistry;
    
    Registry::RegisterProperty<Person>(
        "Name",
        &Person::GetName,
        &Person::SetName
    );
}
```

## 常见模式

### 1. 简单属性绑定

```cpp
class ViewModel {
public:
    std::string GetTitle() const { return title_; }
    void SetTitle(const std::string& v) { title_ = v; }
private:
    std::string title_;
};

void SimpleBindingExample() {
    auto vm = std::make_shared<ViewModel>();
    
    // 注册访问器
    fk::binding::PropertyAccessorRegistry::RegisterProperty<ViewModel>(
        "Title",
        &ViewModel::GetTitle,
        &ViewModel::SetTitle
    );
    
    // 使用路径解析
    fk::binding::BindingPath path("Title");
    std::any value;
    
    if (path.Resolve(vm, value)) {
        auto title = std::any_cast<std::string>(value);
    }
}
```

### 2. 嵌套属性访问

```cpp
class Address {
public:
    std::string GetCity() const { return city_; }
    void SetCity(const std::string& v) { city_ = v; }
private:
    std::string city_;
};

class Person {
public:
    Address& GetAddress() { return address_; }
    const Address& GetAddress() const { return address_; }
private:
    Address address_;
};

void NestedPathExample() {
    // 注册 Address 访问器
    fk::binding::PropertyAccessorRegistry::RegisterProperty<Address>(
        "City",
        &Address::GetCity,
        &Address::SetCity
    );
    
    // 注册 Person 访问器
    fk::binding::PropertyAccessorRegistry::RegisterProperty<Person>(
        "Address",
        &Person::GetAddress,
        [](Person& p, const std::any& v) {
            p.GetAddress() = std::any_cast<Address>(v);
        }
    );
    
    auto person = std::make_shared<Person>();
    
    // 解析嵌套路径
    fk::binding::BindingPath path("Address.City");
    std::any city;
    
    if (path.Resolve(person, city)) {
        std::cout << std::any_cast<std::string>(city) << "\n";
    }
}
```

### 3. 索引访问

```cpp
void IndexedPathExample() {
    std::vector<std::string> items = {"A", "B", "C"};
    
    // 直接访问向量
    fk::binding::BindingPath path("0");  // 访问第一个元素
    std::any value;
    
    if (path.Resolve(std::ref(items), value)) {
        auto item = std::any_cast<std::string>(value);  // "A"
    }
}
```

### 4. 混合路径（属性+索引）

```cpp
class DataContainer {
public:
    std::vector<std::string> GetItems() const { return items_; }
private:
    std::vector<std::string> items_ = {"X", "Y", "Z"};
};

void MixedPathExample() {
    fk::binding::PropertyAccessorRegistry::RegisterProperty<DataContainer>(
        "Items",
        &DataContainer::GetItems,
        [](DataContainer& c, const std::any& v) {
            // 设置逻辑
        }
    );
    
    auto container = std::make_shared<DataContainer>();
    
    // 混合路径：属性 + 索引
    fk::binding::BindingPath path("Items[1]");
    std::any value;
    
    if (path.Resolve(container, value)) {
        auto item = std::any_cast<std::string>(value);  // "Y"
    }
}
```

## 实现细节

### 路径解析

路径字符串的解析规则：
- 点 (`.`) 分隔属性名
- 括号 (`[n]`) 表示索引
- 空路径或仅点的路径无效

**示例解析：**
```
"A.B[0].C"
↓
[Segment::Property("A"),
 Segment::Property("B"),
 Segment::Index(0),
 Segment::Property("C")]
```

### 类型匹配

访问器注册时会注册多个类型变体：
- `Type&` - 非常量引用
- `const Type&` - 常量引用
- `Type*` - 指针
- `std::shared_ptr<Type>` - 共享指针

## 最佳实践

### 1. 在应用启动时注册访问器

```cpp
void InitializeBindingSystem() {
    RegisterPersonAccessors();
    RegisterCompanyAccessors();
    RegisterOrderAccessors();
    // ... 其他类型
}

int main() {
    InitializeBindingSystem();
    // ... 运行应用
}
```

### 2. 验证路径有效性

```cpp
bool IsValidPath(const std::string& path) {
    fk::binding::BindingPath p(path);
    return !p.IsEmpty();
}
```

### 3. 缓存路径对象

```cpp
class BindingCache {
private:
    std::unordered_map<std::string, fk::binding::BindingPath> paths_;
    
public:
    const fk::binding::BindingPath& GetPath(const std::string& pathStr) {
        auto it = paths_.find(pathStr);
        if (it != paths_.end()) {
            return it->second;
        }
        return paths_.emplace(pathStr, pathStr).first->second;
    }
};
```

## 常见问题

### Q: 如何访问容器中的元素？
**A:** 使用索引语法：`"Items[0]"` 访问第一个元素。

### Q: 可以使用非字符串索引吗？
**A:** 当前仅支持数值索引。

### Q: 路径解析失败怎么办？
**A:** `Resolve()` 返回 `false` 表示失败。检查路径语法和访问器注册情况。

### Q: 如何支持自定义类型的属性访问？
**A:** 使用 `PropertyAccessorRegistry::RegisterProperty()` 注册 getter 和 setter。

## 相关文档

- [BindingExpression.md](./BindingExpression.md) - 绑定表达式执行引擎
- [Binding.md](./Binding.md) - 绑定系统
- [ValueConverters.md](./ValueConverters.md) - 类型转换
