#include "fk/ui/ControlTemplate.h"
#include "fk/ui/UIElement.h"
#include <stdexcept>

namespace fk::ui {

void ControlTemplate::SetTargetType(const std::type_info& type) {
    CheckSealed();
    targetType_ = &type;
}

void ControlTemplate::SetFactory(FactoryFunc factory) {
    CheckSealed();
    factory_ = std::move(factory);
}

UIElement* ControlTemplate::Instantiate(UIElement* templatedParent) {
    if (!factory_) {
        return nullptr;
    }
    
    // 调用工厂函数创建视觉树根节点
    UIElement* root = factory_();
    
    // TODO: 设置 TemplatedParent 关联
    // TODO: 处理模板绑定
    
    return root;
}

} // namespace fk::ui
