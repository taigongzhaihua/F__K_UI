# ControlTemplate

## 概览

**目的**：定义控件的视觉结构

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ControlTemplate.h`

## 描述

`ControlTemplate` 定义控件的视觉外观，允许完全自定义控件的呈现方式。

## 公共接口

### 设置模板

#### SetVisualTree
```cpp
void SetVisualTree(std::function<std::shared_ptr<UIElement>()> factory);
```

设置创建视觉树的工厂函数。

## 使用示例

### 自定义按钮模板
```cpp
auto template = std::make_shared<ControlTemplate>();

template->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    border->Background(Colors::LightBlue)
          ->BorderBrush(Colors::Blue)
          ->BorderThickness(Thickness(2))
          ->CornerRadius(CornerRadius(5))
          ->Padding(Thickness(10, 5));
    
    auto contentPresenter = std::make_shared<ContentPresenter>();
    border->SetChild(contentPresenter);
    
    return border;
});

button->SetTemplate(template.get());
```

### 使用TemplateBinding
```cpp
template->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    
    // 绑定到模板化父控件的属性
    border->SetValue(
        Border::BackgroundProperty(),
        TemplateBinding(Control::BackgroundProperty())
    );
    
    return border;
});
```

## 相关类

- [Control](Control.md)
- [ContentPresenter](ContentPresenter.md)
- [TemplateBinding](../Binding/TemplateBinding.md)
