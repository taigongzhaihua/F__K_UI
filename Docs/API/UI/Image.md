# Image

## 概览

**目的**：显示位图图像

**命名空间**：`fk::ui`

**继承**：`FrameworkElement` → `UIElement` → `Visual`

**头文件**：`fk/ui/Image.h`

## 描述

`Image` 元素用于显示位图图像。支持多种图像格式（PNG、JPEG等）和不同的拉伸模式。

## 公共接口

### 图像源

#### Source
```cpp
static const DependencyProperty& SourceProperty();
Image* Source(const std::string& path);
```

设置图像文件路径。

**示例**：
```cpp
image->Source("assets/logo.png");
```

### 拉伸模式

#### Stretch
```cpp
static const DependencyProperty& StretchProperty();
Image* Stretch(Stretch stretch);
```

控制图像如何适应可用空间：
- `None` - 原始大小
- `Fill` - 填充整个空间（可能变形）
- `Uniform` - 保持宽高比，适应空间
- `UniformToFill` - 保持宽高比，填充空间（可能裁剪）

**示例**：
```cpp
image->Stretch(Stretch::Uniform);
```

### 大小

#### Width / Height
```cpp
Image* Width(double width);
Image* Height(double height);
```

显式设置图像显示大小。

**示例**：
```cpp
image->Width(200)->Height(150);
```

## 使用示例

### 基本图像显示
```cpp
auto image = std::make_shared<Image>();
image->Source("images/photo.jpg")
     ->Width(300)
     ->Height(200)
     ->Stretch(Stretch::Uniform);
```

### 作为按钮内容
```cpp
auto button = std::make_shared<Button>();
auto image = std::make_shared<Image>();
image->Source("icons/save.png")
     ->Width(16)
     ->Height(16);
button->Content(image);
```

### 数据绑定
```cpp
image->SetValue(Image::SourceProperty(),
               Binding("ImagePath").Source(viewModel));
```

## 相关类

- [FrameworkElement](FrameworkElement.md) - 基类

## 另请参阅

- [设计文档](../../Design/UI/Image.md)
