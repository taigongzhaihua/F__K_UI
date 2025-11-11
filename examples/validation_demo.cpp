/**
 * @file validation_demo.cpp
 * @brief Demonstrates data validation system
 * 
 * Features:
 * 1. INotifyDataErrorInfo interface
 * 2. ValidationRule classes (NotEmpty, Range, StringLength, Email)
 * 3. Binding validation integration
 * 4. Error tracking and notification
 */

#include "fk/binding/Binding.h"
#include "fk/binding/ValidationRule.h"
#include "fk/binding/INotifyDataErrorInfo.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include <iostream>
#include <string>
#include <memory>
#include <map>

using namespace fk::binding;

// ===== User Model with Validation =====
class UserModel : public INotifyPropertyChanged, public INotifyDataErrorInfo {
public:
    UserModel() = default;

    // Name property
    const std::string& GetName() const { return name_; }
    void SetName(std::string value) {
        if (name_ != value) {
            name_ = std::move(value);
            propertyChanged_("Name");
            ValidateName();
        }
    }

    // Email property
    const std::string& GetEmail() const { return email_; }
    void SetEmail(std::string value) {
        if (email_ != value) {
            email_ = std::move(value);
            propertyChanged_("Email");
            ValidateEmail();
        }
    }

    // Age property
    int GetAge() const { return age_; }
    void SetAge(int value) {
        if (age_ != value) {
            age_ = std::move(value);
            propertyChanged_("Age");
            ValidateAge();
        }
    }

    // INotifyPropertyChanged
    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

    // INotifyDataErrorInfo
    bool HasErrors() const override {
        for (const auto& [_, errors] : errors_) {
            if (!errors.empty()) {
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> GetErrors(std::string_view propertyName) const override {
        auto it = errors_.find(std::string(propertyName));
        if (it != errors_.end()) {
            return it->second;
        }
        return {};
    }

    ErrorsChangedEvent& ErrorsChanged() override { return errorsChanged_; }

private:
    void ValidateName() {
        ClearErrors("Name");
        
        if (name_.empty()) {
            AddError("Name", "Name cannot be empty");
        } else if (name_.length() < 2) {
            AddError("Name", "Name must be at least 2 characters");
        } else if (name_.length() > 50) {
            AddError("Name", "Name must be at most 50 characters");
        }
    }

    void ValidateEmail() {
        ClearErrors("Email");
        
        if (email_.empty()) {
            AddError("Email", "Email cannot be empty");
        } else {
            // Simple email validation
            auto atPos = email_.find('@');
            if (atPos == std::string::npos || atPos == 0 || atPos == email_.length() - 1) {
                AddError("Email", "Invalid email format");
            } else {
                auto dotPos = email_.find('.', atPos);
                if (dotPos == std::string::npos || dotPos == atPos + 1 || dotPos == email_.length() - 1) {
                    AddError("Email", "Invalid email format");
                }
            }
        }
    }

    void ValidateAge() {
        ClearErrors("Age");
        
        if (age_ < 0) {
            AddError("Age", "Age cannot be negative");
        } else if (age_ > 150) {
            AddError("Age", "Age must be at most 150");
        }
    }

    void AddError(std::string propertyName, std::string error) {
        errors_[propertyName].push_back(std::move(error));
        errorsChanged_(propertyName);
    }

    void ClearErrors(std::string propertyName) {
        auto it = errors_.find(propertyName);
        if (it != errors_.end() && !it->second.empty()) {
            it->second.clear();
            errorsChanged_(propertyName);
        }
    }

    std::string name_;
    std::string email_;
    int age_{0};
    
    PropertyChangedEvent propertyChanged_;
    ErrorsChangedEvent errorsChanged_;
    std::map<std::string, std::vector<std::string>> errors_;
};

// ===== Test Validation Rules Independently =====
void TestValidationRules() {
    std::cout << "========== Testing Validation Rules ==========\n\n";

    // Test NotEmptyValidationRule
    {
        std::cout << "[Test 1] NotEmptyValidationRule\n";
        NotEmptyValidationRule rule("Field cannot be empty");
        
        auto result1 = rule.Validate(std::string("Hello"));
        std::cout << "  \"Hello\" -> " << (result1.isValid ? "Valid" : "Invalid: " + result1.errorMessage) << "\n";
        
        auto result2 = rule.Validate(std::string(""));
        std::cout << "  \"\" -> " << (result2.isValid ? "Valid" : "Invalid: " + result2.errorMessage) << "\n";
        
        auto result3 = rule.Validate(std::any{});
        std::cout << "  empty any -> " << (result3.isValid ? "Valid" : "Invalid: " + result3.errorMessage) << "\n\n";
    }

    // Test RangeValidationRule
    {
        std::cout << "[Test 2] RangeValidationRule<int>\n";
        RangeValidationRule<int> rule(0, 100);
        
        auto result1 = rule.Validate(50);
        std::cout << "  50 -> " << (result1.isValid ? "Valid" : "Invalid: " + result1.errorMessage) << "\n";
        
        auto result2 = rule.Validate(-10);
        std::cout << "  -10 -> " << (result2.isValid ? "Valid" : "Invalid: " + result2.errorMessage) << "\n";
        
        auto result3 = rule.Validate(150);
        std::cout << "  150 -> " << (result3.isValid ? "Valid" : "Invalid: " + result3.errorMessage) << "\n\n";
    }

    // Test StringLengthValidationRule
    {
        std::cout << "[Test 3] StringLengthValidationRule\n";
        StringLengthValidationRule rule(3, 10);
        
        auto result1 = rule.Validate(std::string("Hello"));
        std::cout << "  \"Hello\" (5 chars) -> " << (result1.isValid ? "Valid" : "Invalid: " + result1.errorMessage) << "\n";
        
        auto result2 = rule.Validate(std::string("Hi"));
        std::cout << "  \"Hi\" (2 chars) -> " << (result2.isValid ? "Valid" : "Invalid: " + result2.errorMessage) << "\n";
        
        auto result3 = rule.Validate(std::string("VeryLongString"));
        std::cout << "  \"VeryLongString\" (14 chars) -> " << (result3.isValid ? "Valid" : "Invalid: " + result3.errorMessage) << "\n\n";
    }

    // Test EmailValidationRule
    {
        std::cout << "[Test 4] EmailValidationRule\n";
        EmailValidationRule rule;
        
        auto result1 = rule.Validate(std::string("user@example.com"));
        std::cout << "  \"user@example.com\" -> " << (result1.isValid ? "Valid" : "Invalid: " + result1.errorMessage) << "\n";
        
        auto result2 = rule.Validate(std::string("invalid.email"));
        std::cout << "  \"invalid.email\" -> " << (result2.isValid ? "Valid" : "Invalid: " + result2.errorMessage) << "\n";
        
        auto result3 = rule.Validate(std::string("@example.com"));
        std::cout << "  \"@example.com\" -> " << (result3.isValid ? "Valid" : "Invalid: " + result3.errorMessage) << "\n\n";
    }
}

// ===== Test INotifyDataErrorInfo =====
void TestDataErrorInfo() {
    std::cout << "========== Testing INotifyDataErrorInfo ==========\n\n";

    auto user = std::make_shared<UserModel>();

    // Subscribe to ErrorsChanged
    auto connection = user->ErrorsChanged().Connect([](std::string_view propertyName) {
        std::cout << "[ErrorsChanged] Property: " << propertyName << "\n";
    });

    std::cout << "[Test] Setting valid values...\n";
    user->SetName("John Doe");
    user->SetEmail("john@example.com");
    user->SetAge(30);
    std::cout << "HasErrors: " << (user->HasErrors() ? "Yes" : "No") << "\n\n";

    std::cout << "[Test] Setting invalid Name (empty)...\n";
    user->SetName("");
    auto nameErrors = user->GetErrors("Name");
    std::cout << "Name errors (" << nameErrors.size() << "):\n";
    for (const auto& error : nameErrors) {
        std::cout << "  - " << error << "\n";
    }
    std::cout << "HasErrors: " << (user->HasErrors() ? "Yes" : "No") << "\n\n";

    std::cout << "[Test] Setting invalid Email...\n";
    user->SetEmail("invalid");
    auto emailErrors = user->GetErrors("Email");
    std::cout << "Email errors (" << emailErrors.size() << "):\n";
    for (const auto& error : emailErrors) {
        std::cout << "  - " << error << "\n";
    }
    std::cout << "HasErrors: " << (user->HasErrors() ? "Yes" : "No") << "\n\n";

    std::cout << "[Test] Setting invalid Age...\n";
    user->SetAge(200);
    auto ageErrors = user->GetErrors("Age");
    std::cout << "Age errors (" << ageErrors.size() << "):\n";
    for (const auto& error : ageErrors) {
        std::cout << "  - " << error << "\n";
    }
    std::cout << "HasErrors: " << (user->HasErrors() ? "Yes" : "No") << "\n\n";

    std::cout << "[Test] Fixing all errors...\n";
    user->SetName("Jane Smith");
    user->SetEmail("jane@example.com");
    user->SetAge(25);
    std::cout << "HasErrors: " << (user->HasErrors() ? "Yes" : "No") << "\n\n";
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  Data Validation System Demonstration\n";
    std::cout << "=================================================\n\n";

    TestValidationRules();
    TestDataErrorInfo();

    std::cout << "=================================================\n";
    std::cout << "  Validation System Complete!\n";
    std::cout << "=================================================\n";
    std::cout << "\nImplemented Features:\n";
    std::cout << "  ✅ INotifyDataErrorInfo interface\n";
    std::cout << "  ✅ ValidationRule base class\n";
    std::cout << "  ✅ NotEmptyValidationRule\n";
    std::cout << "  ✅ RangeValidationRule<T>\n";
    std::cout << "  ✅ StringLengthValidationRule\n";
    std::cout << "  ✅ EmailValidationRule\n";
    std::cout << "  ✅ Error tracking and notification\n";
    std::cout << "  ⚠️  BindingExpression integration (next step)\n";
    std::cout << "=================================================\n";

    return 0;
}
