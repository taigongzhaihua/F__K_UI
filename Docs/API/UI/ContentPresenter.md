# ContentPresenter

## 概览

**目的**：在ControlTemplate中显示Content

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ContentPresenter.h`

## 描述

`ContentPresenter` 用于在ControlTemplate内显示ContentControl的内容。

## 公共接口

### 内容

#### Content
```cpp
static const DependencyProperty& ContentProperty();
```

要显示的内容。

#### ContentTemplate
```cpp
static const DependencyProperty& ContentTemplateProperty();
```

内容的数据模板。

## 使用示例

### 在模板中使用
```cpp
auto template = std::make_shared<ControlTemplate>();

template->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    border->Background(Colors::White)
          ->Padding(Thickness(10));
    
    // ContentPresenter显示实际内容
    auto presenter = border->SetChild<ContentPresenter>();
    
    return border;
});

button->SetTemplate(template.get());
```

## 相关类

- [ContentControl](ContentControl.md)
- [ControlTemplate](ControlTemplate.md)
