/**
 * @file ControlFactory.h
 * @brief 控件工厂函数 - 提供便捷的控件创建函数
 * 
 * 提供与控件类同名的工厂函数来创建控件指针，简化控件创建语法。
 * 使用单独的命名空间避免与类名冲突。
 * 
 * 示例用法：
 * ```cpp
 * using namespace fk::factory;
 * 
 * // 使用工厂函数创建控件
 * auto window = Window();
 * auto button = Button();
 * 
 * // 使用链式 API
 * window->Title("My Window")
 *       ->Width(800)
 *       ->Height(600);
 * ```
 */

#pragma once

#include "fk/ui/Window.h"
#include "fk/ui/buttons/Button.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/layouts/Grid.h"
#include "fk/ui/text/TextBox.h"
#include "fk/ui/lists/ListBox.h"
// #include "fk/ui/scrolling/ScrollViewer.h"  // TODO: ScrollViewer 正在重构中，暂时禁用
#include "fk/ui/controls/ContentPresenter.h"
#include "fk/ui/controls/Image.h"

namespace fk::factory {

/**
 * @brief Window 工厂函数
 * @return 新创建的 Window 指针
 */
inline ui::Window* Window() {
    return new ui::Window();
}

/**
 * @brief Button 工厂函数
 * @return 新创建的 Button 指针
 */
inline ui::Button* Button() {
    return new ui::Button();
}

/**
 * @brief TextBlock 工厂函数
 * @return 新创建的 TextBlock 指针
 */
inline ui::TextBlock* TextBlock() {
    return new ui::TextBlock();
}

/**
 * @brief Border 工厂函数
 * @return 新创建的 Border 指针
 */
inline ui::Border* Border() {
    return new ui::Border();
}

/**
 * @brief StackPanel 工厂函数
 * @return 新创建的 StackPanel 指针
 */
inline ui::StackPanel* StackPanel() {
    return new ui::StackPanel();
}

/**
 * @brief Grid 工厂函数
 * @return 新创建的 Grid 指针
 */
inline ui::Grid* Grid() {
    return new ui::Grid();
}

/**
 * @brief TextBox 工厂函数
 * @return 新创建的 TextBox 指针
 */
inline ui::TextBox* TextBox() {
    return new ui::TextBox();
}

/**
 * @brief ListBox 工厂函数
 * @return 新创建的 ListBox 指针
 */
template<typename T = void>
inline ui::ListBox<T>* ListBox() {
    return new ui::ListBox<T>();
}

/**
 * @brief ScrollViewer 工厂函数
 * @return 新创建的 ScrollViewer 指针
 * 
 * TODO: ScrollViewer 正在重构中，采用类似 WPF 的架构设计
 * 暂时禁用此工厂方法，待重构完成后恢复
 */
// inline ui::ScrollViewer* ScrollViewer() {
//     return new ui::ScrollViewer();
// }

/**
 * @brief ContentPresenter 工厂函数
 * @return 新创建的 ContentPresenter 指针
 */
template<typename T = std::any>
inline ui::ContentPresenter<T>* ContentPresenter() {
    return new ui::ContentPresenter<T>();
}

/**
 * @brief Image 工厂函数
 * @return 新创建的 Image 指针
 */
inline ui::Image* Image() {
    return new ui::Image();
}

} // namespace fk::factory
