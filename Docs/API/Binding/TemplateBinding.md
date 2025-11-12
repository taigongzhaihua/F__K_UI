# TemplateBinding

## 概览

**目的**：模板内绑定到模板化父控件的属性

**命名空间**：`fk::binding`

**头文件**：`fk/binding/TemplateBinding.h`

## 描述

`TemplateBinding` 是一种特殊的绑定，在ControlTemplate内绑定到模板化父控件的属性。

## 使用示例

### 在模板中使用
```cpp
auto template = std::make_shared<ControlTemplate>();

template->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    
    // 绑定到父控件的Background属性
    border->SetValue(
        Border::BackgroundProperty(),
        TemplateBinding(Control::BackgroundProperty())
    );
    
    return border;
});
```

## 相关类

- [Binding](Binding.md)
- [ControlTemplate](../UI/ControlTemplate.md)
