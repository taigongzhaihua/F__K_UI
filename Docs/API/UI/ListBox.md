# ListBox

## 概览

**目的**：支持选择的列表控件

**命名空间**：`fk::ui`

**继承**：`ItemsControl`

**头文件**：`fk/ui/ListBox.h`

## 描述

`ListBox` 是可选择项目的列表控件，支持单选和多选。

## 公共接口

### 选择

#### SelectedItem / SelectedIndex
```cpp
static const DependencyProperty& SelectedItemProperty();
static const DependencyProperty& SelectedIndexProperty();
```

当前选中的项目和索引。

#### SelectionMode
```cpp
static const DependencyProperty& SelectionModeProperty();
```

选择模式（Single、Multiple、Extended）。

### 事件

#### SelectionChanged
```cpp
core::Event<SelectionChangedEventArgs> SelectionChanged;
```

选择变更事件。

## 使用示例

### 基本列表
```cpp
auto listBox = std::make_shared<ListBox>();

std::vector<std::string> items = {"选项1", "选项2", "选项3"};
listBox->SetValue(ListBox::ItemsProperty(), items);

listBox->SelectionChanged += [](const SelectionChangedEventArgs& e) {
    std::cout << "选择变更" << std::endl;
};
```

### 多选模式
```cpp
listBox->SetValue(
    ListBox::SelectionModeProperty(),
    SelectionMode::Multiple
);
```

## 相关类

- [ItemsControl](ItemsControl.md)
- [ListBoxItem](ListBoxItem.md)
