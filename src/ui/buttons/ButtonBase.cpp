// ButtonBase.cpp - 显式模板实例化
// 模板实现已在 ButtonBase.inl 中，由 ButtonBase.h 包含

#include "fk/ui/buttons/ButtonBase.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/buttons/ToggleButton.h"
#include "fk/ui/buttons/RepeatButton.h"

// 显式实例化 ButtonBase 模板
template class fk::ui::ButtonBase<fk::ui::Button>;
template class fk::ui::ButtonBase<fk::ui::ToggleButton>;
template class fk::ui::ButtonBase<fk::ui::RepeatButton>;
