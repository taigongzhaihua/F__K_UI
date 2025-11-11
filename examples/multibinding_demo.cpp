/**
 * @file multibinding_demo.cpp
 * @brief Demonstrates MultiBinding concept (simplified implementation)
 * 
 * This example shows the idea of combining multiple source properties:
 * 1. IMultiValueConverter interface
 * 2. MultiBinding class structure
 * 3. Combining FirstName + LastName → FullName
 */

#include "fk/binding/Binding.h"
#include "fk/binding/MultiBinding.h"
#include "fk/binding/BindingExpression.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/binding/BindingPath.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace fk::binding;

// ===== Full Name Converter =====
class FullNameConverter : public IMultiValueConverter {
public:
    std::any Convert(
        const std::vector<std::any>& values,
        std::type_index targetType,
        const std::any* parameter) const override {
        
        if (values.size() < 2) {
            return std::string("");
        }

        std::string firstName;
        std::string lastName;

        try {
            if (values[0].has_value()) {
                firstName = std::any_cast<std::string>(values[0]);
            }
            if (values[1].has_value()) {
                lastName = std::any_cast<std::string>(values[1]);
            }
        } catch (...) {
            return std::string("");
        }

        return firstName + " " + lastName;
    }

    std::vector<std::any> ConvertBack(
        const std::any& value,
        const std::vector<std::type_index>& sourceTypes,
        const std::any* parameter) const override {
        // Not supported for this demo
        return {};
    }
};

// ===== Model =====
class PersonModel : public INotifyPropertyChanged {
public:
    PersonModel(std::string firstName, std::string lastName)
        : firstName_(std::move(firstName))
        , lastName_(std::move(lastName)) {}

    const std::string& GetFirstName() const { return firstName_; }
    void SetFirstName(std::string value) {
        if (firstName_ != value) {
            firstName_ = std::move(value);
            propertyChanged_("FirstName");
        }
    }

    const std::string& GetLastName() const { return lastName_; }
    void SetLastName(std::string value) {
        if (lastName_ != value) {
            lastName_ = std::move(value);
            propertyChanged_("LastName");
        }
    }

    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

private:
    std::string firstName_;
    std::string lastName_;
    PropertyChangedEvent propertyChanged_;
};

// ===== Mock UI =====
class MockLabel : public DependencyObject {
public:
    static const DependencyProperty& TextProperty() {
        static const DependencyProperty& prop = DependencyProperty::Register(
            "Text",
            typeid(std::string),
            typeid(MockLabel),
            PropertyMetadata{
                std::string(""),
                nullptr,
                nullptr,
                BindingOptions{BindingMode::OneWay, UpdateSourceTrigger::PropertyChanged, false}
            }
        );
        return prop;
    }

    std::string GetText() const {
        return GetValue<std::string>(TextProperty());
    }
};

// ===== Property Accessors =====
void RegisterAccessors() {
    // Register for INotifyPropertyChanged base class
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "FirstName",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        result = person->GetFirstName();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                            person->SetFirstName(*strPtr);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    );

    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "LastName",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        result = person->GetLastName();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<PersonModel>(*ptr)) {
                        if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                            person->SetLastName(*strPtr);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    );
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  MultiBinding Demonstration\n";
    std::cout << "=================================================\n";

    RegisterAccessors();

    std::cout << "\n========== Concept Demo ==========\n";
    std::cout << "MultiBinding API Structure:\n\n";

    std::cout << "  MultiBinding multi;\n";
    std::cout << "  multi.AddBinding(Binding().Path(\"FirstName\").Source(person));\n";
    std::cout << "  multi.AddBinding(Binding().Path(\"LastName\").Source(person));\n";
    std::cout << "  multi.Converter(std::make_shared<FullNameConverter>());\n";
    std::cout << "  label->SetBinding(Label::TextProperty(), multi);\n\n";

    std::cout << "Result: FirstName + LastName → FullName\n\n";

    // Create model
    auto person = std::make_shared<PersonModel>("John", "Doe");
    std::shared_ptr<INotifyPropertyChanged> source = person;

    // Test converter directly
    auto converter = std::make_shared<FullNameConverter>();
    
    std::vector<std::any> values;
    values.push_back(person->GetFirstName());
    values.push_back(person->GetLastName());

    auto fullName = converter->Convert(values, typeid(std::string), nullptr);
    
    std::cout << "Converter Test:\n";
    std::cout << "  FirstName: \"" << person->GetFirstName() << "\"\n";
    std::cout << "  LastName:  \"" << person->GetLastName() << "\"\n";
    std::cout << "  FullName:  \"" << std::any_cast<std::string>(fullName) << "\"\n\n";

    // Update names
    person->SetFirstName("Jane");
    person->SetLastName("Smith");

    values[0] = person->GetFirstName();
    values[1] = person->GetLastName();
    fullName = converter->Convert(values, typeid(std::string), nullptr);

    std::cout << "After update:\n";
    std::cout << "  FirstName: \"" << person->GetFirstName() << "\"\n";
    std::cout << "  LastName:  \"" << person->GetLastName() << "\"\n";
    std::cout << "  FullName:  \"" << std::any_cast<std::string>(fullName) << "\"\n\n";

    std::cout << "=================================================\n";
    std::cout << "  MultiBinding Infrastructure Complete!\n";
    std::cout << "=================================================\n\n";

    std::cout << "Implementation Status:\n";
    std::cout << "  ✅ IMultiValueConverter interface\n";
    std::cout << "  ✅ MultiBinding class\n";
    std::cout << "  ✅ MultiBindingExpression class\n";
    std::cout << "  ✅ Converter example (FullNameConverter)\n";
    std::cout << "  ⚠️  Full integration pending (requires child binding orchestration)\n\n";

    std::cout << "Note: Complete MultiBinding requires:\n";
    std::cout << "  - Child binding value tracking\n";
    std::cout << "  - Subscription to multiple source changes\n";
    std::cout << "  - Coordinated updates when any source changes\n";
    std::cout << "=================================================\n";

    return 0;
}
