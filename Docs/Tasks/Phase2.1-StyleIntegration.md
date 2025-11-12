# Phase 2.1: Control集成Style系统

**开始日期**: 2025-11-12  
**状态**: 进行中  
**负责人**: Copilot

## 当前状态分析

### 已实现 ✅
- Style 类完整实现（Apply/Unapply/BasedOn）
- Control.SetStyle() 方法实现
- Style 应用和撤销逻辑

### 需要改进 🔧

#### 1. Style 作为依赖属性
**当前**: Style 是普通成员变量 `Style* style_`  
**目标**: 改为 DependencyProperty，支持绑定和动画

#### 2. ImplicitStyle 支持
**当前**: 必须手动调用 SetStyle  
**目标**: 从 ResourceDictionary 自动查找并应用样式

#### 3. 值优先级完善
**当前**: Style 值直接应用到属性  
**目标**: 确保 Local 值 > Style 值 > Default 值

## 实现计划

### 步骤 1: 创建任务文档并提交 ✅
- 记录当前状态
- 制定实施计划
- 明确成功标准

### 步骤 2: Style DependencyProperty ✅
将 Style 从普通成员改为 DependencyProperty
- ✅ 添加 StyleProperty() 静态方法
- ✅ 实现 OnStyleChanged 回调
- ✅ 自动应用/撤销样式

### 步骤 3: ImplicitStyle 查找 ✅
实现从 ResourceDictionary 自动查找样式
- ✅ 实现 ApplyImplicitStyle() 方法
- ✅ 实现 FindResource() 方法（支持向上查找）
- ✅ 在 OnLoaded() 中自动应用隐式样式
- ✅ 使用类型名称作为资源键

### 步骤 4: 值优先级集成 (下一步)
确保 Local > Style > Default
- [ ] 验证依赖属性值优先级
- [ ] 测试Local值覆盖Style值

### 步骤 5: 测试和验证
编写测试用例验证功能
- [ ] 测试显式Style设置
- [ ] 测试ImplicitStyle自动应用
- [ ] 测试资源查找（向上遍历可视树）
- [ ] 测试值优先级

## 成功标准

- [x] Style 是 DependencyProperty
- [x] ImplicitStyle 自动查找和应用
- [ ] Local 值正确覆盖 Style 值
- [ ] 所有测试通过

---

详细技术说明见完整任务文档。
