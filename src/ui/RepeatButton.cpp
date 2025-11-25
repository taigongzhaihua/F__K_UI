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

namespace fk::ui {

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

void RepeatButton::OnPointerExited(PointerEventArgs& e) {
    // 当鼠标离开按钮时，停止重复
    StopRepeat();
    
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerExited(e);
}

void RepeatButton::OnClick() {
    // RepeatButton 的 OnClick 由定时器调用
    // 直接触发 Click 事件
    Click();
}

// ========== 重复逻辑 ==========

void RepeatButton::StartRepeat() {
    // 如果已经在重复中，先停止
    if (isRepeating_.load()) {
        StopRepeat();
    }
    
    isRepeating_.store(true);
    shouldStop_.store(false);
    
    int delay = GetDelay();
    int interval = GetInterval();
    
    // 创建重复线程
    repeatThread_ = std::make_unique<std::thread>([this, delay, interval]() {
        // 等待初始延迟
        {
            std::unique_lock<std::mutex> lock(repeatMutex_);
            if (repeatCv_.wait_for(lock, std::chrono::milliseconds(delay), 
                [this]() { return shouldStop_.load(); })) {
                // 被通知停止
                return;
            }
        }
        
        // 开始重复触发
        while (!shouldStop_.load()) {
            // 触发点击
            if (!shouldStop_.load()) {
                Click();
            }
            
            // 等待间隔
            {
                std::unique_lock<std::mutex> lock(repeatMutex_);
                if (repeatCv_.wait_for(lock, std::chrono::milliseconds(interval),
                    [this]() { return shouldStop_.load(); })) {
                    // 被通知停止
                    return;
                }
            }
        }
    });
}

void RepeatButton::StopRepeat() {
    shouldStop_.store(true);
    isRepeating_.store(false);
    
    // 通知线程停止
    repeatCv_.notify_all();
    
    // 等待线程结束
    if (repeatThread_ && repeatThread_->joinable()) {
        repeatThread_->join();
    }
    repeatThread_.reset();
}

} // namespace fk::ui
