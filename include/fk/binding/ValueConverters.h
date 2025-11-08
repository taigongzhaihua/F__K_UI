#pragma once

#include "fk/binding/Binding.h"

#include <any>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>

namespace fk::binding {

/**
 * @brief 尝试使用框架内置的默认规则进行类型转换。
 * @param value      原始值
 * @param targetType 目标类型
 * @param outResult  转换结果
 * @return true 表示转换成功，false 表示无法转换
 */
bool TryDefaultConvert(const std::any& value, std::type_index targetType, std::any& outResult);

/**
 * @brief 默认的类型转换器实现，配合 IValueConverter 接口供开发者直接使用。
 */
class DefaultValueConverter : public IValueConverter {
public:
    DefaultValueConverter() = default;
    ~DefaultValueConverter() override = default;

    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override;

    /**
     * @brief 获取单例实例，方便重复使用
     */
    static std::shared_ptr<DefaultValueConverter> Instance();
};

/**
 * @brief 一个简单的布尔值到字符串的转换器，可通过参数自定义 true/false 文本。
 *        ConverterParameter 期望传入 std::pair<std::string, std::string>
 */
class BooleanToStringConverter : public IValueConverter {
public:
    BooleanToStringConverter(std::string trueString = "True", std::string falseString = "False");

    void SetTrueString(std::string value);
    void SetFalseString(std::string value);

    std::any Convert(const std::any& value, std::type_index targetType, const std::any* parameter) const override;
    std::any ConvertBack(const std::any& value, std::type_index sourceType, const std::any* parameter) const override;

private:
    std::string trueString_;
    std::string falseString_;
};

} // namespace fk::binding
