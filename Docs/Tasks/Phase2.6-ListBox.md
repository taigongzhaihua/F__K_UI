# Phase 2.6: ListBox 实现

## 概览

实现具有选择支持和键盘导航的 ListBox 控件，构建在具有 ItemContainerGenerator 集成的 ItemsControl 之上。

## 目标

1. 实现 ListBox 类
2. 单项和多项选择
3. 键盘导航
4. 视觉选择反馈

## ListBox 类

```cpp
class ListBox : public ItemsControl<ListBox> {
public:
    // 选择属性
    static const DependencyProperty& SelectedItemProperty();
    static const DependencyProperty& SelectedIndexProperty();
    static const DependencyProperty& SelectionModeProperty();
    
    // 选择方法
    void SelectItem(const std::any& item);
    void UnselectItem(const std::any& item);
    void ClearSelection();
    
    // 事件
    core::Event<SelectionChangedEventArgs> SelectionChanged;
    
protected:
    void OnKeyDown(const KeyEventArgs& e) override;
    void OnMouseDown(const MouseButtonEventArgs& e) override;
};
```

## 功能

### 1. 选择模式
- Single - 单项选择
- Multiple - 多项选择
- Extended - 扩展选择（Shift/Ctrl）

### 2. 键盘导航
- 上/下箭头 - 移动选择
- Home/End - 跳到首尾
- Page Up/Down - 分页滚动

### 3. 鼠标交互
- 点击选择
- Ctrl+Click - 切换选择
- Shift+Click - 范围选择

## 实现阶段

1. **基础选择** - 单项选择和事件
2. **键盘导航** - 箭头键和导航
3. **多项选择** - 多选择模式
4. **视觉状态** - 选中项高亮

## 成功标准

- [ ] 选择正常工作
- [ ] 键盘导航流畅
- [ ] 多选支持
- [ ] 视觉反馈清晰
- [ ] 所有测试通过

## 状态

**计划中** - 依赖 Phase 2.4 和 2.5
