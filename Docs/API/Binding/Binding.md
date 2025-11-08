# Binding - 数据绑定

数据绑定机制，连接 UI 元素和数据源，实现自动同步。

## 概述

`Binding` 是框架的数据绑定核心，提供：

- **双向绑定**：UI ↔ 数据自动同步
- **路径表达式**：支持属性路径（如 `"User.Name"`）
- **值转换器**：自动类型转换
- **验证规则**：数据有效性检查
- **相对源**：绑定到自身或祖先元素
- **元素名称绑定**：通过名称引用其他元素

## 基本用法

### 简单绑定

```cpp
#include <fk/binding/Binding.h>

// 创建 ViewModel
class ViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Name);
};

auto vm = std::make_shared<ViewModel>();
vm->SetName("Alice");

// 绑定到 TextBlock
auto text = textBlock();
text->SetBinding(
    TextBlock::TextProperty(),
    Binding().Path("Name").Source(vm)
);

// 修改数据，UI 自动更新
vm->SetName("Bob");  // TextBlock 显示 "Bob"
```

### 使用辅助函数

```cpp
// 简化语法
text->SetBinding(
    TextBlock::TextProperty(),
    bind("Name").Source(vm)
);
```

## 核心概念

### BindingMode - 绑定模式

```cpp
enum class BindingMode {
    OneWay,     // 单向：源 → 目标
    TwoWay,     // 双向：源 ↔ 目标
    OneTime,    // 一次性：源 → 目标（仅初始化）
    OneWayToSource  // 反向单向：目标 → 源
};
```

### UpdateSourceTrigger - 更新触发器

```cpp
enum class UpdateSourceTrigger {
    Default,          // 使用默认策略
    PropertyChanged,  // 属性变化时立即更新
    LostFocus,        // 失去焦点时更新
    Explicit          // 手动调用 UpdateSource
};
```

## 主要方法

### Path - 设置属性路径

```cpp
Binding& Path(std::string path);
```

指定绑定到数据源的哪个属性。

**示例：**

```cpp
// 简单属性
Binding().Path("Name")

// 嵌套属性
Binding().Path("User.Address.City")

// 索引器（未来支持）
Binding().Path("Items[0].Name")
```

### Source - 设置数据源

```cpp
Binding& Source(std::any source);
```

指定数据源对象。

**示例：**

```cpp
auto vm = std::make_shared<ViewModel>();
Binding().Path("Value").Source(vm)

// 或绑定到智能指针
Binding().Path("Text").Source(otherControl)
```

### Mode - 设置绑定模式

```cpp
Binding& Mode(BindingMode mode);
```

指定数据流向。

**示例：**

```cpp
// 单向绑定（默认）
Binding().Path("Label").Mode(BindingMode::OneWay)

// 双向绑定
Binding().Path("Value").Mode(BindingMode::TwoWay)

// 一次性绑定（性能优化）
Binding().Path("InitialText").Mode(BindingMode::OneTime)
```

### ElementName - 绑定到其他元素

```cpp
Binding& ElementName(std::string name);
```

绑定到同一 UI 树中的其他元素。

**示例：**

```cpp
// 源元素
auto slider = stackPanel()
    .SetElementName("MySlider")
    .Value(50);

// 目标元素（绑定到 slider 的 Value）
auto text = textBlock();
text->SetBinding(
    TextBlock::TextProperty(),
    Binding().ElementName("MySlider").Path("Value")
);
```

### SetRelativeSource - 相对源绑定

```cpp
Binding& SetRelativeSource(RelativeSource relativeSource);
```

绑定到相对位置的元素（自身或祖先）。

**示例：**

```cpp
// 绑定到自身
Binding().SetRelativeSource(RelativeSource::Self()).Path("Width")

// 绑定到祖先（向上查找 Grid）
Binding()
    .SetRelativeSource(RelativeSource::FindAncestor(typeid(Grid), 1))
    .Path("ActualWidth")
```

### Converter - 值转换器

```cpp
Binding& Converter(std::shared_ptr<IValueConverter> converter);
Binding& ConverterParameter(std::any parameter);
```

设置值转换器和参数。

**示例：**

```cpp
// 自定义转换器
class BoolToVisibilityConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override {
        bool visible = std::any_cast<bool>(value);
        return visible ? Visibility::Visible : Visibility::Collapsed;
    }
    
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override {
        auto visibility = std::any_cast<Visibility>(value);
        return visibility == Visibility::Visible;
    }
};

// 使用转换器
auto converter = std::make_shared<BoolToVisibilityConverter>();
text->SetBinding(
    TextBlock::VisibilityProperty(),
    Binding()
        .Path("IsEnabled")
        .Converter(converter)
);
```

### AddValidationRule - 添加验证规则

```cpp
Binding& AddValidationRule(std::shared_ptr<ValidationRule> rule);
Binding& ValidatesOnDataErrors(bool enable);
```

添加数据验证。

**示例：**

```cpp
// 自定义验证规则
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(int min, int max) : min_(min), max_(max) {}
    
    ValidationResult Validate(const std::any& value) const override {
        int num = std::any_cast<int>(value);
        if (num < min_ || num > max_) {
            return ValidationResult::Invalid(
                "Value must be between " + std::to_string(min_) + " and " + std::to_string(max_)
            );
        }
        return ValidationResult::Valid();
    }
    
private:
    int min_, max_;
};

// 使用验证
textBox->SetBinding(
    TextBox::TextProperty(),
    Binding()
        .Path("Age")
        .Mode(BindingMode::TwoWay)
        .AddValidationRule(std::make_shared<RangeValidationRule>(0, 120))
        .ValidatesOnDataErrors(true)
);
```

### SetUpdateSourceTrigger - 更新触发器

```cpp
Binding& SetUpdateSourceTrigger(UpdateSourceTrigger trigger);
```

控制何时更新数据源。

**示例：**

```cpp
// 失去焦点时更新（减少频繁更新）
Binding()
    .Path("Email")
    .Mode(BindingMode::TwoWay)
    .SetUpdateSourceTrigger(UpdateSourceTrigger::LostFocus)
```

## 完整示例

### 示例 1：简单双向绑定

```cpp
#include <fk/binding/Binding.h>
#include <fk/binding/ObservableObject.h>
#include <fk/ui/TextBox.h>

class ViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::string, Username);
    FK_PROPERTY(std::string, Password);
};

int main() {
    auto vm = std::make_shared<ViewModel>();
    
    auto usernameBox = textBox();
    usernameBox->SetBinding(
        TextBox::TextProperty(),
        bind("Username")
            .Source(vm)
            .Mode(BindingMode::TwoWay)
    );
    
    auto passwordBox = textBox();
    passwordBox->SetBinding(
        TextBox::TextProperty(),
        bind("Password")
            .Source(vm)
            .Mode(BindingMode::TwoWay)
    );
    
    // 用户输入自动同步到 ViewModel
}
```

### 示例 2：元素间绑定

```cpp
auto slider = sliderControl()
    .SetElementName("VolumeSlider")
    .Minimum(0)
    .Maximum(100)
    .Value(50);

auto label = textBlock();
label->SetBinding(
    TextBlock::TextProperty(),
    bind("Value")
        .ElementName("VolumeSlider")
        .Converter(std::make_shared<IntToStringConverter>())
);

// Slider 滑动，Label 自动显示数值
```

### 示例 3：相对源绑定（绑定到父容器）

```cpp
auto grid = std::make_shared<Grid>();
grid->SetWidth(400);

auto text = textBlock();
text->SetBinding(
    TextBlock::WidthProperty(),
    bind("ActualWidth")
        .SetRelativeSource(RelativeSource::FindAncestor(typeid(Grid), 1))
        .Converter(std::make_shared<HalfWidthConverter>())
);

grid->AddChild(text);
// TextBlock 宽度 = Grid 宽度的一半
```

### 示例 4：列表绑定

```cpp
class TodoViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::vector<std::any>, Items);
};

auto vm = std::make_shared<TodoViewModel>();
vm->SetItems({
    std::string("Task 1"),
    std::string("Task 2"),
    std::string("Task 3")
});

auto list = std::make_shared<ItemsControl>();
list->SetBinding(
    ItemsControl::ItemsSourceProperty(),
    bind("Items").Source(vm)
);

// Items 变化，列表自动更新
```

### 示例 5：多条件验证

```cpp
class EmailValidationRule : public ValidationRule {
public:
    ValidationResult Validate(const std::any& value) const override {
        auto email = std::any_cast<std::string>(value);
        if (email.find('@') == std::string::npos) {
            return ValidationResult::Invalid("Invalid email format");
        }
        return ValidationResult::Valid();
    }
};

class LengthValidationRule : public ValidationRule {
public:
    LengthValidationRule(size_t minLen) : minLen_(minLen) {}
    
    ValidationResult Validate(const std::any& value) const override {
        auto str = std::any_cast<std::string>(value);
        if (str.length() < minLen_) {
            return ValidationResult::Invalid("Too short");
        }
        return ValidationResult::Valid();
    }
    
private:
    size_t minLen_;
};

// 使用多个验证规则
emailBox->SetBinding(
    TextBox::TextProperty(),
    bind("Email")
        .Mode(BindingMode::TwoWay)
        .AddValidationRule(std::make_shared<EmailValidationRule>())
        .AddValidationRule(std::make_shared<LengthValidationRule>(5))
        .ValidatesOnDataErrors(true)
);
```

### 示例 6：主从视图（Master-Detail）

```cpp
class MasterDetailViewModel : public ObservableObject {
public:
    FK_PROPERTY(std::vector<std::any>, Users);
    FK_PROPERTY(std::any, SelectedUser);
};

auto vm = std::make_shared<MasterDetailViewModel>();

// 主列表
auto userList = std::make_shared<ItemsControl>();
userList->SetBinding(
    ItemsControl::ItemsSourceProperty(),
    bind("Users").Source(vm)
);

// 详情面板（绑定到选中用户）
auto nameText = textBlock();
nameText->SetBinding(
    TextBlock::TextProperty(),
    bind("SelectedUser.Name").Source(vm)
);

auto ageText = textBlock();
ageText->SetBinding(
    TextBlock::TextProperty(),
    bind("SelectedUser.Age").Source(vm)
);

// 选中项变化，详情自动更新
```

### 示例 7：条件可见性

```cpp
class FormViewModel : public ObservableObject {
public:
    FK_PROPERTY(bool, IsAdvancedMode);
};

auto vm = std::make_shared<FormViewModel>();

auto advancedPanel = stackPanel();
advancedPanel->SetBinding(
    StackPanel::VisibilityProperty(),
    bind("IsAdvancedMode")
        .Source(vm)
        .Converter(std::make_shared<BoolToVisibilityConverter>())
);

// IsAdvancedMode 变化，面板显示/隐藏
```

### 示例 8：命令参数绑定

```cpp
auto button = button();
button->SetBinding(
    Button::CommandParameterProperty(),
    bind("SelectedItem.Id").Source(vm)
);

button->SetCommand(deleteCommand);
// 点击按钮，传递选中项的 Id 给命令
```

## 绑定表达式

### BindingExpression - 运行时绑定对象

```cpp
class BindingExpression {
public:
    void UpdateTarget();    // 从源更新目标
    void UpdateSource();    // 从目标更新源
    
    ValidationResult Validate();  // 手动验证
};
```

**使用示例：**

```cpp
auto binding = bind("Value").Source(vm).Mode(BindingMode::TwoWay);
textBox->SetBinding(TextBox::TextProperty(), binding);

// 获取绑定表达式
auto expr = textBox->GetBinding(TextBox::TextProperty());

// 手动更新
expr->UpdateTarget();  // 强制从 VM 更新 UI
expr->UpdateSource();  // 强制从 UI 更新 VM

// 手动验证
auto result = expr->Validate();
if (!result.isValid) {
    std::cout << "Error: " << result.errorMessage << std::endl;
}
```

## 最佳实践

### ✅ 推荐做法

**1. 使用 bind 辅助函数**
```cpp
// ✅ 简洁
text->SetBinding(TextBlock::TextProperty(), bind("Name").Source(vm));

// ❌ 冗长
text->SetBinding(TextBlock::TextProperty(), Binding().Path("Name").Source(vm));
```

**2. 为双向绑定设置合适的触发器**
```cpp
// ✅ 文本输入框：失去焦点时更新（减少频繁验证）
textBox->SetBinding(
    TextBox::TextProperty(),
    bind("Email")
        .Mode(BindingMode::TwoWay)
        .SetUpdateSourceTrigger(UpdateSourceTrigger::LostFocus)
);

// ✅ 滑块：属性变化时立即更新
slider->SetBinding(
    Slider::ValueProperty(),
    bind("Volume")
        .Mode(BindingMode::TwoWay)
        .SetUpdateSourceTrigger(UpdateSourceTrigger::PropertyChanged)
);
```

**3. 使用验证规则保证数据有效性**
```cpp
bind("Age")
    .Mode(BindingMode::TwoWay)
    .AddValidationRule(std::make_shared<RangeValidationRule>(0, 150))
    .ValidatesOnDataErrors(true)
```

**4. 只读数据使用 OneTime 模式（性能优化）**
```cpp
// ✅ 初始化后不变的数据
text->SetBinding(
    TextBlock::TextProperty(),
    bind("AppVersion").Mode(BindingMode::OneTime)
);
```

### ❌ 避免的做法

**1. 忘记设置绑定模式**
```cpp
// ❌ 默认是 OneWay，用户输入不会更新数据源
textBox->SetBinding(TextBox::TextProperty(), bind("Username").Source(vm));

// ✅ 明确指定 TwoWay
textBox->SetBinding(
    TextBox::TextProperty(),
    bind("Username").Source(vm).Mode(BindingMode::TwoWay)
);
```

**2. 路径错误（运行时错误）**
```cpp
// ❌ 属性名拼写错误
bind("Nmae")  // 应该是 "Name"

// ✅ 仔细检查属性名
bind("Name")
```

**3. 循环绑定（死循环）**
```cpp
// ❌ A → B, B → A（双向绑定循环）
elementA->SetBinding(Property(), bind("Value").ElementName("B").Mode(TwoWay));
elementB->SetBinding(Property(), bind("Value").ElementName("A").Mode(TwoWay));
```

## 常见问题

**Q: 绑定何时生效？**

A: 调用 `SetBinding` 后立即生效，会立即从数据源获取初始值。

**Q: 如何调试绑定失败？**

A: 检查：
1. 路径是否正确
2. 数据源是否实现了 `INotifyPropertyChanged`
3. 属性是否公开访问
4. 类型是否兼容（需要转换器）

**Q: 绑定性能如何？**

A: 绑定有一定开销，建议：
- 只读数据使用 `OneTime` 模式
- 避免过多绑定（数百个通常没问题）
- 使用合适的 `UpdateSourceTrigger`

**Q: 如何解除绑定？**

A: 调用 `ClearBinding`：
```cpp
element->ClearBinding(Property());
```

**Q: 绑定支持多级路径吗？**

A: 支持，如 `"User.Address.City"`，中间对象需要实现 `INotifyPropertyChanged`。

## 相关类型

- **ObservableObject** - ViewModel 基类，实现属性通知
- **DependencyProperty** - 可绑定的依赖属性
- **ICommand** - 命令绑定
- **IValueConverter** - 值转换器接口
- **ValidationRule** - 验证规则基类

## 总结

`Binding` 是数据绑定的核心，提供：

✅ 单向/双向/一次性绑定  
✅ 属性路径表达式  
✅ 值转换器  
✅ 验证规则  
✅ 元素名称和相对源绑定  
✅ 灵活的更新触发器

实现了 MVVM 模式的数据层和视图层分离。
