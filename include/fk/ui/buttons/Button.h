#pragma once

#include "fk/ui/buttons/ButtonBase.h"
#include "fk/render/DrawCommand.h"
#include <memory>

// 前向声明
namespace fk::animation {
    class VisualState;
}

namespace fk::ui {

/**
 * @brief 按钮控件
 * 
 * 职责：
 * - 响应点击事件
 * - 显示内容
 * 
 * WPF 对应：Button
 */
class Button : public ButtonBase<Button> {
public:
    Button();
    virtual ~Button() = default;
};

} // namespace fk::ui
