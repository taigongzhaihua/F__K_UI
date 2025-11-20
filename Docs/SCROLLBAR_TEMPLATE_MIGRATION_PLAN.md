# ScrollBar 模板化迁移计划

## 目标

将 ScrollBar 从直接渲染实现迁移到基于模板的组合实现，使其与 Button、ComboBox 等控件保持架构一致。

## 当前状态

### 问题
```cpp
void ScrollBar::OnRender(RenderContext& context) {
    // ❌ 直接调用底层渲染
    renderer->DrawRectangle(bgRect, ...);
    renderer->DrawRectangle(thumbRect, ...);
}
```

**问题：**
- 不可定制：用户无法改变 ScrollBar 的外观
- 架构不一致：与 Button/ComboBox 的模板化实现不同
- 难以扩展：添加新功能需要修改渲染代码

## 目标状态

### 架构
```
ScrollBar（无 OnRender）
    ↓ 默认模板
    Grid {
        RowDefinitions: Auto, *, Auto
        
        RepeatButton (LineUpButton) - Row 0
            ↓ 模板
            Border + Path (向上箭头)
        
        Track - Row 1
            ↓ 模板
            Border (背景) + Thumb (可拖动)
        
        RepeatButton (LineDownButton) - Row 2
            ↓ 模板
            Border + Path (向下箭头)
    }
```

## 实施步骤

### 步骤 1：创建 Thumb 控件

**文件：** `include/fk/ui/Thumb.h`, `src/ui/Thumb.cpp`

```cpp
/**
 * @brief 可拖动滑块控件
 * 
 * 用于 ScrollBar、Slider 等控件中的可拖动部分。
 */
class Thumb : public Control<Thumb> {
public:
    Thumb();
    
    // ========== 事件 ==========
    
    /// 拖动开始事件
    core::Event<float, float> DragStarted;
    
    /// 拖动中事件（deltaX, deltaY）
    core::Event<float, float> DragDelta;
    
    /// 拖动结束事件
    core::Event<> DragCompleted;
    
    // ========== 依赖属性 ==========
    
    static const binding::DependencyProperty& IsDraggingProperty();
    
    bool GetIsDragging() const { return isDragging_; }
    
protected:
    // ========== 事件处理 ==========
    
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerMoved(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    
private:
    bool isDragging_{false};
    Point dragStartPoint_;
    Point lastDragPoint_;
    
    void StartDrag(const Point& point);
    void UpdateDrag(const Point& point);
    void EndDrag();
};
```

**默认模板：**
```cpp
ControlTemplate* Thumb::GetDefaultTemplate() {
    auto tmpl = new ControlTemplate();
    tmpl->SetFactory([]() {
        return new Border()
            ->Background(new SolidColorBrush(Color(0.6f, 0.6f, 0.6f, 1.0f)))
            ->BorderBrush(new SolidColorBrush(Color(0.4f, 0.4f, 0.4f, 1.0f)))
            ->BorderThickness(Thickness(1))
            ->CornerRadius(CornerRadius(2));
    });
    return tmpl;
}
```

### 步骤 2：创建 RepeatButton 控件

**文件：** `include/fk/ui/RepeatButton.h`, `src/ui/RepeatButton.cpp`

```cpp
/**
 * @brief 重复按钮控件
 * 
 * 按下时持续触发 Click 事件，用于 ScrollBar 的增减按钮。
 */
class RepeatButton : public Button {
public:
    RepeatButton();
    
    // ========== 依赖属性 ==========
    
    /// Delay 属性（首次触发延迟，毫秒）
    static const binding::DependencyProperty& DelayProperty();
    
    /// Interval 属性（重复触发间隔，毫秒）
    static const binding::DependencyProperty& IntervalProperty();
    
    int GetDelay() const { return delay_; }
    RepeatButton* SetDelay(int value) {
        delay_ = value;
        return this;
    }
    
    int GetInterval() const { return interval_; }
    RepeatButton* SetInterval(int value) {
        interval_ = value;
        return this;
    }
    
protected:
    void OnPointerPressed(PointerEventArgs& e) override;
    void OnPointerReleased(PointerEventArgs& e) override;
    
private:
    int delay_{300};      // 首次延迟 300ms
    int interval_{100};   // 重复间隔 100ms
    bool isPressed_{false};
    
    // 定时器相关
    void StartRepeatTimer();
    void StopRepeatTimer();
    void OnRepeatTick();
};
```

### 步骤 3：创建 Track 控件（可选）

**文件：** `include/fk/ui/Track.h`, `src/ui/Track.cpp`

```cpp
/**
 * @brief 轨道容器控件
 * 
 * 专门用于布局 ScrollBar/Slider 中的滑块和背景。
 */
class Track : public FrameworkElement<Track> {
public:
    Track();
    
    // ========== 子元素 ==========
    
    UIElement* GetThumb() const { return thumb_; }
    void SetThumb(UIElement* value);
    
    UIElement* GetDecreaseButton() const { return decreaseButton_; }
    void SetDecreaseButton(UIElement* value);
    
    UIElement* GetIncreaseButton() const { return increaseButton_; }
    void SetIncreaseButton(UIElement* value);
    
    // ========== 依赖属性 ==========
    
    static const binding::DependencyProperty& OrientationProperty();
    static const binding::DependencyProperty& ValueProperty();
    static const binding::DependencyProperty& MinimumProperty();
    static const binding::DependencyProperty& MaximumProperty();
    static const binding::DependencyProperty& ViewportSizeProperty();
    
protected:
    Size MeasureOverride(Size availableSize) override;
    Size ArrangeOverride(Size finalSize) override;
    
private:
    Orientation orientation_{Orientation::Vertical};
    UIElement* thumb_{nullptr};
    UIElement* decreaseButton_{nullptr};
    UIElement* increaseButton_{nullptr};
    
    float value_{0.0f};
    float minimum_{0.0f};
    float maximum_{100.0f};
    float viewportSize_{10.0f};
    
    // 计算 Thumb 位置
    Rect CalculateThumbBounds(Size arrangeSize) const;
};
```

### 步骤 4：重构 ScrollBar

**修改：** `include/fk/ui/ScrollBar.h`, `src/ui/ScrollBar.cpp`

```cpp
class ScrollBar : public Control<ScrollBar> {
public:
    ScrollBar();
    
    // ========== 静态方法 ==========
    
    /// 获取默认模板
    static ControlTemplate* GetDefaultTemplate();
    
    // ========== 模板部件名称 ==========
    
    static constexpr const char* PART_Track = "PART_Track";
    static constexpr const char* PART_LineUpButton = "PART_LineUpButton";
    static constexpr const char* PART_LineDownButton = "PART_LineDownButton";
    
protected:
    // ========== 移除 OnRender ==========
    // void OnRender(RenderContext& context);  // ❌ 删除
    
    // ========== 添加模板应用 ==========
    
    void OnApplyTemplate() override;
    
private:
    // 模板部件引用
    Track* track_{nullptr};
    RepeatButton* lineUpButton_{nullptr};
    RepeatButton* lineDownButton_{nullptr};
    Thumb* thumb_{nullptr};
    
    // 事件处理
    void OnLineUpClick();
    void OnLineDownClick();
    void OnThumbDrag(float deltaX, float deltaY);
};
```

**默认模板实现：**
```cpp
ControlTemplate* ScrollBar::GetDefaultTemplate() {
    auto tmpl = new ControlTemplate();
    tmpl->SetFactory([]() {
        // 创建网格布局
        auto grid = new Grid();
        
        // 垂直滚动条：3行（上按钮、轨道、下按钮）
        grid->AddRowDefinition(new RowDefinition(GridLength::Auto()));
        grid->AddRowDefinition(new RowDefinition(GridLength::Star()));
        grid->AddRowDefinition(new RowDefinition(GridLength::Auto()));
        
        // 上按钮
        auto lineUpBtn = new RepeatButton();
        lineUpBtn->SetName("PART_LineUpButton");
        lineUpBtn->SetHeight(16);
        // TODO: 设置箭头图标模板
        Grid::SetRow(lineUpBtn, 0);
        grid->AddChild(lineUpBtn);
        
        // 轨道
        auto track = new Track();
        track->SetName("PART_Track");
        track->SetOrientation(Orientation::Vertical);
        
        // 轨道背景
        auto trackBg = new Border()
            ->Background(new SolidColorBrush(Color(0.9f, 0.9f, 0.9f, 1.0f)));
        
        // 滑块
        auto thumb = new Thumb();
        thumb->SetName("PART_Thumb");
        
        track->SetChild(trackBg);
        track->SetThumb(thumb);
        Grid::SetRow(track, 1);
        grid->AddChild(track);
        
        // 下按钮
        auto lineDownBtn = new RepeatButton();
        lineDownBtn->SetName("PART_LineDownButton");
        lineDownBtn->SetHeight(16);
        // TODO: 设置箭头图标模板
        Grid::SetRow(lineDownBtn, 2);
        grid->AddChild(lineDownBtn);
        
        return grid;
    });
    return tmpl;
}

void ScrollBar::OnApplyTemplate() {
    Control<ScrollBar>::OnApplyTemplate();
    
    // 清理旧的事件订阅
    if (lineUpButton_) {
        // 取消订阅
    }
    
    // 查找模板部件
    track_ = FindTemplateChild<Track>(PART_Track);
    lineUpButton_ = FindTemplateChild<RepeatButton>(PART_LineUpButton);
    lineDownButton_ = FindTemplateChild<RepeatButton>(PART_LineDownButton);
    
    if (track_) {
        thumb_ = dynamic_cast<Thumb*>(track_->GetThumb());
    }
    
    // 订阅事件
    if (lineUpButton_) {
        lineUpButton_->Click.Subscribe([this]() {
            OnLineUpClick();
        });
    }
    
    if (lineDownButton_) {
        lineDownButton_->Click.Subscribe([this]() {
            OnLineDownClick();
        });
    }
    
    if (thumb_) {
        thumb_->DragDelta.Subscribe([this](float dx, float dy) {
            OnThumbDrag(dx, dy);
        });
    }
    
    // 更新轨道属性
    UpdateTrack();
}

void ScrollBar::OnLineUpClick() {
    LineUp();
}

void ScrollBar::OnLineDownClick() {
    LineDown();
}

void ScrollBar::OnThumbDrag(float deltaX, float deltaY) {
    if (!track_) return;
    
    float range = maximum_ - minimum_;
    if (range <= 0.0f) return;
    
    float delta = (orientation_ == Orientation::Vertical) ? deltaY : deltaX;
    float trackSize = (orientation_ == Orientation::Vertical) 
        ? track_->GetRenderSize().height 
        : track_->GetRenderSize().width;
    
    float thumbSize = (orientation_ == Orientation::Vertical)
        ? thumb_->GetRenderSize().height
        : thumb_->GetRenderSize().width;
    
    float moveableRange = trackSize - thumbSize;
    if (moveableRange <= 0.0f) return;
    
    float valueChange = (delta / moveableRange) * range;
    SetValue(value_ + valueChange);
}

void ScrollBar::UpdateTrack() {
    if (!track_) return;
    
    track_->SetOrientation(orientation_);
    track_->SetValue(value_);
    track_->SetMinimum(minimum_);
    track_->SetMaximum(maximum_);
    track_->SetViewportSize(viewportSize_);
}
```

## 测试策略

### 单元测试

```cpp
// test_scrollbar_template.cpp

TEST(ScrollBarTemplate, DefaultTemplateApplied) {
    auto scrollBar = new ScrollBar();
    scrollBar->ApplyTemplate();
    
    // 验证模板部件存在
    auto track = scrollBar->FindTemplateChild<Track>("PART_Track");
    ASSERT_NE(nullptr, track);
    
    auto lineUpBtn = scrollBar->FindTemplateChild<RepeatButton>("PART_LineUpButton");
    ASSERT_NE(nullptr, lineUpBtn);
    
    auto lineDownBtn = scrollBar->FindTemplateChild<RepeatButton>("PART_LineDownButton");
    ASSERT_NE(nullptr, lineDownBtn);
}

TEST(ScrollBarTemplate, CustomTemplateWorks) {
    auto scrollBar = new ScrollBar();
    
    // 设置自定义模板
    auto customTemplate = new ControlTemplate();
    customTemplate->SetFactory([]() {
        return new Border()->Background(new SolidColorBrush(Colors::Red));
    });
    
    scrollBar->SetTemplate(customTemplate);
    scrollBar->ApplyTemplate();
    
    // 验证自定义模板生效
    // ...
}

TEST(Thumb, DragEvents) {
    auto thumb = new Thumb();
    
    bool dragStarted = false;
    bool dragDelta = false;
    bool dragCompleted = false;
    
    thumb->DragStarted.Subscribe([&](float x, float y) {
        dragStarted = true;
    });
    
    thumb->DragDelta.Subscribe([&](float dx, float dy) {
        dragDelta = true;
    });
    
    thumb->DragCompleted.Subscribe([&]() {
        dragCompleted = true;
    });
    
    // 模拟拖动
    PointerEventArgs pressArgs;
    pressArgs.position = Point(10, 10);
    thumb->OnPointerPressed(pressArgs);
    ASSERT_TRUE(dragStarted);
    
    PointerEventArgs moveArgs;
    moveArgs.position = Point(20, 20);
    thumb->OnPointerMoved(moveArgs);
    ASSERT_TRUE(dragDelta);
    
    PointerEventArgs releaseArgs;
    thumb->OnPointerReleased(releaseArgs);
    ASSERT_TRUE(dragCompleted);
}

TEST(RepeatButton, RepeatClick) {
    auto btn = new RepeatButton();
    btn->SetDelay(100);
    btn->SetInterval(50);
    
    int clickCount = 0;
    btn->Click.Subscribe([&]() {
        clickCount++;
    });
    
    // 模拟按下并保持
    PointerEventArgs pressArgs;
    btn->OnPointerPressed(pressArgs);
    
    // 等待重复触发
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // 释放
    PointerEventArgs releaseArgs;
    btn->OnPointerReleased(releaseArgs);
    
    // 验证多次触发
    ASSERT_GT(clickCount, 1);
}
```

### 集成测试

```cpp
// test_scrollbar_integration.cpp

TEST(ScrollBarIntegration, ValueChangeOnButtonClick) {
    auto scrollBar = new ScrollBar();
    scrollBar->SetMinimum(0);
    scrollBar->SetMaximum(100);
    scrollBar->SetValue(50);
    scrollBar->SetSmallChange(10);
    scrollBar->ApplyTemplate();
    
    auto lineDownBtn = scrollBar->FindTemplateChild<RepeatButton>("PART_LineDownButton");
    ASSERT_NE(nullptr, lineDownBtn);
    
    // 点击下按钮
    lineDownBtn->RaiseClick();
    
    // 验证值增加
    ASSERT_FLOAT_EQ(60.0f, scrollBar->GetValue());
}

TEST(ScrollBarIntegration, ValueChangeOnThumbDrag) {
    auto scrollBar = new ScrollBar();
    scrollBar->SetMinimum(0);
    scrollBar->SetMaximum(100);
    scrollBar->SetValue(0);
    scrollBar->SetHeight(200);
    scrollBar->ApplyTemplate();
    scrollBar->Arrange(Rect(0, 0, 20, 200));
    
    auto track = scrollBar->FindTemplateChild<Track>("PART_Track");
    ASSERT_NE(nullptr, track);
    
    auto thumb = dynamic_cast<Thumb*>(track->GetThumb());
    ASSERT_NE(nullptr, thumb);
    
    // 模拟拖动滑块
    thumb->DragDelta(0, 50);  // 向下拖动 50px
    
    // 验证值改变
    ASSERT_GT(scrollBar->GetValue(), 0.0f);
}
```

### 可视化测试

```cpp
// examples/scrollbar_template_demo.cpp

int main() {
    auto window = new Window();
    window->SetTitle("ScrollBar Template Demo");
    window->SetWidth(400);
    window->SetHeight(600);
    
    auto panel = new StackPanel();
    panel->SetOrientation(Orientation::Vertical);
    panel->SetSpacing(20);
    panel->SetPadding(Thickness(20));
    
    // 1. 默认模板的 ScrollBar
    auto text1 = new TextBlock();
    text1->SetText("Default ScrollBar:");
    text1->SetFontSize(16);
    panel->AddChild(text1);
    
    auto scrollBar1 = new ScrollBar();
    scrollBar1->SetHeight(200);
    scrollBar1->SetMinimum(0);
    scrollBar1->SetMaximum(100);
    scrollBar1->SetValue(25);
    scrollBar1->SetViewportSize(20);
    panel->AddChild(scrollBar1);
    
    // 2. 自定义模板的 ScrollBar
    auto text2 = new TextBlock();
    text2->SetText("Custom Styled ScrollBar:");
    text2->SetFontSize(16);
    panel->AddChild(text2);
    
    auto scrollBar2 = new ScrollBar();
    scrollBar2->SetHeight(200);
    // TODO: 设置自定义模板
    panel->AddChild(scrollBar2);
    
    // 3. 水平 ScrollBar
    auto text3 = new TextBlock();
    text3->SetText("Horizontal ScrollBar:");
    text3->SetFontSize(16);
    panel->AddChild(text3);
    
    auto scrollBar3 = new ScrollBar();
    scrollBar3->SetOrientation(Orientation::Horizontal);
    scrollBar3->SetWidth(300);
    scrollBar3->SetHeight(20);
    panel->AddChild(scrollBar3);
    
    window->SetContent(panel);
    window->Show();
    
    return 0;
}
```

## 迁移检查清单

- [ ] 创建 Thumb 控件
  - [ ] 头文件和实现
  - [ ] 拖动事件处理
  - [ ] 默认模板
  - [ ] 单元测试

- [ ] 创建 RepeatButton 控件
  - [ ] 头文件和实现
  - [ ] 重复触发逻辑
  - [ ] 定时器实现
  - [ ] 单元测试

- [ ] （可选）创建 Track 控件
  - [ ] 头文件和实现
  - [ ] 布局逻辑
  - [ ] 单元测试

- [ ] 重构 ScrollBar
  - [ ] 移除 OnRender 实现
  - [ ] 创建默认模板
  - [ ] 实现 OnApplyTemplate
  - [ ] 事件连接和处理
  - [ ] 更新属性同步逻辑

- [ ] 测试
  - [ ] 单元测试（Thumb, RepeatButton, ScrollBar）
  - [ ] 集成测试（完整交互流程）
  - [ ] 可视化测试（Demo 程序）
  - [ ] 性能测试（与旧实现对比）

- [ ] 文档
  - [ ] 更新 ScrollBar 使用文档
  - [ ] 添加自定义模板示例
  - [ ] API 参考更新

- [ ] 向后兼容
  - [ ] 确保现有代码仍能工作
  - [ ] 提供迁移指南（如有 API 变化）

## 预期收益

1. **可定制性**
   - 用户可以完全自定义 ScrollBar 外观
   - 支持主题和样式系统
   - 与其他控件的外观统一

2. **架构一致性**
   - 所有复杂控件使用相同的模板机制
   - 更容易理解和维护
   - 符合 WPF/XAML 设计模式

3. **可扩展性**
   - 添加新功能无需修改渲染代码
   - 子控件可独立演进
   - 更容易添加动画和过渡效果

4. **可测试性**
   - 可以独立测试 Thumb、RepeatButton 等子控件
   - 更细粒度的单元测试
   - 更容易模拟用户交互

## 风险和缓解

### 风险 1：性能下降

**风险：** 从直接渲染改为组合控件可能增加渲染开销。

**缓解：**
- 进行性能测试对比
- 优化子控件的渲染路径
- 使用渲染缓存机制

### 风险 2：行为变化

**风险：** 重构可能改变现有的交互行为。

**缓解：**
- 详细的集成测试覆盖所有交互场景
- 与旧实现进行 side-by-side 对比测试
- 保留旧实现一段时间作为备份

### 风险 3：开发时间

**风险：** 实施可能需要较长时间。

**缓解：**
- 分阶段实施（先 Thumb，再 RepeatButton，最后 ScrollBar）
- 每个阶段都进行充分测试
- 可以先实现简化版本，后续迭代完善

## 时间估算

- Thumb 控件：2-3 天
- RepeatButton 控件：1-2 天
- Track 控件（可选）：1-2 天
- ScrollBar 重构：2-3 天
- 测试和文档：2-3 天
- **总计：8-13 天**

## 结论

ScrollBar 的模板化迁移是一个值得投入的重构任务，它将使框架的架构更加一致和可维护。建议在单独的 PR 中实施，以便更好地管理变更和测试。
