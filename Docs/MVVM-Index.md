# MVVM 数据绑定文档索引

欢迎来到 F__K_UI 框架的 MVVM 数据绑定系统！本目录包含从入门到精通的完整文档。

---

## 📚 文档导航

### 🚀 快速开始（5 分钟）

**[ViewModelMacros-QuickStart.md](ViewModelMacros-QuickStart.md)**
- 什么是 ViewModel 属性宏？
- 三种宏的快速对比
- 完整示例和最佳实践
- 传统方式 vs 宏方式对比

**适合人群**：刚接触框架，想快速上手 MVVM 开发

---

### 📖 使用指南（15 分钟）

**[ViewModelPropertyMacros-Guide.md](ViewModelPropertyMacros-Guide.md)**
- 三种属性宏的详细说明
  - `FK_PROPERTY` - 普通属性
  - `FK_PROPERTY_CUSTOM` - 带依赖通知
  - `FK_PROPERTY_READONLY` - 只读属性
- 完整示例代码
- 故障排除和高级技巧
- 性能优化建议

**适合人群**：需要深入了解宏系统，编写复杂 ViewModel

---

### 🔍 原理详解（30 分钟）

**[MVVM-Binding-Workflow.md](MVVM-Binding-Workflow.md)**
- 绑定系统架构概览
- 核心组件职责详解
- 完整工作流程（6 个阶段）
  1. 初始化（属性注册）
  2. 创建绑定
  3. 激活绑定
  4. 初始更新
  5. 响应属性变化
  6. 双向绑定
- 属性注册机制深度剖析
- 时序图和流程图
- 常见问题排查

**适合人群**：想理解框架内部机制，贡献代码或调试复杂问题

---

### ⚡ 快速参考（随时查阅）

**[MVVM-Binding-Quick-Reference.md](MVVM-Binding-Quick-Reference.md)**
- 数据流向图（OneWay 和 TwoWay）
- 核心组件速查表
- 快速实现绑定模板
- 常见问题一秒诊断表
- 关键代码模板

**适合人群**：已经熟悉框架，需要快速查找 API 或解决问题

---

## 🎯 推荐学习路径

### 初学者路线

```
1️⃣ ViewModelMacros-QuickStart.md（5 分钟）
   ↓ 了解宏的基本用法
   
2️⃣ 运行示例代码
   samples/mvvm_binding_demo/
   examples/simple_property_macro_test.cpp
   ↓ 看到效果，增强信心
   
3️⃣ MVVM-Binding-Quick-Reference.md（10 分钟）
   ↓ 掌握常用模板
   
4️⃣ 开始编写自己的应用
   ↓ 遇到问题时查阅文档
```

### 进阶开发者路线

```
1️⃣ MVVM-Binding-Quick-Reference.md（5 分钟）
   ↓ 快速过一遍数据流向
   
2️⃣ ViewModelPropertyMacros-Guide.md（10 分钟）
   ↓ 了解高级用法和最佳实践
   
3️⃣ MVVM-Binding-Workflow.md（按需阅读）
   ↓ 深入理解原理，便于调试
   
4️⃣ 查看源码
   src/binding/BindingExpression.cpp
   src/binding/BindingPath.cpp
   ↓ 理解实现细节
```

### 框架贡献者路线

```
1️⃣ MVVM-Binding-Workflow.md（完整阅读）
   ↓ 全面理解架构
   
2️⃣ 阅读所有源码
   src/binding/*.cpp
   include/fk/binding/*.h
   ↓ 掌握实现细节
   
3️⃣ 编写测试用例
   tests/binding/*.cpp
   ↓ 验证功能和边界情况
```

---

## 🌟 核心概念速览

### MVVM 模式

```
┌─────────────┐         ┌──────────────┐         ┌──────────┐
│    View     │ ←─────→ │  ViewModel   │ ←─────→ │  Model   │
│   (UI)      │ Binding │ (逻辑+状态)   │         │ (数据)   │
└─────────────┘         └──────────────┘         └──────────┘
```

- **View**: UI 控件（TextBlock, Button 等）
- **ViewModel**: 业务逻辑和 UI 状态（ObservableObject）
- **Model**: 纯数据对象（可选）
- **Binding**: 连接 View 和 ViewModel 的桥梁

### 绑定三要素

1. **DataContext**: 数据源（ViewModel）
2. **Path**: 属性路径（如 "Count", "Person.Name"）
3. **BindingMode**: 更新方向（OneWay, TwoWay, OneTime 等）

### 属性宏三兄弟

| 宏 | 生成内容 | 使用场景 |
|----|---------|---------|
| `FK_PROPERTY` | 成员变量 + getter + setter | 简单属性 |
| `FK_PROPERTY_CUSTOM` | 成员变量 + getter + setter (带自定义代码) | 有依赖的属性 |
| `FK_PROPERTY_READONLY` | 成员变量 + getter | 只读属性 |

---

## 📝 示例代码

### 最小示例

```cpp
// ViewModel
class CounterViewModel : public binding::ObservableObject {
public:
    FK_PROPERTY(int, Count)
};
FK_VIEWMODEL_AUTO(CounterViewModel, Count)

// UI
int main() {
    auto vm = std::make_shared<CounterViewModel>();
    auto window = ui::window();
    window->SetDataContext(std::static_pointer_cast<INotifyPropertyChanged>(vm));
    
    auto text = ui::textBlock();
    text->SetBinding(TextBlock::TextProperty(), binding::Binding().Path("Count"));
    
    return app.Run(window);
}
```

### 完整示例

请查看 `samples/mvvm_binding_demo/src/main.cpp`

---

## 🛠️ 相关工具

### 宏系统

- **定义位置**: `include/fk/binding/ViewModelMacros.h`
- **核心宏**:
  - `FK_PROPERTY(Type, Name)`
  - `FK_PROPERTY_CUSTOM(Type, Name, CustomCode)`
  - `FK_PROPERTY_READONLY(Type, Name)`
  - `FK_VIEWMODEL_BEGIN(ViewModelType)`
  - `FK_VIEWMODEL_PROPERTY(PropertyName, Getter)`
  - `FK_VIEWMODEL_END()`
  - `FK_VIEWMODEL_AUTO(ViewModelType, ...)`

### 绑定系统

- **核心类**:
  - `binding::ObservableObject` - ViewModel 基类
  - `binding::Binding` - 绑定配置
  - `binding::BindingExpression` - 绑定逻辑
  - `binding::PropertyAccessorRegistry` - 属性反射
  - `binding::DependencyObject` - UI 控件基类

---

## ❓ 常见问题快速索引

| 问题 | 查看文档 | 章节 |
|-----|---------|------|
| 如何创建 ViewModel？ | ViewModelMacros-QuickStart.md | "快速开始" |
| 属性宏怎么用？ | ViewModelPropertyMacros-Guide.md | "属性声明宏" |
| UI 不更新怎么办？ | MVVM-Binding-Quick-Reference.md | "常见问题一秒诊断" |
| 绑定的原理是什么？ | MVVM-Binding-Workflow.md | "完整工作流程" |
| 如何实现双向绑定？ | MVVM-Binding-Workflow.md | "阶段 6：双向绑定" |
| 计算属性怎么写？ | ViewModelPropertyMacros-Guide.md | "示例 1：简单计数器" |
| 如何注册属性？ | MVVM-Binding-Workflow.md | "属性注册机制详解" |
| 性能优化建议？ | MVVM-Binding-Workflow.md | "性能优化建议" |

---

## 🚦 版本和兼容性

- **框架版本**: F__K_UI v0.1+
- **C++ 标准**: C++23
- **编译器要求**: 
  - GCC 13+
  - Clang 16+
  - MSVC 2022+

---

## 🤝 贡献

发现文档错误或有改进建议？欢迎提交 Issue 或 Pull Request！

**改进方向**：
- 补充更多示例
- 添加视频教程
- 翻译成其他语言
- 改进图表和可视化

---

## 📜 许可证

所有文档遵循与项目相同的许可证。

---

## 🎉 开始使用

选择适合您的文档，立即开始 MVVM 开发之旅！

**推荐起点**：[ViewModelMacros-QuickStart.md](ViewModelMacros-QuickStart.md) 🚀

---

*最后更新: 2025年11月3日*
