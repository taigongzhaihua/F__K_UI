# ResourceDictionary

## 概览

**目的**：存储和管理资源（样式、模板等）

**命名空间**：`fk::ui`

**头文件**：`fk/ui/ResourceDictionary.h`

## 描述

`ResourceDictionary` 是键值对集合，用于存储和查找资源。

## 公共接口

### 添加资源

#### Add
```cpp
void Add(const std::string& key, std::shared_ptr<Object> resource);
void Add(const std::type_info& key, std::shared_ptr<Object> resource);
```

添加资源。

### 查找资源

#### Find / TryFind
```cpp
std::shared_ptr<Object> Find(const std::string& key);
bool TryFind(const std::string& key, std::shared_ptr<Object>& resource);
```

查找资源。

## 使用示例

### 存储样式
```cpp
auto resources = std::make_shared<ResourceDictionary>();

// 添加按钮样式
auto buttonStyle = std::make_shared<Style>(typeid(Button));
buttonStyle->AddSetter(Control::BackgroundProperty(), Colors::Blue);
resources->Add("PrimaryButtonStyle", buttonStyle);

// 添加颜色
resources->Add("PrimaryColor", Colors::Blue);

// 使用资源
auto style = resources->Find("PrimaryButtonStyle");
button->SetStyle(std::static_pointer_cast<Style>(style).get());
```

### 隐式样式
```cpp
// 按类型存储样式
resources->Add(typeid(Button), buttonStyle);

// 自动应用
button->ApplyImplicitStyle();
```

## 相关类

- [Style](Style.md)
- [ControlTemplate](ControlTemplate.md)
- [FrameworkElement](FrameworkElement.md)
