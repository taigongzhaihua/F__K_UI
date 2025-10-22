// Event.h - minimal event template extracted from original UIFramework.h
#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace fk {

/**
 * @brief 通用事件模板。
 *
 * 提供一个简单的基于回调的事件机制，允许添加/移除监听器并触发它们。
 * - Handler 使用 std::function，接收模板参数中的参数列表。
 * - 支持 Add、Remove、RemoveAll 操作，以及 += / -= 操作符重载以方便使用。
 *
 * 注意：这是一个轻量演示实现，未处理线程安全和复杂比较（移除时通过 target_type 比较类型，可能不足以区分不同闭包）。
 * 在生产环境中建议使用带 id 的订阅模式或可比较的回调包装器。
 *
 * @tparam Args 事件回调参数类型列表
 */
template<typename... Args>
class Event {
public:
    using Handler = std::function<void(Args...)>;

    /**
     * @brief 注册一个事件处理器
     * @param h 要注册的回调
     */
    void Add(const Handler& h) { handlers_.push_back(h); }

    /**
     * @brief 尝试移除与给定 handler 类型相同的监听器（有限的移除策略）
     * @param h 要移除的回调（按类型匹配）
     */
    void Remove(const Handler& h) {
        auto it = std::remove_if(handlers_.begin(), handlers_.end(),
            [&](const Handler& existing) { return existing.target_type() == h.target_type(); });
        handlers_.erase(it, handlers_.end());
    }

    /**
     * @brief 移除所有监听器
     */
    void RemoveAll() { handlers_.clear(); }

    void operator+=(const Handler& h) { Add(h); }
    void operator-=(const Handler& h) { Remove(h); }

    /**
     * @brief 触发事件，按添加顺序依次调用回调
     * @param args 传递给回调的参数
     */
    void operator()(Args... args) const {
        for (auto &h : handlers_) if (h) h(args...);
    }

private:
    std::vector<Handler> handlers_;
};

/// 前向声明：Window 与 Application
class Window;
class Application;

/// 智能指针别名：表示窗口引用
using WindowPtr = std::shared_ptr<Window>;

} // namespace fk
