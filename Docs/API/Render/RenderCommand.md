# RenderCommand - 渲染命令

## 概述

`RenderCommand` 定义了渲染器执行的原子操作。所有 UI 绘制都转换为渲染命令序列,然后提交给渲染器执行。

**命名空间**: `fk::render`  
**头文件**: `fk/render/RenderCommand.h`

## 核心功能

- **命令类型**: 定义所有支持的渲染操作
- **载荷数据**: 每个命令携带必要的绘制参数
- **类型安全**: 使用 `std::variant` 确保类型安全
- **轻量级**: 命令可高效复制和传递

## 命令类型

```cpp
enum class CommandType : std::uint8_t {
    SetClip,           // 设置裁剪区域
    SetTransform,      // 设置变换矩阵
    DrawRectangle,     // 绘制矩形
    DrawText,          // 绘制文本
    DrawImage,         // 绘制图像
    PushLayer,         // 推入图层(透明度/混合)
    PopLayer,          // 弹出图层
};
```

## 载荷结构

### ClipPayload - 裁剪
```cpp
struct ClipPayload {
    ui::Rect clipRect;     // 裁剪矩形
    bool enabled{true};    // true=启用裁剪, false=禁用裁剪
};
```

### TransformPayload - 变换
```cpp
struct TransformPayload {
    float offsetX{0.0f};   // X 偏移
    float offsetY{0.0f};   // Y 偏移
    // TODO: 未来扩展为完整的变换矩阵
};
```

### RectanglePayload - 矩形
```cpp
struct RectanglePayload {
    ui::Rect rect;                     // 矩形位置和大小
    std::array<float, 4> color;        // RGBA 颜色
    float cornerRadius{0.0f};          // 圆角半径
};
```

### TextPayload - 文本
```cpp
struct TextPayload {
    ui::Rect bounds;                   // 文本边界
    std::array<float, 4> color;        // RGBA 颜色
    std::string text;                  // 文本内容
    int fontId{0};                     // 字体 ID
    float fontSize{14.0f};             // 字体大小
    std::string fontFamily;            // 字体族(如 "Arial")
    bool textWrapping{false};          // 是否自动换行
    float maxWidth{0.0f};              // 换行时的最大宽度
};
```

### ImagePayload - 图像
```cpp
struct ImagePayload {
    ui::Rect destRect;                 // 目标矩形
    std::uint32_t textureId{0};        // 纹理 ID
};
```

### LayerPayload - 图层
```cpp
struct LayerPayload {
    float opacity{1.0f};               // 不透明度 [0.0, 1.0]
};
```

## RenderCommand 结构

```cpp
struct RenderCommand {
    CommandType type;           // 命令类型
    CommandPayload payload;     // 命令载荷
    
    RenderCommand(CommandType t, CommandPayload p = {})
        : type(t), payload(std::move(p)) {}
};
```

**CommandPayload** 是载荷联合体:
```cpp
using CommandPayload = std::variant<
    std::monostate,        // 空载荷
    ClipPayload,
    TransformPayload,
    RectanglePayload,
    TextPayload,
    ImagePayload,
    LayerPayload
>;
```

## 使用示例

### 1. 创建矩形命令

```cpp
#include <fk/render/RenderCommand.h>
#include <fk/ui/UIElement.h>

// 绘制红色矩形
fk::render::RectanglePayload rectPayload{
    .rect = {100, 100, 200, 150},           // x, y, width, height
    .color = {1.0f, 0.0f, 0.0f, 1.0f},     // 红色 RGBA
    .cornerRadius = 10.0f                   // 圆角 10px
};

fk::render::RenderCommand cmd(
    fk::render::CommandType::DrawRectangle,
    rectPayload
);
```

### 2. 创建文本命令

```cpp
#include <fk/render/RenderCommand.h>

// 绘制黑色文本
fk::render::TextPayload textPayload{
    .bounds = {50, 50, 300, 100},
    .color = {0.0f, 0.0f, 0.0f, 1.0f},     // 黑色
    .text = "Hello F__K UI",
    .fontId = 0,
    .fontSize = 16.0f,
    .fontFamily = "Arial",
    .textWrapping = false,
    .maxWidth = 0.0f
};

fk::render::RenderCommand cmd(
    fk::render::CommandType::DrawText,
    textPayload
);
```

### 3. 裁剪区域

```cpp
#include <fk/render/RenderCommand.h>

// 设置裁剪区域
fk::render::ClipPayload clipPayload{
    .clipRect = {0, 0, 800, 600},
    .enabled = true
};

fk::render::RenderCommand clipCmd(
    fk::render::CommandType::SetClip,
    clipPayload
);

// ... 绘制被裁剪的内容 ...

// 禁用裁剪
fk::render::ClipPayload disableClip{
    .clipRect = {},
    .enabled = false
};

fk::render::RenderCommand unclipCmd(
    fk::render::CommandType::SetClip,
    disableClip
);
```

### 4. 变换矩阵

```cpp
#include <fk/render/RenderCommand.h>

// 设置偏移变换
fk::render::TransformPayload transformPayload{
    .offsetX = 100.0f,
    .offsetY = 50.0f
};

fk::render::RenderCommand transformCmd(
    fk::render::CommandType::SetTransform,
    transformPayload
);

// 绘制的内容会偏移 (100, 50)
```

### 5. 图层和透明度

```cpp
#include <fk/render/RenderCommand.h>

// 推入半透明图层
fk::render::LayerPayload layerPayload{
    .opacity = 0.5f  // 50% 不透明度
};

fk::render::RenderCommand pushCmd(
    fk::render::CommandType::PushLayer,
    layerPayload
);

// ... 绘制半透明内容 ...

// 弹出图层
fk::render::RenderCommand popCmd(
    fk::render::CommandType::PopLayer,
    std::monostate{}  // 空载荷
);
```

### 6. 处理命令(渲染器侧)

```cpp
#include <fk/render/RenderCommand.h>
#include <variant>

void ExecuteCommand(const fk::render::RenderCommand& cmd) {
    switch (cmd.type) {
        case fk::render::CommandType::DrawRectangle: {
            const auto& payload = std::get<fk::render::RectanglePayload>(cmd.payload);
            DrawRectangleImpl(payload);
            break;
        }
        
        case fk::render::CommandType::DrawText: {
            const auto& payload = std::get<fk::render::TextPayload>(cmd.payload);
            DrawTextImpl(payload);
            break;
        }
        
        case fk::render::CommandType::SetClip: {
            const auto& payload = std::get<fk::render::ClipPayload>(cmd.payload);
            SetClipImpl(payload);
            break;
        }
        
        case fk::render::CommandType::SetTransform: {
            const auto& payload = std::get<fk::render::TransformPayload>(cmd.payload);
            SetTransformImpl(payload);
            break;
        }
        
        case fk::render::CommandType::PushLayer: {
            const auto& payload = std::get<fk::render::LayerPayload>(cmd.payload);
            PushLayerImpl(payload);
            break;
        }
        
        case fk::render::CommandType::PopLayer: {
            PopLayerImpl();
            break;
        }
        
        case fk::render::CommandType::DrawImage: {
            const auto& payload = std::get<fk::render::ImagePayload>(cmd.payload);
            DrawImageImpl(payload);
            break;
        }
    }
}
```

### 7. 批量命令

```cpp
#include <fk/render/RenderCommand.h>
#include <vector>

std::vector<fk::render::RenderCommand> BuildCommandList() {
    std::vector<fk::render::RenderCommand> commands;
    
    // 1. 设置裁剪
    commands.emplace_back(
        fk::render::CommandType::SetClip,
        fk::render::ClipPayload{{0, 0, 800, 600}, true}
    );
    
    // 2. 绘制背景
    commands.emplace_back(
        fk::render::CommandType::DrawRectangle,
        fk::render::RectanglePayload{
            {0, 0, 800, 600},
            {0.9f, 0.9f, 0.9f, 1.0f},
            0.0f
        }
    );
    
    // 3. 推入半透明图层
    commands.emplace_back(
        fk::render::CommandType::PushLayer,
        fk::render::LayerPayload{0.8f}
    );
    
    // 4. 绘制按钮
    commands.emplace_back(
        fk::render::CommandType::DrawRectangle,
        fk::render::RectanglePayload{
            {100, 100, 200, 50},
            {0.2f, 0.6f, 1.0f, 1.0f},
            5.0f
        }
    );
    
    // 5. 绘制文本
    commands.emplace_back(
        fk::render::CommandType::DrawText,
        fk::render::TextPayload{
            {100, 100, 200, 50},
            {1.0f, 1.0f, 1.0f, 1.0f},
            "Click Me",
            0, 14.0f, "Arial", false, 0.0f
        }
    );
    
    // 6. 弹出图层
    commands.emplace_back(
        fk::render::CommandType::PopLayer,
        std::monostate{}
    );
    
    return commands;
}
```

### 8. 使用 std::visit 处理载荷

```cpp
#include <fk/render/RenderCommand.h>
#include <variant>

struct CommandVisitor {
    void operator()(std::monostate) const {
        // 空载荷
    }
    
    void operator()(const fk::render::RectanglePayload& payload) const {
        std::cout << "绘制矩形: " 
                  << payload.rect.X << ", " << payload.rect.Y << "\n";
    }
    
    void operator()(const fk::render::TextPayload& payload) const {
        std::cout << "绘制文本: " << payload.text << "\n";
    }
    
    void operator()(const fk::render::ClipPayload& payload) const {
        std::cout << "设置裁剪: " << (payload.enabled ? "启用" : "禁用") << "\n";
    }
    
    void operator()(const fk::render::TransformPayload& payload) const {
        std::cout << "变换: (" << payload.offsetX << ", " << payload.offsetY << ")\n";
    }
    
    void operator()(const fk::render::LayerPayload& payload) const {
        std::cout << "图层不透明度: " << payload.opacity << "\n";
    }
    
    void operator()(const fk::render::ImagePayload& payload) const {
        std::cout << "绘制图像: 纹理 " << payload.textureId << "\n";
    }
};

void ProcessCommand(const fk::render::RenderCommand& cmd) {
    std::visit(CommandVisitor{}, cmd.payload);
}
```

## 最佳实践

### 1. 命令顺序
```cpp
// 推荐:正确的命令顺序
// 1. 设置状态(裁剪、变换、图层)
// 2. 绘制内容
// 3. 恢复状态

commands.push_back(SetClip);      // 设置
commands.push_back(DrawRect);     // 绘制
commands.push_back(DisableClip);  // 恢复
```

### 2. 图层配对
```cpp
// 推荐:PushLayer 和 PopLayer 必须配对
commands.push_back(PushLayer);
// ... 绘制 ...
commands.push_back(PopLayer);

// 避免:忘记 PopLayer
```

### 3. 使用结构化绑定
```cpp
// 推荐:使用聚合初始化
fk::render::RectanglePayload payload{
    .rect = {0, 0, 100, 100},
    .color = {1.0f, 0.0f, 0.0f, 1.0f},
    .cornerRadius = 5.0f
};
```

### 4. 避免大量字符串复制
```cpp
// 推荐:使用 std::move
std::string text = "长文本...";
fk::render::TextPayload payload{
    .text = std::move(text)  // 移动而非复制
};
```

### 5. 批量提交
```cpp
// 推荐:批量构建命令列表后一次提交
std::vector<RenderCommand> commands;
// ... 构建所有命令 ...
renderer.Draw(commands);

// 避免:每个命令单独提交
```

## 常见问题

### Q1: 如何绘制渐变?
当前版本不直接支持渐变。可以通过多个矩形模拟:
```cpp
for (int i = 0; i < 100; ++i) {
    float t = i / 100.0f;
    auto color = Lerp(startColor, endColor, t);
    commands.emplace_back(DrawRectangle, RectanglePayload{...});
}
```

### Q2: 如何绘制边框?
绘制两个矩形(外框 + 内容):
```cpp
// 外框(边框色)
commands.push_back(DrawRectangle{{0, 0, 100, 100}, borderColor, 0});
// 内容(背景色)
commands.push_back(DrawRectangle{{2, 2, 96, 96}, bgColor, 0});
```

### Q3: 命令可以复用吗?
可以。RenderCommand 是值类型,可以安全复制。

### Q4: 如何调试命令?
```cpp
void DebugCommand(const RenderCommand& cmd) {
    std::cout << "命令类型: " << static_cast<int>(cmd.type) << "\n";
    std::visit([](const auto& payload) {
        using T = std::decay_t<decltype(payload)>;
        std::cout << "载荷类型: " << typeid(T).name() << "\n";
    }, cmd.payload);
}
```

### Q5: 性能开销如何?
- 命令创建: 非常快(栈分配)
- 载荷复制: 根据类型(TextPayload 包含 string 会慢一些)
- std::variant: 零开销抽象

## 性能优化

### 1. 预分配命令列表
```cpp
std::vector<RenderCommand> commands;
commands.reserve(1000);  // 预分配避免重新分配
```

### 2. 避免不必要的字符串复制
```cpp
// 使用 std::string_view (C++17)
// 或提前构造字符串并移动
```

### 3. 批量合并
```cpp
// 合并相同类型的连续命令
// 例如:多个相邻的 DrawRectangle 可以批量绘制
```

### 4. 命令剔除
```cpp
// 裁剪掉不可见的命令
if (!IsVisible(payload.rect, viewportRect)) {
    continue;  // 跳过不可见的绘制命令
}
```

## 扩展指南

### 添加新命令类型

1. **定义命令类型**:
```cpp
enum class CommandType : std::uint8_t {
    // ... 现有类型 ...
    DrawCircle,  // 新增
};
```

2. **定义载荷结构**:
```cpp
struct CirclePayload {
    ui::Point center;
    float radius;
    std::array<float, 4> color;
};
```

3. **添加到联合体**:
```cpp
using CommandPayload = std::variant<
    // ... 现有类型 ...
    CirclePayload
>;
```

4. **实现渲染器处理**:
```cpp
case CommandType::DrawCircle: {
    const auto& payload = std::get<CirclePayload>(cmd.payload);
    DrawCircleImpl(payload);
    break;
}
```

## 内部机制

### 命令生成流程
```
UIElement::Render()
  └─> RenderTreeBuilder::Build()
      └─> 遍历 Visual 树
          └─> 为每个 Visual 生成命令
              └─> RenderList::AddCommand()
```

### 命令执行流程
```
IRenderer::Draw(RenderList)
  └─> 遍历命令列表
      └─> ExecuteCommand(RenderCommand)
          └─> std::get<Payload>(cmd.payload)
              └─> 底层图形 API 调用
```

## 相关类

- **RenderList**: 渲染命令列表容器
- **IRenderer**: 渲染器接口,执行命令
- **GlRenderer**: OpenGL 渲染器实现
- **RenderTreeBuilder**: 从 Visual 树生成命令

## 参考

- [RenderHost.md](RenderHost.md) - 渲染主机
- [IRenderer.md](IRenderer.md) - 渲染器接口
- [UIElement.md](../UI/UIElement.md) - UI 元素基类
