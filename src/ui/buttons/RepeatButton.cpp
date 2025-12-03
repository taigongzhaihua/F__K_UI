/**
 * @file RepeatButton.cpp
 * @brief RepeatButton 重复按钮控件实现
 */

#include "fk/ui/buttons/RepeatButton.h"
#include "fk/ui/styling/ControlTemplate.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/controls/ContentPresenter.h"
#include "fk/ui/graphics/Brush.h"
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

// ========== 构造函�?析构函数 ==========

RepeatButton::RepeatButton() {
    // 设置默认背景�?
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

// ========== 依赖属�?==========

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
    SetValue(IntervalProperty(), std::max(1, milliseconds));  // 最�?1ms
    return this;
}

// ========== 事件处理 ==========

void RepeatButton::OnPointerPressed(PointerEventArgs& e) {
    ButtonBase<RepeatButton>::OnPointerPressed(e);
    
    if (!GetIsEnabled()) {
        return;
    }
    
    // 只响应左键（或根�?PrimaryClickOnly 设置�?
    if (GetPrimaryClickOnly() && !e.IsLeftButton()) {
        return;
    }
    
    // 保存鼠标在窗口中的位置（不转换为本地坐标�?
    // 因为按钮会移动（�?Track 中的 DecreaseButton），需要每次检查时重新计算
    lastPointerPosition_ = e.position;
    
    isPointerInBounds_.store(true);
    
    // 立即触发一次点�?
    Click();
    
    // 开始重复计�?
    StartRepeat();
}

void RepeatButton::OnPointerMoved(PointerEventArgs& e) {
    ButtonBase<RepeatButton>::OnPointerMoved(e);
    
    // 注意：不更新 lastPointerPosition_
    // 我们只关心按下时的位置，如果按钮缩小后不再包含那个位置，就停�?
    // 如果更新坐标，会导致鼠标稍微移动就可能超出缩小后的按钮范�?
}

void RepeatButton::OnPointerReleased(PointerEventArgs& e) {
    // 停止重复
    StopRepeat();
    
    // 调用 ContentControl 的处理（跳过 ButtonBase，避免再次触�?Click�?
    ContentControl<RepeatButton>::OnPointerReleased(e);
    
    if (!GetIsEnabled()) {
        SetPressed(false);
        return;
    }
    
    // 重置按下状�?
    SetPressed(false);
    
    // 更新视觉状态（会根�?IsMouseOver 切换�?MouseOver �?Normal�?
    UpdateVisualState(true);
    
    e.handled = true;
}

void RepeatButton::OnPointerExited(PointerEventArgs& e) {
    // 标记鼠标已离开
    isPointerInBounds_.store(false);
    
    // 当鼠标离开按钮时，停止重复
    StopRepeat();
    
    // 调用基类处理
    ButtonBase<RepeatButton>::OnPointerExited(e);
}

void RepeatButton::OnClick() {
    // RepeatButton �?OnClick 由定时器调用
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
        
        // 开始重复触�?
        while (!shouldStop_.load()) {
            // 每次循环都重新检查鼠标是否在按钮范围�?
            // 这处理了按钮缩小后鼠标不在范围内的情况（鼠标没动，但按钮变小了）
            if (!IsPointerInBounds()) {
                return;
            }
            
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

bool RepeatButton::IsPointerInBounds() const {
    // 计算按钮当前在窗口中的全局位置
    // 按钮可能会移动（�?Track 中的 DecreaseButton/IncreaseButton�?
    Point globalOffset(0, 0);
    const UIElement* current = this;
    while (current) {
        Rect rect = current->GetLayoutRect();
        globalOffset.x += rect.x;
        globalOffset.y += rect.y;
        
        const Visual* parent = current->GetVisualParent();
        current = dynamic_cast<const UIElement*>(parent);
    }
    
    // 计算按钮在窗口中的边�?
    Size renderSize = GetRenderSize();
    Rect bounds(globalOffset.x, globalOffset.y, renderSize.width, renderSize.height);
    
    // 检查保存的鼠标窗口坐标是否在按钮当前的窗口边界�?
    return bounds.Contains(lastPointerPosition_);
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
