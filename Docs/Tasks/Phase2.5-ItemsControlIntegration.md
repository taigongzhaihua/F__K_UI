# Phase 2.5: ItemsControl 与 ItemContainerGenerator 集成

## 概览

将 ItemContainerGenerator 集成到 ItemsControl 类中，用复杂高效的系统替换手动容器管理，该系统具有容器池和回收功能。

## 目标

1. 用 ItemContainerGenerator 替换手动容器管理
2. 实现容器池和回收
3. 改进 Items 变更处理
4. 为 UI 虚拟化做准备

## 集成步骤

### 1. 添加 Generator

```cpp
class ItemsControl {
private:
    std::unique_ptr<ItemContainerGenerator> generator_;
    
protected:
    void OnItemsChanged(const CollectionChangedEventArgs& e);
    UIElement* GetContainerForItem(const std::any& item);
};
```

### 2. 容器生命周期

```cpp
// 添加项目
void ItemsControl::OnItemAdded(const std::any& item) {
    auto container = generator_->GenerateContainer(item);
    panel_->AddChild(container);
}

// 删除项目
void ItemsControl::OnItemRemoved(const std::any& item) {
    auto container = generator_->ContainerFromItem(item);
    panel_->RemoveChild(container);
    generator_->RecycleContainer(container);
}
```

## 性能优势

- 减少内存分配
- 更快的容器创建
- 高效的集合操作
- 为虚拟化做准备

## 成功标准

- [ ] Generator 完全集成
- [ ] 容器正确回收
- [ ] 性能改进
- [ ] 所有测试通过

## 状态

**计划中** - 依赖 Phase 2.4
