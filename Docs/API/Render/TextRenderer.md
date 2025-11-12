# TextRenderer

## 概览

**目的**：文本渲染（使用FreeType）

**命名空间**：`fk::render`

**头文件**：`fk/render/TextRenderer.h`

## 描述

`TextRenderer` 使用FreeType库渲染文本。

## 公共接口

### 渲染文本

#### RenderText
```cpp
void RenderText(
    const std::string& text,
    const Point& position,
    const Font& font,
    const Color& color
);
```

渲染文本字符串。

### 测量文本

#### MeasureText
```cpp
Size MeasureText(const std::string& text, const Font& font);
```

测量文本大小。

## 使用示例

### 渲染文本
```cpp
auto textRenderer = std::make_shared<TextRenderer>();

Font font("Arial", 14);
textRenderer->RenderText("Hello", Point(10, 10), font, Colors::Black);
```

## 相关类

- [GlRenderer](GlRenderer.md)
- [TextBlock](../UI/TextBlock.md)
