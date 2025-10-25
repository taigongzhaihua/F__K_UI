#pragma once

namespace fk::ui {

/**
 * @brief 线程检查模式
 */
enum class ThreadCheckMode {
    /**
     * @brief 禁用线程检查（最快，但不安全）
     * 使用场景：发布版本，确认没有线程安全问题后禁用以提高性能
     */
    Disabled,
    
    /**
     * @brief 只警告，不抛异常（调试用）
     * 使用场景：开发阶段，希望看到跨线程访问但不中断程序
     */
    WarnOnly,
    
    /**
     * @brief 检测到跨线程访问时抛出异常（推荐）
     * 使用场景：调试阶段，强制开发者修复所有线程安全问题
     * 注意：如需跨线程调用，请使用 Dispatcher::Invoke() 或 Dispatcher::InvokeAsync()
     */
    ThrowException
};

/**
 * @brief 线程安全配置（全局单例）
 */
class ThreadingConfig {
public:
    /**
     * @brief 获取全局配置实例
     */
    static ThreadingConfig& Instance();
    
    /**
     * @brief 设置线程检查模式
     */
    void SetThreadCheckMode(ThreadCheckMode mode) { mode_ = mode; }
    
    /**
     * @brief 获取当前线程检查模式
     */
    ThreadCheckMode GetThreadCheckMode() const { return mode_; }
    
    /**
     * @brief 是否启用线程检查（便捷方法）
     */
    bool IsThreadCheckEnabled() const { 
        return mode_ != ThreadCheckMode::Disabled; 
    }
    
    /**
     * @brief 是否只警告不抛异常
     */
    bool IsWarnOnlyMode() const {
        return mode_ == ThreadCheckMode::WarnOnly;
    }

private:
    ThreadingConfig() = default;
    
    ThreadCheckMode mode_{ThreadCheckMode::ThrowException};  // 默认：调试模式抛异常
};

} // namespace fk::ui
