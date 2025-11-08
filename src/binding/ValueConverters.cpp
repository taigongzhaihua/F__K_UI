#include "fk/binding/ValueConverters.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string_view>
#include <system_error>
#include <utility>

namespace fk::binding {

namespace {

template <typename T>
bool AnyCastCopy(const std::any& value, T& out) {
    if (const auto* ptr = std::any_cast<T>(&value)) {
        out = *ptr;
        return true;
    }
    return false;
}

bool TryGetString(const std::any& value, std::string& out) {
    if (AnyCastCopy<std::string>(value, out)) {
        return true;
    }
    if (const auto* sv = std::any_cast<std::string_view>(&value)) {
        out.assign(sv->begin(), sv->end());
        return true;
    }
    if (const auto* cstr = std::any_cast<const char*>(&value)) {
        if (*cstr) {
            out = *cstr;
        } else {
            out.clear();
        }
        return true;
    }
    if (const auto* cstr2 = std::any_cast<char*>(&value)) {
        if (*cstr2) {
            out = *cstr2;
        } else {
            out.clear();
        }
        return true;
    }
    return false;
}

bool TryParseInt64(const std::string& text, std::int64_t& out) {
    std::string_view sv(text);
    const char* first = sv.data();
    const char* last = sv.data() + sv.size();
    std::int64_t value = 0;
    auto result = std::from_chars(first, last, value);
    if (result.ec == std::errc() && result.ptr == last) {
        out = value;
        return true;
    }
    try {
        size_t idx = 0;
        long long parsed = std::stoll(text, &idx, 10);
        if (idx == text.size()) {
            out = static_cast<std::int64_t>(parsed);
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool TryParseUint64(const std::string& text, std::uint64_t& out) {
    std::string_view sv(text);
    const char* first = sv.data();
    const char* last = sv.data() + sv.size();
    std::uint64_t value = 0;
    auto result = std::from_chars(first, last, value);
    if (result.ec == std::errc() && result.ptr == last) {
        out = value;
        return true;
    }
    try {
        size_t idx = 0;
        unsigned long long parsed = std::stoull(text, &idx, 10);
        if (idx == text.size()) {
            out = static_cast<std::uint64_t>(parsed);
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool TryParseDouble(const std::string& text, double& out) {
    std::string_view sv(text);
    const char* first = sv.data();
    const char* last = sv.data() + sv.size();
    double value = 0.0;
    auto result = std::from_chars(first, last, value);
    if (result.ec == std::errc() && result.ptr == last) {
        out = value;
        return true;
    }
    try {
        size_t idx = 0;
        double parsed = std::stod(text, &idx);
        if (idx == text.size()) {
            out = parsed;
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool TryGetDouble(const std::any& value, double& out) {
    if (AnyCastCopy<double>(value, out)) {
        return true;
    }
    float f = 0.0f;
    if (AnyCastCopy<float>(value, f)) {
        out = static_cast<double>(f);
        return true;
    }
    std::int64_t i = 0;
    if (AnyCastCopy<std::int64_t>(value, i)) {
        out = static_cast<double>(i);
        return true;
    }
    std::int32_t i32 = 0;
    if (AnyCastCopy<std::int32_t>(value, i32)) {
        out = static_cast<double>(i32);
        return true;
    }
    std::uint64_t u = 0;
    if (AnyCastCopy<std::uint64_t>(value, u)) {
        out = static_cast<double>(u);
        return true;
    }
    std::uint32_t u32 = 0;
    if (AnyCastCopy<std::uint32_t>(value, u32)) {
        out = static_cast<double>(u32);
        return true;
    }
    bool b = false;
    if (AnyCastCopy<bool>(value, b)) {
        out = b ? 1.0 : 0.0;
        return true;
    }
    std::string str;
    if (TryGetString(value, str)) {
        return TryParseDouble(str, out);
    }
    return false;
}

bool TryGetInt64(const std::any& value, std::int64_t& out) {
    if (AnyCastCopy<std::int64_t>(value, out)) {
        return true;
    }
    std::int32_t i32 = 0;
    if (AnyCastCopy<std::int32_t>(value, i32)) {
        out = static_cast<std::int64_t>(i32);
        return true;
    }
    bool b = false;
    if (AnyCastCopy<bool>(value, b)) {
        out = b ? 1 : 0;
        return true;
    }
    double d = 0.0;
    if (AnyCastCopy<double>(value, d)) {
        if (d < static_cast<double>(std::numeric_limits<std::int64_t>::min()) ||
            d > static_cast<double>(std::numeric_limits<std::int64_t>::max())) {
            return false;
        }
        out = static_cast<std::int64_t>(d);
        return true;
    }
    float f = 0.0f;
    if (AnyCastCopy<float>(value, f)) {
        if (f < static_cast<float>(std::numeric_limits<std::int64_t>::min()) ||
            f > static_cast<float>(std::numeric_limits<std::int64_t>::max())) {
            return false;
        }
        out = static_cast<std::int64_t>(f);
        return true;
    }
    std::string str;
    if (TryGetString(value, str)) {
        return TryParseInt64(str, out);
    }
    return false;
}

bool TryGetUint64(const std::any& value, std::uint64_t& out) {
    if (AnyCastCopy<std::uint64_t>(value, out)) {
        return true;
    }
    std::uint32_t u32 = 0;
    if (AnyCastCopy<std::uint32_t>(value, u32)) {
        out = static_cast<std::uint64_t>(u32);
        return true;
    }
    bool b = false;
    if (AnyCastCopy<bool>(value, b)) {
        out = b ? 1u : 0u;
        return true;
    }
    double d = 0.0;
    if (AnyCastCopy<double>(value, d)) {
        if (d < 0.0 || d > static_cast<double>(std::numeric_limits<std::uint64_t>::max())) {
            return false;
        }
        out = static_cast<std::uint64_t>(d);
        return true;
    }
    float f = 0.0f;
    if (AnyCastCopy<float>(value, f)) {
        if (f < 0.0f || f > static_cast<float>(std::numeric_limits<std::uint64_t>::max())) {
            return false;
        }
        out = static_cast<std::uint64_t>(f);
        return true;
    }
    std::string str;
    if (TryGetString(value, str)) {
        return TryParseUint64(str, out);
    }
    std::int64_t i = 0;
    if (AnyCastCopy<std::int64_t>(value, i)) {
        if (i < 0) {
            return false;
        }
        out = static_cast<std::uint64_t>(i);
        return true;
    }
    return false;
}

bool TryGetBool(const std::any& value, bool& out) {
    if (AnyCastCopy<bool>(value, out)) {
        return true;
    }
    std::string str;
    if (TryGetString(value, str)) {
        std::string lowered = str;
        std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (lowered == "true" || lowered == "1" || lowered == "on" || lowered == "yes") {
            out = true;
            return true;
        }
        if (lowered == "false" || lowered == "0" || lowered == "off" || lowered == "no") {
            out = false;
            return true;
        }
        return false;
    }
    double numeric = 0.0;
    if (TryGetDouble(value, numeric)) {
        out = std::abs(numeric) > std::numeric_limits<double>::epsilon();
        return true;
    }
    return false;
}

std::string ToString(const std::any& value) {
    if (!value.has_value()) {
        return {};
    }
    std::string str;
    if (TryGetString(value, str)) {
        return str;
    }
    bool b = false;
    if (AnyCastCopy<bool>(value, b)) {
        return b ? "true" : "false";
    }
    std::ostringstream oss;
    if (const auto* i = std::any_cast<std::int64_t>(&value)) {
        oss << *i;
        return oss.str();
    }
    if (const auto* i32 = std::any_cast<std::int32_t>(&value)) {
        oss << *i32;
        return oss.str();
    }
    if (const auto* u = std::any_cast<std::uint64_t>(&value)) {
        oss << *u;
        return oss.str();
    }
    if (const auto* u32 = std::any_cast<std::uint32_t>(&value)) {
        oss << *u32;
        return oss.str();
    }
    if (const auto* d = std::any_cast<double>(&value)) {
        oss << *d;
        return oss.str();
    }
    if (const auto* f = std::any_cast<float>(&value)) {
        oss << *f;
        return oss.str();
    }
    // 兜底：直接使用 type name
    return std::string("<") + value.type().name() + ">";
}

} // namespace

bool TryDefaultConvert(const std::any& value, std::type_index targetType, std::any& outResult) {
    if (!value.has_value()) {
        outResult.reset();
        return true;
    }

    if (targetType == std::type_index(value.type())) {
        outResult = value;
        return true;
    }

    if (targetType == std::type_index(typeid(std::string))) {
        outResult = ToString(value);
        return true;
    }

    if (targetType == std::type_index(typeid(bool))) {
        bool result = false;
        if (TryGetBool(value, result)) {
            outResult = result;
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(int))) {
        std::int64_t temp = 0;
        if (TryGetInt64(value, temp) &&
            temp >= std::numeric_limits<int>::min() &&
            temp <= std::numeric_limits<int>::max()) {
            outResult = static_cast<int>(temp);
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(std::int64_t))) {
        std::int64_t temp = 0;
        if (TryGetInt64(value, temp)) {
            outResult = temp;
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(std::uint64_t))) {
        std::uint64_t temp = 0;
        if (TryGetUint64(value, temp)) {
            outResult = temp;
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(std::uint32_t))) {
        std::uint64_t temp = 0;
        if (TryGetUint64(value, temp) && temp <= std::numeric_limits<std::uint32_t>::max()) {
            outResult = static_cast<std::uint32_t>(temp);
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(float))) {
        double temp = 0.0;
        if (TryGetDouble(value, temp)) {
            if (temp < -std::numeric_limits<float>::max() || temp > std::numeric_limits<float>::max()) {
                return false;
            }
            outResult = static_cast<float>(temp);
            return true;
        }
        return false;
    }

    if (targetType == std::type_index(typeid(double))) {
        double temp = 0.0;
        if (TryGetDouble(value, temp)) {
            outResult = temp;
            return true;
        }
        return false;
    }

    // 没有匹配的规则时，返回失败
    return false;
}

std::any DefaultValueConverter::Convert(const std::any& value, std::type_index targetType, const std::any*) const {
    std::any converted;
    if (TryDefaultConvert(value, targetType, converted)) {
        return converted;
    }
    return value;
}

std::any DefaultValueConverter::ConvertBack(const std::any& value, std::type_index sourceType, const std::any*) const {
    std::any converted;
    if (TryDefaultConvert(value, sourceType, converted)) {
        return converted;
    }
    return value;
}

std::shared_ptr<DefaultValueConverter> DefaultValueConverter::Instance() {
    static std::shared_ptr<DefaultValueConverter> instance{new DefaultValueConverter()};
    return instance;
}

BooleanToStringConverter::BooleanToStringConverter(std::string trueString, std::string falseString)
    : trueString_(std::move(trueString))
    , falseString_(std::move(falseString)) {}

void BooleanToStringConverter::SetTrueString(std::string value) {
    trueString_ = std::move(value);
}

void BooleanToStringConverter::SetFalseString(std::string value) {
    falseString_ = std::move(value);
}

std::any BooleanToStringConverter::Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const {
    bool boolValue = false;
    if (!TryGetBool(value, boolValue)) {
        return value;
    }

    std::string trueString = trueString_;
    std::string falseString = falseString_;
    if (parameter) {
        if (const auto* pairParam = std::any_cast<std::pair<std::string, std::string>>(parameter)) {
            trueString = pairParam->first;
            falseString = pairParam->second;
        }
    }

    if (targetType == std::type_index(typeid(std::string))) {
        return boolValue ? trueString : falseString;
    }

    std::any fallback;
    if (TryDefaultConvert(boolValue ? std::any(trueString) : std::any(falseString), targetType, fallback)) {
        return fallback;
    }
    return value;
}

std::any BooleanToStringConverter::ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const {
    std::pair<std::string, std::string> options{trueString_, falseString_};
    if (parameter) {
        if (const auto* pairParam = std::any_cast<std::pair<std::string, std::string>>(parameter)) {
            options = *pairParam;
        }
    }

    bool result = false;
    std::string str;
    if (TryGetString(value, str)) {
        if (str == options.first) {
            result = true;
        } else if (str == options.second) {
            result = false;
        } else {
            if (!TryGetBool(value, result)) {
                return value;
            }
        }
    } else {
        if (!TryGetBool(value, result)) {
            return value;
        }
    }

    std::any converted;
    if (TryDefaultConvert(result, sourceType, converted)) {
        return converted;
    }
    return value;
}

} // namespace fk::binding
