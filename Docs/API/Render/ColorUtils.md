# ColorUtils

## 概览

**目的**：颜色操作工具函数

**命名空间**：`fk::render`

**头文件**：`fk/render/ColorUtils.h`

## 功能

### 颜色转换

#### FromHex
```cpp
Color FromHex(const std::string& hex);
```

从十六进制字符串创建颜色。

#### ToHex
```cpp
std::string ToHex(const Color& color);
```

转换为十六进制字符串。

### 颜色操作

#### Lighten / Darken
```cpp
Color Lighten(const Color& color, double amount);
Color Darken(const Color& color, double amount);
```

调亮或调暗颜色。

#### Blend
```cpp
Color Blend(const Color& a, const Color& b, double factor);
```

混合两种颜色。

## 使用示例

### 从十六进制
```cpp
Color blue = ColorUtils::FromHex("#0000FF");
Color rgba = ColorUtils::FromHex("#FF0000FF");
```

### 调整颜色
```cpp
Color baseColor = Colors::Blue;
Color lighter = ColorUtils::Lighten(baseColor, 0.2);
Color darker = ColorUtils::Darken(baseColor, 0.2);
```

### 颜色混合
```cpp
Color result = ColorUtils::Blend(Colors::Red, Colors::Blue, 0.5);
```
