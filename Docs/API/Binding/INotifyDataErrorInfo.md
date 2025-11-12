# INotifyDataErrorInfo

## 概览

**目的**：数据验证错误通知接口

**命名空间**：`fk::binding`

**头文件**：`fk/binding/INotifyDataErrorInfo.h`

## 描述

`INotifyDataErrorInfo` 接口用于通知绑定系统验证错误。

## 公共接口

### 错误查询

#### GetErrors
```cpp
virtual std::vector<std::string> GetErrors(const std::string& propertyName) = 0;
```

获取指定属性的错误列表。

#### HasErrors
```cpp
virtual bool HasErrors() const = 0;
```

检查是否有验证错误。

### 事件

#### ErrorsChanged
```cpp
virtual core::Event<const std::string&>& ErrorsChanged() = 0;
```

错误变更事件。

## 使用示例

### 实现接口
```cpp
class MyViewModel : public ObservableObject, public INotifyDataErrorInfo {
public:
    std::vector<std::string> GetErrors(const std::string& propertyName) override {
        auto it = errors_.find(propertyName);
        return (it != errors_.end()) ? it->second : std::vector<std::string>{};
    }
    
    bool HasErrors() const override {
        return !errors_.empty();
    }
    
    core::Event<const std::string&>& ErrorsChanged() override {
        return errorsChanged_;
    }
    
private:
    std::map<std::string, std::vector<std::string>> errors_;
    core::Event<const std::string&> errorsChanged_;
};
```

## 相关类

- [ValidationRule](ValidationRule.md)
- [Binding](Binding.md)
