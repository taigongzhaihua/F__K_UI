# ValueConverters

## 概览

**目的**：内置值转换器集合

**命名空间**：`fk::binding`

**头文件**：`fk/binding/ValueConverters.h`

## 描述

提供常用的值转换器实现。

## 内置转换器

### BooleanToVisibilityConverter
将布尔值转换为Visibility枚举。

```cpp
true  -> Visibility::Visible
false -> Visibility::Collapsed
```

### BooleanInverterConverter
反转布尔值。

```cpp
true  -> false
false -> true
```

### StringFormatConverter
格式化字符串。

### NullToVisibilityConverter
将null值转换为Visibility。

## 使用示例

### Boolean到Visibility
```cpp
Binding binding("IsVisible");
binding.Source(viewModel)
       .Converter(std::make_shared<BooleanToVisibilityConverter>());

element->SetValue(UIElement::VisibilityProperty(), binding);
```

### 反转布尔值
```cpp
Binding binding("IsEnabled");
binding.Converter(std::make_shared<BooleanInverterConverter>());

// IsEnabled=true时，绑定目标得到false
```

### 自定义转换器
```cpp
class MyConverter : public IValueConverter {
public:
    std::any Convert(const std::any& value, ...) override {
        // 转换逻辑
        return convertedValue;
    }
    
    std::any ConvertBack(const std::any& value, ...) override {
        // 反向转换
        return originalValue;
    }
};
```

## 相关类

- [IValueConverter](IValueConverter.md)
- [Binding](Binding.md)
