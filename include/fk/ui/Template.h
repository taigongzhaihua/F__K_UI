#pragma once

#include <memory>
#include <functional>

namespace fk::ui {

class UIElement;
class FrameworkElement;

/**
 * @brief 模板基类
 * 
 * 定义如何创建可视化树的蓝图
 */
class Template {
public:
    virtual ~Template() = default;

    /**
     * @brief 从模板创建可视化树
     * @return 创建的根元素
     */
    virtual std::shared_ptr<FrameworkElement> LoadContent() = 0;
};

/**
 * @brief 控件模板
 * 
 * 定义控件的视觉结构，允许完全自定义控件外观而不改变其行为
 * 
 * 示例：
 * @code
 * auto buttonTemplate = std::make_shared<ControlTemplate>();
 * buttonTemplate->SetVisualTreeFactory([]() {
 *     return border()
 *         .Background("#007ACC")
 *         .CornerRadius(4.0f)
 *         .Child(
 *             textBlock()
 *                 .Text("Click Me")
 *                 .HorizontalAlignment(HorizontalAlignment::Center)
 *                 .VerticalAlignment(VerticalAlignment::Center)
 *         );
 * });
 * 
 * myButton->SetTemplate(buttonTemplate);
 * @endcode
 */
class ControlTemplate : public Template {
public:
    ControlTemplate() = default;
    ~ControlTemplate() override = default;

    /**
     * @brief 设置创建可视化树的工厂函数
     * @param factory 返回模板根元素的函数
     */
    void SetVisualTreeFactory(std::function<std::shared_ptr<FrameworkElement>()> factory) {
        factory_ = std::move(factory);
    }

    /**
     * @brief 从模板创建可视化树
     * @return 创建的根元素
     */
    std::shared_ptr<FrameworkElement> LoadContent() override {
        if (factory_) {
            return factory_();
        }
        return nullptr;
    }

    /**
     * @brief 获取目标类型（用于类型检查和样式匹配）
     */
    const std::type_info* GetTargetType() const { return targetType_; }
    
    /**
     * @brief 设置目标类型
     */
    void SetTargetType(const std::type_info* type) { targetType_ = type; }

private:
    std::function<std::shared_ptr<FrameworkElement>()> factory_;
    const std::type_info* targetType_{nullptr};
};

/**
 * @brief 数据模板
 * 
 * 定义如何将数据对象可视化，用于 ItemsControl 等
 */
class DataTemplate : public Template {
public:
    DataTemplate() = default;
    ~DataTemplate() override = default;

    void SetVisualTreeFactory(std::function<std::shared_ptr<FrameworkElement>()> factory) {
        factory_ = std::move(factory);
    }

    std::shared_ptr<FrameworkElement> LoadContent() override {
        if (factory_) {
            return factory_();
        }
        return nullptr;
    }

private:
    std::function<std::shared_ptr<FrameworkElement>()> factory_;
};

} // namespace fk::ui
