/**
 * @file async_binding_demo.cpp
 * @brief Demonstrates async binding functionality
 * 
 * This example shows:
 * 1. Normal synchronous binding
 * 2. Async binding with IsAsync flag
 * 3. Performance comparison
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
#include <chrono>

using namespace fk::binding;

// ===== Model class =====
class DataModel : public INotifyPropertyChanged {
public:
    DataModel(std::string value) : value_(std::move(value)) {}

    const std::string& GetValue() const { return value_; }
    void SetValue(std::string value) {
        if (value_ != value) {
            value_ = std::move(value);
            propertyChanged_("Value");
        }
    }

    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

private:
    std::string value_;
    PropertyChangedEvent propertyChanged_;
};

// ===== Mock UI Control =====
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

    void SetText(const std::string& value) {
        SetValue(TextProperty(), value);
    }
};

// ===== Property Accessor Registration =====
void RegisterAccessors() {
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<DataModel>),
        "Value",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<DataModel>>(&obj)) {
                    result = (*ptr)->GetValue();
                    return true;
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<DataModel>>(&obj)) {
                    if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                        (*ptr)->SetValue(*strPtr);
                        return true;
                    }
                }
                return false;
            }
        }
    );

    // IMPORTANT: Also register for INotifyPropertyChanged base class
    PropertyAccessorRegistry::RegisterAccessor(
        typeid(std::shared_ptr<INotifyPropertyChanged>),
        "Value",
        PropertyAccessorRegistry::Accessor{
            [](const std::any& obj, std::any& result) -> bool {
                if (const auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto model = std::dynamic_pointer_cast<DataModel>(*ptr)) {
                        result = model->GetValue();
                        return true;
                    }
                }
                return false;
            },
            [](std::any& obj, const std::any& value) -> bool {
                if (auto* ptr = std::any_cast<std::shared_ptr<INotifyPropertyChanged>>(&obj)) {
                    if (auto model = std::dynamic_pointer_cast<DataModel>(*ptr)) {
                        if (const auto* strPtr = std::any_cast<std::string>(&value)) {
                            model->SetValue(*strPtr);
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

void TestSyncBinding() {
    std::cout << "\n========== Test 1: Synchronous Binding ==========\n";
    
    auto model = std::make_shared<DataModel>("Initial Value");
    auto label = std::make_shared<MockLabel>();

    // Create synchronous binding
    // IMPORTANT: Cast to base class pointer for INotifyPropertyChanged support
    std::shared_ptr<INotifyPropertyChanged> source = model;
    
    Binding binding;
    binding.Path("Value")
           .Source(source)
           .Mode(BindingMode::OneWay)
           .IsAsync(false);  // Explicitly synchronous

    label->SetBinding(MockLabel::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Model.Value: \"" << model->GetValue() << "\"\n";
    std::cout << "  Label.Text:  \"" << label->GetText() << "\"\n";

    std::cout << "\n[Action] Update model 100 times\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        model->SetValue("Update #" + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Result:\n";
    std::cout << "  Model.Value: \"" << model->GetValue() << "\"\n";
    std::cout << "  Label.Text:  \"" << label->GetText() << "\"\n";
    std::cout << "  Time taken:  " << duration.count() << " μs\n";
}

void TestAsyncBinding() {
    std::cout << "\n========== Test 2: Asynchronous Binding ==========\n";
    
    auto model = std::make_shared<DataModel>("Initial Value");
    auto label = std::make_shared<MockLabel>();

    // Create asynchronous binding
    // IMPORTANT: Cast to base class pointer for INotifyPropertyChanged support
    std::shared_ptr<INotifyPropertyChanged> source = model;
    
    Binding binding;
    binding.Path("Value")
           .Source(source)
           .Mode(BindingMode::OneWay)
           .IsAsync(true);  // Enable async updates

    label->SetBinding(MockLabel::TextProperty(), binding);

    std::cout << "Initial state:\n";
    std::cout << "  Model.Value: \"" << model->GetValue() << "\"\n";
    std::cout << "  Label.Text:  \"" << label->GetText() << "\"\n";

    std::cout << "\n[Action] Update model 100 times (async)\n";
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        model->SetValue("Async Update #" + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Result:\n";
    std::cout << "  Model.Value: \"" << model->GetValue() << "\"\n";
    std::cout << "  Label.Text:  \"" << label->GetText() << "\"\n";
    std::cout << "  Time taken:  " << duration.count() << " μs\n";
    std::cout << "\n  Note: Async binding batches updates and processes them later,\n";
    std::cout << "        reducing UI update overhead in rapid-fire scenarios.\n";
}

void TestAsyncBehavior() {
    std::cout << "\n========== Test 3: Async Update Batching ==========\n";
    
    auto model = std::make_shared<DataModel>("Initial");
    auto label = std::make_shared<MockLabel>();

    // Cast to base class for INotifyPropertyChanged support
    std::shared_ptr<INotifyPropertyChanged> source = model;
    
    Binding binding;
    binding.Path("Value")
           .Source(source)
           .IsAsync(true);

    label->SetBinding(MockLabel::TextProperty(), binding);

    std::cout << "Initial: Model=\"" << model->GetValue() << "\", Label=\"" << label->GetText() << "\"\n";

    std::cout << "\n[Action] Rapid updates:\n";
    model->SetValue("First");
    std::cout << "  After 1st update: Label=\"" << label->GetText() << "\"\n";
    
    model->SetValue("Second");
    std::cout << "  After 2nd update: Label=\"" << label->GetText() << "\"\n";
    
    model->SetValue("Third");
    std::cout << "  After 3rd update: Label=\"" << label->GetText() << "\"\n";

    std::cout << "\n  Note: In a real async implementation with a dispatcher,\n";
    std::cout << "        only the final value would be applied after batching.\n";
    std::cout << "        Current implementation executes immediately for simplicity.\n";
}

// ===== Main =====
int main() {
    std::cout << "=================================================\n";
    std::cout << "  Async Binding Demonstration\n";
    std::cout << "=================================================\n";

    RegisterAccessors();

    try {
        TestSyncBinding();
        TestAsyncBinding();
        TestAsyncBehavior();

        std::cout << "\n=================================================\n";
        std::cout << "  All tests completed!\n";
        std::cout << "=================================================\n";
        std::cout << "\n  Implementation Note:\n";
        std::cout << "  - IsAsync flag added to Binding class\n";
        std::cout << "  - UpdateTargetAsync/UpdateSourceAsync methods added\n";
        std::cout << "  - hasPendingTargetUpdate/hasPendingSourceUpdate flags for batching\n";
        std::cout << "  - In production, integrate with UI framework's Dispatcher\n";
        std::cout << "=================================================\n";

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
