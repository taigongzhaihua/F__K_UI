#pragma once

#include "fk/core/Event.h"

#include <string_view>

namespace fk::binding {

class INotifyPropertyChanged {
public:
    using PropertyChangedEvent = core::Event<std::string_view>;

    virtual ~INotifyPropertyChanged() = default;

    virtual PropertyChangedEvent& PropertyChanged() = 0;
};

} // namespace fk::binding

namespace fk {
    using binding::INotifyPropertyChanged;
}
