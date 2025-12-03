#pragma once

/**
 * @file FK_UI.h
 * @brief F__K UI Framework - 完整框架头文件
 * 
 * 包含此头文件即可使用 F__K UI 框架的所有功能。
 * 如需更精细的控制,可以只包含所需的模块头文件。
 * 
 * 完整用法示例:
 * @code
 * #include <fk/FK_UI.h>
 * 
 * int main() {
 *     fk::Application app;
 *     auto window = std::make_shared<fk::ui::Window>();
 *     window->Title("My App")->Width(800)->Height(600);
 *     return app.Run(window);
 * }
 * @endcode
 * 
 * 模块化用法示例:
 * @code
 * #include <fk/Application>  // 应用程序和窗口
 * #include <fk/Controls>     // UI 控件
 * #include <fk/Layouts>      // 布局容器
 * #include <fk/DataBinding>  // 数据绑定
 * #include <fk/Animation>    // 动画系统
 * @endcode
 */

// 基础模块
#include "fk/Core"
#include "fk/Application"

// UI 基础架构
#include "fk/ui/base/Visual.h"
#include "fk/ui/base/UIElement.h"
#include "fk/ui/base/FrameworkElement.h"

// UI 模块
#include "fk/Controls"
#include "fk/Layouts"
#include "fk/Graphics"
#include "fk/Styling"
#include "fk/Collections"
#include "fk/Input"

// 功能模块
#include "fk/DataBinding"
#include "fk/Animation"
#include "fk/Rendering"

// 性能优化
#include "fk/performance/ObjectPool.h"

/**
 * @namespace fk
 * @brief F__K UI Framework 根命名空间
 */

/**
 * @namespace fk::ui
 * @brief UI 元素和控件
 */

/**
 * @namespace fk::binding
 * @brief 数据绑定系统
 */

/**
 * @namespace fk::animation
 * @brief 动画系统
 */

/**
 * @namespace fk::resources
 * @brief 资源管理
 */

/**
 * @namespace fk::render
 * @brief 渲染系统
 */

/**
 * @namespace fk::core
 * @brief 核心工具和基础设施
 */
