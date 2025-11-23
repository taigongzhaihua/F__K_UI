# ScrollViewer 重构设计文档

## 概述

本文档描述 ScrollViewer 和 ScrollBar 组件的重构设计，采用类似 WPF 的架构。

## 设计目标

1. **职责分离**：清晰分离滚动逻辑、内容呈现和 UI 控件
2. **可模板化**：支持通过样式和模板完全自定义外观
3. **可扩展性**：易于添加新功能（如触摸滚动、惯性滚动等）
4. **WPF 兼容**：API 设计尽可能与 WPF 保持一致

## 架构设计

### 组件层次

```
ScrollViewer (顶层容器)
├── ScrollContentPresenter (内容呈现器) [TODO]
│   └── Content (实际内容)
├── VerticalScrollBar (垂直滚动条)
│   └── Track [TODO]
│       ├── DecreaseRepeatButton (向上按钮) [TODO]
│       ├── Thumb (滑块) [TODO]
│       └── IncreaseRepeatButton (向下按钮) [TODO]
└── HorizontalScrollBar (水平滚动条)
    └── Track [TODO]
        ├── DecreaseRepeatButton (向左按钮) [TODO]
        ├── Thumb (滑块) [TODO]
        └── IncreaseRepeatButton (向右按钮) [TODO]
```

### 核心组件职责

#### 1. ScrollViewer
- **职责**：
  - 作为顶层容器管理滚动策略
  - 管理滚动条的可见性
  - 提供滚动 API（LineUp、PageDown、ScrollToTop 等）
  - 处理鼠标滚轮事件
  - 维护滚动状态（offset、viewport、extent）

- **与 WPF 的对应**：`System.Windows.Controls.ScrollViewer`

- **主要属性**：
  - `HorizontalScrollBarVisibility`: 水平滚动条可见性
  - `VerticalScrollBarVisibility`: 垂直滚动条可见性
  - `HorizontalOffset`: 当前水平偏移（只读）
  - `VerticalOffset`: 当前垂直偏移（只读）
  - `ViewportWidth/Height`: 视口大小（只读）
  - `ExtentWidth/Height`: 内容总大小（只读）
  - `ScrollableWidth/Height`: 可滚动范围（只读）
  - `CanContentScroll`: 是否支持逻辑滚动

#### 2. ScrollContentPresenter [TODO - Phase 2]
- **职责**：
  - 负责内容的实际裁剪和偏移
  - 测量和排列内容
  - 计算 extent 和 viewport 尺寸
  - 将滚动偏移转换为内容的 RenderTransform

- **与 WPF 的对应**：`System.Windows.Controls.ScrollContentPresenter`

- **关键特性**：
  - 实现 `IScrollInfo` 接口（如果支持逻辑滚动）
  - 处理内容的裁剪边界
  - 提供物理滚动（像素级）和逻辑滚动（项目级）两种模式

#### 3. ScrollBar
- **职责**：
  - 显示滚动位置和范围
  - 提供交互式滚动控制
  - 基于 RangeBase 概念管理值范围
  - 完全可模板化

- **与 WPF 的对应**：`System.Windows.Controls.Primitives.ScrollBar`

- **主要属性**：
  - `Orientation`: 方向（水平/垂直）
  - `Minimum/Maximum`: 值范围
  - `Value`: 当前值
  - `ViewportSize`: 视口大小（决定 Thumb 大小）
  - `SmallChange`: 小增量
  - `LargeChange`: 大增量

#### 4. Track [TODO - Phase 3]
- **职责**：
  - 布局和管理滚动条的三个部分
  - 计算 Thumb 的位置和大小
  - 处理轨道点击事件

- **与 WPF 的对应**：`System.Windows.Controls.Primitives.Track`

#### 5. Thumb [TODO - Phase 3]
- **职责**：
  - 提供拖动交互
  - 触发 DragStarted、DragDelta、DragCompleted 事件

- **与 WPF 的对应**：`System.Windows.Controls.Primitives.Thumb`

#### 6. RepeatButton [TODO - Phase 3]
- **职责**：
  - 按住时重复触发命令
  - 用于滚动条两端的递增/递减按钮

- **与 WPF 的对应**：`System.Windows.Controls.Primitives.RepeatButton`

## 实现阶段

### Phase 1: 基础架构和接口定义 ✓
- [x] 定义 ScrollViewer 类接口
- [x] 定义 ScrollBar 类接口
- [x] 定义 ScrollBarVisibility 枚举
- [x] 文档化设计决策
- [x] 删除旧实现

### Phase 2: ScrollContentPresenter 实现
- [ ] 实现 ScrollContentPresenter 类
- [ ] 实现内容测量和排列逻辑
- [ ] 实现内容裁剪和偏移
- [ ] 实现 extent/viewport 计算

### Phase 3: 滚动条基础实现
- [ ] 实现 ScrollBar 基础渲染
- [ ] 实现 Track 布局逻辑
- [ ] 实现 Thumb 拖动交互
- [ ] 实现 RepeatButton 重复点击

### Phase 4: 集成和事件处理
- [ ] 连接 ScrollViewer 和 ScrollBar
- [ ] 实现滚动事件传播
- [ ] 实现鼠标滚轮支持
- [ ] 实现键盘导航（方向键、PageUp/Down）

### Phase 5: 模板和样式
- [ ] 实现 ControlTemplate 支持
- [ ] 创建默认滚动条样式
- [ ] 支持动态切换样式
- [ ] 添加视觉状态（Hover、Pressed、Disabled）

### Phase 6: 高级功能
- [ ] 延迟滚动（IsDeferredScrollingEnabled）
- [ ] 惯性滚动
- [ ] 触摸滚动支持
- [ ] 平滑滚动动画
- [ ] 虚拟化支持（与 VirtualizingPanel 集成）

## 与旧实现的对比

### 旧架构的问题
1. **职责混乱**：ScrollViewer 直接管理 ScrollBar 实例和渲染
2. **不可扩展**：外观硬编码，无法自定义
3. **缺少中间层**：没有 ScrollContentPresenter，逻辑复杂
4. **事件处理简单**：直接回调，不支持路由事件

### 新架构的优势
1. **清晰的职责分离**：每个组件有明确的职责
2. **完全可模板化**：支持自定义外观
3. **标准化设计**：遵循 WPF 设计模式，易于理解
4. **易于扩展**：新功能可以作为独立组件添加

## API 兼容性

### 保持兼容的 API
- `LineUp()`, `LineDown()`, `PageUp()`, `PageDown()`
- `ScrollToTop()`, `ScrollToBottom()`
- `GetHorizontalOffset()`, `GetVerticalOffset()`

### 新增的 API
- `ScrollToElement(UIElement*)`: 滚动到指定元素
- `GetScrollableWidth()`, `GetScrollableHeight()`: 可滚动范围
- 附加属性支持（用于在子元素上设置滚动行为）

### 更改的行为
- 滚动条现在通过模板定义，而不是直接创建
- 滚动偏移现在是只读的，通过滚动方法修改

## 使用示例

### 基本使用
```cpp
auto scrollViewer = new ScrollViewer();
scrollViewer->SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
scrollViewer->SetContent(myLargeContent);
```

### 程序化滚动
```cpp
scrollViewer->ScrollToVerticalOffset(100.0f);
scrollViewer->ScrollToBottom();
scrollViewer->LineDown();
```

### 自定义滚动条外观（未来）
```cpp
auto style = new Style(typeid(ScrollBar));
// 定义自定义模板...
scrollViewer->SetStyle(style);
```

## 参考资料

- [WPF ScrollViewer](https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.scrollviewer)
- [WPF ScrollBar](https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.primitives.scrollbar)
- [WPF ScrollContentPresenter](https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.scrollcontentpresenter)
- [WPF IScrollInfo](https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.primitives.iscrollinfo)

## 下一步行动

1. 完成当前 Phase 1（接口定义和文档）
2. 更新引用这些类的其他文件
3. 确保项目可以编译
4. 开始 Phase 2 的实现
