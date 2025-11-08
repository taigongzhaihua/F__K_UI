# ScrollViewer - 滚动查看器

可滚动的内容容器，支持垂直/水平滚动、滚动条、鼠标滚轮。

## 概述

`ScrollViewer` 是一个内容控件，为超出可见区域的内容提供滚动功能：

- 垂直和水平滚动
- 滚动条可见性控制
- 鼠标滚轮支持
- 编程式滚动（ScrollTo* 方法）
- 滚动事件通知
- 链式 API

## 类层次结构

```
DependencyObject
  └─ DispatcherObject
      └─ UIElement
          └─ FrameworkElement
              └─ Control
                  └─ ContentControl
                      └─ ScrollViewer    ← 你在这里
```

## 基本用法

### 创建滚动查看器

```cpp
#include <fk/ui/ScrollViewer.h>
#include <fk/ui/TextBlock.h>

// 工厂函数
auto viewer = scrollViewer();

// 设置内容
viewer->SetContent(
    textBlock().Text("Very long text content...")
);
```

### 链式 API

```cpp
auto viewer = scrollViewer()
    .Content(
        textBlock()
            .Text("Long content that needs scrolling")
            .Width(1000)
            .Height(2000)
    )
    .Width(300)
    .Height(400)
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
```

## 核心属性

### ScrollBarVisibility - 滚动条可见性

```cpp
enum class ScrollBarVisibility {
    Auto,      // 需要时自动显示
    Visible,   // 始终可见
    Hidden,    // 始终隐藏
    Disabled   // 禁用(隐藏且不占用空间)
};
```

**示例：**

```cpp
// 垂直滚动条：自动显示
viewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);

// 水平滚动条：始终隐藏
viewer->SetHorizontalScrollBarVisibility(ScrollBarVisibility::Hidden);
```

### VerticalScrollBarVisibility / HorizontalScrollBarVisibility

```cpp
ScrollBarVisibility GetVerticalScrollBarVisibility() const;
void SetVerticalScrollBarVisibility(ScrollBarVisibility value);

ScrollBarVisibility GetHorizontalScrollBarVisibility() const;
void SetHorizontalScrollBarVisibility(ScrollBarVisibility value);
```

控制滚动条的显示策略。

**示例：**

```cpp
auto viewer = scrollViewer()
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto)
    .HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
```

### VerticalOffset / HorizontalOffset - 滚动偏移

```cpp
double GetVerticalOffset() const;
void SetVerticalOffset(double offset);

double GetHorizontalOffset() const;
void SetHorizontalOffset(double offset);
```

当前滚动位置（像素）。

**示例：**

```cpp
// 获取当前位置
double vOffset = viewer->GetVerticalOffset();
std::cout << "Vertical offset: " << vOffset << std::endl;

// 设置滚动位置
viewer->SetVerticalOffset(100.0);
```

### ViewportWidth / ViewportHeight - 可见区域尺寸

```cpp
double GetViewportWidth() const;
double GetViewportHeight() const;
```

滚动查看器的可见区域大小（只读）。

### ExtentWidth / ExtentHeight - 内容总尺寸

```cpp
double GetExtentWidth() const;
double GetExtentHeight() const;
```

内容的总尺寸（只读）。

**示例：**

```cpp
double viewport = viewer->GetViewportHeight();
double extent = viewer->GetExtentHeight();

if (extent > viewport) {
    std::cout << "Content is scrollable" << std::endl;
}
```

## 主要方法

### ScrollToTop / ScrollToBottom - 滚动到顶部/底部

```cpp
void ScrollToTop();
void ScrollToBottom();
```

快速滚动到内容顶部或底部。

**示例：**

```cpp
// 滚动到顶部
viewer->ScrollToTop();

// 滚动到底部
viewer->ScrollToBottom();

// 链式
viewer->ScrollToBottom()->Width(300);
```

### ScrollToLeft / ScrollToRight - 滚动到左侧/右侧

```cpp
void ScrollToLeft();
void ScrollToRight();
```

快速滚动到内容左侧或右侧。

### ScrollToVerticalOffset / ScrollToHorizontalOffset

```cpp
void ScrollToVerticalOffset(double offset);
void ScrollToHorizontalOffset(double offset);
```

滚动到指定偏移位置。

**示例：**

```cpp
// 滚动到垂直位置 200
viewer->ScrollToVerticalOffset(200.0);

// 滚动到水平位置 100
viewer->ScrollToHorizontalOffset(100.0);
```

### LineUp / LineDown - 按行滚动

```cpp
void LineUp();      // 向上滚动一行
void LineDown();    // 向下滚动一行
```

按行为单位滚动（默认 16 像素）。

**示例：**

```cpp
// 向上滚动一行
viewer->LineUp();

// 向下滚动一行
viewer->LineDown();
```

### PageUp / PageDown - 按页滚动

```cpp
void PageUp();      // 向上滚动一页
void PageDown();    // 向下滚动一页
```

按页为单位滚动（页 = 可见区域高度）。

**示例：**

```cpp
// 向上滚动一页
viewer->PageUp();

// 向下滚动一页
viewer->PageDown();
```

### LineLeft / LineRight / PageLeft / PageRight

```cpp
void LineLeft();    // 向左滚动一列
void LineRight();   // 向右滚动一列
void PageLeft();    // 向左滚动一页
void PageRight();   // 向右滚动一页
```

水平方向的行/页滚动。

## 事件

### ScrollChanged - 滚动变化事件

```cpp
core::Event<double, double> ScrollChanged;
```

滚动位置变化时触发，参数为 `(horizontalOffset, verticalOffset)`。

**示例：**

```cpp
viewer->ScrollChanged.Subscribe([](double hOffset, double vOffset) {
    std::cout << "Scroll position: (" << hOffset << ", " << vOffset << ")" << std::endl;
});

// 或使用链式 API
viewer->OnScrollChanged([](double h, double v) {
    std::cout << "Scrolled to: " << h << ", " << v << std::endl;
});
```

## 完整示例

### 示例 1：简单的可滚动文本

```cpp
#include <fk/ui/ScrollViewer.h>
#include <fk/ui/TextBlock.h>

int main() {
    auto viewer = scrollViewer()
        .Content(
            textBlock()
                .Text("Line 1\nLine 2\nLine 3\n...\nLine 100")
                .FontSize(14)
        )
        .Width(300)
        .Height(200)
        .VerticalScrollBarVisibility(ScrollBarVisibility::Auto)
        .HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
    
    // 添加到窗口...
}
```

### 示例 2：大图片查看器

```cpp
auto imageViewer = scrollViewer()
    .Content(
        image()
            .Source("large_image.png")
            .Width(2000)
            .Height(1500)
    )
    .Width(800)
    .Height(600)
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto)
    .HorizontalScrollBarVisibility(ScrollBarVisibility::Auto);
```

### 示例 3：编程式滚动控制

```cpp
auto viewer = scrollViewer()
    .Content(longContent)
    .Width(400)
    .Height(300);

// 创建控制按钮
auto topButton = button()
    .Content(textBlock().Text("Top"))
    .OnClick([viewer]() {
        viewer->ScrollToTop();
    });

auto bottomButton = button()
    .Content(textBlock().Text("Bottom"))
    .OnClick([viewer]() {
        viewer->ScrollToBottom();
    });

auto upButton = button()
    .Content(textBlock().Text("Page Up"))
    .OnClick([viewer]() {
        viewer->PageUp();
    });

auto downButton = button()
    .Content(textBlock().Text("Page Down"))
    .OnClick([viewer]() {
        viewer->PageDown();
    });
```

### 示例 4：监听滚动位置

```cpp
auto viewer = scrollViewer()
    .Content(longContent)
    .OnScrollChanged([](double h, double v) {
        std::cout << "Horizontal: " << h << ", Vertical: " << v << std::endl;
        
        // 实现"返回顶部"按钮的显示/隐藏
        if (v > 100) {
            ShowBackToTopButton();
        } else {
            HideBackToTopButton();
        }
    });
```

### 示例 5：检测滚动到底部

```cpp
auto viewer = scrollViewer()
    .Content(dataList)
    .OnScrollChanged([viewer](double h, double v) {
        double viewport = viewer->GetViewportHeight();
        double extent = viewer->GetExtentHeight();
        
        // 检查是否滚动到底部
        if (v + viewport >= extent - 10) {  // 10 像素容差
            std::cout << "Reached bottom! Loading more..." << std::endl;
            LoadMoreData();
        }
    });
```

### 示例 6：嵌套滚动查看器

```cpp
auto outerViewer = scrollViewer()
    .Content(
        stackPanel()
            .Children({
                textBlock().Text("Header"),
                scrollViewer()  // 内部滚动器
                    .Content(
                        textBlock().Text("Inner scrollable content")
                    )
                    .Height(200)
                    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto),
                textBlock().Text("Footer")
            })
    )
    .Width(400)
    .Height(600)
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto);
```

### 示例 7：带滚动动画

```cpp
class AnimatedScrollViewer {
public:
    AnimatedScrollViewer(std::shared_ptr<ScrollViewerView> viewer,
                        std::shared_ptr<Dispatcher> dispatcher) 
        : viewer_(viewer), timer_(std::make_shared<Timer>(dispatcher)) {
        
        timer_->Tick.Subscribe([this]() {
            UpdateScroll();
        });
    }
    
    void ScrollToOffset(double targetOffset, double duration) {
        startOffset_ = viewer_->GetVerticalOffset();
        targetOffset_ = targetOffset;
        startTime_ = std::chrono::steady_clock::now();
        duration_ = duration;
        
        timer_->Start(std::chrono::milliseconds(16), true);  // 60 FPS
    }
    
private:
    void UpdateScroll() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - startTime_).count();
        
        if (elapsed >= duration_) {
            viewer_->ScrollToVerticalOffset(targetOffset_);
            timer_->Stop();
            return;
        }
        
        // 线性插值
        double progress = elapsed / duration_;
        double currentOffset = startOffset_ + (targetOffset_ - startOffset_) * progress;
        viewer_->ScrollToVerticalOffset(currentOffset);
    }
    
    std::shared_ptr<ScrollViewerView> viewer_;
    std::shared_ptr<Timer> timer_;
    double startOffset_{0};
    double targetOffset_{0};
    double duration_{0};
    std::chrono::steady_clock::time_point startTime_;
};

// 使用
auto viewer = scrollViewer();
auto animator = std::make_shared<AnimatedScrollViewer>(viewer, dispatcher);
animator->ScrollToOffset(500.0, 1.0);  // 1 秒动画到 500 像素
```

### 示例 8：自定义滚动条样式（未来）

```cpp
// 目前 ScrollBar 样式由内部控制
// 未来可能支持自定义：
auto viewer = scrollViewer()
    .Content(content)
    .VerticalScrollBarWidth(12)  // 未来 API
    .ScrollBarBackground(Color::LightGray())  // 未来 API
    .ScrollBarThumbColor(Color::DarkGray());  // 未来 API
```

## 鼠标滚轮支持

`ScrollViewer` 自动处理鼠标滚轮事件：

- 垂直滚轮：滚动垂直内容
- Shift + 滚轮：滚动水平内容（部分平台）
- 滚动速度：可配置（未来）

**示例：**

```cpp
auto viewer = scrollViewer()
    .Content(longContent);

// 鼠标滚轮自动工作，无需额外代码
```

## 键盘支持（未来）

未来版本可能支持键盘导航：

- `↑/↓` 箭头：行滚动
- `Page Up/Down`：页滚动
- `Home/End`：滚动到顶部/底部

## 最佳实践

### ✅ 推荐做法

**1. 合理设置滚动条可见性**
```cpp
// ✅ 根据内容类型选择
auto textViewer = scrollViewer()
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto)
    .HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);  // 文本通常不需要横向滚动

auto imageViewer = scrollViewer()
    .VerticalScrollBarVisibility(ScrollBarVisibility::Auto)
    .HorizontalScrollBarVisibility(ScrollBarVisibility::Auto);  // 图片可能需要双向滚动
```

**2. 监听滚动事件实现交互**
```cpp
viewer->OnScrollChanged([](double h, double v) {
    // 更新 UI 状态、加载数据等
});
```

**3. 使用编程式滚动**
```cpp
// ✅ 提供用户友好的滚动控制
backToTopButton->OnClick([viewer]() {
    viewer->ScrollToTop();
});
```

**4. 检查滚动能力**
```cpp
if (viewer->GetExtentHeight() > viewer->GetViewportHeight()) {
    // 内容可滚动
    showScrollHint();
}
```

### ❌ 避免的做法

**1. 过度嵌套滚动查看器**
```cpp
// ❌ 嵌套滚动容易混淆用户
scrollViewer()
    .Content(
        scrollViewer()  // 内部滚动器
            .Content(
                scrollViewer()  // 更深层次滚动器！
            )
    );
```

**2. 固定过小的尺寸**
```cpp
// ❌ 过小的尺寸影响可用性
viewer->Width(50)->Height(50);  // 太小！

// ✅ 合理的尺寸
viewer->Width(300)->Height(400);
```

**3. 禁用必要的滚动条**
```cpp
// ❌ 内容超出但禁用滚动
viewer->Content(veryLargeContent)
      ->VerticalScrollBarVisibility(ScrollBarVisibility::Disabled);  // 用户无法查看完整内容！
```

## 常见问题

**Q: 如何实现"无限滚动"（加载更多数据）？**

A: 监听 ScrollChanged 事件，检测滚动到底部时加载：
```cpp
viewer->OnScrollChanged([viewer](double h, double v) {
    if (v + viewer->GetViewportHeight() >= viewer->GetExtentHeight() - 50) {
        LoadMoreData();
    }
});
```

**Q: 如何禁用滚动？**

A: 设置两个方向的滚动条为 `Disabled`：
```cpp
viewer->VerticalScrollBarVisibility(ScrollBarVisibility::Disabled)
      ->HorizontalScrollBarVisibility(ScrollBarVisibility::Disabled);
```

**Q: 滚动条样式可以自定义吗？**

A: 当前版本使用默认样式，未来版本可能支持自定义。

**Q: 如何实现平滑滚动动画？**

A: 使用 Timer 实现插值动画（参考示例 7）。

**Q: ScrollViewer 和 Panel 有什么区别？**

A: `ScrollViewer` 提供**滚动功能**，`Panel` 提供**布局功能**。通常将 Panel 放入 ScrollViewer 中。

**Q: 内容比可见区域小时滚动条会显示吗？**

A: 如果设置为 `Auto`，则不会显示；如果设置为 `Visible`，则始终显示（但禁用状态）。

## 性能考虑

- **大内容**：ScrollViewer 渲染整个内容，非常大的内容可能影响性能
- **虚拟化**：未来版本可能支持虚拟化列表（只渲染可见项）
- **滚动平滑度**：取决于内容复杂度和渲染性能

**优化建议：**
- 避免在 ScrollViewer 中放置过于复杂的内容
- 考虑分页加载大数据集
- 使用简化的项模板

## 相关类型

- **ContentControl** - 父类
- **ScrollBar** - 内部使用的滚动条控件
- **Panel** - 通常作为 ScrollViewer 的内容

## 总结

`ScrollViewer` 是可滚动的内容容器，提供：

✅ 垂直/水平滚动  
✅ 滚动条可见性控制  
✅ 编程式滚动方法  
✅ 鼠标滚轮支持  
✅ 滚动事件通知  
✅ 链式 API

适用于长文本、大图片、列表等需要滚动的场景。
