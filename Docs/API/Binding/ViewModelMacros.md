# ViewModelMacros - ViewModel 宏系统

## 概述

`ViewModelMacros` 提供了一套便利的宏，简化 ViewModel 属性的声明和注册，减少重复代码。

## 主要宏

### FK_VIEWMODEL_BEGIN / FK_VIEWMODEL_END

```cpp
FK_VIEWMODEL_BEGIN(MyViewModel)
    FK_VIEWMODEL_PROPERTY(PropertyName, &MyViewModel::GetterMethod)
    FK_VIEWMODEL_PROPERTY_RW(PropertyName, &MyViewModel::GetterMethod, &MyViewModel::SetterMethod)
FK_VIEWMODEL_END()
```

### FK_VIEWMODEL_AUTO

自动注册属性，假设 getter 为 `Get{PropertyName}()`，setter 为 `Set{PropertyName}()`：

```cpp
FK_VIEWMODEL_AUTO(MyViewModel, Username, Password, Email)
// 自动注册三个属性和对应的 getter/setter
```

## 使用示例

```cpp
class LoginViewModel : public fk::binding::ObservableObject {
public:
    std::string GetUsername() const { return username_; }
    void SetUsername(const std::string& value) {
        username_ = value;
        RaisePropertyChanged("Username");
    }
    
private:
    std::string username_;
};

// 注册属性（在某个 .cpp 文件中）
FK_VIEWMODEL_AUTO(LoginViewModel, Username)
```

## 好处

- 减少样板代码
- 确保属性注册完整
- 提供类型安全

## 相关文档

- [PropertyAccessors.md](./PropertyAccessors.md) - 属性访问器注册
- [ObservableObject.md](./ObservableObject.md) - ViewModel 基类
