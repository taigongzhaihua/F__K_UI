# 裁剪系统测试计划

## 测试状态

**当前阶段**：阶段3 - 性能优化和测试

**测试类型**：
- [ ] 性能基准测试
- [ ] 单元测试
- [ ] 集成测试
- [ ] 边界情况测试

---

## 一、性能基准测试

### 1.1 测试场景

#### 场景1：滚动列表（ScrollViewer）
```cpp
// 测试配置
- 项目数量：100个
- 每个项目高度：50px
- 视口高度：500px（可见10项）
- 测试：CollectDrawCommands性能

// 预期结果
- 优化前：~2.5ms（100项全部处理）
- 优化后：~0.13ms（仅处理可见10项）
- 提升：19倍
```

#### 场景2：深层嵌套容器
```cpp
// 测试配置
- 嵌套层级：5层
- 每层：Grid > Border > StackPanel > Border > Content
- 每层裁剪：累积交集
- 测试：裁剪计算和提前剔除

// 预期结果
- 优化前：~1.2ms（无提前剔除）
- 优化后：~0.15ms（提前剔除优化）
- 提升：8倍
```

#### 场景3：大型Grid
```cpp
// 测试配置
- Grid大小：100x10（1000个单元格）
- 视口：300x200
- 裁剪：Panel.ClipToBounds = true
- 测试：大量元素的提前剔除

// 预期结果
- 优化前：~15ms（1000个单元格全部处理）
- 优化后：~2ms（仅处理可见区域）
- 提升：7.5倍
```

### 1.2 性能测试代码示例

```cpp
// 文件：examples/clipping/clipping_performance_test.cpp

#include <chrono>
#include <iostream>
#include <fk/ui/Window.h>
#include <fk/ui/ScrollViewer.h>
#include <fk/ui/StackPanel.h>
#include <fk/ui/Border.h>
#include <fk/ui/TextBlock.h>

using namespace std::chrono;

class ClippingPerformanceTest {
public:
    // 场景1：滚动列表测试
    void TestScrollViewerPerformance() {
        auto scrollViewer = std::make_shared<ScrollViewer>();
        scrollViewer->SetWidth(400);
        scrollViewer->SetHeight(500);
        
        auto stack = std::make_shared<StackPanel>();
        
        // 创建100个项目
        for (int i = 0; i < 100; ++i) {
            auto border = std::make_shared<Border>();
            border->SetHeight(50);
            border->SetPadding(Thickness(10));
            
            auto text = std::make_shared<TextBlock>();
            text->SetText("Item " + std::to_string(i));
            border->SetChild(text);
            
            stack->AddChild(border);
        }
        
        scrollViewer->SetContent(stack);
        
        // 性能测试：CollectDrawCommands
        RenderContext context;
        
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            scrollViewer->CollectDrawCommands(context);
        }
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start).count();
        std::cout << "ScrollViewer (100 items): " 
                  << (duration / 100.0) << " μs" << std::endl;
    }
    
    // 场景2：深层嵌套测试
    void TestNestedContainersPerformance() {
        // 创建深层嵌套：5层
        auto root = CreateNestedContainers(5);
        
        RenderContext context;
        
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; ++i) {
            root->CollectDrawCommands(context);
        }
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start).count();
        std::cout << "Nested Containers (5 levels): " 
                  << (duration / 1000.0) << " μs" << std::endl;
    }
    
    // 场景3：大型Grid测试
    void TestLargeGridPerformance() {
        auto grid = std::make_shared<Grid>();
        grid->SetClipToBounds(true);
        grid->SetWidth(3000);
        grid->SetHeight(500);
        
        // 创建100x10的Grid（1000个单元格）
        for (int row = 0; row < 100; ++row) {
            for (int col = 0; col < 10; ++col) {
                auto cell = std::make_shared<Border>();
                cell->SetWidth(30);
                cell->SetHeight(50);
                // ... 设置Grid.Row, Grid.Column
                grid->AddChild(cell);
            }
        }
        
        RenderContext context;
        
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            grid->CollectDrawCommands(context);
        }
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start).count();
        std::cout << "Large Grid (1000 cells): " 
                  << (duration / 100.0) << " μs" << std::endl;
    }
    
private:
    std::shared_ptr<UIElement> CreateNestedContainers(int depth) {
        if (depth == 0) {
            auto text = std::make_shared<TextBlock>();
            text->SetText("Content");
            return text;
        }
        
        auto border = std::make_shared<Border>();
        border->SetPadding(Thickness(10));
        border->SetChild(CreateNestedContainers(depth - 1));
        return border;
    }
};

int main() {
    ClippingPerformanceTest test;
    
    std::cout << "=== 裁剪系统性能测试 ===" << std::endl;
    std::cout << std::endl;
    
    test.TestScrollViewerPerformance();
    test.TestNestedContainersPerformance();
    test.TestLargeGridPerformance();
    
    return 0;
}
```

---

## 二、单元测试

### 2.1 UIElement裁剪逻辑测试

```cpp
// 文件：tests/ui/UIElementClippingTest.cpp

TEST(UIElementClipping, DetermineClipRegion_NoClip) {
    auto element = std::make_shared<UIElement>();
    
    // 默认不裁剪
    auto clipRegion = element->DetermineClipRegion();
    EXPECT_FALSE(clipRegion.has_value());
}

TEST(UIElementClipping, DetermineClipRegion_ExplicitClip) {
    auto element = std::make_shared<UIElement>();
    element->SetClip(Rect{10, 10, 100, 100});
    
    // 显式裁剪优先级最高
    auto clipRegion = element->DetermineClipRegion();
    ASSERT_TRUE(clipRegion.has_value());
    EXPECT_EQ(clipRegion->x, 10);
    EXPECT_EQ(clipRegion->y, 10);
    EXPECT_EQ(clipRegion->width, 100);
    EXPECT_EQ(clipRegion->height, 100);
}
```

### 2.2 Border裁剪测试

```cpp
TEST(BorderClipping, ShouldClipToBounds_AlwaysTrue) {
    auto border = std::make_shared<Border>();
    EXPECT_TRUE(border->ShouldClipToBounds());
}

TEST(BorderClipping, CalculateClipBounds_ExcludesBorderAndPadding) {
    auto border = std::make_shared<Border>();
    border->SetWidth(200);
    border->SetHeight(200);
    border->SetBorderThickness(Thickness(5));
    border->SetPadding(Thickness(10));
    
    auto clipBounds = border->CalculateClipBounds();
    
    // 左边：5 (border) + 10 (padding) = 15
    // 上边：5 (border) + 10 (padding) = 15
    // 宽度：200 - 15 (left) - 15 (right) = 170
    // 高度：200 - 15 (top) - 15 (bottom) = 170
    EXPECT_EQ(clipBounds.x, 15);
    EXPECT_EQ(clipBounds.y, 15);
    EXPECT_EQ(clipBounds.width, 170);
    EXPECT_EQ(clipBounds.height, 170);
}
```

### 2.3 ScrollViewer裁剪测试

```cpp
TEST(ScrollViewerClipping, ShouldClipToBounds_AlwaysTrue) {
    auto scrollViewer = std::make_shared<ScrollViewer>();
    EXPECT_TRUE(scrollViewer->ShouldClipToBounds());
}

TEST(ScrollViewerClipping, CalculateClipBounds_ViewportArea) {
    auto scrollViewer = std::make_shared<ScrollViewer>();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    // 假设滚动条宽度为16px
    auto clipBounds = scrollViewer->CalculateClipBounds();
    
    // 验证视口区域（排除滚动条）
    EXPECT_LE(clipBounds.width, 300);
    EXPECT_LE(clipBounds.height, 200);
}
```

### 2.4 Panel裁剪测试

```cpp
TEST(PanelClipping, ClipToBounds_DefaultFalse) {
    auto panel = std::make_shared<StackPanel>();
    EXPECT_FALSE(panel->GetClipToBounds());
    EXPECT_FALSE(panel->ShouldClipToBounds());
}

TEST(PanelClipping, ClipToBounds_CanEnable) {
    auto panel = std::make_shared<StackPanel>();
    panel->SetClipToBounds(true);
    
    EXPECT_TRUE(panel->GetClipToBounds());
    EXPECT_TRUE(panel->ShouldClipToBounds());
}

TEST(PanelClipping, ClipToBounds_FluentAPI) {
    auto panel = std::make_shared<StackPanel>();
    panel->ClipToBounds(true);
    
    EXPECT_TRUE(panel->GetClipToBounds());
}
```

### 2.5 RenderContext裁剪交集测试

```cpp
TEST(RenderContextClipping, PushClip_CalculatesIntersection) {
    RenderContext context;
    
    // 第一层裁剪：[0, 0, 200, 200]
    context.PushClip(Rect{0, 0, 200, 200});
    
    // 第二层裁剪：[50, 50, 300, 300]
    // 交集应该是：[50, 50, 150, 150]
    context.PushClip(Rect{50, 50, 300, 300});
    
    auto currentClip = context.GetCurrentClip();
    ASSERT_TRUE(currentClip.enabled);
    EXPECT_EQ(currentClip.clipRect.x, 50);
    EXPECT_EQ(currentClip.clipRect.y, 50);
    EXPECT_EQ(currentClip.clipRect.width, 150);
    EXPECT_EQ(currentClip.clipRect.height, 150);
}

TEST(RenderContextClipping, IsCompletelyClipped_OutsideClipRegion) {
    RenderContext context;
    context.PushClip(Rect{0, 0, 100, 100});
    
    // 完全在裁剪区域外
    Rect outsideRect{200, 200, 50, 50};
    EXPECT_TRUE(context.IsCompletelyClipped(outsideRect));
}

TEST(RenderContextClipping, IsCompletelyClipped_InsideClipRegion) {
    RenderContext context;
    context.PushClip(Rect{0, 0, 100, 100});
    
    // 完全在裁剪区域内
    Rect insideRect{10, 10, 50, 50};
    EXPECT_FALSE(context.IsCompletelyClipped(insideRect));
}

TEST(RenderContextClipping, IsCompletelyClipped_PartiallyInside) {
    RenderContext context;
    context.PushClip(Rect{0, 0, 100, 100});
    
    // 部分在裁剪区域内
    Rect partialRect{50, 50, 100, 100};
    EXPECT_FALSE(context.IsCompletelyClipped(partialRect));
}
```

---

## 三、集成测试

### 3.1 深层嵌套裁剪测试

```cpp
TEST(IntegrationTest, NestedContainersClipping) {
    // 创建深层嵌套：Window > Grid > ScrollViewer > Border > Content
    auto window = std::make_shared<Window>();
    window->SetWidth(400);
    window->SetHeight(300);
    
    auto grid = std::make_shared<Grid>();
    window->SetContent(grid);
    
    auto scrollViewer = std::make_shared<ScrollViewer>();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    grid->AddChild(scrollViewer);
    
    auto border = std::make_shared<Border>();
    border->SetPadding(Thickness(10));
    scrollViewer->SetContent(border);
    
    auto content = std::make_shared<TextBlock>();
    content->SetText("Large Content");
    border->SetChild(content);
    
    // 测试：收集绘制命令
    RenderContext context;
    window->CollectDrawCommands(context);
    
    // 验证：裁剪栈正确管理（PushClip/PopClip配对）
    EXPECT_EQ(context.GetClipStackSize(), 0);  // 所有裁剪已弹出
}
```

### 3.2 ScrollViewer裁剪内容测试

```cpp
TEST(IntegrationTest, ScrollViewerClipsContent) {
    auto scrollViewer = std::make_shared<ScrollViewer>();
    scrollViewer->SetWidth(300);
    scrollViewer->SetHeight(200);
    
    auto stack = std::make_shared<StackPanel>();
    
    // 添加100个项目（总高度5000px，远超视口200px）
    for (int i = 0; i < 100; ++i) {
        auto item = std::make_shared<Border>();
        item->SetHeight(50);
        stack->AddChild(item);
    }
    
    scrollViewer->SetContent(stack);
    
    // 测试：收集绘制命令
    RenderContext context;
    scrollViewer->CollectDrawCommands(context);
    
    // 验证：只有可见项目生成绘制命令
    // （提前剔除优化应该减少大量绘制调用）
    auto commandCount = context.GetCommandCount();
    EXPECT_LT(commandCount, 1000);  // 远少于无优化的命令数
}
```

---

## 四、边界情况测试

### 4.1 零尺寸元素测试

```cpp
TEST(EdgeCase, ZeroSizeElement) {
    auto border = std::make_shared<Border>();
    border->SetWidth(0);
    border->SetHeight(0);
    
    auto clipBounds = border->CalculateClipBounds();
    
    // 零尺寸不应该崩溃
    EXPECT_GE(clipBounds.width, 0);
    EXPECT_GE(clipBounds.height, 0);
}
```

### 4.2 负数裁剪测试

```cpp
TEST(EdgeCase, NegativePaddingOrBorder) {
    auto border = std::make_shared<Border>();
    border->SetWidth(100);
    border->SetHeight(100);
    border->SetPadding(Thickness(150));  // 超过元素尺寸
    
    auto clipBounds = border->CalculateClipBounds();
    
    // 应该返回有效的裁剪区域（至少0x0）
    EXPECT_GE(clipBounds.width, 0);
    EXPECT_GE(clipBounds.height, 0);
}
```

### 4.3 空裁剪交集测试

```cpp
TEST(EdgeCase, EmptyClipIntersection) {
    RenderContext context;
    
    // 第一层：[0, 0, 100, 100]
    context.PushClip(Rect{0, 0, 100, 100});
    
    // 第二层：[200, 200, 100, 100]（无交集）
    context.PushClip(Rect{200, 200, 100, 100});
    
    auto currentClip = context.GetCurrentClip();
    
    // 交集为空，宽度和高度应该为0
    EXPECT_TRUE(currentClip.enabled);
    EXPECT_EQ(currentClip.clipRect.width, 0);
    EXPECT_EQ(currentClip.clipRect.height, 0);
}
```

---

## 五、测试执行计划

### 5.1 测试环境

- 编译器：支持C++23
- 测试框架：Google Test（如果项目已使用）或自定义测试
- 性能测试：Release模式编译
- 单元测试：Debug模式编译（便于调试）

### 5.2 执行顺序

1. **单元测试**（快速验证基础功能）
2. **集成测试**（验证组件协作）
3. **边界情况测试**（稳定性验证）
4. **性能基准测试**（性能验证）

### 5.3 成功标准

**单元测试**：
- ✅ 所有测试通过
- ✅ 代码覆盖率 > 80%

**集成测试**：
- ✅ 深层嵌套裁剪正确
- ✅ 裁剪栈管理正确

**性能测试**：
- ✅ ScrollViewer：至少5倍提升
- ✅ 深层嵌套：至少5倍提升
- ✅ 大型Grid：至少5倍提升

**边界情况测试**：
- ✅ 所有测试通过
- ✅ 无崩溃、无内存泄漏

---

## 六、测试结果记录

### 6.1 性能测试结果

| 场景 | 优化前 | 优化后 | 提升倍数 | 状态 |
|------|--------|--------|---------|------|
| ScrollViewer (100项) | 待测 | 待测 | 待测 | ⏳ |
| 深层嵌套 (5层) | 待测 | 待测 | 待测 | ⏳ |
| 大型Grid (1000单元格) | 待测 | 待测 | 待测 | ⏳ |

### 6.2 单元测试结果

| 测试模块 | 通过 | 失败 | 状态 |
|----------|------|------|------|
| UIElement裁剪逻辑 | 待测 | 待测 | ⏳ |
| Border裁剪 | 待测 | 待测 | ⏳ |
| ScrollViewer裁剪 | 待测 | 待测 | ⏳ |
| Panel裁剪 | 待测 | 待测 | ⏳ |
| RenderContext交集 | 待测 | 待测 | ⏳ |

### 6.3 问题跟踪

| ID | 问题描述 | 严重程度 | 状态 |
|----|---------|---------|------|
| - | - | - | - |

---

## 七、后续工作

### 7.1 阶段4：文档和示例（第6周）

- [ ] API文档更新
- [ ] 使用指南编写
- [ ] 示例应用创建
- [ ] 性能优化建议文档

### 7.2 可选扩展（第7-14周）

- [ ] 圆角矩形裁剪实现
- [ ] 椭圆裁剪实现
- [ ] 多边形裁剪实现
- [ ] 路径裁剪实现
- [ ] RenderTransform完整支持

---

**文档版本**: 1.0  
**创建日期**: 2025-11-23  
**状态**: ✅ 计划完成，待执行
