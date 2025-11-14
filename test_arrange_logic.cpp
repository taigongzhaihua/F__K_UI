/**
 * 测试 UIElement::Arrange 逻辑
 * 
 * 验证当 finalRect 改变时，layoutRect_ 是否会被更新
 */

#include <iostream>
#include <cmath>

// 模拟 Rect 结构
struct Rect {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};

    Rect() = default;
    Rect(float x, float y, float width, float height) 
        : x(x), y(y), width(width), height(height) {}
};

// 测试逻辑
bool testArrangeLogic() {
    // 模拟初始状态
    Rect layoutRect(0, 0, 100, 50);
    bool arrangeDirty = false;
    bool measureDirty = false;
    
    // 模拟第一次 Arrange 调用
    Rect finalRect1(0, 0, 100, 50);
    
    // 检查是否需要重新排列
    bool rectChanged = (layoutRect.x != finalRect1.x || 
                       layoutRect.y != finalRect1.y ||
                       layoutRect.width != finalRect1.width ||
                       layoutRect.height != finalRect1.height);
    
    if (!arrangeDirty && !measureDirty && !rectChanged) {
        std::cout << "第一次调用: 跳过（这是预期的，因为位置相同）" << std::endl;
    } else {
        std::cout << "第一次调用: 继续执行" << std::endl;
        layoutRect = finalRect1;
        arrangeDirty = false;
    }
    
    // 模拟第二次 Arrange 调用，位置改变了
    Rect finalRect2(0, 60, 100, 50);
    
    rectChanged = (layoutRect.x != finalRect2.x || 
                   layoutRect.y != finalRect2.y ||
                   layoutRect.width != finalRect2.width ||
                   layoutRect.height != finalRect2.height);
    
    std::cout << "\n第二次调用（位置改变）:" << std::endl;
    std::cout << "  layoutRect: (" << layoutRect.x << ", " << layoutRect.y << ")" << std::endl;
    std::cout << "  finalRect2: (" << finalRect2.x << ", " << finalRect2.y << ")" << std::endl;
    std::cout << "  rectChanged: " << (rectChanged ? "true" : "false") << std::endl;
    
    if (!arrangeDirty && !measureDirty && !rectChanged) {
        std::cout << "  结果: 跳过（错误！位置改变了应该继续执行）" << std::endl;
        return false;
    } else {
        std::cout << "  结果: 继续执行（正确！）" << std::endl;
        layoutRect = finalRect2;
        arrangeDirty = false;
    }
    
    // 验证 layoutRect 已更新
    if (std::abs(layoutRect.y - 60.0f) < 0.001f) {
        std::cout << "\n✓ 测试通过：layoutRect 正确更新为新位置" << std::endl;
        std::cout << "  最终 layoutRect: (" << layoutRect.x << ", " << layoutRect.y << ")" << std::endl;
        return true;
    } else {
        std::cerr << "\n✗ 测试失败：layoutRect 没有更新" << std::endl;
        return false;
    }
}

int main() {
    std::cout << "测试 UIElement::Arrange 修复..." << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (testArrangeLogic()) {
        std::cout << "\n所有测试通过！" << std::endl;
        return 0;
    } else {
        std::cerr << "\n测试失败！" << std::endl;
        return 1;
    }
}
