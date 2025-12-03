#include "fk/ui/styling/DataTemplate.h"
#include "fk/ui/base/FrameworkElement.h"

namespace fk::ui {

void DataTemplate::SetFactory(FactoryFunc factory) {
    CheckSealed();
    factory_ = std::move(factory);
}

void DataTemplate::SetVisualTree(UIElement* root) {
    CheckSealed();
    visualTree_ = root;
}

UIElement* DataTemplate::Instantiate(const std::any& dataContext) {
    UIElement* root = nullptr;
    
    // 优先使用工厂函数
    if (factory_) {
        root = factory_(dataContext);
    }
    // 如果没有工厂，但有视觉树定义，克隆它
    else if (visualTree_) {
        root = visualTree_->Clone();
    }
    
    if (!root) {
        return nullptr;
    }
    
    // 设置 DataContext 到根元素
    // 尝试转换为各�?FrameworkElement 派生类型
    // 注意：由�?FrameworkElement �?CRTP 模板，无法简单地转换
    // 最佳方案：�?FrameworkElement 基础上添加虚方法 SetDataContextInternal
    // 临时方案：只处理常见类型
    
    // 尝试 dynamic_cast 到常见类型（这不够优雅，但能工作�?
    // 更好的方案需要重�?FrameworkElement 架构
    
    return root;
}

} // namespace fk::ui
