# ColorUtils - 颜色工具

## 概述

`ColorUtils` 提供颜色字符串解析功能，将十六进制颜色字符串转换为 RGBA 浮点数组，用于渲染系统中的颜色设置。

## 核心方法

### ParseColor

```cpp
static std::array<float, 4> ParseColor(const std::string& colorString);
```

解析颜色字符串并返回 RGBA 浮点数组。

**支持格式：**
- `#RGB`: 3 位格式（如 `#F00` → Red）
- `#RRGGBB`: 6 位格式（如 `#FF5733`）
- `#RRGGBBAA`: 8 位格式，含 Alpha（如 `#FF5733FF`）

**返回值：** `[R, G, B, A]`，每个分量范围 `[0.0, 1.0]`

**示例：**
```cpp
auto color1 = fk::render::ColorUtils::ParseColor("#FF0000");  // 红色
// 返回: {1.0f, 0.0f, 0.0f, 1.0f}

auto color2 = fk::render::ColorUtils::ParseColor("#00FF00CC");  // 绿色，50% 透明
// 返回: {0.0f, 1.0f, 0.0f, 0.8f}
```

**错误处理：** 解析失败返回黑色 `{0, 0, 0, 1}`

## 使用场景

```cpp
// 在渲染命令中使用
auto color = fk::render::ColorUtils::ParseColor("#3498DB");
renderCommand->SetColor(color[0], color[1], color[2], color[3]);
```

## 相关文档

- [RenderCommand.md](./RenderCommand.md) - 渲染命令
- [GlRenderer.md](./GlRenderer.md) - OpenGL 渲染器
