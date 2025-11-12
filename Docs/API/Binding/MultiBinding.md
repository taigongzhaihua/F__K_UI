# MultiBinding

## 概览

**目的**：多源数据绑定配置

**命名空间**：`fk::binding`

**头文件**：`fk/binding/MultiBinding.h`

## 描述

`MultiBinding` 允许将多个源属性绑定到单个目标属性，通过转换器合并值。

## 公共接口

### 添加绑定

#### AddBinding
```cpp
MultiBinding& AddBinding(const Binding& binding);
```

添加一个绑定源。

### 转换器

#### Converter
```cpp
MultiBinding& Converter(std::shared_ptr<IMultiValueConverter> converter);
```

设置多值转换器。

## 使用示例

### 组合多个属性
```cpp
class FullNameConverter : public IMultiValueConverter {
public:
    std::any Convert(const std::vector<std::any>& values, ...) override {
        auto firstName = std::any_cast<std::string>(values[0]);
        auto lastName = std::any_cast<std::string>(values[1]);
        return firstName + " " + lastName;
    }
};

MultiBinding multiBinding;
multiBinding.AddBinding(Binding("FirstName"))
            .AddBinding(Binding("LastName"))
            .Converter(std::make_shared<FullNameConverter>());

textBlock->SetValue(TextBlock::TextProperty(), multiBinding);
```

## 相关类

- [Binding](Binding.md)
- [IMultiValueConverter](IMultiValueConverter.md)
