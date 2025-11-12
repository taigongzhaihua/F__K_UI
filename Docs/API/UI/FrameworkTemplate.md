# FrameworkTemplate

## 概览

**目的**：模板系统基类

**命名空间**：`fk::ui`

**头文件**：`fk/ui/FrameworkTemplate.h`

## 描述

`FrameworkTemplate` 是所有模板的基类。

## 派生类

- [ControlTemplate](ControlTemplate.md) - 控件模板
- [DataTemplate](DataTemplate.md) - 数据模板

## 公共接口

### 应用模板

#### LoadContent
```cpp
virtual std::shared_ptr<UIElement> LoadContent() = 0;
```

## 相关类

- [Control](Control.md)
