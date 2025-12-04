# Popup 重构详细开发计划

## 📅 总体时间表

- **预计开始时间**: 2025-12-05
- **预计结束时间**: 2025-12-19
- **总工时**: 11 人天

---

## Phase 1: 基础设施 (3天)

### Day 1: PopupRoot 基础实现
**目标**: 创建能够独立渲染内容的无边框原生窗口。

- [ ] **文件创建**
  - [ ] `include/fk/ui/window/PopupRoot.h`
  - [ ] `src/ui/window/PopupRoot.cpp`
- [ ] **PopupRoot 类结构**
  - [ ] 定义 `PopupRoot` 类
  - [ ] 引入 `GlRenderer`, `RenderList`, `UIElement`
- [ ] **窗口创建 (GLFW)**
  - [ ] 实现 `Create(int width, int height)`
  - [ ] 设置 GLFW 窗口提示: `GLFW_DECORATED = false` (无边框)
  - [ ] 设置 GLFW 窗口提示: `GLFW_FLOATING = true` (置顶)
  - [ ] 设置 GLFW 窗口提示: `GLFW_TRANSPARENT_FRAMEBUFFER = true` (透明支持)
  - [ ] 设置 GLFW 窗口提示: `GLFW_VISIBLE = false` (初始隐藏)
- [ ] **窗口管理**
  - [ ] 实现 `Destroy()`: 销毁 GLFW 窗口和渲染资源
  - [ ] 实现 `Show(Point screenPos)`: 设置位置并显示
  - [ ] 实现 `Hide()`: 隐藏窗口
  - [ ] 实现 `SetSize(int width, int height)`
- [ ] **渲染循环**
  - [ ] 实现 `RenderFrame()`
  - [ ] 初始化独立的 `GlRenderer` 实例
  - [ ] 创建 `RenderList` 和 `RenderContext`
  - [ ] 执行 `content->Measure()` 和 `content->Arrange()`
  - [ ] 执行 `content->CollectDrawCommands()`
  - [ ] 调用 `renderer->BeginFrame()`, `renderer->Draw()`, `renderer->EndFrame()`
  - [ ] 调用 `glfwSwapBuffers()`

### Day 2: 坐标系统扩展
**目标**: 实现 UI 元素坐标到屏幕绝对坐标的转换。

- [ ] **Window 类扩展**
  - [ ] 修改 `include/fk/ui/Window.h`
  - [ ] 实现 `Point ClientToScreen(Point clientPoint)`: 使用 `glfwGetWindowPos`
  - [ ] 实现 `Point ScreenToClient(Point screenPoint)`
  - [ ] 考虑 DPI 缩放因子 (如果框架已支持)
- [ ] **UIElement 类扩展**
  - [ ] 修改 `include/fk/ui/UIElement.h`
  - [ ] 实现 `Point PointToScreen(Point localPoint)`
    - [ ] 递归计算 `TransformToRoot` 获取相对于 Window 的坐标
    - [ ] 调用 `Window::ClientToScreen`
  - [ ] 实现 `Rect GetBoundsOnScreen()`

### Day 3: PopupService 框架
**目标**: 建立全局 Popup 管理中心。

- [ ] **文件创建**
  - [ ] `include/fk/ui/PopupService.h`
  - [ ] `src/ui/PopupService.cpp`
- [ ] **单例模式**
  - [ ] 实现 `static PopupService& Instance()`
- [ ] **注册管理**
  - [ ] 定义 `std::vector<Popup*> activePopups_`
  - [ ] 实现 `RegisterPopup(Popup*)`
  - [ ] 实现 `UnregisterPopup(Popup*)`
- [ ] **生命周期集成**
  - [ ] 在 `Window::ProcessEvents` 中添加 `PopupService::Instance().Update()` 调用
  - [ ] 在 `Window::RenderFrame` 后添加 `PopupService::Instance().RenderAll()` (或者让 PopupRoot 自己管理渲染循环)

---

## Phase 2: Popup 控件核心 (4天)

### Day 4: Popup 控件结构
**目标**: 实现 Popup 控件的基本属性和逻辑。

- [ ] **文件创建**
  - [ ] `include/fk/ui/controls/Popup.h`
  - [ ] `src/ui/controls/Popup.cpp`
- [ ] **类定义**
  - [ ] 继承自 `FrameworkElement`
- [ ] **依赖属性定义**
  - [ ] `IsOpenProperty` (bool)
  - [ ] `ChildProperty` (UIElement*)
  - [ ] `PlacementTargetProperty` (UIElement*)
  - [ ] `PlacementProperty` (Enum)
  - [ ] `HorizontalOffsetProperty` (float)
  - [ ] `VerticalOffsetProperty` (float)
  - [ ] `StaysOpenProperty` (bool)
- [ ] **属性回调**
  - [ ] 实现 `OnIsOpenChanged`: 触发 Open/Close 逻辑
  - [ ] 实现 `OnChildChanged`: 更新 PopupRoot 内容
- [ ] **Open/Close 逻辑**
  - [ ] `Open()`: 创建/获取 PopupRoot，计算位置，显示
  - [ ] `Close()`: 隐藏 PopupRoot

### Day 5: 定位逻辑实现
**目标**: 实现各种定位模式的坐标计算。

- [ ] **定位算法**
  - [ ] 实现 `Point CalculateScreenPosition()`
- [ ] **模式实现**
  - [ ] `PlacementMode::Absolute`: 使用 Offset 作为屏幕坐标
  - [ ] `PlacementMode::Relative`: 相对于 Target 左上角 + Offset
  - [ ] `PlacementMode::Bottom`: Target 左下角
  - [ ] `PlacementMode::Right`: Target 右上角
  - [ ] `PlacementMode::Center`: Target 中心
  - [ ] `PlacementMode::Mouse`: 当前鼠标位置
- [ ] **边界检测 (初步)**
  - [ ] 确保 Popup 不会出现在屏幕外 (简单 clamp)

### Day 6: 事件系统与集成
**目标**: 完善事件通知和输入集成。

- [ ] **事件定义**
  - [ ] `Opened` 事件
  - [ ] `Closed` 事件
- [ ] **输入传递**
  - [ ] 确保 PopupRoot 能接收鼠标/键盘输入
  - [ ] 验证 `InputManager` 在多窗口下的行为 (可能需要为每个 PopupRoot 创建 InputManager)

### Day 7: 完善 PopupService
**目标**: 实现点击外部关闭等交互逻辑。

- [ ] **全局点击检测**
  - [ ] 实现 `HandleGlobalMouseDown(Point screenPos)`
  - [ ] 遍历所有 Active Popups
  - [ ] 检查点击位置是否在 PopupRoot 范围内
  - [ ] 如果不在且 `StaysOpen == false`，关闭 Popup
- [ ] **Window 集成**
  - [ ] 在 `Window` 的鼠标事件处理中调用 `PopupService`

---

## Phase 3: 高级功能 (3天)

### Day 8: 动画与透明度
**目标**: 提升视觉体验。

- [ ] **透明度支持**
  - [ ] 验证 `GLFW_TRANSPARENT_FRAMEBUFFER` 是否生效
  - [ ] 实现 `AllowsTransparency` 属性
- [ ] **简单动画**
  - [ ] 实现 Popup 打开时的淡入效果 (Opacity 0 -> 1)
  - [ ] 实现 Popup 关闭时的淡出效果

### Day 9: 屏幕边界处理
**目标**: 智能调整 Popup 位置以避免被遮挡。

- [ ] **获取屏幕信息**
  - [ ] 使用 `glfwGetMonitors` 获取显示器列表和工作区大小
- [ ] **碰撞检测**
  - [ ] 在 `CalculateScreenPosition` 中检测是否超出屏幕
  - [ ] 实现自动翻转逻辑 (例如 Bottom 超出则变为 Top)

### Day 10: 性能优化
**目标**: 确保多窗口渲染不卡顿。

- [ ] **资源共享**
  - [ ] 尝试配置 GLFW 共享 Context (主窗口和 Popup 窗口共享纹理/Shader)
- [ ] **按需渲染**
  - [ ] 只有当 Popup 内容变化或需要重绘时才执行 `RenderFrame`

---

## Phase 4: 测试与完善 (2天)

### Day 11: ComboBox 重构验证
**目标**: 使用新 Popup 重新实现 ComboBox。

- [ ] **ComboBox 恢复**
  - [ ] 重新创建 `ComboBox.h` / `ComboBox.cpp`
  - [ ] 使用新 `Popup` 控件作为下拉容器
  - [ ] 验证下拉列表是否能超出主窗口
- [ ] **ListBox 集成**
  - [ ] 在 Popup 中放入 ListBox
  - [ ] 验证选择事件

### Day 12: 综合测试与 Bug 修复
**目标**: 稳定性和兼容性测试。

- [ ] **多显示器测试**
  - [ ] 在副显示器上打开 Popup
  - [ ] 跨显示器移动窗口
- [ ] **压力测试**
  - [ ] 快速连续打开/关闭
  - [ ] 同时打开多个 Popup
- [ ] **文档更新**
  - [ ] 更新 API 文档
  - [ ] 编写使用示例

---

## 📝 每日检查清单

- [ ] 代码是否编译通过？
- [ ] 是否有内存泄漏 (特别是原生窗口资源)？
- [ ] 坐标计算是否准确？
- [ ] 是否影响了主窗口的渲染？
