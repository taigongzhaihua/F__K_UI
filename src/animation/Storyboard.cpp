#include "fk/animation/Storyboard.h"
#include "fk/animation/ColorAnimation.h"
#include "fk/animation/DoubleAnimation.h"
#include "fk/ui/UIElement.h"
#include "fk/ui/Control.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Brush.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace fk::animation {

// 静态成员初始化
std::unordered_map<Timeline*, binding::DependencyObject*> Storyboard::targetMap_;
std::unordered_map<Timeline*, std::string> Storyboard::targetPropertyMap_;
std::unordered_map<Timeline*, std::string> Storyboard::targetNameMap_;
std::unordered_map<Storyboard*, binding::DependencyObject*> Storyboard::templateRootMap_;

Storyboard::Storyboard() {
}

void Storyboard::AddChild(std::shared_ptr<Timeline> child) {
    if (!child) return;
    
    // 检查是否已存在
    auto it = std::find_if(children_.begin(), children_.end(),
        [&child](const std::shared_ptr<Timeline>& t) {
            return t.get() == child.get();
        });
    
    if (it == children_.end()) {
        children_.push_back(child);
    }
}

void Storyboard::RemoveChild(Timeline* child) {
    if (!child) return;
    
    children_.erase(
        std::remove_if(children_.begin(), children_.end(),
            [child](const std::shared_ptr<Timeline>& t) {
                return t.get() == child;
            }),
        children_.end()
    );
}

void Storyboard::ClearChildren() {
    children_.clear();
}

void Storyboard::Begin() {
    Timeline::Begin();
    
    // 在启动子动画前，先解析所有使用TargetName的动画
    auto templateRoot = GetTemplateRoot(this);
    if (templateRoot) {
        for (auto& child : children_) {
            if (!child) continue;
            
            // 检查是否使用了TargetName
            std::string targetName = GetTargetName(child.get());
            std::string propertyPath = GetTargetProperty(child.get());
            
            if (!targetName.empty() && !propertyPath.empty()) {
                // 尝试将 templateRoot 转换为 UIElement
                ui::UIElement* rootElement = dynamic_cast<ui::UIElement*>(templateRoot);
                if (rootElement) {
                    // 在模板树中查找命名元素
                    ui::UIElement* targetElement = rootElement->FindName(targetName);
                    if (targetElement) {
                        // 解析属性路径（例如 "BorderBrush.Color"）
                        ResolvePropertyPath(child.get(), targetElement, propertyPath);
                    }
                }
            }
        }
    }
    
    // 启动所有子动画
    for (auto& child : children_) {
        if (child) {
            child->Begin();
        }
    }
}

void Storyboard::Stop() {
    Timeline::Stop();
    
    // 停止所有子动画
    for (auto& child : children_) {
        if (child) {
            child->Stop();
        }
    }
}

void Storyboard::Pause() {
    Timeline::Pause();
    
    // 暂停所有子动画
    for (auto& child : children_) {
        if (child && child->IsActive()) {
            child->Pause();
        }
    }
}

void Storyboard::Resume() {
    Timeline::Resume();
    
    // 恢复所有子动画
    for (auto& child : children_) {
        if (child && child->IsPaused()) {
            child->Resume();
        }
    }
}

void Storyboard::Seek(std::chrono::milliseconds offset) {
    Timeline::Seek(offset);
    
    // 移动所有子动画到指定时间
    for (auto& child : children_) {
        if (child) {
            child->Seek(offset);
        }
    }
}

Duration Storyboard::GetNaturalDuration() const {
    // 返回所有子动画中最长的持续时间
    std::chrono::milliseconds maxDuration(0);
    
    for (const auto& child : children_) {
        if (child) {
            Duration childDuration = child->GetDuration();
            if (childDuration.HasTimeSpan()) {
                auto childEnd = child->GetBeginTime() + childDuration.timeSpan;
                if (childEnd > maxDuration) {
                    maxDuration = childEnd;
                }
            }
        }
    }
    
    return Duration(maxDuration);
}

void Storyboard::UpdateCurrentValue(double progress) {
    // Storyboard 本身不更新值，由子动画负责
    // 这里可以添加整体进度的监控逻辑
}

// 附加属性实现
void Storyboard::SetTarget(Timeline* timeline, binding::DependencyObject* target) {
    if (timeline) {
        targetMap_[timeline] = target;
    }
}

void Storyboard::SetTarget(Timeline* timeline, binding::DependencyObject* target, const binding::DependencyProperty* property) {
    if (!timeline || !target || !property) {
        return;
    }
    
    // 设置目标对象
    targetMap_[timeline] = target;
    
    // 尝试将 timeline 转换为具体的动画类型并设置目标
    if (auto* colorAnim = dynamic_cast<ColorAnimation*>(timeline)) {
        colorAnim->SetTarget(target, property);
    } else if (auto* doubleAnim = dynamic_cast<DoubleAnimation*>(timeline)) {
        doubleAnim->SetTarget(target, property);
    }
    // 可以继续添加其他动画类型
}

binding::DependencyObject* Storyboard::GetTarget(Timeline* timeline) {
    if (timeline && targetMap_.count(timeline)) {
        return targetMap_[timeline];
    }
    return nullptr;
}

void Storyboard::SetTargetProperty(Timeline* timeline, const std::string& propertyPath) {
    if (timeline) {
        targetPropertyMap_[timeline] = propertyPath;
    }
}

std::string Storyboard::GetTargetProperty(Timeline* timeline) {
    if (timeline && targetPropertyMap_.count(timeline)) {
        return targetPropertyMap_[timeline];
    }
    return "";
}

void Storyboard::SetTargetName(Timeline* timeline, const std::string& targetName) {
    if (timeline) {
        targetNameMap_[timeline] = targetName;
    }
}

std::string Storyboard::GetTargetName(Timeline* timeline) {
    if (timeline && targetNameMap_.count(timeline)) {
        return targetNameMap_[timeline];
    }
    return "";
}

void Storyboard::SetTemplateRoot(Storyboard* storyboard, binding::DependencyObject* root) {
    if (storyboard) {
        templateRootMap_[storyboard] = root;
    }
}

binding::DependencyObject* Storyboard::GetTemplateRoot(Storyboard* storyboard) {
    if (storyboard && templateRootMap_.count(storyboard)) {
        return templateRootMap_[storyboard];
    }
    return nullptr;
}



void Storyboard::ResolvePropertyPath(Timeline* timeline, ui::UIElement* targetElement, const std::string& propertyPath) {
    if (!timeline || !targetElement || propertyPath.empty()) {
        return;
    }
    
    // 检查是否是嵌套属性路径（例如 "BorderBrush.Color"）
    size_t dotPos = propertyPath.find('.');
    if (dotPos == std::string::npos) {
        // 简单属性路径，直接在目标元素上查找
        auto prop = targetElement->FindProperty(propertyPath);
        if (prop) {
            SetTarget(timeline, targetElement, prop);
        }
        return;
    }
    
    // 嵌套属性路径：分割为对象名和属性名
    std::string objectPropertyName = propertyPath.substr(0, dotPos);
    std::string subPropertyName = propertyPath.substr(dotPos + 1);
    
    // 获取中间对象的 DependencyProperty
    auto objectProperty = targetElement->FindProperty(objectPropertyName);
    if (!objectProperty) {
        return;
    }
    
    try {
        // 获取属性值（例如获取 BorderBrush）
        auto objectValue = targetElement->GetValue(*objectProperty);
        if (!objectValue.has_value()) {
            return;
        }
        
        // 尝试获取 DependencyObject 指针（Brush 继承自 DependencyObject）
        binding::DependencyObject* subObject = nullptr;
        if (objectValue.type() == typeid(ui::Brush*)) {
            subObject = std::any_cast<ui::Brush*>(objectValue);
        }
        
        if (subObject) {
            // 在子对象上查找子属性（例如 "Color"）
            auto subProperty = subObject->FindProperty(subPropertyName);
            if (subProperty) {
                SetTarget(timeline, subObject, subProperty);
            }
        }
    } catch (...) {
        // 属性访问失败
    }
}

} // namespace fk::animation
