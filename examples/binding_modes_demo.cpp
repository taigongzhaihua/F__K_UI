/**
 * @file binding_modes_demo.cpp
 * @brief Demonstrates all BindingMode and UpdateSourceTrigger options
 * 
 * This example shows:
 * 1. TwoWay binding with PropertyChanged trigger
 * 2. TwoWay binding with LostFocus trigger
 * 3. TwoWay binding with Explicit trigger
 * 4. OneWayToSource binding
 */

#include "fk/binding/Binding.h"
#include "fk/binding/BindingExpression.h"
#include "fk/binding/DependencyObject.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/INotifyPropertyChanged.h"
#include "fk/binding/BindingPath.h"
#include <iostream>
#include <string>
#include <memory>

using namespace fk::binding;

// ===== Model class with INotifyPropertyChanged =====
class Person : public INotifyPropertyChanged {
public:
    Person(std::string name, int age) : name_(std::move(name)), age_(age) {}

    const std::string& GetName() const { return name_; }
    void SetName(std::string value) {
        if (name_ != value) {
            name_ = std::move(value);
            NotifyPropertyChanged("Name");
        }
    }

    int GetAge() const { return age_; }
    void SetAge(int value) {
        if (age_ != value) {
            age_ = value;
            NotifyPropertyChanged("Age");
        }
    }

    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

private:
    void NotifyPropertyChanged(const std::string& propertyName) {
        propertyChanged_(propertyName);  // Event uses operator(), not Invoke
    }

    std::string name_;
    int age_;
    PropertyChangedEvent propertyChanged_;
};

// ===== Mock UI Control for testing =====
class MockTextBox : public DependencyObject {
public:
    static const DependencyProperty& TextProperty() {
        static const DependencyProperty& prop = DependencyProperty::Register(
            "Text",
            typeid(std::string),
            typeid(MockTextBox),
            PropertyMetadata{
                std::string(""),  // defaultValue
                nullptr,          // propertyChangedCallback
                nullptr,          // validateCallback
                BindingOptions{   // bindingOptions
                    BindingMode::TwoWay,
                    UpdateSourceTrigger::PropertyChanged,
                    false
                }
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

    // Simulate user input
    void SimulateUserInput(const std::string& text) {
        std::cout << "  [User Input] Typing: \"" << text << "\"\n";
        SetValue(TextProperty(), text);
    }

    // Simulate focus loss
    void SimulateLostFocus() {
        std::cout << "  [Focus Event] Lost focus\n";
        // Trigger UpdateSource for LostFocus bindings
        auto binding = GetBinding(TextProperty());
        if (binding && binding->IsActive() && 
            binding->GetEffectiveUpdateSourceTrigger() == UpdateSourceTrigger::LostFocus) {
            binding->UpdateSource();
        }
    }
};

// ===== Property Accessor Registration =====

void RegisterPersonAccessors() {
    using namespace fk::binding;
    
    // Register Name property
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<Person>),
        "Name",
        PropertyAccessorRegistry::Accessor{
            // Getter
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<Person>>(&obj)) {
                    result = (*ptr)->GetName();
                    return true;
                }
                return false;
            },
            // Setter
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<Person>>(&obj)) {
                    if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                        (*ptr)->SetName(*strPtr);
                        return true;
                    }
                }
                return false;
            }
        }
    );

    // Register Age property
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<Person>),
        "Age",
        PropertyAccessorRegistry::Accessor{
            // Getter
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<Person>>(&obj)) {
                    result = (*ptr)->GetAge();
                    return true;
                }
                return false;
            },
            // Setter
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<Person>>(&obj)) {
                    if (const auto* intPtr = std::any_cast<int>(&value)) {
                        (*ptr)->SetAge(*intPtr);
                        return true;
                    }
                }
                return false;
            }
        }
    );

    // IMPORTANT: Also register for INotifyPropertyChanged base class pointer
    // This allows TryGetNotifier to find the notifier interface
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "Name",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    // Try to downcast to Person
                    if (auto person = std::dynamic_pointer_cast<Person>(*ptr)) {
                        result = person->GetName();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<Person>(*ptr)) {
                        if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                            person->SetName(*strPtr);
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
        "Age",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<Person>(*ptr)) {
                        result = person->GetAge();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto person = std::dynamic_pointer_cast<Person>(*ptr)) {
                        if (const auto* intPtr = std::any_cast<int>(&value)) {
                            person->SetAge(*intPtr);
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    );
}

// ===== Test Functions =====

void TestTwoWayPropertyChanged() {
    std::cout << "\n========== Test 1: TwoWay + PropertyChanged ==========\n";
    
    auto person = std::make_shared<Person>("Alice", 30);
    auto textBox = std::make_shared<MockTextBox>();

    // Create binding: TextBox.Text <-> Person.Name (PropertyChanged)
    // Cast to base class for INotifyPropertyChanged support
    std::shared_ptr<INotifyPropertyChanged> source = person;
    
    Binding binding;
    binding.Path("Name")
           .Source(source)
           .Mode(BindingMode::TwoWay)
           .SetUpdateSourceTrigger(UpdateSourceTrigger::PropertyChanged);

    textBox->SetBinding(MockTextBox::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // Test source-to-target
    std::cout << "\n[Action] Change Person.Name to \"Bob\"\n";
    person->SetName("Bob");
    std::cout << "Result:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // Test target-to-source (PropertyChanged)
    std::cout << "\n[Action] User types \"Charlie\" in TextBox\n";
    textBox->SimulateUserInput("Charlie");
    std::cout << "Result (PropertyChanged trigger fires immediately):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";
}

void TestTwoWayLostFocus() {
    std::cout << "\n========== Test 2: TwoWay + LostFocus ==========\n";
    
    auto person = std::make_shared<Person>("Alice", 30);
    auto textBox = std::make_shared<MockTextBox>();

    // Create binding: TextBox.Text <-> Person.Name (LostFocus)
    std::shared_ptr<INotifyPropertyChanged> source = person;
    
    Binding binding;
    binding.Path("Name")
           .Source(source)
           .Mode(BindingMode::TwoWay)
           .SetUpdateSourceTrigger(UpdateSourceTrigger::LostFocus);

    textBox->SetBinding(MockTextBox::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // User types but hasn't lost focus yet
    std::cout << "\n[Action] User types \"David\" in TextBox\n";
    textBox->SimulateUserInput("David");
    std::cout << "Result (no update yet - waiting for focus loss):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (unchanged)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // User loses focus - now update should happen
    std::cout << "\n[Action] TextBox loses focus\n";
    textBox->SimulateLostFocus();
    std::cout << "Result (LostFocus trigger fires):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (updated!)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";
}

void TestTwoWayExplicit() {
    std::cout << "\n========== Test 3: TwoWay + Explicit ==========\n";
    
    auto person = std::make_shared<Person>("Alice", 30);
    auto textBox = std::make_shared<MockTextBox>();

    // Create binding: TextBox.Text <-> Person.Name (Explicit)
    std::shared_ptr<INotifyPropertyChanged> source = person;
    
    Binding binding;
    binding.Path("Name")
           .Source(source)
           .Mode(BindingMode::TwoWay)
           .SetUpdateSourceTrigger(UpdateSourceTrigger::Explicit);

    textBox->SetBinding(MockTextBox::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // User types but no automatic update
    std::cout << "\n[Action] User types \"Eve\" in TextBox\n";
    textBox->SimulateUserInput("Eve");
    std::cout << "Result (no automatic update):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (unchanged)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // User loses focus - still no update (Explicit mode)
    std::cout << "\n[Action] TextBox loses focus (no effect in Explicit mode)\n";
    textBox->SimulateLostFocus();
    std::cout << "Result:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (still unchanged)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // Manual trigger
    std::cout << "\n[Action] Manually call UpdateSource()\n";
    textBox->UpdateSource(MockTextBox::TextProperty());
    std::cout << "Result (Explicit trigger fires):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (updated!)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";
}

void TestOneWayToSource() {
    std::cout << "\n========== Test 4: OneWayToSource ==========\n";
    
    auto person = std::make_shared<Person>("Alice", 30);
    auto textBox = std::make_shared<MockTextBox>();

    // Set initial textbox value
    textBox->SetText("Initial TextBox Value");

    // Create binding: TextBox.Text -> Person.Name (target-to-source only)
    std::shared_ptr<INotifyPropertyChanged> source = person;
    
    Binding binding;
    binding.Path("Name")
           .Source(source)
           .Mode(BindingMode::OneWayToSource);

    textBox->SetBinding(MockTextBox::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";

    // Test source-to-target (should NOT update)
    std::cout << "\n[Action] Change Person.Name to \"Bob\"\n";
    person->SetName("Bob");
    std::cout << "Result (OneWayToSource - source changes don't flow to target):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\"\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\" (unchanged)\n";

    // Test target-to-source (should update)
    std::cout << "\n[Action] User types \"Frank\" in TextBox\n";
    textBox->SimulateUserInput("Frank");
    std::cout << "Result (target-to-source flows):\n";
    std::cout << "  Person.Name: \"" << person->GetName() << "\" (updated!)\n";
    std::cout << "  TextBox.Text: \"" << textBox->GetText() << "\"\n";
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  Binding Modes & UpdateSourceTrigger Demo\n";
    std::cout << "=================================================\n";

    // Register property accessors for Person class
    RegisterPersonAccessors();

    try {
        TestTwoWayPropertyChanged();
        TestTwoWayLostFocus();
        TestTwoWayExplicit();
        TestOneWayToSource();

        std::cout << "\n=================================================\n";
        std::cout << "  All tests completed successfully!\n";
        std::cout << "=================================================\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
