# DataTemplate

## 概览

**目的**：定义数据对象的可视化表示

**命名空间**：`fk::ui`

**头文件**：`fk/ui/DataTemplate.h`

## 描述

`DataTemplate` 定义如何显示数据对象。

## 公共接口

### 设置模板

#### SetVisualTree
```cpp
void SetVisualTree(std::function<std::shared_ptr<UIElement>()> factory);
```

设置创建视觉树的工厂函数。

## 使用示例

### 项目模板
```cpp
auto itemTemplate = std::make_shared<DataTemplate>();

itemTemplate->SetVisualTree([]() {
    auto stack = std::make_shared<StackPanel>();
    stack->Orientation(Orientation::Horizontal)
         ->Spacing(10);
    
    auto image = stack->AddChild<Image>();
    image->Width(32)->Height(32)
         ->SetValue(Image::SourceProperty(), Binding("IconPath"));
    
    auto text = stack->AddChild<TextBlock>();
    text->SetValue(TextBlock::TextProperty(), Binding("Name"));
    
    return stack;
});

listBox->SetValue(ListBox::ItemTemplateProperty(), itemTemplate);
```

## 相关类

- [ControlTemplate](ControlTemplate.md)
- [ContentPresenter](ContentPresenter.md)
- [ItemsControl](ItemsControl.md)
