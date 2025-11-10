# 内存管理策略设计

## 核心原则

1. **父对象拥有子对象**：将子对象添加到父对象时，父对象获得所有权
2. **Application 拥有 Window**：所有窗口由 Application 统一管理
3. **用户只需管理 Application**：Application 析构时自动清理所有资源

## 实现细节

### 1. UIElement 基类增加子对象管理

```cpp
class UIElement {
protected:
    // 拥有的子对象（自动管理）
    std::vector<std::unique_ptr<UIElement>> ownedChildren_;
    
    // 添加子对象并获取所有权
    void TakeOwnership(UIElement* child) {
        if (child) {
            ownedChildren_.emplace_back(child);
        }
    }
    
public:
    virtual ~UIElement() {
        // 自动清理所有子对象
        ownedChildren_.clear();
    }
};
```

### 2. Panel 添加子对象时转移所有权

```cpp
template<typename Derived>
class Panel : public FrameworkElement<Derived> {
public:
    Derived* AddChild(UIElement* child) {
        if (child) {
            children_.push_back(child);
            this->AddVisualChild(child);
            this->TakeOwnership(child);  // 转移所有权
            this->InvalidateMeasure();
        }
        return static_cast<Derived*>(this);
    }
};
```

### 3. ContentControl 设置内容时转移所有权

```cpp
template<typename Derived>
class ContentControl : public Control<Derived> {
public:
    void SetContent(const std::any& value) {
        this->SetValue(ContentProperty(), value);
        
        // 如果是 UIElement*，转移所有权
        if (value.type() == typeid(UIElement*)) {
            auto* element = std::any_cast<UIElement*>(value);
            this->TakeOwnership(element);
        }
    }
};
```

### 4. Application 管理所有窗口

```cpp
class Application {
private:
    std::vector<std::unique_ptr<Window>> windows_;
    
public:
    // 创建窗口（返回原始指针供使用）
    Window* CreateWindow() {
        auto window = std::make_unique<Window>();
        auto* ptr = window.get();
        windows_.push_back(std::move(window));
        return ptr;
    }
    
    // 析构时自动清理所有窗口
    ~Application() {
        windows_.clear();
    }
};
```

## 使用示例

```cpp
int main() {
    Application app;
    
    // 创建窗口（由 app 管理）
    auto* window = app.CreateWindow();
    
    // 创建 UI（子对象由父对象管理）
    window->Title("My Application")
          ->Width(800)
          ->Height(600)
          ->Content(new StackPanel()  // StackPanel 由 window 管理
              ->AddChild(new TextBlock()->Text("Hello"))  // TextBlock 由 StackPanel 管理
              ->AddChild(new Button()->Content("Click")))  // Button 由 StackPanel 管理
          ->Show();
    
    return app.Run();  // app 析构时自动清理所有资源
}
```

## 优点

1. ✅ **用户友好**：无需手动 delete，无内存泄漏风险
2. ✅ **链式调用**：保持流畅的 API 风格
3. ✅ **明确所有权**：每个对象只有一个拥有者
4. ✅ **自动清理**：析构时自动释放所有子对象
5. ✅ **WPF 风格**：符合现代 UI 框架习惯

## 注意事项

1. **不要在多处引用同一个子对象**：
   ```cpp
   auto* text = new TextBlock();
   panel1->AddChild(text);
   panel2->AddChild(text);  // ❌ 错误！text 已被 panel1 拥有
   ```

2. **不要在栈上创建 UI 对象**：
   ```cpp
   TextBlock text;  // ❌ 错误！必须使用 new 创建
   panel->AddChild(&text);
   ```

3. **获取子对象后不要 delete**：
   ```cpp
   auto* child = panel->GetChild(0);
   delete child;  // ❌ 错误！由 panel 管理
   ```
