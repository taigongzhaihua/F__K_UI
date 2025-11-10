#include "fk/ui/FrameworkTemplate.h"
#include <stdexcept>

namespace fk::ui {

void FrameworkTemplate::Seal() {
    if (!isSealed_) {
        isSealed_ = true;
    }
}

void FrameworkTemplate::CheckSealed() const {
    if (isSealed_) {
        throw std::runtime_error("Cannot modify a sealed template");
    }
}

} // namespace fk::ui
