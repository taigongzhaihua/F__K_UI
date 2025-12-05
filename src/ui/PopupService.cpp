/**
 * @file PopupService.cpp
 * @brief PopupService 实现
 */

#include "fk/ui/PopupService.h"
#include "fk/ui/controls/Popup.h"
#include <algorithm>

namespace fk::ui {

PopupService& PopupService::Instance() {
    static PopupService instance;
    return instance;
}

void PopupService::RegisterPopup(Popup* popup) {
    if (!popup) {
        return;
    }

    // 避免重复注册
    if (IsRegistered(popup)) {
        return;
    }

    activePopups_.push_back(popup);
}

void PopupService::UnregisterPopup(Popup* popup) {
    if (!popup) {
        return;
    }

    auto it = std::find(activePopups_.begin(), activePopups_.end(), popup);
    if (it != activePopups_.end()) {
        activePopups_.erase(it);
    }
}

bool PopupService::IsRegistered(const Popup* popup) const {
    return std::find(activePopups_.begin(), activePopups_.end(), popup) 
           != activePopups_.end();
}

void PopupService::Update() {
    // 当前版本：简单遍历所有 Popup
    // 未来可以添加：
    // - 外部点击检测（StaysOpen=false 时关闭 Popup）
    // - PlacementTarget 位置变化检测并更新 Popup 位置
    // - 窗口失焦处理
    
    // 注意：在遍历时可能会有 Popup 被关闭并从列表中移除
    // 因此需要小心处理迭代器失效问题
    for (size_t i = 0; i < activePopups_.size(); ) {
        Popup* popup = activePopups_[i];
        
        if (popup) {
            popup->UpdatePopup();
        }
        
        ++i;
    }
}

void PopupService::RenderAll() {
    // 遍历所有活跃 Popup，触发其 PopupRoot 渲染
    // 注意：PopupRoot 有独立的 OpenGL 上下文和 GLFW 窗口
    // 不需要在主窗口上下文中渲染
    
    for (Popup* popup : activePopups_) {
        if (popup) {
            popup->RenderPopup();
        }
    }
}

void PopupService::CloseAll() {
    // 创建副本以避免在迭代中修改列表
    std::vector<Popup*> popupsCopy = activePopups_;
    
    for (Popup* popup : popupsCopy) {
        if (popup) {
            popup->SetIsOpen(false);
        }
    }
    
    // 清空列表（如果 Close() 正确调用了 UnregisterPopup，这里应该已经为空）
    activePopups_.clear();
}

} // namespace fk::ui
