# MultiBindingExpression

## 概述

`MultiBindingExpression` 表示一个活动的多源绑定实例，负责管理多个源属性到单个目标属性的数据流。它是 `MultiBinding` 的运行时表示，处理多个源值的聚合、转换和更新。

## 命名空间

```cpp
namespace fk::binding
```

## 继承关系

```
Object
  └─ BindingExpressionBase
       └─ MultiBindingExpression
```

## 主要功能

### 1. 多源管理

管理多个绑定表达式：

```cpp
class MultiBindingExpression : public BindingExpressionBase {
public:
    // 获取子绑定表达式
    const std::vector<std::shared_ptr<BindingExpression>>& GetBindingExpressions() const;
    
    // 子绑定数量
    size_t GetBindingCount() const;
};
```

**使用示例：**

```cpp
auto multiBinding = std::make_shared<MultiBinding>();
multiBinding->AddBinding(binding1);
multiBinding->AddBinding(binding2);

auto expression = multiBinding->CreateExpression(target, property);
auto multiExpr = std::dynamic_pointer_cast<MultiBindingExpression>(expression);

// 访问各个子绑定
for (const auto& childExpr : multiExpr->GetBindingExpressions()) {
    std::cout << "子绑定状态: " << childExpr->GetStatus() << std::endl;
}
```

### 2. 值聚合

聚合多个源值：

```cpp
// 收集所有源值
std::vector<std::any> GetSourceValues() const;

// 检查是否所有源都有效
bool AreAllSourcesValid() const;
```

**使用示例：**

```cpp
// 假设有三个源绑定到 Person 的不同属性
auto fullNameBinding = std::make_shared<MultiBinding>();
fullNameBinding->AddBinding(firstNameBinding);
fullNameBinding->AddBinding(middleNameBinding);
fullNameBinding->AddBinding(lastNameBinding);

// 设置转换器来组合值
fullNameBinding->SetConverter(std::make_shared<NameCombinerConverter>());

// 创建表达式
auto expression = fullNameBinding->CreateExpression(textBlock, TextBlock::TextProperty);
auto multiExpr = std::dynamic_pointer_cast<MultiBindingExpression>(expression);

// 检查源值
if (multiExpr->AreAllSourcesValid()) {
    auto values = multiExpr->GetSourceValues();
    // values[0] = "John", values[1] = "F", values[2] = "Kennedy"
}
```

### 3. 转换器应用

应用多值转换器：

```cpp
// 转换所有源值到目标值
std::any ConvertValues(const std::vector<std::any>& values);

// 反向转换（如果支持）
std::vector<std::any> ConvertBackValue(const std::any& value);
```

**使用示例：**

```cpp
// 自定义多值转换器
class StringFormatConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                    const std::type_info& targetType) override {
        if (values.size() >= 2) {
            auto name = std::any_cast<std::string>(values[0]);
            auto age = std::any_cast<int>(values[1]);
            return name + " (" + std::to_string(age) + " 岁)";
        }
        return std::string("");
    }
};

auto binding = std::make_shared<MultiBinding>();
binding->AddBinding(nameBinding);
binding->AddBinding(ageBinding);
binding->SetConverter(std::make_shared<StringFormatConverter>());

// MultiBindingExpression 会自动应用转换器
```

### 4. 更新管理

处理源更新：

```cpp
// 当任何源值改变时调用
void OnSourceValueChanged(BindingExpression* sourceExpression);

// 更新目标
void UpdateTarget();

// 更新所有源（双向绑定）
void UpdateSources();
```

**使用示例：**

```cpp
// 任何源属性改变时，MultiBindingExpression 会：
// 1. 收集所有当前源值
// 2. 应用转换器
// 3. 更新目标属性

person->SetFirstName("Jane");  // 触发更新
// MultiBindingExpression 自动：
// - 收集 ["Jane", "F", "Kennedy"]
// - 转换为 "Jane F Kennedy"
// - 更新 TextBlock.Text
```

### 5. 状态跟踪

跟踪绑定状态：

```cpp
enum class BindingStatus {
    Unattached,      // 未附加
    Inactive,        // 非活动
    Active,          // 活动
    Detached,        // 已分离
    AsyncRequestPending  // 异步请求挂起
};

BindingStatus GetStatus() const;
bool IsActive() const;
```

**使用示例：**

```cpp
auto expression = multiBinding->CreateExpression(target, property);
auto multiExpr = std::dynamic_pointer_cast<MultiBindingExpression>(expression);

switch (multiExpr->GetStatus()) {
    case BindingStatus::Active:
        std::cout << "多源绑定活动中" << std::endl;
        break;
    case BindingStatus::Inactive:
        std::cout << "多源绑定非活动" << std::endl;
        break;
    default:
        break;
}
```

## 完整示例

### 示例 1：格式化显示

```cpp
// ViewModel
class PersonViewModel : public ObservableObject {
private:
    std::string firstName_;
    std::string lastName_;
    int age_;
    
public:
    // 属性定义...
    NOTIFY_PROPERTY(std::string, FirstName, firstName_)
    NOTIFY_PROPERTY(std::string, LastName, lastName_)
    NOTIFY_PROPERTY(int, Age, age_)
};

// 创建多源绑定
auto multiBinding = std::make_shared<MultiBinding>();

// 添加三个源绑定
auto firstNameBinding = std::make_shared<Binding>("FirstName");
auto lastNameBinding = std::make_shared<Binding>("LastName");
auto ageBinding = std::make_shared<Binding>("Age");

multiBinding->AddBinding(firstNameBinding);
multiBinding->AddBinding(lastNameBinding);
multiBinding->AddBinding(ageBinding);

// 设置格式化转换器
class PersonInfoConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                    const std::type_info& targetType) override {
        if (values.size() >= 3) {
            auto first = std::any_cast<std::string>(values[0]);
            auto last = std::any_cast<std::string>(values[1]);
            auto age = std::any_cast<int>(values[2]);
            return first + " " + last + ", " + std::to_string(age) + " 岁";
        }
        return std::string("未知");
    }
};

multiBinding->SetConverter(std::make_shared<PersonInfoConverter>());

// 创建表达式
auto textBlock = std::make_shared<TextBlock>();
auto expression = multiBinding->CreateExpression(textBlock, TextBlock::TextProperty);

// 设置数据上下文
auto viewModel = std::make_shared<PersonViewModel>();
viewModel->SetFirstName("张");
viewModel->SetLastName("三");
viewModel->SetAge(25);

textBlock->SetDataContext(viewModel);

// 结果：TextBlock 显示 "张 三, 25 岁"
```

### 示例 2：计算绑定

```cpp
// 计算总价转换器
class TotalPriceConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                    const std::type_info& targetType) override {
        if (values.size() >= 2) {
            auto price = std::any_cast<double>(values[0]);
            auto quantity = std::any_cast<int>(values[1]);
            return price * quantity;
        }
        return 0.0;
    }
};

// 创建绑定
auto multiBinding = std::make_shared<MultiBinding>();
multiBinding->AddBinding(std::make_shared<Binding>("Price"));
multiBinding->AddBinding(std::make_shared<Binding>("Quantity"));
multiBinding->SetConverter(std::make_shared<TotalPriceConverter>());

// 绑定到文本显示
auto totalText = std::make_shared<TextBlock>();
multiBinding->CreateExpression(totalText, TextBlock::TextProperty);

// 当 Price 或 Quantity 改变时，Total 自动更新
```

### 示例 3：可见性控制

```cpp
// 多条件可见性转换器
class VisibilityConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values,
                    const std::type_info& targetType) override {
        // 所有条件都为 true 时才显示
        bool allTrue = true;
        for (const auto& value : values) {
            if (!std::any_cast<bool>(value)) {
                allTrue = false;
                break;
            }
        }
        return allTrue ? Visibility::Visible : Visibility::Collapsed;
    }
};

auto multiBinding = std::make_shared<MultiBinding>();
multiBinding->AddBinding(std::make_shared<Binding>("IsLoggedIn"));
multiBinding->AddBinding(std::make_shared<Binding>("HasPermission"));
multiBinding->AddBinding(std::make_shared<Binding>("IsEnabled"));
multiBinding->SetConverter(std::make_shared<VisibilityConverter>());

// 只有三个条件都满足时才显示按钮
auto button = std::make_shared<Button>();
multiBinding->CreateExpression(button, UIElement::VisibilityProperty);
```

## 性能考虑

1. **延迟更新**：聚合多个源的更新，避免过多的中间更新
2. **缓存策略**：缓存转换结果，避免重复计算
3. **智能失效**：只在必要时重新计算值
4. **内存管理**：正确管理子表达式的生命周期

## 最佳实践

1. **转换器设计**：转换器应该是无状态的纯函数
2. **错误处理**：处理源值缺失或类型不匹配的情况
3. **性能优化**：对于复杂计算，考虑使用缓存
4. **调试支持**：提供清晰的错误信息和状态跟踪

## 相关类

- `MultiBinding` - 多源绑定配置
- `BindingExpression` - 单源绑定表达式
- `IMultiValueConverter` - 多值转换器接口
- `BindingExpressionBase` - 绑定表达式基类

## 另请参阅

- [MultiBinding](MultiBinding.md) - 多源绑定配置
- [BindingExpression](BindingExpression.md) - 单源绑定表达式
- [ValueConverters](ValueConverters.md) - 值转换器
- [Binding](Binding.md) - 数据绑定
