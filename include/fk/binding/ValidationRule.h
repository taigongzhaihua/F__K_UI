/**
 * @file ValidationRule.h
 * @brief Base class and common validation rules for data binding
 */

#pragma once

#include <any>
#include <string>
#include <memory>
#include <functional>
#include <limits>

namespace fk::binding {

/**
 * @brief Result of a validation operation
 */
struct ValidationResult {
    bool isValid{true};
    std::string errorMessage{};

    static ValidationResult Success() {
        return ValidationResult{true, ""};
    }

    static ValidationResult Error(std::string message) {
        return ValidationResult{false, std::move(message)};
    }
};

/**
 * @brief Base class for validation rules
 */
class ValidationRule {
public:
    virtual ~ValidationRule() = default;

    /**
     * @brief Validate a value
     * @param value The value to validate
     * @return ValidationResult indicating success or error
     */
    virtual ValidationResult Validate(const std::any& value) const = 0;
};

/**
 * @brief Validation rule that uses a custom function
 */
class FunctionValidationRule : public ValidationRule {
public:
    using ValidatorFunc = std::function<ValidationResult(const std::any&)>;

    explicit FunctionValidationRule(ValidatorFunc validator)
        : validator_(std::move(validator)) {}

    ValidationResult Validate(const std::any& value) const override {
        if (validator_) {
            return validator_(value);
        }
        return ValidationResult::Success();
    }

private:
    ValidatorFunc validator_;
};

/**
 * @brief Validation rule that checks if a value is not empty
 */
class NotEmptyValidationRule : public ValidationRule {
public:
    explicit NotEmptyValidationRule(std::string errorMessage = "Value cannot be empty")
        : errorMessage_(std::move(errorMessage)) {}

    ValidationResult Validate(const std::any& value) const override {
        if (!value.has_value()) {
            return ValidationResult::Error(errorMessage_);
        }

        // Check for empty string
        if (const auto* str = std::any_cast<std::string>(&value)) {
            if (str->empty()) {
                return ValidationResult::Error(errorMessage_);
            }
        }

        return ValidationResult::Success();
    }

private:
    std::string errorMessage_;
};

/**
 * @brief Validation rule for numeric range
 */
template<typename T>
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(T min, T max, std::string errorMessage = "")
        : min_(min), max_(max), errorMessage_(std::move(errorMessage)) {
        if (errorMessage_.empty()) {
            errorMessage_ = "Value must be between " + std::to_string(min) + 
                           " and " + std::to_string(max);
        }
    }

    ValidationResult Validate(const std::any& value) const override {
        if (!value.has_value()) {
            return ValidationResult::Error(errorMessage_);
        }

        const T* numPtr = std::any_cast<T>(&value);
        if (!numPtr) {
            return ValidationResult::Error("Invalid type for range validation");
        }

        if (*numPtr < min_ || *numPtr > max_) {
            return ValidationResult::Error(errorMessage_);
        }

        return ValidationResult::Success();
    }

private:
    T min_;
    T max_;
    std::string errorMessage_;
};

/**
 * @brief Validation rule for string length
 */
class StringLengthValidationRule : public ValidationRule {
public:
    StringLengthValidationRule(
        std::size_t minLength = 0,
        std::size_t maxLength = std::numeric_limits<std::size_t>::max(),
        std::string errorMessage = "")
        : minLength_(minLength)
        , maxLength_(maxLength)
        , errorMessage_(std::move(errorMessage)) {
        if (errorMessage_.empty()) {
            if (minLength_ > 0 && maxLength_ < std::numeric_limits<std::size_t>::max()) {
                errorMessage_ = "String length must be between " + 
                               std::to_string(minLength_) + " and " + 
                               std::to_string(maxLength_);
            } else if (minLength_ > 0) {
                errorMessage_ = "String length must be at least " + 
                               std::to_string(minLength_);
            } else {
                errorMessage_ = "String length must be at most " + 
                               std::to_string(maxLength_);
            }
        }
    }

    ValidationResult Validate(const std::any& value) const override {
        if (!value.has_value()) {
            if (minLength_ > 0) {
                return ValidationResult::Error(errorMessage_);
            }
            return ValidationResult::Success();
        }

        const auto* str = std::any_cast<std::string>(&value);
        if (!str) {
            return ValidationResult::Error("Value must be a string");
        }

        std::size_t length = str->length();
        if (length < minLength_ || length > maxLength_) {
            return ValidationResult::Error(errorMessage_);
        }

        return ValidationResult::Success();
    }

private:
    std::size_t minLength_;
    std::size_t maxLength_;
    std::string errorMessage_;
};

/**
 * @brief Validation rule for email format
 */
class EmailValidationRule : public ValidationRule {
public:
    explicit EmailValidationRule(std::string errorMessage = "Invalid email format")
        : errorMessage_(std::move(errorMessage)) {}

    ValidationResult Validate(const std::any& value) const override {
        if (!value.has_value()) {
            return ValidationResult::Error(errorMessage_);
        }

        const auto* str = std::any_cast<std::string>(&value);
        if (!str) {
            return ValidationResult::Error("Value must be a string");
        }

        // Simple email validation: contains @ and . after @
        auto atPos = str->find('@');
        if (atPos == std::string::npos || atPos == 0 || atPos == str->length() - 1) {
            return ValidationResult::Error(errorMessage_);
        }

        auto dotPos = str->find('.', atPos);
        if (dotPos == std::string::npos || dotPos == atPos + 1 || dotPos == str->length() - 1) {
            return ValidationResult::Error(errorMessage_);
        }

        return ValidationResult::Success();
    }

private:
    std::string errorMessage_;
};

} // namespace fk::binding

namespace fk {
    using binding::ValidationRule;
    using binding::ValidationResult;
    using binding::FunctionValidationRule;
    using binding::NotEmptyValidationRule;
    using binding::RangeValidationRule;
    using binding::StringLengthValidationRule;
    using binding::EmailValidationRule;
}
