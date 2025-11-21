# 视觉状态动画 TargetName 解析未实现 [已解决]

## 问题描述

CheckBox 的视觉状态动画中，使用 `TargetName` 指定目标元素的动画无法正常工作，特别是 `BorderBrush.Color` 动画不生效。

## 根本原因

在 `src/animation/Storyboard.cpp` 的 `Begin()` 方法中（第56行），TargetName 的解析逻辑尚未实现：

```cpp
// TODO: 实现在模板树中查找命名元素的逻辑
// 这需要访问UIElement的FindName方法
// 暂时跳过，因为需要将DependencyObject转换为UIElement
```

## ✅ 解决方案（2025-11-21）

### 1. 实现了 TargetName 解析

在 `Storyboard::Begin()` 中添加了完整的 TargetName 解析逻辑：

```cpp
// 在启动子动画前，先解析所有使用TargetName的动画
auto templateRoot = GetTemplateRoot(this);
if (templateRoot) {
    for (auto& child : children_) {
        std::string targetName = GetTargetName(child.get());
        std::string propertyPath = GetTargetProperty(child.get());
        
        if (!targetName.empty() && !propertyPath.empty()) {
            ui::UIElement* rootElement = dynamic_cast<ui::UIElement*>(templateRoot);
            if (rootElement) {
                // 在模板树中查找命名元素
                ui::UIElement* targetElement = rootElement->FindName(targetName);
                if (targetElement) {
                    // 解析属性路径（例如 "BorderBrush.Color"）
                    ResolvePropertyPath(child.get(), targetElement, propertyPath);
                }
            }
        }
    }
}
```

### 2. 实现了嵌套属性路径解析

在 `Storyboard::ResolvePropertyPath()` 中支持 "BorderBrush.Color" 这样的嵌套属性路径：

```cpp
void Storyboard::ResolvePropertyPath(Timeline* timeline, ui::UIElement* targetElement, 
                                     const std::string& propertyPath) {
    size_t dotPos = propertyPath.find('.');
    if (dotPos == std::string::npos) {
        // 简单属性路径
        auto prop = targetElement->FindProperty(propertyPath);
        if (prop) {
            SetTarget(timeline, targetElement, prop);
        }
    } else {
        // 嵌套属性路径：分割为对象名和属性名
        std::string objectPropertyName = propertyPath.substr(0, dotPos);
        std::string subPropertyName = propertyPath.substr(dotPos + 1);
        
        // 获取中间对象（如 BorderBrush）
        auto objectProperty = targetElement->FindProperty(objectPropertyName);
        if (objectProperty) {
            auto objectValue = targetElement->GetValue(*objectProperty);
            if (objectValue.type() == typeid(ui::Brush*)) {
                binding::DependencyObject* subObject = std::any_cast<ui::Brush*>(objectValue);
                if (subObject) {
                    // 在子对象上查找子属性（如 Color）
                    auto subProperty = subObject->FindProperty(subPropertyName);
                    if (subProperty) {
                        SetTarget(timeline, subObject, subProperty);
                    }
                }
            }
        }
    }
}
```

### 3. 实现了通用的属性查找机制

**关键改进：** 利用 `DependencyPropertyRegistry` 已有的映射关系，实现了零耦合的属性查找：

```cpp
// DependencyObject.cpp
const DependencyProperty* DependencyObject::FindProperty(const std::string& propertyName) const {
    // 使用运行时类型自动查找，无需派生类重写
    extern const DependencyProperty* FindDependencyProperty(std::type_index ownerType, const std::string& name);
    return FindDependencyProperty(typeid(*this), propertyName);
}

// DependencyProperty.cpp
const DependencyProperty* FindDependencyProperty(std::type_index ownerType, const std::string& name) {
    return DependencyPropertyRegistry::Instance().Find(ownerType, name);
}
```

**优势：**
- ✅ 利用 Register 时已记录的属性名和类型信息
- ✅ 派生类无需重写 FindProperty
- ✅ 新增属性自动支持查找
- ✅ 零维护成本，低耦合

### 4. 添加了 SetTarget 重载

支持同时设置目标对象和属性：

```cpp
void Storyboard::SetTarget(Timeline* timeline, binding::DependencyObject* target, 
                          const binding::DependencyProperty* property) {
    targetMap_[timeline] = target;
    
    if (auto* colorAnim = dynamic_cast<ColorAnimation*>(timeline)) {
        colorAnim->SetTarget(target, property);
    } else if (auto* doubleAnim = dynamic_cast<DoubleAnimation*>(timeline)) {
        doubleAnim->SetTarget(target, property);
    }
}
```

## 当前状态

- ✅ Background.Color 动画工作正常
- ✅ BorderBrush.Color 动画工作正常（通过 TargetName 解析）
- ✅ 所有视觉状态动画完全正常工作
- ✅ CheckBox、ToggleButton、Button 的视觉状态全部生效

## 测试验证

已通过以下测试：
1. CheckBox 鼠标悬停时边框从灰色过渡到蓝色 ✅
2. CheckBox 按下时边框变深蓝色 ✅
3. CheckBox 禁用时边框变浅灰色 ✅
4. CheckBox 勾选/取消勾选状态切换正常 ✅

## 相关文件修改

- ✅ `src/animation/Storyboard.cpp` - 实现 TargetName 解析和属性路径解析
- ✅ `include/fk/animation/Storyboard.h` - 添加 ResolvePropertyPath 方法声明
- ✅ `src/binding/DependencyProperty.cpp` - 添加全局 FindDependencyProperty 函数
- ✅ `src/binding/DependencyObject.cpp` - 实现通用 FindProperty
- ✅ `include/fk/binding/DependencyObject.h` - 添加虚方法声明

## 完成日期

**问题发现：** 2025-11-21  
**问题解决：** 2025-11-21

---

**状态：已解决并验证** ✅
