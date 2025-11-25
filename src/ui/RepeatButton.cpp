/**
 * @file RepeatButton.cpp
 * @brief RepeatButton 重复按钮控件实现
 */

#include "fk/ui/RepeatButton.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/ui/Border.h"
#include "fk/ui/ContentPresenter.h"
#include "fk/ui/Brush.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/animation/VisualStateBuilder.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace fk::ui {

// ========== 简易定时器实现 ==========
// 由于项目没有全局 Dispatcher，使用简单的线程 + 回调方式
// 注意：回调会在后台线程中执行，需要注意线程安全

class SimpleTimer {
public:
    using Callback = std::function<void()>;
    
    SimpleTimer() = default;
    ~SimpleTimer() { Stop(); }
    
    void Start(int intervalMs, bool repeat, Callback callback) {
        Stop();
        
        running_ = true;
        callback_ = std::move(callback);
        interval_ = intervalMs;
        repeat_ = repeat;
        
        thread_ = std::thread([this]() {
            // 使用条件变量实现可中断的等待
            std::unique_lock<std::mutex> lock(mutex_);
            
            while (running_) {
                // 等待指定时间或被中断
                auto status = cv_.wait_for(lock, std::chrono::milliseconds(interval_));
                
                if (!running_) {
                    break;
                }
                
                // 超时触发回调
                if (status == std::cv_status::timeout && callback_) {
                    // 临时释放锁执行回调，避免死锁
                    lock.unlock();
                    callback_();
                    lock.lock();
                }
                
                if (!repeat_) {
                    break;
                }
            }
        });
    }
    
    void Stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
        }
        cv_.notify_all();  // 唤醒等待的线程
        
        if (thread_.joinable()) {
            thread_.join();
        }
        callback_ = nullptr;
    }
    
    bool IsRunning() const { return running_; }
    
private:
    std::thread thread_;
    std::atomic<bool> running_{false};
    std::mutex mutex_;
    std::condition_variable cv_;
    Callback callback_;
    int interval_{100};
    bool repeat_{false};
};

// ========== 默认模板 ==========

static ControlTemplate* CreateDefaultRepeatButtonTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(RepeatButton))
        ->SetFactory([]() -> UIElement* {
            return (new Border())
                ->Name("RootBorder")
                ->Background(binding::TemplateBinding(Control<RepeatButton>::BackgroundProperty()))
                ->BorderBrush(binding::TemplateBinding(Control<RepeatButton>::BorderBrushProperty()))
                ->BorderThickness(binding::TemplateBinding(Control<RepeatButton>::BorderThicknessProperty()))
                ->Padding(4.0f, 2.0f, 4.0f, 2.0f)
                ->CornerRadius(2.0f)
                ->Child(
                    (new ContentPresenter<>())
                        ->SetHAlign(HorizontalAlignment::Center)
                        ->SetVAlign(VerticalAlignment::Center)
                );
        })
        ->AddVisualStateGroup(
            animation::VisualStateBuilder::CreateGroup("CommonStates")
                ->State("Normal")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(Control<RepeatButton>::BackgroundProperty())
                    ->Duration(75)
                    ->EndAnimation()
                ->EndState()
                ->State("MouseOver")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(RepeatButton::MouseOverBackgroundProperty())
                    ->Duration(50)
                    ->EndAnimation()
                ->EndState()
                ->State("Pressed")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->ToBinding(RepeatButton::PressedBackgroundProperty())
                    ->Duration(50)
                    ->EndAnimation()
                ->EndState()
                ->State("Disabled")
                    ->ColorAnimation("RootBorder", "Background.Color")
                    ->To(Color::FromRGB(200, 200, 200, 255))
                    ->Duration(100)
                    ->EndAnimation()
                    ->DoubleAnimation("RootBorder", "Opacity")
                    ->To(0.6)
                    ->Duration(100)
                    ->EndAnimation()
                ->EndState()
                ->Build()
        );
    
    return tmpl;
}

// ========== 构造函数/析构函数 ==========

RepeatButton::RepeatButton() {
    // 设置默认背景色
    if (!GetBackground()) {
        SetBackground(new SolidColorBrush(Color::FromRGB(180, 180, 180, 255)));
    }
    
    // 设置默认模板
    if (!GetTemplate()) {
        SetTemplate(CreateDefaultRepeatButtonTemplate());
    }
}

RepeatButton::~RepeatButton() {
    StopRepeat();
}

// ========== 依赖属性 ==========

const binding::DependencyProperty& RepeatButton::DelayProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Delay",
        typeid(int),
        typeid(RepeatButton),
        binding::PropertyMetadata{std::any(250)}  // 默认 250ms
    );
    return property;
}

const binding::DependencyProperty& RepeatButton::IntervalProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Interval",
        typeid(int),
        typeid(RepeatButton),
        binding::PropertyMetadata{std::any(33)}   // 默认 ~30fps
    );
    return property;
}

// ========== 属性访问器 ==========

int RepeatButton::GetDelay() const {
    return GetValue<int>(DelayProperty());
}

RepeatButton* RepeatButton::SetDelay(int milliseconds) {
    SetValue(DelayProperty(), std::max(0, milliseconds));
    return this;
}

int RepeatButton::GetInterval() const {
    return GetValue<int>(IntervalProperty());
}

RepeatButton* RepeatButton::SetInterval(int milliseconds) {
    SetValue(IntervalProperty(), std::max(1, milliseconds));  // 最小 1ms
    return this;
}

// ========== 事件处理 ==========

void RepeatButton::OnPointerPressed(PointerEventArgs& e) {
    ButtonBase<RepeatButton>::OnPointerPressed(e);
    
    if (!GetIsEnabled()) {
        return;
    }
    
    // 只响应左键（或根据 PrimaryClickOnly 设置）
    if (GetPrimaryClickOnly() && !e.IsLeftButton()) {
        return;
    }
    
    // 立即触发一次点击
    Click();
    
    // 开始重复计时
    StartRepeat();
}

void RepeatButton::OnPointerReleased(PointerEventArgs& e) {
    // 停止重复
    StopRepeat();
    
    // 调用基类处理（但不再触发 Click，因为我们已经在重复中触发了）
    // 注意：直接调用 ContentControl 的处理，跳过 ButtonBase 的 Click 触发
    ContentControl<RepeatButton>::OnPointerReleased(e);
    
    // 更新视觉状态
    UpdateVisualState(true);
}

void RepeatButton::OnClick() {
    // RepeatButton 的 OnClick 由定时器调用
    // 直接触发 Click 事件
    Click();
}

// ========== 重复逻辑 ==========

void RepeatButton::StartRepeat() {
    if (isRepeating_) {
        return;
    }
    
    isRepeating_ = true;
    
    // 创建延迟定时器
    delayTimer_ = std::make_unique<SimpleTimer>();
    
    // 启动延迟定时器（单次）
    delayTimer_->Start(GetDelay(), false, [this]() {
        if (!isRepeating_) {
            return;
        }
        
        // 延迟结束后触发一次
        OnClick();
        
        // 启动重复定时器
        repeatTimer_ = std::make_unique<SimpleTimer>();
        repeatTimer_->Start(GetInterval(), true, [this]() {
            if (isRepeating_) {
                OnClick();
            }
        });
    });
}

void RepeatButton::StopRepeat() {
    isRepeating_ = false;
    
    if (delayTimer_) {
        delayTimer_->Stop();
        delayTimer_.reset();
    }
    
    if (repeatTimer_) {
        repeatTimer_->Stop();
        repeatTimer_.reset();
    }
}

} // namespace fk::ui
