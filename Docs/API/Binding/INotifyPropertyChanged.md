# INotifyPropertyChanged

## 概览

**目的**：属性变更通知接口

**命名空间**：`fk::binding`

**头文件**：`fk/binding/INotifyPropertyChanged.h`

## 描述

`INotifyPropertyChanged` 是一个接口，实现此接口的类可以在属性值变更时通知绑定系统。

## 公共接口

### 事件

#### PropertyChanged
```cpp
virtual core::Event<const std::string&>& PropertyChanged() = 0;
```

属性变更时触发的事件，参数为属性名称。

## 使用示例

### 实现接口
```cpp
class MyClass : public INotifyPropertyChanged {
public:
    core::Event<const std::string&>& PropertyChanged() override {
        return propertyChanged_;
    }
    
    void SetName(const std::string& name) {
        if (name_ != name) {
            name_ = name;
            propertyChanged_.Invoke("Name");
        }
    }
    
private:
    std::string name_;
    core::Event<const std::string&> propertyChanged_;
};
```

### 在绑定中使用
```cpp
auto obj = std::make_shared<MyClass>();
element->SetValue(property, Binding("Name").Source(obj));

// 变更会自动更新UI
obj->SetName("新值");
```

## 相关类

- [ObservableObject](ObservableObject.md)
- [Binding](Binding.md)
- [BindingExpression](BindingExpression.md)
