#pragma once

#include "fk/binding/INotifyPropertyChanged.h"

#include <string>

namespace fk::binding {

// Forward declaration for friend access
template<typename T, typename Owner>
class ObservableProperty;

class ObservableObject : public INotifyPropertyChanged {
public:
    ObservableObject() = default;
    ~ObservableObject() override = default;

    PropertyChangedEvent& PropertyChanged() override { return propertyChanged_; }

protected:
    void RaisePropertyChanged(std::string_view propertyName) {
        propertyChanged_(propertyName);
    }

    void RaisePropertyChanged(const std::string& propertyName) {
        propertyChanged_(propertyName);
    }
    
    // Allow ObservableProperty to call RaisePropertyChanged
    template<typename T, typename Owner>
    friend class ObservableProperty;

private:
    PropertyChangedEvent propertyChanged_;
};

} // namespace fk::binding

namespace fk {
    using binding::ObservableObject;
}
