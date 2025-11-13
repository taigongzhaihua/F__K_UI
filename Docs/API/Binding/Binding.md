# Binding

## 概览

**目的**：配置属性和数据源之间的数据绑定

**命名空间**：`fk::binding`

**头文件**：`fk/binding/Binding.h`

## 描述

`Binding` 类配置数据绑定，指定源对象、属性路径、绑定模式、转换器等。它是数据绑定系统的配置对象。

## 公共接口

### 构造函数

```cpp
Binding();
explicit Binding(const std::string& path);
```

创建绑定配置。

**示例**：
```cpp
Binding binding("PropertyName");
```

### 属性路径

#### Path
```cpp
Binding& Path(const std::string& path);
const std::string& Path() const;
```

设置要绑定的属性路径。

**示例**：
```cpp
binding.Path("User.Name");  // 嵌套属性
```

### 绑定源

#### Source
```cpp
Binding& Source(std::shared_ptr<DependencyObject> source);
std::shared_ptr<DependencyObject> Source() const;
```

设置数据源对象。

**示例**：
```cpp
auto viewModel = std::make_shared<ViewModel>();
binding.Source(viewModel);
```

### 绑定模式

#### Mode
```cpp
Binding& Mode(BindingMode mode);
BindingMode Mode() const;
```

设置绑定模式：
- `OneWay` - 从源到目标的单向绑定
- `TwoWay` - 双向绑定
- `OneTime` - 一次性绑定
- `OneWayToSource` - 从目标到源

**示例**：
```cpp
binding.Mode(BindingMode::TwoWay);
```

### 值转换器

#### Converter
```cpp
Binding& Converter(std::shared_ptr<IValueConverter> converter);
std::shared_ptr<IValueConverter> Converter() const;
```

设置值转换器，在绑定时转换值。

**示例**：
```cpp
binding.Converter(std::make_shared<BoolToVisibilityConverter>());
```

### 更新触发

#### UpdateSourceTrigger
```cpp
Binding& UpdateSourceTrigger(UpdateSourceTrigger trigger);
```

设置何时更新源：
- `PropertyChanged` - 属性变更时立即更新
- `LostFocus` - 失去焦点时更新
- `Explicit` - 显式调用更新

### 其他选项

#### FallbackValue / TargetNullValue
```cpp
Binding& FallbackValue(const std::any& value);
Binding& TargetNullValue(const std::any& value);
```

设置回退值和空值替代。

## 使用示例

### 基本绑定
```cpp
// 创建绑定
Binding binding("Username");
binding.Source(viewModel);

// 应用到UI元素
textBox->SetValue(TextBox::TextProperty(), binding);
```

### 双向绑定
```cpp
Binding binding("Age");
binding.Source(viewModel)
       .Mode(BindingMode::TwoWay);

textBox->SetValue(TextBox::TextProperty(), binding);
```

### 带转换器的绑定
```cpp
class BoolToStringConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, ...) override {
        return std::any_cast<bool>(value) ? "是" : "否";
    }
    
    std::any ConvertBack(const std::any& value, ...) override {
        return std::any_cast<std::string>(value) == "是";
    }
};

Binding binding("IsEnabled");
binding.Source(viewModel)
       .Converter(std::make_shared<BoolToStringConverter>());

textBlock->SetValue(TextBlock::TextProperty(), binding);
```

### 使用 DataContext
```cpp
// 设置父元素的DataContext
window->SetValue(FrameworkElement::DataContextProperty(), viewModel);

// 子元素自动使用继承的DataContext
textBox->SetValue(TextBox::TextProperty(), Binding("Username"));
```

### 嵌套属性路径
```cpp
Binding binding("User.Address.City");
binding.Source(viewModel);
textBlock->SetValue(TextBlock::TextProperty(), binding);
```

## 相关类

- [BindingExpression](BindingExpression.md) - 活动绑定实例
- [IValueConverter](IValueConverter.md) - 值转换器接口
- [DependencyObject](DependencyObject.md) - 绑定目标基类

## 另请参阅

- [设计文档](../../Design/Binding/Binding.md)
