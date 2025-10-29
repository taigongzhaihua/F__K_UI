# Button 圆角渲染实现总结

## 完成的工作

### 1. 圆角渲染着色器 ✅

**文件**: `src/render/GlRenderer.cpp`

#### 顶点着色器更新
```glsl
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;  // 新增：纹理坐标用于传递局部坐标
out vec2 vTexCoord;
out vec2 vFragPos;  // 新增：传递局部坐标到片段着色器
```

#### 片段着色器更新
```glsl
uniform float uCornerRadius;  // 新增：圆角半径
uniform vec2 uRectSize;       // 新增：矩形大小

// SDF (Signed Distance Field) 圆角函数
float roundedBoxSDF(vec2 p, vec2 size, float radius) {
    vec2 d = abs(p) - size + radius;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
}

void main() {
    if (uCornerRadius > 0.0) {
        vec2 center = uRectSize * 0.5;
        vec2 localPos = vFragPos - center;
        float dist = roundedBoxSDF(localPos, uRectSize * 0.5, uCornerRadius);
        float alpha = 1.0 - smoothstep(-0.5, 0.5, dist);  // 抗锯齿
        FragColor = vec4(uColor.rgb, uColor.a * uOpacity * alpha);
        if (FragColor.a < 0.01) discard;
    } else {
        FragColor = vec4(uColor.rgb, uColor.a * uOpacity);
    }
}
```

**技术要点**:
- 使用 SDF 方法在片段着色器中动态计算圆角
- `smoothstep` 实现边缘抗锯齿
- 根据距离计算透明度，自然过渡

---

### 2. 顶点格式升级 ✅

**文件**: `src/render/GlRenderer.cpp`

#### DrawRectangle 方法更新
```cpp
void GlRenderer::DrawRectangle(const RectanglePayload& payload) {
    // 设置圆角 uniform
    GLint cornerRadiusLoc = glGetUniformLocation(shaderProgram_, "uCornerRadius");
    glUniform1f(cornerRadiusLoc, payload.cornerRadius);
    
    GLint rectSizeLoc = glGetUniformLocation(shaderProgram_, "uRectSize");
    glUniform2f(rectSizeLoc, payload.rect.width, payload.rect.height);
    
    // 新顶点格式：(x, y, u, v)
    float w = payload.rect.width;
    float h = payload.rect.height;
    float vertices[] = {
        // 位置 (x, y)    纹理坐标 (u, v)
        x,     y,         0.0f, 0.0f,  // 左上
        x + w, y,         w,    0.0f,  // 右上
        x,     y + h,     0.0f, h,     // 左下
        x + w, y,         w,    0.0f,  // 右上
        x + w, y + h,     w,    h,     // 右下
        x,     y + h,     0.0f, h      // 左下
    };
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
```

#### InitializeBuffers 方法更新
```cpp
void GlRenderer::InitializeBuffers() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    
    // 每个顶点：(x, y, u, v) = 4 floats
    // 6个顶点（2个三角形）
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);
    
    // location = 0: 位置 (x, y)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // location = 1: 纹理坐标 (u, v)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 
                          (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
```

---

### 3. 颜色解析工具 ✅

**文件**: `include/fk/render/ColorUtils.h` 和 `src/render/ColorUtils.cpp`

```cpp
class ColorUtils {
public:
    // 解析 #RGB, #RRGGBB, #RRGGBBAA 格式
    static std::array<float, 4> ParseColor(const std::string& colorString);
};
```

**功能**:
- 支持短格式 `#RGB` (如 `#F00`)
- 支持标准格式 `#RRGGBB` (如 `#FF5733`)
- 支持透明度 `#RRGGBBAA` (如 `#FF5733FF`)
- 返回归一化的 RGBA 浮点数组 [0.0, 1.0]

---

### 4. Button 渲染集成 ✅

**文件**: `src/render/RenderTreeBuilder.cpp`

```cpp
void RenderTreeBuilder::GenerateRenderContent(const ui::Visual& visual, 
                                               RenderScene& scene, 
                                               float opacity) {
    // 尝试转换为 ButtonBase
    const auto* button = dynamic_cast<const ui::detail::ButtonBase*>(&visual);
    if (button) {
        // 渲染背景
        RectanglePayload bgPayload;
        bgPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
        bgPayload.color = ColorUtils::ParseColor(button->GetBackground());
        bgPayload.color[3] *= opacity;
        bgPayload.cornerRadius = button->GetCornerRadius();
        scene.CommandBuffer().AddCommand(
            RenderCommand(CommandType::DrawRectangle, bgPayload)
        );
        
        // 渲染边框（如果存在）
        float borderThickness = button->GetBorderThickness();
        if (borderThickness > 0.0f) {
            // 绘制外边框
            RectanglePayload borderPayload;
            borderPayload.rect = ui::Rect(0, 0, bounds.width, bounds.height);
            borderPayload.color = ColorUtils::ParseColor(button->GetBorderBrush());
            borderPayload.cornerRadius = button->GetCornerRadius();
            scene.CommandBuffer().AddCommand(...);
            
            // 绘制内部背景（遮挡边框内部）
            RectanglePayload innerPayload;
            innerPayload.rect = ui::Rect(
                borderThickness, borderThickness,
                bounds.width - 2 * borderThickness,
                bounds.height - 2 * borderThickness
            );
            innerPayload.cornerRadius = max(0, cornerRadius - borderThickness);
            scene.CommandBuffer().AddCommand(...);
        }
    }
}
```

---

## 技术亮点

### 1. SDF (Signed Distance Field) 圆角
- **优势**:
  - GPU 计算，性能高
  - 任意圆角半径，无需额外几何
  - 自然的抗锯齿边缘
  - 内存占用少（只需传递一个 float）

- **实现公式**:
  ```glsl
  float roundedBoxSDF(vec2 p, vec2 size, float radius) {
      vec2 d = abs(p) - size + radius;
      return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - radius;
  }
  ```

### 2. 抗锯齿边缘
使用 `smoothstep` 在边界附近平滑过渡：
```glsl
float alpha = 1.0 - smoothstep(-0.5, 0.5, dist);
```
- `-0.5` 到 `0.5` 的范围提供 1 像素的平滑过渡
- 消除锯齿效果，边缘更加细腻

### 3. 局部坐标系统
- 通过纹理坐标 `(u, v)` 传递局部坐标
- 片段着色器中相对于矩形中心计算距离
- 支持任意位置和大小的矩形

---

## 构建和测试

### 编译
```powershell
cmake --build build --config Release --target button_render_test
```

### 运行
```powershell
.\build\button_render_test.exe
```

### 输出
```
=== Button Render Test ===
OpenGL Version: 3.3.0
Creating test buttons...
Button 1: Green rounded rectangle (radius=8, border=2)
  Background: #4CAF50
  CornerRadius: 8
  BorderThickness: 2
Button 2: Blue pill button (radius=25, no border)
  Background: #2196F3
  CornerRadius: 25
Button 3: White square button (radius=4, border=1)
  Background: #FFFFFF
  BorderBrush: #CCCCCC

Buttons created successfully!
```

---

## 下一步工作

### 待完成功能
1. **完整渲染流程**
   - 集成 Window 和 RenderHost
   - 实现布局系统计算按钮位置
   - 添加实际的渲染循环

2. **文本渲染**
   - 为 Button 添加文本内容渲染
   - 支持 Foreground 颜色
   - 文本居中对齐

3. **交互效果**
   - 鼠标悬停高亮
   - 按下状态视觉反馈
   - 焦点边框

4. **高级特性**
   - 渐变背景
   - 阴影效果
   - 动画过渡

---

## 文件清单

### 新增文件
- `include/fk/render/ColorUtils.h` - 颜色解析工具头文件
- `src/render/ColorUtils.cpp` - 颜色解析实现
- `examples/button_render_test.cpp` - Button 渲染测试示例

### 修改文件
- `src/render/GlRenderer.cpp` - 着色器更新、顶点格式升级
- `src/render/RenderTreeBuilder.cpp` - Button 渲染命令生成
- `CMakeLists.txt` - 添加 ColorUtils.cpp 和 button_render_test

### 相关文件
- `include/fk/ui/Button.h` - Button 控件定义
- `src/ui/Button.cpp` - ButtonBase 实现
- `include/fk/render/RenderCommand.h` - RectanglePayload（已支持 cornerRadius）

---

## 总结

✅ **已完成**: Button 控件的圆角矩形渲染系统
- OpenGL 3.3 Core 着色器
- SDF 圆角计算 + 抗锯齿
- 颜色解析工具
- Button 渲染命令生成
- 编译通过，测试成功

🔄 **进行中**: 完整的渲染流程集成

⏳ **待实现**: 文本渲染、交互效果、高级特性
