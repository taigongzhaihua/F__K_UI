#include "fk/ui/DataTemplate.h"

namespace fk::ui {

void DataTemplate::SetFactory(FactoryFunc factory) {
    CheckSealed();
    factory_ = std::move(factory);
}

UIElement* DataTemplate::Instantiate(const std::any& dataContext) {
    if (!factory_) {
        return nullptr;
    }
    
    // 调用工厂函数创建视觉树，传入数据上下文
    UIElement* root = factory_(dataContext);
    
    // TODO: 设置 DataContext 到根元素
    
    return root;
}

} // namespace fk::ui
