# TextRenderer - 文本渲染器

## 概述

`TextRenderer` 使用 FreeType 库加载字体并生成字形纹理,支持 UTF-8 编码的文本渲染和测量。

**命名空间**: `fk::render`  
**头文件**: `fk/render/TextRenderer.h`

## 核心功能

- **字体加载**: 加载 TrueType/OpenType 字体
- **字形缓存**: 自动缓存已渲染的字形
- **文本测量**: 计算文本显示尺寸
- **UTF-8 支持**: 完整的 Unicode 支持
- **纹理生成**: 将文本渲染为 OpenGL 纹理

## 数据结构

### Glyph - 字形信息
```cpp
struct Glyph {
    unsigned int textureID;  // OpenGL 纹理 ID
    int width;              // 字形宽度(像素)
    int height;             // 字形高度(像素)
    int bearingX;           // 字形水平偏移
    int bearingY;           // 字形垂直偏移
    int advance;            // 水平前进值
};
```

### FontFace - 字体信息
```cpp
struct FontFace {
    FT_Face face;                                     // FreeType 字体对象
    std::unordered_map<char32_t, Glyph> glyphs;     // 字符到字形映射
};
```

## API 参考

### 构造/析构

```cpp
TextRenderer();
~TextRenderer();

// 不可拷贝
TextRenderer(const TextRenderer&) = delete;
TextRenderer& operator=(const TextRenderer&) = delete;
```

### 初始化

```cpp
bool Initialize();
```
初始化 FreeType 库。
- **返回**: 成功返回 `true`,失败返回 `false`
- **说明**: 必须在使用其他方法前调用

### 字体管理

```cpp
int LoadFont(const std::string& fontPath, unsigned int fontSize);
```
加载字体文件。
- **参数**: 
  - `fontPath` - 字体文件路径(TTF/OTF)
  - `fontSize` - 字体大小(像素)
- **返回**: 字体 ID(≥0),失败返回 `-1`
- **说明**: 同一字体不同大小需要分别加载

```cpp
int GetLineHeight(int fontId) const;
```
获取字体的行高。
- **参数**: `fontId` - 字体 ID
- **返回**: 行高(像素)

```cpp
const Glyph* GetGlyph(char32_t c, int fontId);
```
获取字符字形。
- **参数**: 
  - `c` - Unicode 字符码点
  - `fontId` - 字体 ID
- **返回**: 字形指针,不存在返回 `nullptr`

### 文本测量

```cpp
void MeasureText(
    const std::string& text,
    int fontId,
    int& outWidth,
    int& outHeight
);
```
测量文本尺寸。
- **参数**: 
  - `text` - UTF-8 文本
  - `fontId` - 字体 ID
  - `outWidth` - 输出宽度
  - `outHeight` - 输出高度

### 文本渲染

```cpp
unsigned int RenderTextToTexture(
    const std::string& text,
    int fontId,
    const std::array<float, 4>& color,
    int& outWidth,
    int& outHeight
);
```
将文本渲染为纹理。
- **参数**: 
  - `text` - UTF-8 文本
  - `fontId` - 字体 ID
  - `color` - RGBA 颜色
  - `outWidth` - 输出纹理宽度
  - `outHeight` - 输出纹理高度
- **返回**: OpenGL 纹理 ID,失败返回 `0`

### 工具方法

```cpp
std::u32string Utf8ToUtf32(const std::string& utf8);
```
UTF-8 转 UTF-32。
- **参数**: `utf8` - UTF-8 字符串
- **返回**: UTF-32 字符数组

## 使用示例

### 1. 初始化和加载字体

```cpp
#include <fk/render/TextRenderer.h>

// 创建 TextRenderer
auto textRenderer = std::make_unique<fk::render::TextRenderer>();

// 初始化 FreeType
if (!textRenderer->Initialize()) {
    std::cerr << "FreeType 初始化失败\n";
    return false;
}

// 加载字体
int fontId = textRenderer->LoadFont("C:/Windows/Fonts/Arial.ttf", 16);
if (fontId < 0) {
    std::cerr << "字体加载失败\n";
    return false;
}

std::cout << "字体加载成功,ID: " << fontId << "\n";
```

### 2. 测量文本尺寸

```cpp
#include <fk/render/TextRenderer.h>

void MeasureTextExample(fk::render::TextRenderer& textRenderer, int fontId) {
    std::string text = "Hello World!";
    
    int width, height;
    textRenderer.MeasureText(text, fontId, width, height);
    
    std::cout << "文本 \"" << text << "\" 尺寸: " 
              << width << "x" << height << " 像素\n";
}
```

### 3. 获取字体信息

```cpp
#include <fk/render/TextRenderer.h>

void GetFontInfo(fk::render::TextRenderer& textRenderer, int fontId) {
    // 获取行高
    int lineHeight = textRenderer.GetLineHeight(fontId);
    std::cout << "行高: " << lineHeight << " 像素\n";
    
    // 获取字符字形
    auto* glyph = textRenderer.GetGlyph(U'A', fontId);
    if (glyph) {
        std::cout << "字符 'A':\n";
        std::cout << "  宽度: " << glyph->width << "\n";
        std::cout << "  高度: " << glyph->height << "\n";
        std::cout << "  前进值: " << glyph->advance << "\n";
    }
}
```

### 4. 渲染文本到纹理

```cpp
#include <fk/render/TextRenderer.h>

unsigned int RenderText(fk::render::TextRenderer& textRenderer, int fontId) {
    std::string text = "Click Me";
    
    // 设置颜色(白色)
    std::array<float, 4> color = {1.0f, 1.0f, 1.0f, 1.0f};
    
    int width, height;
    unsigned int textureId = textRenderer.RenderTextToTexture(
        text, fontId, color, width, height
    );
    
    if (textureId > 0) {
        std::cout << "文本纹理创建成功: " << textureId << "\n";
        std::cout << "纹理尺寸: " << width << "x" << height << "\n";
    }
    
    return textureId;
}
```

### 5. 多字体支持

```cpp
#include <fk/render/TextRenderer.h>

struct FontLibrary {
    int arial16;
    int arial24;
    int arialBold16;
    int timesNewRoman16;
};

FontLibrary LoadFonts(fk::render::TextRenderer& textRenderer) {
    FontLibrary fonts;
    
    // 加载不同字体和大小
    fonts.arial16 = textRenderer.LoadFont("Arial.ttf", 16);
    fonts.arial24 = textRenderer.LoadFont("Arial.ttf", 24);
    fonts.arialBold16 = textRenderer.LoadFont("ArialBold.ttf", 16);
    fonts.timesNewRoman16 = textRenderer.LoadFont("TimesNewRoman.ttf", 16);
    
    return fonts;
}

void UseMultipleFonts(fk::render::TextRenderer& textRenderer, 
                      const FontLibrary& fonts) {
    // 小标题
    int w1, h1;
    textRenderer.MeasureText("标题", fonts.arial24, w1, h1);
    
    // 正文
    int w2, h2;
    textRenderer.MeasureText("正文内容", fonts.arial16, w2, h2);
    
    // 强调
    int w3, h3;
    textRenderer.MeasureText("重要!", fonts.arialBold16, w3, h3);
}
```

### 6. UTF-8 文本处理

```cpp
#include <fk/render/TextRenderer.h>

void HandleUtf8Text(fk::render::TextRenderer& textRenderer, int fontId) {
    // UTF-8 文本
    std::string utf8Text = "你好世界! Hello 🌍";
    
    // 转换为 UTF-32
    std::u32string utf32 = textRenderer.Utf8ToUtf32(utf8Text);
    
    std::cout << "UTF-8 字符数: " << utf8Text.size() << "\n";
    std::cout << "UTF-32 字符数: " << utf32.size() << "\n";
    
    // 测量中文文本
    int width, height;
    textRenderer.MeasureText(utf8Text, fontId, width, height);
    std::cout << "文本尺寸: " << width << "x" << height << "\n";
}
```

### 7. 文本对齐辅助

```cpp
#include <fk/render/TextRenderer.h>

struct TextMetrics {
    int width;
    int height;
    int lineHeight;
};

TextMetrics GetTextMetrics(fk::render::TextRenderer& textRenderer,
                           const std::string& text, int fontId) {
    TextMetrics metrics;
    
    textRenderer.MeasureText(text, fontId, metrics.width, metrics.height);
    metrics.lineHeight = textRenderer.GetLineHeight(fontId);
    
    return metrics;
}

// 居中对齐
int CalculateCenteredX(int containerWidth, int textWidth) {
    return (containerWidth - textWidth) / 2;
}

// 右对齐
int CalculateRightAlignedX(int containerWidth, int textWidth) {
    return containerWidth - textWidth;
}

// 垂直居中
int CalculateCenteredY(int containerHeight, int lineHeight) {
    return (containerHeight - lineHeight) / 2;
}
```

### 8. 字形缓存检查

```cpp
#include <fk/render/TextRenderer.h>

void PreloadCharacters(fk::render::TextRenderer& textRenderer, int fontId) {
    // 预加载常用字符
    std::string commonChars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
    
    for (char c : commonChars) {
        auto* glyph = textRenderer.GetGlyph(static_cast<char32_t>(c), fontId);
        if (!glyph) {
            std::cerr << "字符 '" << c << "' 加载失败\n";
        }
    }
    
    std::cout << "预加载完成\n";
}
```

## 最佳实践

### 1. 初始化顺序
```cpp
// 推荐:先初始化后加载字体
textRenderer->Initialize();
int fontId = textRenderer->LoadFont("font.ttf", 16);

// 避免:忘记初始化
auto fontId = textRenderer->LoadFont("font.ttf", 16);  // 错误!
```

### 2. 字体 ID 管理
```cpp
// 推荐:保存字体 ID
class FontManager {
    std::unordered_map<std::string, int> fonts_;
    
public:
    int GetFont(const std::string& name) {
        return fonts_[name];
    }
};

// 避免:重复加载同一字体
```

### 3. 错误检查
```cpp
// 推荐:检查返回值
int fontId = textRenderer->LoadFont(path, size);
if (fontId < 0) {
    // 处理错误
}

unsigned int texId = textRenderer->RenderTextToTexture(...);
if (texId == 0) {
    // 处理错误
}
```

### 4. 字体路径
```cpp
// 推荐:使用绝对路径或配置文件
#ifdef _WIN32
    std::string fontPath = "C:/Windows/Fonts/Arial.ttf";
#else
    std::string fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif

int fontId = textRenderer->LoadFont(fontPath, 16);
```

### 5. 性能优化
```cpp
// 推荐:缓存测量结果
std::unordered_map<std::string, std::pair<int, int>> sizeCache;

auto GetTextSize = [&](const std::string& text) {
    if (sizeCache.count(text)) {
        return sizeCache[text];
    }
    int w, h;
    textRenderer->MeasureText(text, fontId, w, h);
    sizeCache[text] = {w, h};
    return std::make_pair(w, h);
};
```

## 常见问题

### Q1: 支持哪些字体格式?
TrueType (.ttf) 和 OpenType (.otf) 字体。

### Q2: 如何加载系统字体?
```cpp
// Windows
int fontId = textRenderer->LoadFont("C:/Windows/Fonts/Arial.ttf", 16);

// Linux
int fontId = textRenderer->LoadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);

// macOS
int fontId = textRenderer->LoadFont("/System/Library/Fonts/Helvetica.ttc", 16);
```

### Q3: 如何处理字体加载失败?
```cpp
int fontId = textRenderer->LoadFont(path, size);
if (fontId < 0) {
    // 尝试备用字体
    fontId = textRenderer->LoadFont("fallback.ttf", size);
    if (fontId < 0) {
        throw std::runtime_error("无法加载任何字体");
    }
}
```

### Q4: 文本测量不准确?
确保使用正确的字体 ID 和已加载的字体。某些字符可能不在字体中。

### Q5: 如何支持 Emoji?
需要加载支持 Emoji 的字体:
```cpp
int emojiFont = textRenderer->LoadFont("NotoColorEmoji.ttf", 16);
```

### Q6: 内存占用过大?
字形缓存会占用内存。考虑:
- 仅加载需要的字符
- 限制字体大小和数量
- 定期清理不常用的字形

## 性能优化

### 1. 字体预加载
```cpp
// 启动时预加载常用字符
void PreloadFont(fk::render::TextRenderer& textRenderer, int fontId) {
    std::string ascii = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ";
    for (char c : ascii) {
        textRenderer.GetGlyph(c, fontId);
    }
}
```

### 2. 测量缓存
```cpp
// 缓存文本测量结果
struct TextSizeCache {
    std::unordered_map<std::string, std::pair<int, int>> cache;
    
    std::pair<int, int> GetSize(const std::string& text, 
                                 fk::render::TextRenderer& renderer, 
                                 int fontId) {
        if (cache.count(text)) {
            return cache[text];
        }
        int w, h;
        renderer.MeasureText(text, fontId, w, h);
        cache[text] = {w, h};
        return {w, h};
    }
};
```

### 3. 按需加载
```cpp
// 仅在需要时加载字体
class LazyFontLoader {
    fk::render::TextRenderer& renderer_;
    std::unordered_map<std::string, int> fonts_;
    
public:
    int GetFont(const std::string& name, unsigned int size) {
        std::string key = name + "_" + std::to_string(size);
        if (!fonts_.count(key)) {
            fonts_[key] = renderer_.LoadFont(name, size);
        }
        return fonts_[key];
    }
};
```

### 4. 纹理重用
```cpp
// 缓存常用文本纹理
std::unordered_map<std::string, unsigned int> textureCache;

unsigned int GetTextTexture(const std::string& text) {
    if (textureCache.count(text)) {
        return textureCache[text];
    }
    int w, h;
    auto texId = textRenderer.RenderTextToTexture(text, fontId, color, w, h);
    textureCache[text] = texId;
    return texId;
}
```

## 线程安全

TextRenderer **不是线程安全的**。必须在单线程上使用,或使用外部同步:

```cpp
std::mutex textRendererMutex;

void ThreadSafeRender(fk::render::TextRenderer& renderer) {
    std::lock_guard<std::mutex> lock(textRendererMutex);
    // 安全使用 renderer
}
```

## 内部机制

### 字形生成流程
```
LoadCharacter(char32_t c)
  └─> FT_Load_Char(face, c, FT_LOAD_RENDER)
      └─> 生成位图
          └─> 创建 OpenGL 纹理
              └─> 缓存到 glyphs map
```

### 文本测量流程
```
MeasureText(text)
  └─> Utf8ToUtf32(text)
      └─> 遍历字符
          └─> GetGlyph(c) → 累加 advance
              └─> 返回总宽度和最大高度
```

## 依赖项

- **FreeType 2**: 字体加载和字形渲染
- **OpenGL**: 纹理创建和管理

## 相关类

- **GlRenderer**: OpenGL 渲染器,使用 TextRenderer
- **RenderCommand**: TextPayload 包含文本渲染参数
- **IRenderer**: 渲染器接口

## 参考

- [GlRenderer.md](GlRenderer.md) - OpenGL 渲染器
- [RenderCommand.md](RenderCommand.md) - 渲染命令
- [FreeType 官方文档](https://www.freetype.org/freetype2/docs/)
