#pragma once

#include "fk/binding/DependencyProperty.h"
#include <any>
#include <memory>

namespace fk {
namespace binding {
    class DependencyObject;
}

namespace ui {

/**
 * @brief Setter - 属性设置器
 * 
 * 用于在 Style 中设置控件属性值
 * 
 * WPF 对应：Setter
 */
class Setter {
public:
    Setter() = default;
    
    /**
     * @brief 构造函数
     * @param property 要设置的依赖属性
     * @param value 要设置的值
     */
    Setter(const binding::DependencyProperty& property, const std::any& value)
        : property_(&property), value_(value) {}
    
    /**
     * @brief 设置属性
     */
    void SetProperty(const binding::DependencyProperty& property) {
        property_ = &property;
    }
    
    /**
     * @brief 获取属性
     */
    const binding::DependencyProperty* GetProperty() const {
        return property_;
    }
    
    /**
     * @brief 设置值
     */
    void SetValue(const std::any& value) {
        value_ = value;
    }
    
    /**
     * @brief 获取值
     */
    const std::any& GetValue() const {
        return value_;
    }
    
    /**
     * @brief 应用 setter 到目标对象
     */
    void Apply(binding::DependencyObject* target) const;
    
    /**
     * @brief 撤销 setter（恢复默认值）
     */
    void Unapply(binding::DependencyObject* target) const;

private:
    const binding::DependencyProperty* property_{nullptr};
    std::any value_;
};

/**
 * @brief SetterCollection - Setter 集合
 */
class SetterCollection {
public:
    SetterCollection() = default;
    
    /**
     * @brief 添加 setter
     */
    void Add(const Setter& setter) {
        setters_.push_back(setter);
    }
    
    /**
     * @brief 添加 setter（通过属性和值）
     */
    void Add(const binding::DependencyProperty& property, const std::any& value) {
        setters_.emplace_back(property, value);
    }
    
    /**
     * @brief 获取 setter 数量
     */
    size_t Count() const {
        return setters_.size();
    }
    
    /**
     * @brief 获取指定索引的 setter
     */
    const Setter& operator[](size_t index) const {
        return setters_[index];
    }
    
    /**
     * @brief 清空集合
     */
    void Clear() {
        setters_.clear();
    }
    
    /**
     * @brief 应用所有 setters
     */
    void Apply(binding::DependencyObject* target) const {
        for (const auto& setter : setters_) {
            setter.Apply(target);
        }
    }
    
    /**
     * @brief 撤销所有 setters
     */
    void Unapply(binding::DependencyObject* target) const {
        for (const auto& setter : setters_) {
            setter.Unapply(target);
        }
    }
    
    // 迭代器支持
    auto begin() { return setters_.begin(); }
    auto end() { return setters_.end(); }
    auto begin() const { return setters_.begin(); }
    auto end() const { return setters_.end(); }

private:
    std::vector<Setter> setters_;
};

} // namespace ui
} // namespace fk
