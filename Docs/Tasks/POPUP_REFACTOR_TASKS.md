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

### Day 2: 坐标系统扩展 ✅
**目标**: 实现 UI 元素坐标到屏幕绝对坐标的转换。

- [x] **Window 类扩展**
  - [x] 修改 `include/fk/ui/Window.h`
  - [x] 实现 `Point ClientToScreen(Point clientPoint)`: 使用 `glfwGetWindowPos`
  - [x] 实现 `Point ScreenToClient(Point screenPoint)`
  - [x] 实现 `Rect GetWindowBoundsOnScreen()`
  - [x] 考虑 DPI 缩放因子 (当前版本未实现，预留接口)
- [x] **UIElement 类扩展**
  - [x] 修改 `include/fk/ui/base/UIElement.h`
  - [x] 实现 `Point PointToScreen(Point localPoint)`
    - [x] 递归计算 `TransformToRoot` 获取相对于 Window 的坐标
    - [x] 调用 `Window::ClientToScreen`
  - [x] 实现 `Point PointFromScreen(Point screenPoint)` (逆向转换)
  - [x] 实现 `Point TransformToRoot(Point localPoint)` (累积布局偏移)
  - [x] 实现 `Point TransformFromRoot(Point rootPoint)` (逆向累积)
  - [x] 实现 `Window* GetRootWindow()` (visual tree 遍历)
  - [x] 实现 `Rect GetBoundsOnScreen()`
- [x] **测试验证**
  - [x] 创建 `examples/popup/coordinate_test.cpp`
  - [x] Window 坐标方法测试通过 (ClientToScreen/ScreenToClient 往返正确)
  - **注意**: UIElement 的坐标方法依赖完整 visual tree 和布局完成，在实际 Popup 场景中验证

### Day 3: PopupService 框架 ✅
**目标**: 建立全局 Popup 管理中心。

- [x] **文件创建**
  - [x] `include/fk/ui/PopupService.h`
  - [x] `src/ui/PopupService.cpp`
- [x] **单例模式**
  - [x] 实现 `static PopupService& Instance()` - Meyers Singleton 模式
- [x] **注册管理**
  - [x] 定义 `std::vector<Popup*> activePopups_`
  - [x] 实现 `RegisterPopup(Popup*)` - 支持重复注册检测
  - [x] 实现 `UnregisterPopup(Popup*)`
  - [x] 实现 `IsRegistered(const Popup*)` - 查询注册状态
  - [x] 实现 `GetActivePopups()` - 获取活跃 Popup 列表
  - [x] 实现 `CloseAll()` - 关闭所有 Popup
- [x] **生命周期集成**
  - [x] 在 `Window::ProcessEvents` 中添加 `PopupService::Instance().Update()` 调用
  - [x] 在 `Window::RenderFrame` 后添加 `PopupService::Instance().RenderAll()` 调用
- [x] **测试验证**
  - [x] 创建 `examples/popup/popupservice_test.cpp`
  - [x] 单例模式测试通过
  - [x] 注册/注销功能测试通过（包括重复注册检测）
  - [x] Update/RenderAll 空调用测试通过
  - [x] Window 集成测试通过（5 帧事件循环）
  - [x] CloseAll 功能测试通过
  - **注意**: Update() 和 RenderAll() 当前为空实现，将在 Day 4-5 实现 Popup 类后完善

---

## Phase 2: Popup 控件核心 (4天)

### Day 4: Popup 控件结构 ✅
**目标**: 实现 Popup 控件的基本属性和逻辑。

- [x] **文件创建**
  - [x] `include/fk/ui/controls/Popup.h`
  - [x] `src/ui/controls/Popup.cpp`
- [x] **类定义**
  - [x] 继承自 `FrameworkElement<Popup>`
  - [x] 在 `FrameworkElement.cpp` 中添加显式模板实例化
- [x] **PlacementMode 枚举**
  - [x] Absolute, Relative, Bottom, Top, Right, Left, Center, Mouse
- [x] **依赖属性定义**
  - [x] `IsOpenProperty` (bool) - 默认 false
  - [x] `ChildProperty` (UIElement*)
  - [x] `PlacementTargetProperty` (UIElement*)
  - [x] `PlacementProperty` (PlacementMode) - 默认 Bottom
  - [x] `HorizontalOffsetProperty` (float)
  - [x] `VerticalOffsetProperty` (float)
  - [x] `StaysOpenProperty` (bool) - 默认 true
  - [x] Width/Height 继承自 FrameworkElement (默认 200x150)
- [x] **属性回调**
  - [x] 实现 `OnIsOpenChanged`: 触发 Open/Close 逻辑
  - [x] 实现 `OnChildChanged`: 更新 PopupRoot 内容
- [x] **Open/Close 逻辑**
  - [x] `Open()`: 创建 PopupRoot，设置内容，计算位置，显示，注册到 PopupService
  - [x] `Close()`: 隐藏 PopupRoot，从 PopupService 注销
  - [x] PopupRoot 复用机制（关闭时保留，重新打开时复用）
- [x] **事件系统**
  - [x] `Opened` 事件 - 使用 `core::Event<>()`
  - [x] `Closed` 事件
- [x] **定位计算 (初步)**
  - [x] 实现 `CalculateScreenPosition()` - 支持所有 PlacementMode
  - [x] Absolute 模式（直接使用偏移）
  - [x] 其他模式使用 `target->GetBoundsOnScreen()` + 偏移计算
- [x] **PopupService 集成**
  - [x] 更新 `PopupService::Update()` 调用 `Popup::UpdatePopup()`
  - [x] 更新 `PopupService::RenderAll()` 调用 `Popup::RenderPopup()`
  - [x] 更新 `PopupService::CloseAll()` 调用 `Popup::SetIsOpen(false)`
- [x] **测试验证**
  - [x] 创建 `examples/popup/popup_test.cpp`
  - [x] 属性设置测试通过 (Width:300, Height:200)
  - [x] 内容创建测试通过 (Border + TextBlock)
  - [x] 打开/关闭逻辑测试通过
  - [x] 事件触发测试通过 (Opened/Closed ✓)
  - [x] PopupService 集成测试通过 (注册/注销)
  - [x] 渲染测试通过 (91 frames @ ~60fps for 3 seconds)
  - [x] 重新打开测试通过 (PopupRoot 复用)

### Day 5: 定位逻辑实现
**目标**: 实现各种定位模式的坐标计算。

- [x] **定位算法**
  - [x] 实现 `Point CalculateScreenPosition()`
- [x] **模式实现**
  - [x] `PlacementMode::Absolute`: 使用 Offset 作为屏幕坐标
  - [x] `PlacementMode::Relative`: 相对于 Target 左上角 + Offset
  - [x] `PlacementMode::Bottom`: Target 左下角
  - [x] `PlacementMode::Right`: Target 右上角
  - [x] `PlacementMode::Center`: Target 中心
  - [x] `PlacementMode::Mouse`: 当前鼠标位置
- [x] **边界检测 (初步)**
  - [x] 确保 Popup 不会出现在屏幕外 (简单 clamp)
- [x] **测试验证**
  - [x] 创建 `examples/popup/popup_placement_test.cpp`
  - [x] 所有 PlacementMode 测试通过
  - [x] 边界检测测试通过 (极端坐标 x=10000, y=10000)

### Day 6: 事件系统与集成
**目标**: 完善事件通知和输入集成。

- [x] **事件定义**
  - [x] `Opened` 事件 (已在 Day 4 完成)
  - [x] `Closed` 事件 (已在 Day 4 完成)
- [x] **输入传递**
  - [x] 为 PopupRoot 添加 InputManager 成员
  - [x] 在 PopupRoot::Create 中设置 GLFW 输入回调 (MouseButton, CursorPos, Scroll, Key, Char)
  - [x] 实现 PopupRoot::ProcessEvents() 方法
  - [x] PopupService::Update() 调用 PopupRoot::ProcessEvents()
  - [x] 每个 PopupRoot 都有独立的 InputManager
- [x] **测试验证**
  - [x] 创建 `examples/popup/popup_input_test.cpp`
  - [x] 验证 InputManager 创建成功
  - [x] 验证多个 Popup 的 InputManager 独立性
  - [x] GLFW 输入回调已设置

### Day 7: 完善 PopupService
**目标**: 实现点击外部关闭等交互逻辑。

- [x] **全局点击检测**
  - [x] 实现 `HandleGlobalMouseDown(int screenX, int screenY)`
  - [x] 遍历所有 Active Popups
  - [x] 检查点击位置是否在 PopupRoot 范围内
  - [x] 实现 `PopupRoot::ContainsScreenPoint(int, int)`
  - [x] 如果不在且 `StaysOpen == false`，关闭 Popup
- [x] **Window 集成**
  - [x] 在 `Window` 的鼠标按下事件中调用 `PopupService::HandleGlobalMouseDown`
  - [x] 将窗口客户区坐标转换为屏幕坐标
- [x] **测试验证**
  - [x] 创建 `examples/popup/popup_staysopen_test.cpp`
  - [x] 测试 StaysOpen=true 的 Popup 不会因点击外部关闭
  - [x] 测试 StaysOpen=false 的 Popup 点击外部自动关闭
  - [x] 测试多个 StaysOpen=false 的 Popup 同时关闭
  - [x] 验证点击 Popup 内部不会关闭

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
