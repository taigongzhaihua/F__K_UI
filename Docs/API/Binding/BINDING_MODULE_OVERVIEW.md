# Binding 模块类梳理

## 概述

Binding 模块是 F__K_UI 框架的数据绑定系统核心，提供了类似 WPF 的数据绑定功能。该模块包含依赖属性系统、数据绑定机制、属性变更通知、值转换和验证等功能。

## 模块位置

- **头文件目录**：`include/fk/binding/`
- **源文件目录**：`src/binding/`
- **命名空间**：`fk::binding`（大部分类在 `fk` 命名空间下也有别名）

---

## 类列表

### 1. 核心绑定类

| 类名 | 头文件 | 源文件 | 描述 |
|------|--------|--------|------|
| `Binding` | `Binding.h` | `Binding.cpp` | 数据绑定配置类，支持路径、源、模式、转换器等配置 |
| `BindingExpression` | `BindingExpression.h` | `BindingExpression.cpp` | 活动绑定实例，管理绑定的激活、更新和订阅 |
| `BindingContext` | `BindingContext.h` | `BindingContext.cpp` | 绑定上下文，管理 DataContext 的继承和变更通知 |
| `BindingPath` | `BindingPath.h` | `BindingPath.cpp` | 属性路径解析器，支持嵌套属性和索引访问 |

### 2. 依赖属性系统

| 类名 | 头文件 | 源文件 | 描述 |
|------|--------|--------|------|
| `DependencyObject` | `DependencyObject.h` | `DependencyObject.cpp` | 依赖属性系统基类，支持属性存储、绑定和数据上下文 |
| `DependencyProperty` | `DependencyProperty.h` | `DependencyProperty.cpp` | 依赖属性元数据和注册，支持普通属性和附加属性 |
| `PropertyStore` | `PropertyStore.h` | `PropertyStore.cpp` | 属性值存储，管理多层级属性值（本地、绑定、样式、继承） |

### 3. 多绑定

| 类名 | 头文件 | 源文件 | 描述 |
|------|--------|--------|------|
| `MultiBinding` | `MultiBinding.h` | `MultiBinding.cpp` | 多源绑定配置，将多个源属性组合为一个目标值 |
| `MultiBindingExpression` | `MultiBindingExpression.h` | `MultiBindingExpression.cpp` | 多绑定活动实例，管理子绑定并组合值 |

### 4. 模板绑定

| 类名 | 头文件 | 描述 |
|------|--------|------|
| `TemplateBinding` | `TemplateBinding.h` | 模板绑定，用于在 ControlTemplate 中绑定到 TemplatedParent 属性 |

### 5. 属性变更通知

| 类名/接口 | 头文件 | 描述 |
|-----------|--------|------|
| `INotifyPropertyChanged` | `INotifyPropertyChanged.h` | 属性变更通知接口，ViewModel 实现此接口以支持绑定 |
| `ObservableObject` | `ObservableObject.h` | `INotifyPropertyChanged` 的基础实现，ViewModel 基类 |
| `ObservableProperty<T, Owner>` | `ObservableProperty.h` | 可观察属性模板类，自动注册和变更通知 |

### 6. 值转换

| 类名/接口 | 头文件 | 源文件 | 描述 |
|-----------|--------|--------|------|
| `IValueConverter` | `Binding.h` | - | 值转换器接口，支持双向转换 |
| `IMultiValueConverter` | `MultiBinding.h` | - | 多值转换器接口，用于 MultiBinding |
| `DefaultValueConverter` | `ValueConverters.h` | `ValueConverters.cpp` | 默认类型转换器实现 |
| `BooleanToStringConverter` | `ValueConverters.h` | `ValueConverters.cpp` | 布尔值到字符串转换器 |

### 7. 数据验证

| 类名/接口 | 头文件 | 描述 |
|-----------|--------|------|
| `ValidationRule` | `ValidationRule.h` | 验证规则基类 |
| `ValidationResult` | `ValidationRule.h` | 验证结果结构体 |
| `FunctionValidationRule` | `ValidationRule.h` | 使用自定义函数的验证规则 |
| `NotEmptyValidationRule` | `ValidationRule.h` | 非空验证规则 |
| `RangeValidationRule<T>` | `ValidationRule.h` | 数值范围验证规则模板 |
| `StringLengthValidationRule` | `ValidationRule.h` | 字符串长度验证规则 |
| `EmailValidationRule` | `ValidationRule.h` | 邮箱格式验证规则 |
| `INotifyDataErrorInfo` | `INotifyDataErrorInfo.h` | 数据错误通知接口 |

### 8. 辅助类

| 类名 | 头文件 | 描述 |
|------|--------|------|
| `PropertyAccessorRegistry` | `BindingPath.h` | 属性访问器注册表，支持字符串路径绑定 |
| `RelativeSource` | `Binding.h` | 相对源配置，用于 FindAncestor 和 Self 绑定 |

---

## 枚举类型

### BindingMode（绑定模式）
**定义位置**：`DependencyProperty.h`

| 值 | 描述 |
|----|------|
| `OneTime` | 一次性绑定，初始化后不再更新 |
| `OneWay` | 单向绑定，源变更时更新目标 |
| `TwoWay` | 双向绑定，源和目标互相同步 |
| `OneWayToSource` | 反向单向绑定，目标变更时更新源 |

### UpdateSourceTrigger（更新源触发器）
**定义位置**：`DependencyProperty.h`

| 值 | 描述 |
|----|------|
| `Default` | 使用属性默认设置 |
| `PropertyChanged` | 属性变更时立即更新源 |
| `LostFocus` | 失去焦点时更新源 |
| `Explicit` | 显式调用 `UpdateSource()` 时更新 |

### ValueSource（值来源）
**定义位置**：`PropertyStore.h`

| 值 | 描述 |
|----|------|
| `Default` | 默认值 |
| `Inherited` | 继承值 |
| `Style` | 样式设置值 |
| `Binding` | 绑定值 |
| `Local` | 本地设置值（优先级最高） |

### RelativeSourceMode（相对源模式）
**定义位置**：`Binding.h`

| 值 | 描述 |
|----|------|
| `Self` | 绑定到自身元素 |
| `FindAncestor` | 在可视树中查找指定类型的祖先元素 |

---

## 结构体

### PropertyMetadata
**定义位置**：`DependencyProperty.h`

依赖属性的元数据：
- `defaultValue` - 默认值
- `propertyChangedCallback` - 属性变更回调
- `validateCallback` - 值验证回调
- `bindingOptions` - 绑定选项

### BindingOptions
**定义位置**：`DependencyProperty.h`

绑定选项：
- `defaultMode` - 默认绑定模式
- `updateSourceTrigger` - 默认更新触发器
- `inheritsDataContext` - 是否继承数据上下文

### ValidationResult
**定义位置**：`ValidationRule.h`

验证结果：
- `isValid` - 是否验证通过
- `errorMessage` - 错误消息

### BindingPath::Segment
**定义位置**：`BindingPath.h`

路径段：
- `kind` - 段类型（Property 或 Index）
- `name` - 属性名（Property 类型）
- `index` - 索引值（Index 类型）

---

## 宏定义

### 属性访问器注册宏
**定义位置**：`PropertyAccessors.h`

| 宏 | 描述 |
|----|------|
| `FK_BINDING_REGISTER_GETTER` | 注册只读属性访问器 |
| `FK_BINDING_REGISTER_SETTER` | 注册只写属性访问器 |
| `FK_BINDING_REGISTER_PROPERTY` | 注册读写属性访问器 |

### ViewModel 宏
**定义位置**：`ViewModelMacros.h`

| 宏 | 描述 |
|----|------|
| `FK_VIEWMODEL_BEGIN` | 开始 ViewModel 属性注册块 |
| `FK_VIEWMODEL_PROPERTY` | 注册只读属性 |
| `FK_VIEWMODEL_PROPERTY_RW` | 注册可读写属性 |
| `FK_VIEWMODEL_END` | 结束 ViewModel 属性注册块 |
| `FK_VIEWMODEL_AUTO` | 自动注册多个属性（简化版） |
| `FK_PROPERTY_READONLY` | 声明只读属性（自动生成代码） |
| `FK_PROPERTY` | 声明可读写属性（自动生成代码） |
| `FK_PROPERTY_CUSTOM` | 声明自定义 setter 的属性 |

---

## 类型别名（Type Aliases）

以下类型在 `fk` 命名空间下有别名：

```cpp
namespace fk {
    using binding::Binding;
    using binding::BindingExpression;
    using binding::BindingContext;
    using binding::BindingPath;
    using binding::BindingMode;
    using binding::UpdateSourceTrigger;
    using binding::BindingOptions;
    using binding::DependencyObject;
    using binding::DependencyProperty;
    using binding::PropertyMetadata;
    using binding::PropertyChangedCallback;
    using binding::ValidateValueCallback;
    using binding::PropertyStore;
    using binding::ValueSource;
    using binding::IValueConverter;
    using binding::IMultiValueConverter;
    using binding::MultiBinding;
    using binding::MultiBindingExpression;
    using binding::ValidationRule;
    using binding::ValidationResult;
    using binding::RelativeSource;
    using binding::RelativeSourceMode;
    using binding::INotifyPropertyChanged;
    using binding::INotifyDataErrorInfo;
    using binding::ObservableObject;
    using binding::ObservableProperty;
    using binding::PropertyAccessorRegistry;
    // 验证规则类
    using binding::FunctionValidationRule;
    using binding::NotEmptyValidationRule;
    using binding::RangeValidationRule;
    using binding::StringLengthValidationRule;
    using binding::EmailValidationRule;
}
```

---

## 回调类型

| 类型 | 定义位置 | 描述 |
|------|----------|------|
| `PropertyChangedCallback` | `DependencyProperty.h` | 属性变更回调 |
| `ValidateValueCallback` | `DependencyProperty.h` | 值验证回调 |
| `PropertyStore::ValueChangedCallback` | `PropertyStore.h` | 存储值变更回调 |
| `PropertyAccessorRegistry::Accessor::Getter` | `BindingPath.h` | 属性 getter 函数 |
| `PropertyAccessorRegistry::Accessor::Setter` | `BindingPath.h` | 属性 setter 函数 |

---

## 辅助函数

### bind()
**定义位置**：`Binding.h`

便捷的绑定创建函数：
```cpp
inline Binding bind(std::string path);
```

### TryDefaultConvert()
**定义位置**：`ValueConverters.h`

尝试使用框架内置规则进行类型转换：
```cpp
bool TryDefaultConvert(const std::any& value, std::type_index targetType, std::any& outResult);
```

---

## 类关系图

```
INotifyPropertyChanged (接口)
    └── ObservableObject (基础实现)

INotifyDataErrorInfo (接口)

IValueConverter (接口)
    └── DefaultValueConverter
    └── BooleanToStringConverter

IMultiValueConverter (接口)

ValidationRule (基类)
    ├── FunctionValidationRule
    ├── NotEmptyValidationRule
    ├── RangeValidationRule<T>
    ├── StringLengthValidationRule
    └── EmailValidationRule

Binding (基类)
    └── TemplateBinding

DependencyObject
    ├── BindingContext (内部使用)
    └── PropertyStore (内部使用)

DependencyProperty
    └── PropertyMetadata

BindingExpression
    └── 使用 Binding, BindingPath, BindingContext

MultiBinding
    └── MultiBindingExpression

BindingPath
    └── PropertyAccessorRegistry
```

---

## 文件索引

| 头文件 | 主要内容 |
|--------|----------|
| `Binding.h` | Binding, IValueConverter, RelativeSource, RelativeSourceMode |
| `BindingContext.h` | BindingContext |
| `BindingExpression.h` | BindingExpression |
| `BindingPath.h` | BindingPath, PropertyAccessorRegistry |
| `DependencyObject.h` | DependencyObject |
| `DependencyProperty.h` | DependencyProperty, PropertyMetadata, BindingMode, UpdateSourceTrigger, BindingOptions |
| `INotifyDataErrorInfo.h` | INotifyDataErrorInfo |
| `INotifyPropertyChanged.h` | INotifyPropertyChanged |
| `MultiBinding.h` | MultiBinding, IMultiValueConverter |
| `MultiBindingExpression.h` | MultiBindingExpression |
| `ObservableObject.h` | ObservableObject |
| `ObservableProperty.h` | ObservableProperty<T, Owner> |
| `PropertyAccessors.h` | 属性访问器注册宏 |
| `PropertyStore.h` | PropertyStore, ValueSource |
| `TemplateBinding.h` | TemplateBinding |
| `ValidationRule.h` | ValidationRule, ValidationResult, 各种验证规则实现 |
| `ValueConverters.h` | TryDefaultConvert, DefaultValueConverter, BooleanToStringConverter |
| `ViewModelMacros.h` | ViewModel 宏定义 |

---

## 下一步

此文档完成了 binding 模块的类梳理工作。接下来可以：

1. 检查现有文档是否覆盖所有类
2. 按照 Application.md 的格式重构各类的详细文档
3. 补充缺失的类文档（如 ObservableProperty, PropertyAccessors 等）
4. 更新 API/README.md 的索引
