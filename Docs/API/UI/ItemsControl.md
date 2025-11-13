# ItemsControl

## 概览

**目的**：显示项目集合的控件

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ItemsControl.h`

## 描述

`ItemsControl` 用于显示数据项集合。

## 公共接口

### 项目集合

#### Items
```cpp
static const DependencyProperty& ItemsProperty();
```

项目集合属性。

### 模板

#### ItemTemplate
```cpp
static const DependencyProperty& ItemTemplateProperty();
```

项目数据模板。

#### ItemsPanel
```cpp
static const DependencyProperty& ItemsPanelProperty();
```

承载项目的面板模板。

## 使用示例

### 基本列表
```cpp
auto itemsControl = std::make_shared<ItemsControl<ItemsControl>>();

std::vector<std::string> items = {"项目1", "项目2", "项目3"};
itemsControl->SetValue(ItemsControl::ItemsProperty(), items);
```

### 自定义项目模板
```cpp
auto itemTemplate = std::make_shared<DataTemplate>();
itemTemplate->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    border->Padding(Thickness(5))
          ->BorderBrush(Colors::Gray)
          ->BorderThickness(Thickness(1));
    
    auto textBlock = border->SetChild<TextBlock>();
    textBlock->SetValue(
        TextBlock::TextProperty(),
        Binding()  // 绑定到数据项
    );
    
    return border;
});

itemsControl->SetValue(ItemsControl::ItemTemplateProperty(), itemTemplate);
```

## 相关类

- [ListBox](ListBox.md)
- [DataTemplate](DataTemplate.md)
- [ItemContainerGenerator](ItemContainerGenerator.md)
