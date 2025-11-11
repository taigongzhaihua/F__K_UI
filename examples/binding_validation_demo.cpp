/**
 * @file binding_validation_demo.cpp
 * @brief Demonstrates Binding with ValidationRule integration
 * 
 * Features:
 * 1. Binding.AddValidationRule() API
 * 2. Automatic validation on UpdateSource
 * 3. ValidationErrorsChanged event
 * 4. Error display and user feedback
 */

#include "fk/binding/Binding.h"
#include "fk/binding/ValidationRule.h"
#include "fk/binding/BindingExpression.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/BindingPath.h"
#include <iostream>
#include <string>
#include <memory>

using namespace fk::binding;

// ===== Simple Model =====
class PersonModel : public INotifyPropertyChanged {
public:
    const std::string& GetName() const { return name_; }
    void SetName(std::string value) {
        if (name_ != value) {
            std::cout << "[Model] Name changed: \"" << name_ << "\" -> \"" << value << "\"\n";
            name_ = std::move(value);
            propertyChanged_("Name");
        }
    }

    int GetAge() const { return age_; }
    void SetAge(int value) {
        if (age_ != value) {
            std::cout << "[Model] Age changed: " << age_ << " -> " << value << "\n";
            age_ = value;
            propertyChanged_("Age");
        }
    }

    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

private:
    std::string name_;
    int age_{0};
    PropertyChangedEvent propertyChanged_;
};

// ===== Mock TextBox =====
class MockTextBox : public DependencyObject {
public:
    static const DependencyProperty& TextProperty() {
        static const DependencyProperty& prop = DependencyProperty::Register(
            "Text",
            typeid(std::string),
            typeid(MockTextBox),
            PropertyMetadata{
                std::string(""),
                nullptr,
                nullptr,
                BindingOptions{BindingMode::TwoWay, UpdateSourceTrigger::PropertyChanged, false}
            }
        );
        return prop;
    }

    std::string GetText() const {
        return GetValue<std::string>(TextProperty());
    }

    void SetText(const std::string& value) {
        SetValue(TextProperty(), value);
    }
};

// ===== Mock NumericInput =====
class MockNumericInput : public DependencyObject {
public:
    static const DependencyProperty& ValueProperty() {
        static const DependencyProperty& prop = DependencyProperty::Register(
            "Value",
            typeid(int),
            typeid(MockNumericInput),
            PropertyMetadata{
                0,
                nullptr,
                nullptr,
                BindingOptions{BindingMode::TwoWay, UpdateSourceTrigger::PropertyChanged, false}
            }
        );
        return prop;
    }

    int GetNumber() const {
        return DependencyObject::GetValue<int>(ValueProperty());
    }

    void SetNumber(int value) {
        DependencyObject::SetValue(ValueProperty(), value);
    }
};

// ===== Property Accessors =====
void RegisterAccessors() {
    // PersonModel - Name
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "Name",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        result = person->GetName();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        if (const auto* str = std::any_cast<std::string>(&value)) {
                            person->SetName(*str);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    );

    // PersonModel - Age
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "Age",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        result = person->GetAge();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        if (const auto* num = std::any_cast<int>(&value)) {
                            person->SetAge(*num);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    );
}

// ===== Test: Name with Validation =====
void TestNameValidation() {
    std::cout << "\n========== Test 1: Name Validation ==========\n";

    auto person = std::make_shared<PersonModel>();
    std::shared_ptr<INotifyPropertyChanged> source = person;

    auto textBox = std::make_shared<MockTextBox>();

    // Create binding with validation rules
    Binding binding;
    binding.Path("Name")
           .Source(source)
           .Mode(BindingMode::TwoWay)
           .AddValidationRule(std::make_shared<NotEmptyValidationRule>("Name cannot be empty"))
           .AddValidationRule(std::make_shared<StringLengthValidationRule>(2, 20, "Name must be 2-20 characters"));

    textBox->SetBinding(MockTextBox::TextProperty(), binding);

    // Subscribe to validation errors after binding is set
    auto bindingExpr = textBox->GetBinding(MockTextBox::TextProperty());
    BindingExpression::ValidationErrorsChangedEvent::Connection errorConnection;
    if (bindingExpr) {
        std::cout << "[Setup] Subscribing to ValidationErrorsChanged...\n";
        errorConnection = bindingExpr->ValidationErrorsChanged.Connect([](const std::vector<ValidationResult>& errors) {
            if (errors.empty()) {
                std::cout << "[Validation] ✅ No errors\n";
            } else {
                std::cout << "[Validation] ❌ " << errors.size() << " error(s):\n";
                for (const auto& error : errors) {
                    std::cout << "  - " << error.errorMessage << "\n";
                }
            }
        });
    } else {
        std::cout << "[Setup] Warning: No binding expression found!\n";
    }

    std::cout << "\n[Test] Setting valid name...\n";
    textBox->SetText("Alice");
    std::cout << "Person.Name: \"" << person->GetName() << "\"\n";

    std::cout << "\n[Test] Setting empty name (should fail validation)...\n";
    textBox->SetText("");
    std::cout << "Person.Name: \"" << person->GetName() << "\" (unchanged)\n";

    std::cout << "\n[Test] Setting too short name...\n";
    textBox->SetText("A");
    std::cout << "Person.Name: \"" << person->GetName() << "\" (unchanged)\n";

    std::cout << "\n[Test] Setting too long name...\n";
    textBox->SetText("ThisNameIsWayTooLongForValidation");
    std::cout << "Person.Name: \"" << person->GetName() << "\" (unchanged)\n";

    std::cout << "\n[Test] Setting another valid name...\n";
    textBox->SetText("Bob");
    std::cout << "Person.Name: \"" << person->GetName() << "\"\n";
}

// ===== Test: Age with Range Validation =====
void TestAgeValidation() {
    std::cout << "\n========== Test 2: Age Range Validation ==========\n";

    auto person = std::make_shared<PersonModel>();
    std::shared_ptr<INotifyPropertyChanged> source = person;

    auto numericInput = std::make_shared<MockNumericInput>();

    // Create binding with range validation
    Binding binding;
    binding.Path("Age")
           .Source(source)
           .Mode(BindingMode::TwoWay)
           .AddValidationRule(std::make_shared<RangeValidationRule<int>>(0, 150, "Age must be 0-150"));

    numericInput->SetBinding(MockNumericInput::ValueProperty(), binding);

    // Subscribe to validation errors after binding is set
    auto bindingExpr = numericInput->GetBinding(MockNumericInput::ValueProperty());
    BindingExpression::ValidationErrorsChangedEvent::Connection errorConnection;
    if (bindingExpr) {
        std::cout << "[Setup] Subscribing to ValidationErrorsChanged...\n";
        errorConnection = bindingExpr->ValidationErrorsChanged.Connect([](const std::vector<ValidationResult>& errors) {
            if (errors.empty()) {
                std::cout << "[Validation] ✅ No errors\n";
            } else {
                std::cout << "[Validation] ❌ " << errors.size() << " error(s):\n";
                for (const auto& error : errors) {
                    std::cout << "  - " << error.errorMessage << "\n";
                }
            }
        });
    } else {
        std::cout << "[Setup] Warning: No binding expression found!\n";
    }

    std::cout << "\n[Test] Setting valid age (25)...\n";
    numericInput->SetNumber(25);
    std::cout << "Person.Age: " << person->GetAge() << "\n";

    std::cout << "\n[Test] Setting negative age (-5)...\n";
    numericInput->SetNumber(-5);
    std::cout << "Person.Age: " << person->GetAge() << " (unchanged)\n";

    std::cout << "\n[Test] Setting age over limit (200)...\n";
    numericInput->SetNumber(200);
    std::cout << "Person.Age: " << person->GetAge() << " (unchanged)\n";

    std::cout << "\n[Test] Setting another valid age (42)...\n";
    numericInput->SetNumber(42);
    std::cout << "Person.Age: " << person->GetAge() << "\n";
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  Binding + Validation Integration Demo\n";
    std::cout << "=================================================\n";

    RegisterAccessors();

    TestNameValidation();
    TestAgeValidation();

    std::cout << "\n=================================================\n";
    std::cout << "  Binding Validation Complete!\n";
    std::cout << "=================================================\n";
    std::cout << "\nImplemented Features:\n";
    std::cout << "  ✅ Binding.AddValidationRule() API\n";
    std::cout << "  ✅ Automatic validation on UpdateSource\n";
    std::cout << "  ✅ ValidationErrorsChanged event\n";
    std::cout << "  ✅ Validation prevents invalid updates\n";
    std::cout << "  ✅ Multiple validation rules per binding\n";
    std::cout << "=================================================\n";

    return 0;
}
