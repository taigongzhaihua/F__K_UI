#include "fk/ui/ThreadingConfig.h"

namespace fk::ui {

ThreadingConfig& ThreadingConfig::Instance() {
    static ThreadingConfig instance;
    return instance;
}

} // namespace fk::ui
