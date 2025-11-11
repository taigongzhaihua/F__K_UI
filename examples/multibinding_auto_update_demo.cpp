/**
 * @file multibinding_auto_update_demo.cpp
 * @brief Demonstrates MultiBinding automatic update mechanism
 * 
 * Tests:
 * 1. Source property changes trigger automatic target updates
 * 2. Multiple sources tracked independently
 * 3. Converter receives updated values
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
        
        std::cout << "  [Converter] Called with " << values.size() << " values\n";
        
        if (values.size() < 2) {
            return std::string("[Missing values]");
        }

        std::string firstName = "[empty]";
        std::string lastName = "[empty]";

        try {
            if (values[0].has_value()) {
                firstName = std::any_cast<std::string>(values[0]);
                std::cout << "  [Converter]   FirstName: \"" << firstName << "\"\n";
            }
            if (values[1].has_value()) {
                lastName = std::any_cast<std::string>(values[1]);
                std::cout << "  [Converter]   LastName: \"" << lastName << "\"\n";
            }
        } catch (const std::exception& e) {
            std::cout << "  [Converter]   Error: " << e.what() << "\n";
            return std::string("[Error]");
        }

        std::string result = firstName + " " + lastName;
        std::cout << "  [Converter]   Result: \"" << result << "\"\n";
        return result;
    }

    std::vector<std::any> ConvertBack(
        const std::any& value,
        const std::vector<std::type_index>& sourceTypes,
        const std::any* parameter) const override {
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
            std::cout << "[Model] FirstName changed: \"" << firstName_ 
                      << "\" -> \"" << value << "\"\n";
            firstName_ = std::move(value);
            propertyChanged_("FirstName");
        }
    }

    const std::string& GetLastName() const { return lastName_; }
    void SetLastName(std::string value) {
        if (lastName_ != value) {
            std::cout << "[Model] LastName changed: \"" << lastName_ 
                      << "\" -> \"" << value << "\"\n";
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
                [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any& newVal) {
                    if (const auto* text = std::any_cast<std::string>(&newVal)) {
                        std::cout << "[Label] Text updated: \"" << *text << "\"\n";
                    }
                },
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

void TestAutoUpdate() {
    std::cout << "\n========== Test: Auto Update on Source Change ==========\n";

    auto person = std::make_shared<PersonModel>("John", "Doe");
    std::shared_ptr<INotifyPropertyChanged> source = person;

    auto label = std::make_shared<MockLabel>();

    MultiBinding multi;
    multi.AddBinding(Binding().Path("FirstName").Source(source));
    multi.AddBinding(Binding().Path("LastName").Source(source));
    multi.Converter(std::make_shared<FullNameConverter>());

    std::cout << "\n[Test] Setting up binding...\n";
    label->DependencyObject::SetBinding(MockLabel::TextProperty(), multi);

    std::cout << "\n[Test] Initial value: \"" << label->GetText() << "\"\n";

    std::cout << "\n[Test] Changing FirstName to 'Jane'...\n";
    person->SetFirstName("Jane");
    std::cout << "[Test] Label.Text after FirstName change: \"" << label->GetText() << "\"\n";

    std::cout << "\n[Test] Changing LastName to 'Smith'...\n";
    person->SetLastName("Smith");
    std::cout << "[Test] Label.Text after LastName change: \"" << label->GetText() << "\"\n";

    std::cout << "\n[Test] Changing both properties...\n";
    person->SetFirstName("Alice");
    person->SetLastName("Johnson");
    std::cout << "[Test] Final Label.Text: \"" << label->GetText() << "\"\n";
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  MultiBinding Auto-Update Demonstration\n";
    std::cout << "=================================================\n";

    RegisterAccessors();
    TestAutoUpdate();

    std::cout << "\n=================================================\n";
    std::cout << "  Test Complete\n";
    std::cout << "=================================================\n";

    return 0;
}
