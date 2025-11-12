# Button - 设计文档

## 概览

**目的**：提供可点击的交互式控件，响应用户输入并触发操作。

**命名空间**：`fk::ui`

**头文件**：`fk/ui/Button.h`

## 架构

### 类层次结构

```
DependencyObject
    └── Visual
        └── UIElement
            └── FrameworkElement
                └── Control
                    └── ContentControl<Button>
                        └── Button
```

### 核心职责

1. **输入处理** - 鼠标和键盘事件
2. **视觉状态** - Normal、MouseOver、Pressed、Disabled
3. **内容承载** - 承载任意 UI 内容
4. **事件触发** - 触发 Click 事件
5. **焦点管理** - 键盘导航支持

### 类设计

```cpp
class Button : public ContentControl<Button> {
public:
    Button();
    virtual ~Button() = default;
    
    // 事件
    core::Event<> Click;
    
protected:
    // 输入处理
    void OnMouseEnter(const MouseEventArgs& e) override;
    void OnMouseLeave(const MouseEventArgs& e) override;
    void OnMouseDown(const MouseButtonEventArgs& e) override;
    void OnMouseUp(const MouseButtonEventArgs& e) override;
    void OnKeyDown(const KeyEventArgs& e) override;
    void OnKeyUp(const KeyEventArgs& e) override;
    
    // 渲染
    void OnRender(const RenderContext& context) override;
    
    // 状态
    void UpdateVisualState();
    
private:
    bool isMouseOver_;
    bool isPressed_;
    bool isKeyPressed_;
};
```

## 设计决策

### 1. CRTP 模式

**决策**：使用 `ContentControl<Button>` 作为基类

**理由**：
- 类型安全的方法链
- 编译时多态
- 无 vtable 开销
- 流畅的 API 设计

**示例**：
```cpp
button->Content("点击我")
      ->Width(100)
      ->Height(40);  // 返回 Button* 而不是 Control*
```

### 2. 事件处理

**决策**：直接处理鼠标和键盘事件

**理由**：
- 完全控制交互行为
- 可以实现自定义点击逻辑
- 支持可访问性（键盘激活）

**点击检测**：
```cpp
void Button::OnMouseUp(const MouseButtonEventArgs& e) {
    if (isPressed_ && isMouseOver_) {
        isPressed_ = false;
        UpdateVisualState();
        Click.Invoke();  // 触发 Click 事件
    }
}
```

### 3. 视觉状态管理

**决策**：内部管理视觉状态

**理由**：
- 简单实现 - 不需要 VisualStateManager（Phase 2）
- 性能 - 直接状态检查
- 向后兼容 - 稍后可以迁移到 VSM

**状态**：
- `Normal` - isMouseOver_ = false, isPressed_ = false
- `MouseOver` - isMouseOver_ = true, isPressed_ = false
- `Pressed` - isPressed_ = true
- `Disabled` - IsEnabled() = false

### 4. 内容模型

**决策**：继承自 ContentControl

**理由**：
- 可以承载任何 UI 元素
- 自动布局和渲染
- 与 WPF Button 一致

**支持的内容**：
```cpp
// 文本
button->Content("保存");

// 图像
auto image = std::make_shared<Image>();
button->Content(image);

// 复杂布局
auto stack = std::make_shared<StackPanel>();
button->Content(stack);
```

### 5. 键盘支持

**决策**：支持 Space 和 Enter 键激活

**理由**：
- 可访问性要求
- 键盘导航
- 与桌面应用程序约定一致

**实现**：
```cpp
void Button::OnKeyDown(const KeyEventArgs& e) {
    if (e.Key == Key::Space || e.Key == Key::Enter) {
        isKeyPressed_ = true;
        isPressed_ = true;
        UpdateVisualState();
    }
}

void Button::OnKeyUp(const KeyEventArgs& e) {
    if ((e.Key == Key::Space || e.Key == Key::Enter) && isKeyPressed_) {
        isKeyPressed_ = false;
        isPressed_ = false;
        UpdateVisualState();
        Click.Invoke();
    }
}
```

## 实现详情

### 鼠标交互状态机

```
     [Normal]
        ↓ MouseEnter
    [MouseOver]
        ↓ MouseDown
    [Pressed]
        ↓ MouseUp (在按钮内)
     → Click!
        ↓
    [MouseOver]
        ↓ MouseLeave
     [Normal]
```

### 键盘交互状态机

```
     [Normal]
        ↓ Focus
     [Focused]
        ↓ Space/Enter Down
    [Pressed]
        ↓ Space/Enter Up
     → Click!
        ↓
     [Focused]
```

### 渲染逻辑

```cpp
void Button::OnRender(const RenderContext& context) {
    // 确定背景色基于状态
    Color bgColor = GetBackgroundColor();
    if (!IsEnabled()) {
        bgColor = Colors::Gray;
    } else if (isPressed_) {
        bgColor = DarkenColor(bgColor, 0.2f);
    } else if (isMouseOver_) {
        bgColor = LightenColor(bgColor, 0.1f);
    }
    
    // 绘制背景
    context.FillRectangle(GetBounds(), bgColor);
    
    // 绘制边框
    context.DrawRectangle(GetBounds(), GetBorderBrush(), GetBorderThickness());
    
    // 渲染内容（由 ContentControl 处理）
    ContentControl::OnRender(context);
}
```

## 性能考虑

### 事件处理

- **O(1)** - 直接事件调用，无树遍历
- **最小分配** - 状态标志是布尔值
- **无动态调度** - CRTP 编译时解析

### 渲染

- **简单几何** - 矩形和边框
- **无复杂效果** - 基本颜色转换
- **缓存的背景** - 在状态变更时失效

### 内存占用

```
Button 实例：
- ContentControl 基础：~250 字节
- Button 特定：~20 字节（状态标志）
- 内容：取决于内容元素
- 总计：~270 字节 + 内容
```

## 测试策略

### 单元测试

```cpp
TEST(ButtonTest, ClickEvent) {
    auto button = std::make_shared<Button>();
    bool clicked = false;
    button->Click += [&clicked]() { clicked = true; };
    
    // 模拟点击
    button->OnMouseDown(MouseButtonEventArgs(MouseButton::Left, Point(0, 0)));
    button->OnMouseUp(MouseButtonEventArgs(MouseButton::Left, Point(0, 0)));
    
    EXPECT_TRUE(clicked);
}

TEST(ButtonTest, NoClickWhenDisabled) {
    auto button = std::make_shared<Button>();
    button->IsEnabled(false);
    
    bool clicked = false;
    button->Click += [&clicked]() { clicked = true; };
    
    button->OnMouseDown(MouseButtonEventArgs(MouseButton::Left, Point(0, 0)));
    button->OnMouseUp(MouseButtonEventArgs(MouseButton::Left, Point(0, 0)));
    
    EXPECT_FALSE(clicked);
}

TEST(ButtonTest, MouseOverState) {
    auto button = std::make_shared<Button>();
    
    EXPECT_FALSE(button->IsMouseOver());
    
    button->OnMouseEnter(MouseEventArgs(Point(0, 0)));
    EXPECT_TRUE(button->IsMouseOver());
    
    button->OnMouseLeave(MouseEventArgs(Point(0, 0)));
    EXPECT_FALSE(button->IsMouseOver());
}
```

### 集成测试

```cpp
TEST(ButtonTest, WithContent) {
    auto button = std::make_shared<Button>();
    auto textBlock = std::make_shared<TextBlock>();
    textBlock->Text("点击我");
    
    button->Content(textBlock);
    
    EXPECT_EQ(button->GetContent(), textBlock);
}

TEST(ButtonTest, KeyboardActivation) {
    auto button = std::make_shared<Button>();
    button->Focus();
    
    bool clicked = false;
    button->Click += [&clicked]() { clicked = true; };
    
    button->OnKeyDown(KeyEventArgs(Key::Space));
    button->OnKeyUp(KeyEventArgs(Key::Space));
    
    EXPECT_TRUE(clicked);
}
```

## 使用模式

### 命令模式

```cpp
class ICommand {
public:
    virtual void Execute() = 0;
    virtual bool CanExecute() = 0;
};

class SaveCommand : public ICommand {
    void Execute() override { /* 保存逻辑 */ }
    bool CanExecute() override { return hasChanges_; }
};

auto saveCommand = std::make_shared<SaveCommand>();
button->Click += [saveCommand]() {
    if (saveCommand->CanExecute()) {
        saveCommand->Execute();
    }
};

// 绑定 IsEnabled 到 CanExecute
button->SetBinding(Button::IsEnabledProperty(),
                  Binding("CanExecute").Source(saveCommand));
```

### 异步操作

```cpp
button->Click += [button]() {
    button->IsEnabled(false);
    button->Content("处理中...");
    
    std::thread([button]() {
        PerformLongRunningOperation();
        
        // 返回到 UI 线程
        dispatcher->InvokeAsync([button]() {
            button->IsEnabled(true);
            button->Content("完成");
        });
    }).detach();
};
```

### 带确认的对话框

```cpp
button->Click += [window]() {
    auto dialog = std::make_shared<ConfirmDialog>();
    dialog->Message("确定要删除吗？");
    
    dialog->Confirmed += []() {
        PerformDelete();
    };
    
    window->ShowDialog(dialog);
};
```

## 未来增强

### 1. VisualStateManager 集成

```cpp
class Button : public ContentControl<Button> {
protected:
    void GoToState(const std::string& stateName, bool useTransitions);
};

// 状态定义在 ControlTemplate 中
```

### 2. ControlTemplate 支持

```cpp
auto template = std::make_shared<ControlTemplate>();
template->SetVisualTree([]() {
    auto border = std::make_shared<Border>();
    auto contentPresenter = std::make_shared<ContentPresenter>();
    border->SetChild(contentPresenter);
    return border;
});

button->Template(template);
```

### 3. 默认按钮支持

```cpp
button->IsDefault(true);  // Enter 激活此按钮
button->IsCancel(true);   // Escape 激活此按钮
```

### 4. 触摸支持

```cpp
void Button::OnTouchDown(const TouchEventArgs& e);
void Button::OnTouchUp(const TouchEventArgs& e);
void Button::OnTouchMove(const TouchEventArgs& e);
```

### 5. 动画过渡

```cpp
// 状态变更之间的平滑动画
button->StateChangeAnimation(
    std::make_shared<ColorAnimation>(
        Colors::Gray, Colors::Blue, 200ms
    )
);
```

## 可访问性

### 当前支持

- 键盘激活（Space/Enter）
- 焦点管理
- IsEnabled 状态

### 未来支持

- 屏幕阅读器支持
- 高对比度主题
- 可自定义的激活键
- ARIA 角色和属性

## 另请参阅

- [API 文档](../../API/UI/Button.md)
- [ContentControl 设计](ContentControl.md)
- [Control 设计](Control.md)
- [事件系统设计](../../Designs/EventSystem.md)
