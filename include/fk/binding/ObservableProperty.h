#pragma once

#include "fk/binding/ObservableObject.h"
#include "fk/binding/BindingPath.h"

#include <functional>
#include <string>
#include <string_view>
#include <any>

namespace fk::binding {

/**
 * @brief 可观察属性模板类
 * 
 * 提供自动属性变更通知和注册功能，用于替代 ViewModel 宏系统。
 * 需要显式指定 Owner 类型参数，以确保类型安全。
 * 
 * @tparam T 属性值类型
 * @tparam Owner 属性所属的类类型（必须继承自 ObservableObject）
 * 
 * 特性：
 * - 自动注册到 PropertyAccessorRegistry，支持字符串路径绑定
 * - 支持自定义 getter、setter、validator
 * - 类型安全的属性访问
 * - 隐式转换支持
 * 
 * 优势（相比 ViewModel 宏）：
 * - 无需在类外使用注册宏
 * - 代码更清晰，更易调试
 * - 编译期类型检查
 * 
 * 使用示例：
 * @code
 * class UserData : public ObservableObject {
 * public:
 *     ObservableProperty<std::string, UserData> name{this, "Name"};
 *     ObservableProperty<int, UserData> age{this, "Age"};
 *     
 *     // 使用 Validator 验证输入
 *     ObservableProperty<int, UserData> score{this, "Score"}
 *         .Validator([](int val) { return val >= 0 && val <= 100; });
 * };
 * @endcode
 */
template<typename T, typename Owner>
class ObservableProperty {
public:
    /**
     * @brief 构造函数
     * @param owner 属性所属对象指针
     * @param name 属性名称（用于绑定路径和变更通知）
     */
    ObservableProperty(Owner* owner, std::string_view name) 
        : owner_(owner), name_(name), value_{} {
        // 设置默认行为
        getter_ = [this]() -> const T& { return value_; };
        
        setter_ = [this](const T& val) {
            if (value_ != val) {
                value_ = val;
                owner_->RaisePropertyChanged(name_);
            }
        };
        
        validator_ = [](const T&) -> bool { return true; };  // 默认总是验证通过
        
        // 自动注册到 PropertyAccessorRegistry
        using Base = ::fk::binding::INotifyPropertyChanged;
        ::fk::binding::PropertyAccessorRegistry::RegisterProperty<Base>(
            std::string(name),
            [this](const Base&) -> const T& { return this->get(); },
            [this](Base&, const std::any& val) { this->set(std::any_cast<T>(val)); }
        );
    }
    
    /**
     * @brief 自定义 getter 函数
     * @param func 自定义的 getter 函数，签名: const T&()
     * @return 返回 *this 以支持链式调用
     * 
     * 完全替换默认的 getter 行为。
     */
    ObservableProperty& Getter(std::function<const T&()> func) {
        getter_ = std::move(func);
        return *this;
    }
    
    /**
     * @brief 自定义 setter 函数
     * @param func 自定义的 setter 函数，签名: void(const T&)
     * @return 返回 *this 以支持链式调用
     * 
     * 完全替换默认的 setter 行为（包括变更通知）。
     * 如果需要变更通知，需要在自定义 setter 中手动调用 owner_->RaisePropertyChanged()。
     */
    ObservableProperty& Setter(std::function<void(const T&)> func) {
        setter_ = std::move(func);
        return *this;
    }
    
    /**
     * @brief 设置验证函数
     * @param func 验证函数，签名: bool(const T&)
     * @return 返回 *this 以支持链式调用
     * 
     * 在调用 setter 前验证值。如果返回 false，则不执行 setter。
     */
    ObservableProperty& Validator(std::function<bool(const T&)> func) {
        validator_ = std::move(func);
        return *this;
    }
    
    /**
     * @brief 获取属性值
     * @return 属性值的常量引用
     */
    const T& get() const { 
        return getter_(); 
    }
    
    /**
     * @brief 设置属性值
     * @param val 新值
     * 
     * 执行流程：验证 → setter
     * 如果验证失败，不执行 setter。
     */
    void set(const T& val) {
        if (!validator_(val)) return;  // 验证失败则不设置
        setter_(val);
    }
    
    /**
     * @brief 隐式转换为属性值类型
     * @return 属性值的常量引用
     */
    operator const T&() const { 
        return get(); 
    }
    
    /**
     * @brief 赋值运算符
     * @param val 新值
     * @return 返回 *this
     */
    ObservableProperty& operator=(const T& val) { 
        set(val); 
        return *this; 
    }

private:
    T value_;                                    ///< 属性值存储
    Owner* owner_;                                ///< 所属对象指针
    std::string_view name_;                       ///< 属性名称
    
    std::function<const T&()> getter_;            ///< 可自定义的 getter
    std::function<void(const T&)> setter_;        ///< 可自定义的 setter
    std::function<bool(const T&)> validator_;     ///< 可自定义的 validator
};

} // namespace fk::binding

namespace fk {
    using binding::ObservableProperty;
}
