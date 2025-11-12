# ItemContainerGenerator

## 概览

**目的**：为ItemsControl生成和管理容器

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ItemContainerGenerator.h`

## 描述

`ItemContainerGenerator` 管理ItemsControl的项目容器。

## 公共接口

### 容器生成

#### GenerateContainer
```cpp
UIElement* GenerateContainer(const std::any& item);
```

### 容器回收

#### RecycleContainer
```cpp
void RecycleContainer(UIElement* container);
```

### 映射

#### ContainerFromItem / ItemFromContainer
```cpp
UIElement* ContainerFromItem(const std::any& item);
std::any ItemFromContainer(UIElement* container);
```

## 使用示例

内部由ItemsControl使用。

## 相关类

- [ItemsControl](ItemsControl.md)
- [ListBox](ListBox.md)
