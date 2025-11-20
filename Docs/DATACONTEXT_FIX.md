# DataContext bad_any_cast 错误修复

## 问题描述

在测试 TemplateBinding 和绑定功能时，发现 `SetDataContext` 后调用 `GetDataContext` 会抛出 `std::bad_any_cast` 异常。

## 根本原因

`FrameworkElement` 类将 `DataContext` 实现为一个 `DependencyProperty`，类型注册为 `typeid(std::any)`：

```cpp
// 原来的实现（错误）
void SetDataContext(const std::any& value) {
    SetValue(DataContextProperty(), value);  // 将 any 包装在 any 中
}

std::any GetDataContext() const {
    return GetValue<std::any>(DataContextProperty());  // 尝试解包导致 bad_any_cast
}
```

问题流程：
1. 用户调用 `window->SetDataContext(shared_ptr<ViewModel>)`
2. `shared_ptr<ViewModel>` 被包装成 `std::any(shared_ptr<ViewModel>)`
3. `SetValue(DataContextProperty(), value)` 将其再次包装成 `std::any(std::any(shared_ptr<ViewModel>))`（双重包装）
4. `GetValue<std::any>()` 尝试将 `std::any(std::any(...))` 转换为 `std::any` 类型时失败，抛出 `bad_any_cast`

## 解决方案

### 1. 移除 DataContextProperty 依赖属性

DataContext 不应该作为 DependencyProperty，因为：
- 它有特殊的继承行为（从父元素继承）
- 它需要通过 `BindingContext` 管理以支持数据绑定
- 将 `std::any` 作为依赖属性类型会导致双重包装问题

### 2. 使用 DependencyObject 的 DataContext 实现

`DependencyObject` 基类已经通过 `BindingContext` 提供了正确的 DataContext 管理：

```cpp
// DependencyObject 中的实现
void SetDataContext(std::any value) {
    bindingContext_.SetLocalDataContext(std::move(value));
}

const std::any& GetDataContext() const noexcept {
    return bindingContext_.GetDataContext();
}
```

### 3. 修改 FrameworkElement

**修改前：**
```cpp
void SetDataContext(const std::any& value) {
    SetValue(DataContextProperty(), value);
    InvalidateVisual();
}

std::any GetDataContext() const {
    return GetValue<std::any>(DataContextProperty());
}
```

**修改后：**
```cpp
// 使用 DependencyObject 的实现，添加链式 API 支持
Derived* DataContext(const std::any& value) {
    DependencyObject::SetDataContext(value);
    this->InvalidateVisual();
    return static_cast<Derived*>(this);
}

// 类型安全版本（推荐使用）
template<typename T>
Derived* DataContext(std::shared_ptr<T> value) {
    static_assert(std::is_base_of_v<binding::INotifyPropertyChanged, T>, 
                  "DataContext must be derived from INotifyPropertyChanged");
    DependencyObject::SetDataContext(value);
    this->InvalidateVisual();
    return static_cast<Derived*>(this);
}

const std::any& DataContext() const { 
    return DependencyObject::GetDataContext(); 
}
```

## 修改的文件

1. **include/fk/ui/FrameworkElement.h**
   - 移除 `DataContextProperty()` 声明
   - 修改 `SetDataContext` 和 `GetDataContext` 方法
   - 添加类型安全的模板版本

2. **src/ui/FrameworkElement.cpp**
   - 移除 `DataContextProperty()` 的注册实现

## 类型安全改进

新实现添加了类型检查，确保 DataContext 只能设置为 `INotifyPropertyChanged` 的派生类：

```cpp
template<typename T>
Derived* DataContext(std::shared_ptr<T> value) {
    static_assert(std::is_base_of_v<binding::INotifyPropertyChanged, T>, 
                  "DataContext must be derived from INotifyPropertyChanged");
    // ...
}
```

这样可以：
- 在编译时捕获类型错误
- 确保 DataContext 支持属性变更通知
- 提高代码的类型安全性

## 测试验证

修复后的测试结果：

```
=== 测试 DataContext 设置 ===

[1] 创建 ViewModel
    类型: std::shared_ptr<TestVM>
    值: test

[2] 创建 Window

[3] 设置 DataContext
    ✓ SetDataContext 完成

[4] 获取 BindingContext
    HasDataContext: 是
    HasLocalDataContext: 是

[5] 获取 DataContext
    ✓ GetDataContext 返回
    有值: 是
    类型: std::shared_ptr<TestVM>

[6] 尝试转换回 shared_ptr<TestVM>
    ✓ 直接转换成功
    值: test

=== 测试完成 ===
```

## 后续工作

DataContext 的设置和获取已经修复，但绑定的值传递还需要进一步工作：
- BindingExpression 的激活时机
- DataContext 变更事件的订阅
- 属性路径解析和值传递

这些将在后续提交中解决。
