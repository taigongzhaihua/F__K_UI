# ObservableCollection

## 概览

**目的**：可观察的泛型集合

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ObservableCollection.h`

## 描述

`ObservableCollection<T>` 是可观察的集合，元素变更时发出通知。

## 公共接口

### 集合操作

#### Add / Remove / Clear
```cpp
void Add(const T& item);
void Remove(const T& item);
void Clear();
```

### 事件

#### CollectionChanged
```cpp
core::Event<CollectionChangedEventArgs> CollectionChanged;
```

集合变更事件。

## 使用示例

### 在ItemsControl中使用
```cpp
auto items = std::make_shared<ObservableCollection<std::string>>();

items->CollectionChanged += [](const CollectionChangedEventArgs& e) {
    // 处理变更
};

items->Add("项目1");
items->Add("项目2");

itemsControl->SetValue(ItemsControl::ItemsProperty(), items);
```

## 相关类

- [ItemsControl](ItemsControl.md)
