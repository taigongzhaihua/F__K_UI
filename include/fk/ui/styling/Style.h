#pragma once

#include "fk/ui/styling/Setter.h"
#include <string>
#include <typeindex>
#include <memory>

namespace fk::ui {

/**
 * @brief Style - 样式类
 * 
 * 定义控件的属性集合，支持样式继承
 * 
 * WPF 对应：Style
 */
class Style {
public:
    Style() = default;
    
    /**
     * @brief 构造函数
     * @param targetType 目标类型
     */
    explicit Style(std::type_index targetType)
        : targetType_(targetType) {}
    
    /**
     * @brief 设置目标类型
     */
    void SetTargetType(std::type_index targetType) {
        targetType_ = targetType;
    }
    
    /**
     * @brief 获取目标类型
     */
    std::type_index GetTargetType() const {
        return targetType_;
    }
    
    /**
     * @brief 设置基样式（继承）
     */
    void SetBasedOn(Style* baseStyle) {
        basedOn_ = baseStyle;
    }
    
    /**
     * @brief 获取基样式
     */
    Style* GetBasedOn() const {
        return basedOn_;
    }
    
    /**
     * @brief 获取 Setters 集合
     */
    SetterCollection& Setters() {
        return setters_;
    }
    
    /**
     * @brief 获取 Setters 集合（const）
     */
    const SetterCollection& Setters() const {
        return setters_;
    }
    
    /**
     * @brief 检查样式是否适用于指定类型
     */
    bool IsApplicableTo(std::type_index type) const {
        return targetType_ == type;
    }
    
    /**
     * @brief 应用样式到目标对象
     */
    void Apply(binding::DependencyObject* target) const {
        if (!target) {
            return;
        }
        
        // 先应用基样式
        if (basedOn_) {
            basedOn_->Apply(target);
        }
        
        // 再应用当前样式的 setters
        setters_.Apply(target);
    }
    
    /**
     * @brief 撤销样式
     */
    void Unapply(binding::DependencyObject* target) const {
        if (!target) {
            return;
        }
        
        // 撤销当前样式
        setters_.Unapply(target);
        
        // 撤销基样式
        if (basedOn_) {
            basedOn_->Unapply(target);
        }
    }
    
    /**
     * @brief 密封样式（阻止修改）
     */
    void Seal() {
        isSealed_ = true;
    }
    
    /**
     * @brief 检查是否已密封
     */
    bool IsSealed() const {
        return isSealed_;
    }

private:
    std::type_index targetType_{typeid(void)};  // 目标类型
    Style* basedOn_{nullptr};                   // 基样式
    SetterCollection setters_;                  // Setter 集合
    bool isSealed_{false};                      // 是否已密封
};

} // namespace fk::ui
