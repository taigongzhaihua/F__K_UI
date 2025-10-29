#include "fk/render/ColorUtils.h"

namespace fk::render {

uint8_t ColorUtils::HexCharToInt(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

std::array<float, 4> ColorUtils::ParseColor(const std::string& colorString) {
    // 默认返回黑色不透明
    std::array<float, 4> result = {0.0f, 0.0f, 0.0f, 1.0f};

    if (colorString.empty() || colorString[0] != '#') {
        return result;
    }

    std::string hex = colorString.substr(1); // 去掉 #
    size_t len = hex.length();

    if (len == 3) {
        // #RGB -> #RRGGBB
        uint8_t r = HexCharToInt(hex[0]);
        uint8_t g = HexCharToInt(hex[1]);
        uint8_t b = HexCharToInt(hex[2]);
        
        result[0] = (r * 16 + r) / 255.0f;
        result[1] = (g * 16 + g) / 255.0f;
        result[2] = (b * 16 + b) / 255.0f;
        result[3] = 1.0f;
    }
    else if (len == 6) {
        // #RRGGBB
        uint8_t r = HexCharToInt(hex[0]) * 16 + HexCharToInt(hex[1]);
        uint8_t g = HexCharToInt(hex[2]) * 16 + HexCharToInt(hex[3]);
        uint8_t b = HexCharToInt(hex[4]) * 16 + HexCharToInt(hex[5]);
        
        result[0] = r / 255.0f;
        result[1] = g / 255.0f;
        result[2] = b / 255.0f;
        result[3] = 1.0f;
    }
    else if (len == 8) {
        // #RRGGBBAA
        uint8_t r = HexCharToInt(hex[0]) * 16 + HexCharToInt(hex[1]);
        uint8_t g = HexCharToInt(hex[2]) * 16 + HexCharToInt(hex[3]);
        uint8_t b = HexCharToInt(hex[4]) * 16 + HexCharToInt(hex[5]);
        uint8_t a = HexCharToInt(hex[6]) * 16 + HexCharToInt(hex[7]);
        
        result[0] = r / 255.0f;
        result[1] = g / 255.0f;
        result[2] = b / 255.0f;
        result[3] = a / 255.0f;
    }

    return result;
}

} // namespace fk::render
