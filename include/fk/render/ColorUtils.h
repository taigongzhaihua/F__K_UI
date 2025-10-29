#pragma once

#include <array>
#include <string>
#include <cstdint>

namespace fk::render {

/**
 * @brief 颜色工具类
 * 
 * 提供颜色字符串解析功能，支持以下格式：
 * - #RGB (如 #F00)
 * - #RRGGBB (如 #FF5733)
 * - #RRGGBBAA (如 #FF5733FF)
 */
class ColorUtils {
public:
    /**
     * @brief 解析颜色字符串为 RGBA 浮点数组
     * @param colorString 颜色字符串（#RGB, #RRGGBB, #RRGGBBAA）
     * @return [R, G, B, A] 每个分量范围 [0.0, 1.0]，如果解析失败返回黑色 {0, 0, 0, 1}
     */
    static std::array<float, 4> ParseColor(const std::string& colorString);

private:
    static uint8_t HexCharToInt(char c);
};

} // namespace fk::render
