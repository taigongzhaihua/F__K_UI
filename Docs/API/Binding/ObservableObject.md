# ObservableObject

## 概览

**目的**：ViewModel基类，实现INotifyPropertyChanged接口

**命名空间**：`fk::binding`

**继承**：`DependencyObject`

**头文件**：`fk/binding/ObservableObject.h`

## 描述

`ObservableObject` 是创建ViewModel的便利基类，自动实现属性变更通知。

## 公共接口

### 属性宏

#### PROPERTY
```cpp
PROPERTY(Type, Name, DefaultValue)
```

定义可观察属性。

**示例**：
```cpp
class MyViewModel : public ObservableObject {
public:
    PROPERTY(std::string, Username, "")
    PROPERTY(int, Age, 0)
    PROPERTY(bool, IsEnabled, true)
};
```

### 属性变更通知

#### PropertyChanged
```cpp
core::Event<const std::string&> PropertyChanged;
```

属性变更时触发的事件。

## 使用示例

### 创建ViewModel
```cpp
class UserViewModel : public ObservableObject {
public:
    PROPERTY(std::string, FirstName, "")
    PROPERTY(std::string, LastName, "")
    PROPERTY(int, Age, 0)
    
    std::string GetFullName() const {
        return GetFirstName() + " " + GetLastName();
    }
};

auto viewModel = std::make_shared<UserViewModel>();
viewModel->SetFirstName("张");
viewModel->SetLastName("三");
```

### 绑定到UI
```cpp
auto textBox = std::make_shared<TextBox>();
textBox->SetValue(
    TextBox::TextProperty(),
    Binding("FirstName").Source(viewModel).Mode(BindingMode::TwoWay)
);

// UI更新会自动同步到ViewModel
```

### 订阅变更
```cpp
viewModel->PropertyChanged += [](const std::string& propertyName) {
    std::cout << "属性变更: " << propertyName << std::endl;
};

viewModel->SetAge(25);  // 触发PropertyChanged
```

## 相关类

- [INotifyPropertyChanged](INotifyPropertyChanged.md)
- [Binding](Binding.md)

## 另请参阅

- [设计文档](../../Design/Binding/ObservableObject.md)
