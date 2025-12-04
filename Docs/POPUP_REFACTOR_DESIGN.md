# Popup 重构设计文档 - 原生子窗口方案

## 文档信息

- **版本**: 1.0
- **日期**: 2025年12月4日
- **作者**: GitHub Copilot
- **状态**: 设计阶段

## 目录

1. [概述](#概述)
2. [设计目标](#设计目标)
3. [架构设计](#架构设计)
4. [核心组件](#核心组件)
5. [API设计](#api设计)
6. [实现计划](#实现计划)
7. [依赖和前提条件](#依赖和前提条件)
8. [测试策略](#测试策略)
9. [风险评估](#风险评估)
10. [附录](#附录)

---

## 概述

### 背景

当前 F__K_UI 框架的 Popup 实现已被完全移除，需要重新设计和实现一个能够超出窗口边界显示的 Popup 系统。基于对现有渲染架构的分析，采用**原生子窗口方案**作为实现策略。

### 核心挑战

1. **超出窗口边界**: 传统 UI 框架的渲染被限制在窗口边界内
2. **多窗口管理**: 需要管理多个原生窗口的生命周期
3. **坐标转换**: 需要在窗口坐标和屏幕坐标之间转换
4. **输入处理**: 需要处理跨窗口的输入事件
5. **性能优化**: 避免多个 OpenGL 上下文的性能开销

### 设计原则

- **原生体验**: 利用操作系统原生窗口特性，提供系统级别的 Popup 行为
- **跨平台兼容**: 基于 GLFW 抽象层，确保 Windows/Linux 兼容性
- **性能优先**: 最小化渲染开销，优化窗口管理
- **API 一致性**: 保持与 WPF 类似的 API 设计

---

## 设计目标

### 功能目标

1. **超出边界显示**: Popup 能够显示在主窗口边界之外
2. **多显示器支持**: 支持跨多显示器显示
3. **定位灵活性**: 支持多种定位模式（绝对、相对、鼠标等）
4. **层级管理**: 支持嵌套 Popup 和 Z-Order 管理
5. **输入交互**: 支持点击外部关闭、键盘导航等
6. **动画支持**: 支持打开/关闭动画
7. **主题一致性**: 与主窗口保持视觉一致性

### 非功能目标

1. **性能**: 渲染性能不低于主窗口
2. **内存**: 合理的内存占用
3. **响应性**: 流畅的打开/关闭体验
4. **稳定性**: 健壮的错误处理和资源管理

---

## 架构设计

### 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                    应用程序层 (Application)                   │
└─────────────────────┬───────────────────────────────────────┘
                      │
          ┌───────────┼───────────┐
          │           │           │
┌─────────▼─────────┐ ┌─────────▼─────────┐ ┌─────────▼─────────┐
│   主窗口 (Window)  │ │ Popup 服务 (Popup │ │  Popup 控件 (Popup│
│                   │ │   Service)        │ │   Control)        │
│ • GLFW 主窗口      │ │ • 全局管理        │ │ • UI 逻辑         │
│ • OpenGL 上下文    │ │ • 生命周期管理    │ │ • 定位计算        │
│ • 主 UI 树         │ │ • 事件分发        │ │ • 属性绑定        │
└─────────┬─────────┘ └─────────┬─────────┘ └─────────┬─────────┘
          │                     │                     │
          └─────────┬───────────┼───────────┬─────────┘
                    │           │           │
          ┌─────────▼───────────▼───────────▼─────────┐
          │              Popup 根容器 (PopupRoot)       │
          │                                             │
          │ • 无边框原生窗口                            │
          │ • 独立 OpenGL 上下文                        │
          │ • 内容渲染                                  │
          │ • 屏幕坐标定位                              │
          └─────────────────────────────────────────────┘
```

### 数据流

```
用户交互 → Window::ProcessEvents() → PopupService::HandleGlobalMouseDown()
                                      ↓
                                PopupService::Update() → PopupRoot::UpdatePosition()
                                      ↓
                                Popup::CalculateScreenPosition() → 屏幕坐标
                                      ↓
                                PopupRoot::Show(screenPos) → 原生窗口显示
                                      ↓
                                PopupRoot::RenderFrame() → 独立渲染
```

### 组件关系

- **Window**: 主窗口，负责创建和管理 PopupService
- **PopupService**: 单例服务，全局管理所有 Popup
- **Popup**: UI 控件，定义 Popup 的逻辑和属性
- **PopupRoot**: 渲染容器，管理原生窗口和渲染

---

## 核心组件

### 1. PopupRoot - Popup 根容器

#### 职责

- 创建和管理无边框原生窗口
- 提供独立的 OpenGL 渲染上下文
- 处理窗口位置、大小和显示状态
- 管理 Popup 内容的渲染

#### 接口设计

```cpp
class PopupRoot {
public:
    // 生命周期
    void Initialize();
    void Shutdown();

    // 窗口管理
    void Create(int width, int height);
    void Destroy();
    void Show(Point screenPosition);
    void Hide();
    bool IsVisible() const;

    // 内容管理
    void SetContent(UIElement* content);
    UIElement* GetContent() const;

    // 渲染
    void RenderFrame();
    void Resize(int width, int height);

    // 属性
    void SetAllowsTransparency(bool allow);
    void SetTopmost(bool topmost);

private:
    void* nativeHandle_;                    // GLFW 窗口句柄
    std::unique_ptr<GlRenderer> renderer_;  // 独立渲染器
    std::unique_ptr<RenderList> renderList_; // 渲染命令列表
    UIElement* content_;                    // Popup 内容
    bool isVisible_;                        // 显示状态
    bool allowsTransparency_;               // 是否允许透明
    bool isTopmost_;                        // 是否置顶
};
```

#### 实现要点

1. **无边框窗口创建**:
   ```cpp
   glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);  // 无边框
   glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);    // 置顶
   glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // 透明缓冲区
   ```

2. **独立渲染上下文**:
   - 每个 PopupRoot 有独立的 GlRenderer 实例
   - 共享纹理资源以减少内存占用
   - 独立的渲染循环

3. **屏幕坐标定位**:
   - 使用 `glfwSetWindowPos()` 设置屏幕绝对坐标
   - 处理多显示器坐标系统

### 2. PopupService - Popup 服务

#### 职责

- 全局管理所有活动的 Popup
- 处理 Popup 的生命周期
- 管理 Popup 的 Z-Order
- 处理全局输入事件（点击外部关闭）

#### 接口设计

```cpp
class PopupService {
public:
    static PopupService& Instance();

    // Popup 管理
    void RegisterPopup(Popup* popup);
    void UnregisterPopup(Popup* popup);
    void OpenPopup(Popup* popup);
    void ClosePopup(Popup* popup);
    void CloseAllPopups();

    // 更新和渲染
    void Update();           // 每帧调用，更新位置
    void RenderAll();        // 渲染所有 Popup

    // 事件处理
    void HandleGlobalMouseDown(Point screenPos);
    void HandleGlobalKeyDown(Key key);

    // 查询
    bool IsAnyPopupOpen() const;
    Popup* GetTopmostPopup() const;

private:
    std::vector<Popup*> activePopups_;
    std::stack<Popup*> popupStack_;      // 支持嵌套
    std::unordered_map<Popup*, std::unique_ptr<PopupRoot>> popupRoots_;
};
```

#### 实现要点

1. **单例模式**: 确保全局唯一实例
2. **Popup 栈**: 使用栈结构管理嵌套 Popup
3. **事件路由**: 拦截全局鼠标事件，判断是否点击外部
4. **生命周期同步**: 与主窗口的生命周期同步

### 3. Popup - Popup 控件

#### 职责

- 定义 Popup 的 UI 逻辑和属性
- 计算定位位置
- 管理打开/关闭状态
- 提供事件接口

#### 接口设计

```cpp
class Popup : public FrameworkElement<Popup> {
public:
    Popup();
    virtual ~Popup();

    // ========== 依赖属性 ==========

    /// IsOpen 属性：是否打开
    static const binding::DependencyProperty& IsOpenProperty();

    /// Child 属性：Popup 内容
    static const binding::DependencyProperty& ChildProperty();

    /// PlacementTarget 属性：定位参考元素
    static const binding::DependencyProperty& PlacementTargetProperty();

    /// Placement 属性：定位模式
    static const binding::DependencyProperty& PlacementProperty();

    /// HorizontalOffset 属性：水平偏移
    static const binding::DependencyProperty& HorizontalOffsetProperty();

    /// VerticalOffset 属性：垂直偏移
    static const binding::DependencyProperty& VerticalOffsetProperty();

    /// StaysOpen 属性：是否保持打开状态
    static const binding::DependencyProperty& StaysOpenProperty();

    /// AllowsTransparency 属性：是否允许透明
    static const binding::DependencyProperty& AllowsTransparencyProperty();

    // ========== 属性访问器 ==========

    bool GetIsOpen() const;
    void SetIsOpen(bool value);
    void Open();
    void Close();

    UIElement* GetChild() const;
    void SetChild(UIElement* value);

    UIElement* GetPlacementTarget() const;
    void SetPlacementTarget(UIElement* value);

    PlacementMode GetPlacement() const;
    void SetPlacement(PlacementMode value);

    float GetHorizontalOffset() const;
    void SetHorizontalOffset(float value);

    float GetVerticalOffset() const;
    void SetVerticalOffset(float value);

    bool GetStaysOpen() const;
    void SetStaysOpen(bool value);

    bool GetAllowsTransparency() const;
    void SetAllowsTransparency(bool value);

    // ========== 事件 ==========

    core::Event<> Opened;      // Popup 打开事件
    core::Event<> Closed;      // Popup 关闭事件

protected:
    // 框架方法重写
    virtual Size MeasureOverride(const Size& availableSize) override;
    virtual Size ArrangeOverride(const Size& finalSize) override;
    virtual void OnRender(render::RenderContext& context) override;

private:
    // 私有方法
    Point CalculateScreenPosition();
    void UpdatePopupRoot();
    void OnIsOpenChanged(bool oldValue, bool newValue);

    // 属性变更回调
    static void OnIsOpenChangedCallback(
        binding::DependencyObject& d,
        const binding::DependencyProperty& prop,
        const std::any& oldValue,
        const std::any& newValue
    );
};
```

#### 定位模式枚举

```cpp
enum class PlacementMode {
    Absolute,           // 绝对屏幕坐标
    Relative,           // 相对于 PlacementTarget
    Bottom,             // PlacementTarget 下方
    Top,                // PlacementTarget 上方
    Left,               // PlacementTarget 左侧
    Right,              // PlacementTarget 右侧
    Center,             // PlacementTarget 中心
    Mouse,              // 当前鼠标位置
    MousePoint,         // 鼠标点击位置（需要记录点击点）
    Custom              // 自定义定位（通过事件）
};
```

### 4. 坐标系统扩展

#### UIElement 扩展

```cpp
class UIElement {
public:
    // 坐标转换方法
    Point PointToScreen(Point localPoint) const;
    Point PointFromScreen(Point screenPoint) const;
    Rect GetBoundsOnScreen() const;

protected:
    // 辅助方法
    Point TransformToRoot(Point localPoint) const;
    Point TransformFromRoot(Point rootPoint) const;
};
```

#### Window 扩展

```cpp
class Window {
public:
    // Popup 服务访问
    PopupService* GetPopupService() const;

    // 坐标转换
    Point ClientToScreen(Point clientPoint) const;
    Point ScreenToClient(Point screenPoint) const;

    // 窗口信息
    Rect GetWindowBoundsOnScreen() const;
    int GetCurrentMonitor() const;

protected:
    // 事件处理扩展
    virtual void OnPopupOpened(Popup* popup);
    virtual void OnPopupClosed(Popup* popup);
};
```

---

## API设计

### 基本用法

```cpp
// 创建 Popup
auto popup = new Popup();

// 设置内容
auto stackPanel = new StackPanel();
stackPanel->AddChild(new TextBlock()->Text("Item 1"));
stackPanel->AddChild(new TextBlock()->Text("Item 2"));
popup->SetChild(stackPanel);

// 设置定位
popup->SetPlacementTarget(button);  // 相对于按钮定位
popup->SetPlacement(PlacementMode::Bottom);  // 显示在下方
popup->SetHorizontalOffset(10);     // 水平偏移
popup->SetVerticalOffset(5);        // 垂直偏移

// 设置行为
popup->SetStaysOpen(false);         // 点击外部自动关闭

// 绑定到按钮点击
button->Click += [popup]() {
    popup->SetIsOpen(!popup->GetIsOpen());
};

// 监听事件
popup->Opened += []() { std::cout << "Popup opened" << std::endl; };
popup->Closed += []() { std::cout << "Popup closed" << std::endl; };
```

### 高级用法

```cpp
// 自定义定位
popup->SetPlacement(PlacementMode::Absolute);
popup->SetHorizontalOffset(100);  // 屏幕 X 坐标
popup->SetVerticalOffset(200);    // 屏幕 Y 坐标

// 透明 Popup
popup->SetAllowsTransparency(true);

// 保持打开状态
popup->SetStaysOpen(true);

// 嵌套 Popup
auto nestedPopup = new Popup();
nestedPopup->SetPlacementTarget(parentItem);
nestedPopup->SetPlacement(PlacementMode::Right);
```

### 工厂函数

```cpp
namespace fk::factory {

// Popup 工厂函数
inline ui::Popup* Popup() {
    return new ui::Popup();
}

} // namespace fk::factory
```

---

## 实现计划

### Phase 1: 基础设施 (3天)

#### Day 1: PopupRoot 基础实现
- [ ] 创建 PopupRoot 类
- [ ] 实现无边框 GLFW 窗口创建
- [ ] 配置 OpenGL 上下文
- [ ] 实现基本的显示/隐藏功能

#### Day 2: 坐标系统扩展
- [ ] 实现 UIElement::PointToScreen()
- [ ] 实现 Window::ClientToScreen()
- [ ] 添加多显示器支持
- [ ] 处理 DPI 缩放

#### Day 3: PopupService 框架
- [ ] 创建 PopupService 单例
- [ ] 实现基本的注册/注销逻辑
- [ ] 集成到 Window 的消息循环

### Phase 2: Popup 控件核心 (4天)

#### Day 4-5: Popup 控件实现
- [ ] 创建 Popup 类和依赖属性
- [ ] 实现基本的打开/关闭逻辑
- [ ] 实现定位计算算法
- [ ] 添加事件支持

#### Day 6: 定位模式实现
- [ ] 实现所有 PlacementMode
- [ ] 处理边界检测和调整
- [ ] 支持自定义定位

### Phase 3: 高级功能 (3天)

#### Day 7: 输入处理和交互
- [ ] 实现点击外部关闭
- [ ] 添加键盘导航支持
- [ ] 处理焦点管理

#### Day 8: 动画和视觉效果
- [ ] 实现打开/关闭动画
- [ ] 支持透明度设置
- [ ] 添加阴影效果

#### Day 9: 性能优化
- [ ] 优化渲染性能
- [ ] 实现资源共享
- [ ] 添加内存管理

### Phase 4: 测试和完善 (2天)

#### Day 10: 集成测试
- [ ] 创建示例应用
- [ ] 测试各种定位模式
- [ ] 验证跨显示器支持

#### Day 11: 边界情况和错误处理
- [ ] 处理异常情况
- [ ] 改进错误处理
- [ ] 性能基准测试

### 总计: 11个工作日

---

## 依赖和前提条件

### 编译依赖

- **GLFW 3.3+**: 多窗口支持
- **GLAD**: OpenGL 函数加载
- **现有框架**: UIElement, FrameworkElement, DependencyProperty

### 运行时依赖

- **OpenGL 3.3+**: 渲染支持
- **操作系统**: Windows/Linux 原生窗口 API

### 前提条件

1. **GLFW 多窗口**: 确保 GLFW 支持创建多个窗口
2. **OpenGL 上下文**: 支持多个独立上下文
3. **屏幕坐标**: 操作系统提供屏幕坐标 API
4. **窗口管理**: 支持无边框、透明、置顶窗口

---

## 测试策略

### 单元测试

```cpp
// PopupRoot 测试
TEST(PopupRootTest, CreateDestroy) {
    PopupRoot root;
    root.Create(200, 100);
    EXPECT_TRUE(root.IsCreated());
    root.Destroy();
    EXPECT_FALSE(root.IsCreated());
}

TEST(PopupRootTest, ShowHide) {
    PopupRoot root;
    root.Create(200, 100);
    root.Show(Point(100, 100));
    EXPECT_TRUE(root.IsVisible());
    root.Hide();
    EXPECT_FALSE(root.IsVisible());
}

// Popup 定位测试
TEST(PopupTest, PlacementBottom) {
    auto button = new Button();
    button->SetBounds(Rect(50, 50, 100, 30));

    auto popup = new Popup();
    popup->SetPlacementTarget(button);
    popup->SetPlacement(PlacementMode::Bottom);

    Point pos = popup->CalculateScreenPosition();
    EXPECT_EQ(pos.x, 50);
    EXPECT_EQ(pos.y, 80);  // button.y + button.height
}
```

### 集成测试

1. **基本功能测试**
   - Popup 打开/关闭
   - 各种定位模式
   - 点击外部关闭

2. **边界情况测试**
   - 窗口边缘定位
   - 多显示器跨越
   - 屏幕边界调整

3. **性能测试**
   - 多个 Popup 同时显示
   - 快速打开/关闭
   - 内存泄漏检测

### 手动测试用例

1. 创建一个包含按钮的窗口
2. 点击按钮打开 Popup
3. 验证 Popup 显示在正确位置
4. 在 Popup 外部点击，验证关闭
5. 测试不同的定位模式
6. 调整窗口大小，验证 Popup 位置更新

---

## 风险评估

### 高风险项目

1. **多 OpenGL 上下文管理**
   - **风险**: 上下文切换开销，资源竞争
   - **缓解**: 限制同时活动的 Popup 数量，使用共享上下文

2. **跨平台窗口管理**
   - **风险**: Windows/Linux 行为差异
   - **缓解**: 充分测试所有目标平台，抽象平台差异

3. **坐标系统复杂性**
   - **风险**: 多显示器、DPI 缩放导致坐标错误
   - **缓解**: 实现完善的坐标转换测试，处理边界情况

### 中风险项目

1. **性能影响**
   - **风险**: 多个窗口增加 CPU/GPU 开销
   - **缓解**: 实现 Popup 池化，优化渲染频率

2. **内存管理**
   - **风险**: PopupRoot 生命周期管理不当导致泄漏
   - **缓解**: 使用智能指针，添加引用计数

### 低风险项目

1. **API 设计**
   - **风险**: 与现有框架不一致
   - **缓解**: 遵循现有模式，保持向后兼容

2. **用户体验**
   - **风险**: 系统行为与用户期望不符
   - **缓解**: 参考 WPF/Qt 实现，提供一致体验

---

## 附录

### A. 参考资料

1. **WPF Popup**: https://docs.microsoft.com/en-us/dotnet/api/system.windows.controls.popup
2. **Qt QMenu**: https://doc.qt.io/qt-5/qmenu.html
3. **Electron BrowserWindow**: https://www.electronjs.org/docs/api/browser-window

### B. 术语表

- **PlacementTarget**: 定位参考元素
- **PlacementMode**: 定位模式
- **PopupRoot**: Popup 的原生窗口容器
- **PopupService**: 全局 Popup 管理服务
- **屏幕坐标**: 以屏幕左上角为原点的绝对坐标
- **客户端坐标**: 以窗口左上角为原点的相对坐标

### C. 变更历史

| 版本 | 日期 | 变更内容 | 作者 |
|------|------|---------|------|
| 1.0 | 2025-12-04 | 初始设计文档 | GitHub Copilot |

---

*本文档为 F__K_UI Popup 重构的设计规范，所有实现应以此为准。*</content>
<parameter name="filePath">g:\Documents\Visual Studio Code\F__K_UI\Docs\POPUP_REFACTOR_DESIGN.md