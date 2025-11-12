# DependencyProperty 设计文档

## 类概述

`DependencyProperty` 是依赖属性系统的核心元数据类，负责注册、存储和管理依赖属性的元数据信息。它是整个数据绑定和属性系统的基础，为属性提供变更通知、验证、默认值和绑定配置等功能。

## 继承关系

```
DependencyProperty (无基类)
```

**设计说明**：
- 独立的元数据类，不继承任何基类
- 采用不可复制、不可移动的设计，确保全局唯一性
- 通过 `friend class DependencyPropertyRegistry` 实现注册表访问控制

## 核心职责

### 主要职责

1. **属性注册管理**
   - 注册普通依赖属性
   - 注册附加属性（Attached Property）
   - 维护属性的全局唯一标识（ID）

2. **元数据存储**
   - 存储属性名称、类型信息
   - 存储所有者类型信息
   - 管理属性元数据（默认值、回调、验证器等）

3. **属性配置**
   - 配置绑定模式（OneWay/TwoWay等）
   - 配置更新源触发器
   - 配置数据上下文继承行为

4. **类型安全**
   - 使用 `std::type_index` 确保类型安全
   - 在编译时和运行时进行类型检查

## 成员函数职责

### 公共静态方法

#### `Register(name, propertyType, ownerType, metadata)`
**职责**：注册普通依赖属性
**参数**：
- `name`: 属性名称
- `propertyType`: 属性值类型
- `ownerType`: 所有者类类型
- `metadata`: 属性元数据（默认值、回调等）

**实现状态**：✅ 已实现
**实现细节**：调用内部的 `RegisterInternal()`，传递 `attached=false`

#### `RegisterAttached(name, propertyType, ownerType, metadata)`
**职责**：注册附加属性
**参数**：同 `Register()`
**实现状态**：✅ 已实现
**实现细节**：调用内部的 `RegisterInternal()`，传递 `attached=true`

### 公共访问器方法

#### `Name()` 
**职责**：获取属性名称
**实现状态**：✅ 已实现
**返回**：属性名称的常量引用

#### `PropertyType()`
**职责**：获取属性值类型
**实现状态**：✅ 已实现
**返回**：`std::type_index` 类型信息

#### `OwnerType()`
**职责**：获取所有者类型
**实现状态**：✅ 已实现
**返回**：`std::type_index` 类型信息

#### `Metadata()`
**职责**：获取属性元数据
**实现状态**：✅ 已实现
**返回**：`PropertyMetadata` 结构的常量引用

#### `Id()`
**职责**：获取属性的全局唯一标识
**实现状态**：✅ 已实现
**返回**：属性ID（用于快速查找）

#### `IsAttached()`
**职责**：判断是否为附加属性
**实现状态**：✅ 已实现
**返回**：布尔值，true表示附加属性

### 私有方法

#### `DependencyProperty()` 构造函数
**职责**：私有构造函数，防止外部直接创建实例
**实现状态**：✅ 已实现

#### `RegisterInternal()`
**职责**：内部注册实现，被 `Register()` 和 `RegisterAttached()` 调用
**实现状态**：✅ 已实现
**实现细节**：
- 创建 `DependencyProperty` 实例
- 分配全局唯一ID
- 注册到 `DependencyPropertyRegistry`
- 返回属性的常量引用

## 相关类型

### PropertyMetadata 结构
**职责**：存储属性的元数据
**成员**：
- `defaultValue`: 属性默认值（`std::any`）
- `propertyChangedCallback`: 属性变更回调函数
- `validateCallback`: 值验证回调函数
- `bindingOptions`: 绑定配置选项

**实现状态**：✅ 已实现

### BindingOptions 结构
**职责**：配置绑定行为
**成员**：
- `defaultMode`: 默认绑定模式（OneWay/TwoWay等）
- `updateSourceTrigger`: 更新源的触发时机
- `inheritsDataContext`: 是否继承数据上下文

**实现状态**：✅ 已实现

### BindingMode 枚举
**职责**：定义绑定模式
**值**：
- `OneTime`: 一次性绑定
- `OneWay`: 单向绑定（源到目标）
- `TwoWay`: 双向绑定
- `OneWayToSource`: 反向单向绑定（目标到源）

**实现状态**：✅ 已实现

### UpdateSourceTrigger 枚举
**职责**：定义更新源的触发时机
**值**：
- `Default`: 使用默认触发器
- `PropertyChanged`: 属性改变时立即更新
- `LostFocus`: 失去焦点时更新
- `Explicit`: 显式调用时更新

**实现状态**：✅ 已实现

## 实现状态总结

### 已实现功能 ✅
1. ✅ 属性注册系统（普通属性和附加属性）
2. ✅ 元数据存储和访问
3. ✅ 类型安全的属性类型系统
4. ✅ 全局唯一ID分配
5. ✅ 属性变更回调机制
6. ✅ 值验证回调机制
7. ✅ 绑定模式配置
8. ✅ 更新触发器配置

### 简单实现须扩充 ⚠️
1. ⚠️ **属性继承机制**：当前的 `inheritsDataContext` 较为简单，可能需要更复杂的继承链管理
2. ⚠️ **属性优先级系统**：缺少类似WPF的属性值优先级（本地值、样式、模板、继承值等）
3. ⚠️ **属性元数据覆盖**：子类覆盖父类属性元数据的机制尚不完善

### 未实现功能 ❌
1. ❌ **ReadOnly属性**：只读依赖属性的支持
2. ❌ **PropertyMetadata继承**：子类继承并修改父类属性元数据
3. ❌ **CoerceValue回调**：强制转换属性值的回调机制
4. ❌ **动画支持**：属性动画的元数据标记

## 实现原理

### 注册机制

```cpp
// 注册流程
Register() → RegisterInternal() → DependencyPropertyRegistry::Register()
                                                ↓
                                  分配全局唯一ID → 存储到注册表 → 返回引用
```

**核心设计**：
1. **单例注册表**：使用 `DependencyPropertyRegistry` 管理所有属性
2. **延迟初始化**：属性在首次使用时注册
3. **全局ID**：每个属性获得唯一的数字ID，用于快速查找

### 类型安全

使用 `std::type_index` 实现运行时类型检查：

```cpp
// 属性类型
std::type_index propertyType_;  // 例如：typeid(int), typeid(std::string)

// 所有者类型
std::type_index ownerType_;     // 例如：typeid(Button), typeid(TextBlock)
```

### 元数据存储

使用 `std::any` 实现类型擦除的默认值存储：

```cpp
struct PropertyMetadata {
    std::any defaultValue;  // 可存储任意类型的默认值
    // ...
};
```

### 回调机制

```cpp
// 属性变更回调
PropertyChangedCallback: (DependencyObject&, const DependencyProperty&, 
                         const std::any& oldValue, const std::any& newValue) -> void

// 值验证回调
ValidateValueCallback: (const std::any& value) -> bool
```

## 扩展方向

### 短期扩展（高优先级）

1. **只读属性支持**
   ```cpp
   static const DependencyProperty& RegisterReadOnly(
       std::string name,
       std::type_index propertyType,
       std::type_index ownerType,
       PropertyMetadata metadata);
   ```
   - 返回一个 `DependencyPropertyKey` 用于内部设置
   - 外部只能读取，不能直接设置

2. **属性值优先级**
   ```cpp
   enum class PropertyValuePriority {
       Default,        // 默认值
       Inherited,      // 继承值
       Theme,          // 主题样式
       Style,          // 样式
       Template,       // 模板
       Animation,      // 动画
       Local           // 本地值（最高优先级）
   };
   ```

3. **CoerceValue回调**
   ```cpp
   using CoerceValueCallback = std::function<std::any(DependencyObject&, const std::any& baseValue)>;
   ```
   - 在属性值实际应用前强制转换或限制值

### 中期扩展

1. **元数据覆盖机制**
   ```cpp
   static void OverrideMetadata(
       const DependencyProperty& dp,
       std::type_index forType,
       PropertyMetadata metadata);
   ```
   - 允许子类修改继承属性的元数据

2. **属性组（Property Groups）**
   - 将相关属性分组管理
   - 批量设置和验证

3. **表达式支持**
   - 属性值可以是表达式而非具体值
   - 表达式在需要时动态计算

### 长期扩展

1. **属性动画系统集成**
   - 标记可动画属性
   - 提供动画插值回调

2. **跨线程属性访问**
   - 线程安全的属性访问机制
   - 跨线程属性变更通知

3. **序列化支持**
   - 属性序列化为JSON/XML
   - 从配置文件加载属性值

## 性能考虑

### 当前性能特点

1. **ID查找**：O(1) 时间复杂度
   - 使用数字ID进行快速索引

2. **类型检查**：运行时开销
   - `std::type_index` 比较的性能开销
   - 建议在Debug模式启用，Release模式优化

3. **内存占用**：中等
   - 每个属性约 100-200 字节（包含回调和元数据）

### 优化建议

1. **属性ID缓存**
   ```cpp
   // 在类中缓存常用属性的ID
   static constexpr size_t NamePropertyId = /* ... */;
   ```

2. **回调内联**
   - 对简单的回调使用内联函数
   - 减少函数调用开销

3. **元数据共享**
   - 相同元数据的属性共享 `PropertyMetadata` 实例

## 设计决策

### 1. 为什么使用静态注册？

**决策**：属性通过静态方法 `Register()` 注册，返回常量引用

**理由**：
- 确保属性在整个应用生命周期中唯一
- 避免意外的属性重复定义
- 支持延迟初始化（首次使用时注册）

**替代方案**：
- ❌ 实例注册：每个对象创建自己的属性副本（内存浪费）
- ❌ 全局变量：初始化顺序问题

### 2. 为什么使用 std::any？

**决策**：默认值使用 `std::any` 存储

**理由**：
- 类型擦除，支持任意类型的默认值
- 运行时类型安全
- 简化API设计（无需模板）

**权衡**：
- ✅ 灵活性高
- ⚠️ 运行时类型检查开销
- ⚠️ 需要类型转换

### 3. 为什么禁止复制和移动？

**决策**：`DependencyProperty` 禁止复制和移动

**理由**：
- 属性必须全局唯一
- 防止意外的属性副本
- 强制使用引用或指针

**影响**：
- 必须通过引用传递属性
- 不能存储属性副本（只能存储引用或指针）

### 4. 附加属性的设计

**决策**：附加属性与普通属性共享同一类，通过标志区分

**理由**：
- 统一的属性系统
- 减少类的复杂度
- 附加属性和普通属性的行为差异较小

**权衡**：
- ✅ 简化实现
- ⚠️ 需要运行时检查 `IsAttached()`

## 使用示例

### 注册普通属性

```cpp
class Button : public Control {
public:
    static const DependencyProperty& ContentProperty() {
        static const auto& prop = DependencyProperty::Register(
            "Content",
            typeid(std::string),
            typeid(Button),
            PropertyMetadata{
                .defaultValue = std::string(""),
                .propertyChangedCallback = &OnContentChanged
            }
        );
        return prop;
    }

private:
    static void OnContentChanged(DependencyObject& obj, 
                                 const DependencyProperty& prop,
                                 const std::any& oldValue, 
                                 const std::any& newValue) {
        // 处理内容变更
    }
};
```

### 注册附加属性

```cpp
class Grid : public Panel {
public:
    static const DependencyProperty& RowProperty() {
        static const auto& prop = DependencyProperty::RegisterAttached(
            "Row",
            typeid(int),
            typeid(Grid),
            PropertyMetadata{
                .defaultValue = 0,
                .validateCallback = [](const std::any& value) {
                    return std::any_cast<int>(value) >= 0;
                }
            }
        );
        return prop;
    }

    static int GetRow(const DependencyObject& element) {
        return element.GetValue<int>(RowProperty());
    }

    static void SetRow(DependencyObject& element, int row) {
        element.SetValue(RowProperty(), row);
    }
};
```

### 使用绑定配置

```cpp
static const auto& prop = DependencyProperty::Register(
    "Text",
    typeid(std::string),
    typeid(TextBox),
    PropertyMetadata{
        .defaultValue = std::string(""),
        .bindingOptions = BindingOptions{
            .defaultMode = BindingMode::TwoWay,
            .updateSourceTrigger = UpdateSourceTrigger::PropertyChanged
        }
    }
);
```

## 测试策略

### 单元测试

1. **属性注册测试**
   - 测试普通属性注册
   - 测试附加属性注册
   - 测试重复注册的行为

2. **元数据测试**
   - 测试默认值读取
   - 测试回调触发
   - 测试验证器功能

3. **类型安全测试**
   - 测试类型不匹配时的行为
   - 测试类型转换

### 集成测试

1. **与 DependencyObject 集成**
   - 测试属性值的设置和获取
   - 测试属性变更通知

2. **与绑定系统集成**
   - 测试不同绑定模式
   - 测试更新触发器

### 性能测试

1. **注册性能**：测试大量属性注册的时间
2. **查找性能**：测试属性ID查找的速度
3. **回调性能**：测试回调函数的调用开销

## 相关文档

- [DependencyObject 设计文档](./DependencyObject.md) - 依赖对象基类
- [Binding 设计文档](./Binding.md) - 数据绑定系统
- [PropertyStore 设计文档](./PropertyStore.md) - 属性值存储
- [DependencyProperty API文档](../../API/Binding/DependencyProperty.md) - API参考

## 总结

`DependencyProperty` 是整个属性系统的核心，提供了强大而灵活的属性注册和管理机制。当前实现已经覆盖了基本功能，但在属性优先级、只读属性和元数据覆盖等方面还有扩展空间。通过合理使用 `std::type_index` 和 `std::any`，在保持类型安全的同时实现了高度的灵活性。
