/**
 * @file property_macros_demo.cpp
 * @brief 演示如何使用 PropertyMacros.h 来简化依赖属性声明
 * 
 * 展示了：
 * 1. 使用宏前后的代码对比
 * 2. 各种宏的使用方法
 * 3. 代码量的显著减少
 */

#include <iostream>
#include <string>

// 模拟的简化版本，用于演示
namespace demo {

// ============================================================================
// 方式 1：传统方式（手动编写所有代码）
// ============================================================================

class TraditionalStyle {
public:
    // 每个属性需要编写 5 个方法，约 20 行代码
    
    // Width 属性
    float GetWidth() const { return width_; }
    void SetWidth(float value) { width_ = value; }
    TraditionalStyle* Width(float value) { SetWidth(value); return this; }
    TraditionalStyle* Width(std::string /*binding*/) { /* 绑定逻辑 */ return this; }
    float Width() const { return GetWidth(); }
    
    // Height 属性
    float GetHeight() const { return height_; }
    void SetHeight(float value) { height_ = value; }
    TraditionalStyle* Height(float value) { SetHeight(value); return this; }
    TraditionalStyle* Height(std::string /*binding*/) { /* 绑定逻辑 */ return this; }
    float Height() const { return GetHeight(); }
    
    // Foreground 属性
    std::string GetForeground() const { return foreground_; }
    void SetForeground(const std::string& value) { foreground_ = value; }
    TraditionalStyle* Foreground(const std::string& value) { SetForeground(value); return this; }
    TraditionalStyle* Foreground(std::string /*binding*/) { /* 绑定逻辑 */ return this; }
    std::string Foreground() const { return GetForeground(); }
    
    // 总计：15 个方法，约 60 行代码（仅 3 个属性）

private:
    float width_{0.0f};
    float height_{0.0f};
    std::string foreground_;
};

// ============================================================================
// 方式 2：使用宏（大幅简化）
// ============================================================================

// 定义简化的宏（实际版本在 PropertyMacros.h 中）
#define DEMO_PROPERTY(Name, Type) \
    Type Get##Name() const { return Name##_; } \
    void Set##Name(Type value) { Name##_ = value; } \
    MacroStyle* Name(Type value) { Set##Name(value); return this; } \
    MacroStyle* Name(std::string /*binding*/) { /* 绑定逻辑 */ return this; } \
    Type Name() const { return Get##Name(); }

class MacroStyle {
public:
    // 每个属性只需 1 行！
    DEMO_PROPERTY(Width, float)
    DEMO_PROPERTY(Height, float)
    
    // 总计：2 行代码（2 个属性）
    // 相比传统方式减少了 95% 的代码量！

private:
    float Width_{0.0f};
    float Height_{0.0f};
};

} // namespace demo

int main() {
    std::cout << "=== PropertyMacros 使用演示 ===\n\n";
    
    std::cout << "[代码量对比]\n\n";
    
    std::cout << "传统方式（手动编写）：\n";
    std::cout << "  每个属性需要：\n";
    std::cout << "    - Type GetPropertyName() const           (1 行)\n";
    std::cout << "    - void SetPropertyName(Type value)       (2 行)\n";
    std::cout << "    - Derived* PropertyName(Type value)      (2 行)\n";
    std::cout << "    - Derived* PropertyName(Binding binding) (3 行)\n";
    std::cout << "    - Type PropertyName() const              (1 行)\n";
    std::cout << "  总计：约 9-12 行代码/属性\n";
    std::cout << "  \n";
    std::cout << "  FrameworkElement 有 6 个尺寸属性：\n";
    std::cout << "    6 × 12 = 72 行代码\n";
    std::cout << "  Control 有 3 个外观属性：\n";
    std::cout << "    3 × 12 = 36 行代码\n";
    std::cout << "  总计：约 108 行重复代码\n\n";
    
    std::cout << "使用宏后：\n";
    std::cout << "  每个属性只需：1 行宏调用\n";
    std::cout << "  \n";
    std::cout << "  FrameworkElement 的 6 个属性：6 行\n";
    std::cout << "  Control 的 3 个属性：3 行\n";
    std::cout << "  总计：9 行代码\n";
    std::cout << "  \n";
    std::cout << "  代码减少：91.7%！\n\n";
    
    std::cout << "=== 实际使用示例 ===\n\n";
    
    std::cout << "[FrameworkElement.h 重构前]\n";
    std::cout << R"(
    void SetWidth(float value) { SetValue(WidthProperty(), value); InvalidateMeasure(); }
    float GetWidth() const { return GetValue<float>(WidthProperty()); }
    Derived* Width(float value) { SetWidth(value); return static_cast<Derived*>(this); }
    Derived* Width(binding::Binding binding) { 
        this->SetBinding(WidthProperty(), std::move(binding)); 
        return static_cast<Derived*>(this); 
    }
    float Width() const { return GetWidth(); }
    
    // 重复 5 次（Height, MinWidth, MaxWidth, MinHeight, MaxHeight）...
)";
    
    std::cout << "\n[FrameworkElement.h 重构后]\n";
    std::cout << R"(
    #include "fk/ui/PropertyMacros.h"
    
    FK_PROPERTY_MEASURE(Width, float, Derived)
    FK_PROPERTY_MEASURE(Height, float, Derived)
    FK_PROPERTY_MEASURE(MinWidth, float, Derived)
    FK_PROPERTY_MEASURE(MaxWidth, float, Derived)
    FK_PROPERTY_MEASURE(MinHeight, float, Derived)
    FK_PROPERTY_MEASURE(MaxHeight, float, Derived)
)";
    
    std::cout << "\n[Control.h 重构前]\n";
    std::cout << R"(
    Brush* GetForeground() const { ... }
    void SetForeground(Brush* brush) { ... }
    Derived* Foreground(Brush* brush) { ... }
    Derived* Foreground(binding::Binding binding) { ... }
    Brush* Foreground() const { ... }
    
    // 重复 2 次（Background, BorderBrush）...
)";
    
    std::cout << "\n[Control.h 重构后]\n";
    std::cout << R"(
    #include "fk/ui/PropertyMacros.h"
    
    FK_PROPERTY_VISUAL(Foreground, Brush*, Derived)
    FK_PROPERTY_VISUAL(Background, Brush*, Derived)
    FK_PROPERTY_VISUAL(BorderBrush, Brush*, Derived)
)";
    
    std::cout << "\n\n=== 可用的宏 ===\n\n";
    
    std::cout << "1. FK_PROPERTY_SIMPLE(Name, Type, Derived, Action)\n";
    std::cout << "   - 用于简单值类型（int, float, bool 等）\n";
    std::cout << "   - Action: 属性变更时执行的操作\n\n";
    
    std::cout << "2. FK_PROPERTY_POINTER(Name, Type, Derived, Action)\n";
    std::cout << "   - 用于指针类型（Brush*, Style* 等）\n\n";
    
    std::cout << "3. FK_PROPERTY_COMPLEX(Name, Type, Derived, Action)\n";
    std::cout << "   - 用于复杂类型（Thickness, Rect 等）\n";
    std::cout << "   - 使用 const 引用传参\n\n";
    
    std::cout << "4. 便捷宏（预定义的常用操作）：\n";
    std::cout << "   - FK_PROPERTY_MEASURE(Name, Type, Derived)\n";
    std::cout << "     触发 InvalidateMeasure()\n";
    std::cout << "   - FK_PROPERTY_ARRANGE(Name, Type, Derived)\n";
    std::cout << "     触发 InvalidateArrange()\n";
    std::cout << "   - FK_PROPERTY_VISUAL(Name, Type, Derived)\n";
    std::cout << "     触发 InvalidateVisual()\n\n";
    
    std::cout << "=== 优势 ===\n\n";
    std::cout << "✅ 代码量减少 90%+\n";
    std::cout << "✅ 减少复制粘贴错误\n";
    std::cout << "✅ 统一的代码风格\n";
    std::cout << "✅ 更易维护（修改宏定义即可更新所有属性）\n";
    std::cout << "✅ 保持类型安全（编译时检查）\n";
    std::cout << "✅ 保持代码可读性（宏名称清晰表达意图）\n\n";
    
    std::cout << "=== 注意事项 ===\n\n";
    std::cout << "⚠️  宏会在预处理阶段展开，调试时可能需要查看展开后的代码\n";
    std::cout << "⚠️  IDE 的代码补全可能无法很好地处理宏生成的代码\n";
    std::cout << "✓  但这些小缺点相比代码简化的巨大优势是值得的\n\n";
    
    std::cout << "=== 下一步 ===\n\n";
    std::cout << "可以逐步将现有代码重构为使用宏：\n";
    std::cout << "1. 从 FrameworkElement 开始（6 个属性）\n";
    std::cout << "2. 然后 Control（3 个属性）\n";
    std::cout << "3. 再到 Border、TextBlock 等具体控件\n";
    std::cout << "4. 确保每次重构后编译通过并运行测试\n\n";
    
    std::cout << "参考文件：include/fk/ui/PropertyMacros.h\n";
    
    return 0;
}
