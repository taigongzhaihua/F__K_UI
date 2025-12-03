#include "fk/ui/styling/Setter.h"
#include "fk/binding/DependencyObject.h"

namespace fk::ui {

void Setter::Apply(binding::DependencyObject* target) const {
    if (!target || !property_) {
        return;
    }
    
    // 设置属性�?
    target->SetValue(*property_, value_);
}

void Setter::Unapply(binding::DependencyObject* target) const {
    if (!target || !property_) {
        return;
    }
    
    // 恢复默认�?
    target->ClearValue(*property_);
}

} // namespace fk::ui
