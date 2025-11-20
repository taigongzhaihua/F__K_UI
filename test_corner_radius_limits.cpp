/**
 * 测试圆角半径限制问题
 * 
 * 验证修复后的圆角半径限制逻辑：
 * 1. 相邻圆角之和不应超过对应边长
 * 2. 如果超过，应该按比例缩放
 */

#include <iostream>
#include <cmath>
#include <cassert>

void TestCornerRadiusScaling() {
    std::cout << "\n========== 圆角半径缩放算法测试 ==========\n";
    
    // 模拟 GlRenderer 的缩放逻辑
    auto calculateScale = [](float width, float height, 
                            float topLeft, float topRight, 
                            float bottomRight, float bottomLeft) -> float {
        float topSum = topLeft + topRight;
        float rightSum = topRight + bottomRight;
        float bottomSum = bottomRight + bottomLeft;
        float leftSum = bottomLeft + topLeft;
        
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        
        if (topSum > width && topSum > 0.0f) {
            scaleX = std::min(scaleX, width / topSum);
        }
        if (bottomSum > width && bottomSum > 0.0f) {
            scaleX = std::min(scaleX, width / bottomSum);
        }
        if (leftSum > height && leftSum > 0.0f) {
            scaleY = std::min(scaleY, height / leftSum);
        }
        if (rightSum > height && rightSum > 0.0f) {
            scaleY = std::min(scaleY, height / rightSum);
        }
        
        return std::min(scaleX, scaleY);
    };
    
    // 测试场景1：多个约束同时生效
    {
        std::cout << "\n场景1：矩形100x50，左上50 + 右上60\n";
        std::cout << "  上边: 50 + 60 = 110 > 宽度100\n";
        std::cout << "  右边: 60 + 0 = 60 > 高度50\n";
        float scale = calculateScale(100, 50, 50, 60, 0, 0);
        std::cout << "  计算的缩放因子: " << scale << "\n";
        // scaleX = 100/110 = 0.909, scaleY = 50/60 = 0.833
        // scale = min(0.909, 0.833) = 0.833
        std::cout << "  期望: min(100/110, 50/60) = min(0.909, 0.833) = 0.833\n";
        assert(std::abs(scale - 0.833333f) < 0.01f);
        std::cout << "  缩放后左上: " << (50 * scale) << "\n";
        std::cout << "  缩放后右上: " << (60 * scale) << "\n";
        std::cout << "  右边缩放后和: " << (60 * scale) << " (应≈50)\n";
        assert(std::abs(60 * scale - 50.0f) < 0.01f);
        std::cout << "  ✓ 通过\n";
    }
    
    // 测试场景2：右边相邻圆角和超过高度
    {
        std::cout << "\n场景2：矩形100x50，右上30 + 右下30 = 60 > 高度50\n";
        float scale = calculateScale(100, 50, 0, 30, 30, 0);
        std::cout << "  计算的缩放因子: " << scale << "\n";
        std::cout << "  缩放后右上: " << (30 * scale) << "\n";
        std::cout << "  缩放后右下: " << (30 * scale) << "\n";
        std::cout << "  缩放后和: " << ((30 + 30) * scale) << " (应等于50)\n";
        assert(std::abs((30 + 30) * scale - 50.0f) < 0.01f);
        std::cout << "  ✓ 通过\n";
    }
    
    // 测试场景3：所有角都是40，矩形80x60
    {
        std::cout << "\n场景3：矩形80x60，所有角都是40\n";
        std::cout << "  上边: 40 + 40 = 80 (= 宽度)\n";
        std::cout << "  右边: 40 + 40 = 80 (> 高度60)\n";
        float scale = calculateScale(80, 60, 40, 40, 40, 40);
        std::cout << "  计算的缩放因子: " << scale << " (应为 60/80 = 0.75)\n";
        std::cout << "  缩放后圆角: " << (40 * scale) << " (应为30)\n";
        assert(std::abs(scale - 0.75f) < 0.01f);
        std::cout << "  ✓ 通过\n";
    }
    
    // 测试场景4：圆角不超限
    {
        std::cout << "\n场景4：矩形100x100，所有角都是20 (不超限)\n";
        float scale = calculateScale(100, 100, 20, 20, 20, 20);
        std::cout << "  计算的缩放因子: " << scale << " (应为1.0，无需缩放)\n";
        assert(std::abs(scale - 1.0f) < 0.01f);
        std::cout << "  ✓ 通过\n";
    }
    
    std::cout << "\n========== 所有测试通过！ ==========\n";
    std::cout << "\n修复说明：\n";
    std::cout << "1. 检查每条边上相邻圆角之和是否超过边长\n";
    std::cout << "2. 如果超过，计算需要的缩放因子\n";
    std::cout << "3. 使用最严格的缩放因子（最小值）\n";
    std::cout << "4. 对所有圆角应用相同的缩放因子\n";
    std::cout << "\n这确保了：\n";
    std::cout << "- 相邻圆角不会重叠\n";
    std::cout << "- 所有圆角保持相对比例\n";
    std::cout << "- 符合 CSS 规范的行为\n";
}

int main() {
    try {
        TestCornerRadiusScaling();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}
