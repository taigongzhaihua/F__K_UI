# 彩色 Emoji 支持说明

## 当前状态
- ✅ Emoji 字形正确显示（不再是方块）
- ✅ Font fallback 机制工作正常
- ⚠️ Emoji 显示为彩虹渐变效果（非真实颜色）

## 原因
Windows 的 Segoe UI Emoji 使用 **COLR/CPAL** 矢量格式，这是一种分层彩色字体格式。
FreeType 2.9.0 只能将其渲染为灰度位图（pixel_mode=2），无法自动渲染为 BGRA。

## 解决方案

### 方案 A：升级 FreeType（推荐）

升级到 FreeType 2.10+ 可使用 COLR 分层 API：

```cpp
// FreeType 2.10+ 代码示例
FT_LayerIterator iterator;
FT_UInt layerGlyphIndex;
FT_UInt layerColorIndex;

iterator.p = NULL;
while (FT_Get_Color_Glyph_Layer(face, baseGlyphIndex, 
                                &layerGlyphIndex, &layerColorIndex, &iterator)) {
    // 获取颜色
    FT_Color color = palette[layerColorIndex];
    
    // 渲染当前层
    FT_Load_Glyph(face, layerGlyphIndex, FT_LOAD_DEFAULT);
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    
    // 将层叠加到最终纹理，应用颜色
    BlendLayer(face->glyph->bitmap, color);
}
```

**步骤：**
1. 下载 FreeType 2.13.2（最新版）
2. 替换 `third_party/freetype`
3. 重新编译

### 方案 B：使用 DirectWrite（Windows 专用）

使用 Windows DirectWrite API 原生渲染彩色 emoji。

**优点：**
- 系统原生支持
- 完整的 emoji 颜色
- 自动处理 COLR/CPAL

**缺点：**
- 仅支持 Windows
- 需要集成 Direct2D/DirectWrite
- 代码复杂度增加

### 方案 C：下载位图 Emoji 字体

使用 Noto Color Emoji 或 Apple Color Emoji（SBIX 格式）。

**Noto Color Emoji 下载：**
https://github.com/googlefonts/noto-emoji/releases

将字体放在项目中并加载即可，FreeType 2.9 可直接渲染 SBIX 为 BGRA。

## 当前实现

### 渐变效果代码

`src/render/GlRenderer.cpp` 中的着色器对灰度 emoji 应用彩虹渐变：

```glsl
// 创建彩虹渐变效果
float h = TexCoords.x * 0.3 + TexCoords.y * 0.7;
vec3 emojiColor;
float hue = h * 6.0;
// ... HSV 到 RGB 转换 ...
color = vec4(emojiColor, shape * uOpacity);
```

这提供了视觉上丰富的效果，但不是真实的 emoji 颜色。

## 技术细节

### COLR 格式说明
- **COLR 表**：定义字形的分层结构
- **CPAL 表**：定义调色板颜色
- 每个 emoji 由多个单色层组成，每层使用调色板中的颜色

### FreeType 版本对比
| 版本 | COLR 支持 | API |
|------|----------|-----|
| 2.9.0（当前）| 基本支持（仅灰度）| `FT_LOAD_COLOR`（无效）|
| 2.10+ | 完整支持 | `FT_Get_Color_Glyph_Layer`, `FT_Palette_Select` |

## 建议

对于生产环境，建议**升级到 FreeType 2.10+** 并实现 COLR 分层渲染，这是最通用的解决方案。
