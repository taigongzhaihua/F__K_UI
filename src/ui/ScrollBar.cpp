/**
 * @file ScrollBar.cpp
 * @brief ScrollBar 滚动条控件实现
 */

#include "fk/ui/ScrollBar.h"
#include "fk/ui/RepeatButton.h"
#include "fk/ui/Thumb.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/TemplateBinding.h"
#include "fk/animation/VisualStateManager.h"
#include "fk/animation/VisualStateBuilder.h"
#include <algorithm>
#include <cmath>

namespace fk::ui {

// ========== 模板显式实例化 ==========
template class Control<ScrollBar>;
template class FrameworkElement<ScrollBar>;

// ========== 默认模板 ==========

static ControlTemplate* CreateVerticalScrollBarTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(ScrollBar))
        ->SetFactory([]() -> UIElement* {
            auto* grid = new Grid();
            grid->Name("ScrollBarRoot");
            
            // 定义三行：LineUp按钮、Track、LineDown按钮
            grid->RowDefinitions({
                RowDefinition::Auto(),      // LineUp 按钮
                RowDefinition::Star(1),     // Track（填充剩余空间）
                RowDefinition::Auto()       // LineDown 按钮
            });
            
            // LineUp 按钮（向上箭头）
            auto* lineUpButton = (new RepeatButton())
            ->Name("PART_LineUpButton")
            ->Delay(250)
            ->Interval(33)
            ->Height(16)
            ->MinHeight(16)
            ->Background(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)))
            ->MouseOverBackground(new SolidColorBrush(Color::FromRGB(180, 180, 180, 255)))
            ->PressedBackground(new SolidColorBrush(Color::FromRGB(160, 160, 160, 255)))
            ->SetHAlign(HorizontalAlignment::Stretch)
            ->SetVAlign(VerticalAlignment::Stretch)
            ->Content((new TextBlock())
                ->Text("^")
                ->FontSize(10)
                ->SetHAlign(HorizontalAlignment::Center)
                ->SetVAlign(VerticalAlignment::Center));
            Grid::SetRow(lineUpButton, 0);
            grid->AddChild(lineUpButton);
            
            // Track
            auto* track = new Track();
            track->Name("PART_Track");
            track->SetOrientation(Orientation::Vertical);
            
            // 为 Track 设置子组件
            auto* decreaseButton = new RepeatButton();
            decreaseButton->SetBackground(nullptr);  // 透明
            decreaseButton->SetDelay(250);
            decreaseButton->SetInterval(33);
            track->SetDecreaseRepeatButton(decreaseButton);
            
            auto* thumb = new Thumb();
            thumb->SetBackground(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255)));
            track->SetThumb(thumb);
            
            auto* increaseButton = new RepeatButton();
            increaseButton->SetBackground(nullptr);  // 透明
            increaseButton->SetDelay(250);
            increaseButton->SetInterval(33);
            track->SetIncreaseRepeatButton(increaseButton);
            
            Grid::SetRow(track, 1);
            grid->AddChild(track);
            
            // LineDown 按钮（向下箭头）
            auto* lineDownButton = new RepeatButton();
            lineDownButton->Name("PART_LineDownButton");
            lineDownButton->SetDelay(250);
            lineDownButton->SetInterval(33);
            lineDownButton->Height(16);
            lineDownButton->MinHeight(16);
            lineDownButton->SetBackground(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)));
            lineDownButton->SetHAlign(HorizontalAlignment::Stretch);
            lineDownButton->SetVAlign(VerticalAlignment::Stretch);
            lineDownButton->Content((new TextBlock())
                ->Text("v")
                ->FontSize(10)
                ->SetHAlign(HorizontalAlignment::Center)
                ->SetVAlign(VerticalAlignment::Center));
            Grid::SetRow(lineDownButton, 2);
            grid->AddChild(lineDownButton);
            
            return grid;
        });
    
    return tmpl;
}

static ControlTemplate* CreateHorizontalScrollBarTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(ScrollBar))
        ->SetFactory([]() -> UIElement* {
            auto* grid = new Grid();
            grid->Name("ScrollBarRoot");
            
            // 定义三列：LineLeft按钮、Track、LineRight按钮
            grid->ColumnDefinitions({
                ColumnDefinition::Auto(),      // LineLeft 按钮
                ColumnDefinition::Star(1),     // Track（填充剩余空间）
                ColumnDefinition::Auto()       // LineRight 按钮
            });
            
            // LineLeft 按钮（向左箭头）
            auto* lineUpButton = new RepeatButton();
            lineUpButton->Name("PART_LineUpButton");
            lineUpButton->SetDelay(250);
            lineUpButton->SetInterval(33);
            lineUpButton->Width(16);
            lineUpButton->MinWidth(16);
            lineUpButton->SetBackground(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)));
            lineUpButton->SetHAlign(HorizontalAlignment::Stretch);
            lineUpButton->SetVAlign(VerticalAlignment::Stretch);
            lineUpButton->Content((new TextBlock())
                ->Text("<")
                ->FontSize(10)
                ->SetHAlign(HorizontalAlignment::Center)
                ->SetVAlign(VerticalAlignment::Center));
            Grid::SetColumn(lineUpButton, 0);
            grid->AddChild(lineUpButton);
            
            // Track
            auto* track = new Track();
            track->Name("PART_Track");
            track->SetOrientation(Orientation::Horizontal);
            
            // 为 Track 设置子组件
            auto* decreaseButton = new RepeatButton();
            decreaseButton->SetBackground(nullptr);  // 透明
            decreaseButton->SetDelay(250);
            decreaseButton->SetInterval(33);
            track->SetDecreaseRepeatButton(decreaseButton);
            
            auto* thumb = new Thumb();
            thumb->SetBackground(new SolidColorBrush(Color::FromRGB(120, 120, 120, 255)));
            track->SetThumb(thumb);
            
            auto* increaseButton = new RepeatButton();
            increaseButton->SetBackground(nullptr);  // 透明
            increaseButton->SetDelay(250);
            increaseButton->SetInterval(33);
            track->SetIncreaseRepeatButton(increaseButton);
            
            Grid::SetColumn(track, 1);
            grid->AddChild(track);
            
            // LineRight 按钮（向右箭头）
            auto* lineDownButton = new RepeatButton();
            lineDownButton->Name("PART_LineDownButton");
            lineDownButton->SetDelay(250);
            lineDownButton->SetInterval(33);
            lineDownButton->Width(16);
            lineDownButton->MinWidth(16);
            lineDownButton->SetBackground(new SolidColorBrush(Color::FromRGB(200, 200, 200, 255)));
            lineDownButton->SetHAlign(HorizontalAlignment::Stretch);
            lineDownButton->SetVAlign(VerticalAlignment::Stretch);
            lineDownButton->Content((new TextBlock())
                ->Text(">")
                ->FontSize(10)
                ->SetHAlign(HorizontalAlignment::Center)
                ->SetVAlign(VerticalAlignment::Center));
            Grid::SetColumn(lineDownButton, 2);
            grid->AddChild(lineDownButton);
            
            return grid;
        });
    
    return tmpl;
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& ScrollBar::OrientationProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Orientation",
        typeid(ui::Orientation),
        typeid(ScrollBar),
        binding::PropertyMetadata{
            std::any(ui::Orientation::Vertical),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any& newValue) {
                auto& scrollBar = static_cast<ScrollBar&>(obj);
                // 方向变化时，需要重新应用模板
                auto orientation = std::any_cast<ui::Orientation>(newValue);
                if (orientation == ui::Orientation::Vertical) {
                    scrollBar.SetTemplate(CreateVerticalScrollBarTemplate());
                } else {
                    scrollBar.SetTemplate(CreateHorizontalScrollBarTemplate());
                }
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::MinimumProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Minimum",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{
            std::any(0.0f),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any&) {
                auto& scrollBar = static_cast<ScrollBar&>(obj);
                scrollBar.SyncTrackProperties();
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::MaximumProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Maximum",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{
            std::any(100.0f),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any&) {
                auto& scrollBar = static_cast<ScrollBar&>(obj);
                scrollBar.SyncTrackProperties();
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::ValueProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "Value",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{
            std::any(0.0f),
            [](DependencyObject& obj, const DependencyProperty&, const std::any& oldValue, const std::any& newValue) {
                auto& scrollBar = static_cast<ScrollBar&>(obj);
                float oldVal = std::any_cast<float>(oldValue);
                float newVal = std::any_cast<float>(newValue);
                scrollBar.SyncTrackProperties();
                scrollBar.ValueChanged(oldVal, newVal);
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::ViewportSizeProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "ViewportSize",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{
            std::any(0.0f),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any&) {
                auto& scrollBar = static_cast<ScrollBar&>(obj);
                scrollBar.SyncTrackProperties();
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::SmallChangeProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "SmallChange",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{std::any(1.0f)}
    );
    return prop;
}

const binding::DependencyProperty& ScrollBar::LargeChangeProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "LargeChange",
        typeid(float),
        typeid(ScrollBar),
        binding::PropertyMetadata{std::any(10.0f)}
    );
    return prop;
}

// ========== 构造/析构 ==========

ScrollBar::ScrollBar() {
    // 确保依赖属性已注册
    OrientationProperty();
    MinimumProperty();
    MaximumProperty();
    ValueProperty();
    ViewportSizeProperty();
    SmallChangeProperty();
    LargeChangeProperty();
    
    // 设置默认背景色
    if (!GetBackground()) {
        SetBackground(new SolidColorBrush(Color::FromRGB(240, 240, 240, 255)));
    }
    
    // 设置默认模板（垂直方向）
    SetTemplate(CreateVerticalScrollBarTemplate());
}

ScrollBar::~ScrollBar() = default;

// ========== 属性访问器 ==========

ui::Orientation ScrollBar::GetOrientation() const {
    return DependencyObject::GetValue<ui::Orientation>(OrientationProperty());
}

ScrollBar* ScrollBar::SetOrientation(ui::Orientation value) {
    DependencyObject::SetValue(OrientationProperty(), value);
    return this;
}

float ScrollBar::GetMinimum() const {
    return DependencyObject::GetValue<float>(MinimumProperty());
}

ScrollBar* ScrollBar::SetMinimum(float value) {
    DependencyObject::SetValue(MinimumProperty(), value);
    return this;
}

float ScrollBar::GetMaximum() const {
    return DependencyObject::GetValue<float>(MaximumProperty());
}

ScrollBar* ScrollBar::SetMaximum(float value) {
    DependencyObject::SetValue(MaximumProperty(), value);
    return this;
}

float ScrollBar::GetValue() const {
    return DependencyObject::GetValue<float>(ValueProperty());
}

ScrollBar* ScrollBar::SetValue(float value) {
    float min = GetMinimum();
    float max = GetMaximum();
    float clamped = std::clamp(value, min, max);
    DependencyObject::SetValue(ValueProperty(), clamped);
    return this;
}

float ScrollBar::GetViewportSize() const {
    return DependencyObject::GetValue<float>(ViewportSizeProperty());
}

ScrollBar* ScrollBar::SetViewportSize(float value) {
    DependencyObject::SetValue(ViewportSizeProperty(), std::max(0.0f, value));
    return this;
}

float ScrollBar::GetSmallChange() const {
    return DependencyObject::GetValue<float>(SmallChangeProperty());
}

ScrollBar* ScrollBar::SetSmallChange(float value) {
    DependencyObject::SetValue(SmallChangeProperty(), std::max(0.0f, value));
    return this;
}

float ScrollBar::GetLargeChange() const {
    return DependencyObject::GetValue<float>(LargeChangeProperty());
}

ScrollBar* ScrollBar::SetLargeChange(float value) {
    DependencyObject::SetValue(LargeChangeProperty(), std::max(0.0f, value));
    return this;
}

// ========== 滚动操作 ==========

void ScrollBar::LineUp() {
    float newValue = GetValue() - GetSmallChange();
    ScrollToValue(newValue);
    RaiseScrollEvent(ScrollEventType::LineUp, GetValue());
}

void ScrollBar::LineDown() {
    float newValue = GetValue() + GetSmallChange();
    ScrollToValue(newValue);
    RaiseScrollEvent(ScrollEventType::LineDown, GetValue());
}

void ScrollBar::PageUp() {
    float newValue = GetValue() - GetLargeChange();
    ScrollToValue(newValue);
    RaiseScrollEvent(ScrollEventType::PageUp, GetValue());
}

void ScrollBar::PageDown() {
    float newValue = GetValue() + GetLargeChange();
    ScrollToValue(newValue);
    RaiseScrollEvent(ScrollEventType::PageDown, GetValue());
}

void ScrollBar::ScrollToValue(float value) {
    SetValue(value);  // SetValue 内部会 clamp
}

void ScrollBar::ScrollToMinimum() {
    ScrollToValue(GetMinimum());
    RaiseScrollEvent(ScrollEventType::First, GetValue());
}

void ScrollBar::ScrollToMaximum() {
    ScrollToValue(GetMaximum());
    RaiseScrollEvent(ScrollEventType::Last, GetValue());
}

// ========== 模板应用 ==========

void ScrollBar::OnTemplateApplied() {
    Control<ScrollBar>::OnTemplateApplied();
    
    // 获取模板根元素
    auto* templateRoot = GetTemplateRoot();
    if (!templateRoot) return;
    
    // 查找模板部件
    track_ = dynamic_cast<Track*>(templateRoot->FindName(PART_Track));
    lineUpButton_ = dynamic_cast<RepeatButton*>(templateRoot->FindName(PART_LineUpButton));
    lineDownButton_ = dynamic_cast<RepeatButton*>(templateRoot->FindName(PART_LineDownButton));
    
    // 绑定 Track 事件
    if (track_) {
        track_->ValueChanged += [this](float oldVal, float newVal) {
            OnTrackValueChanged(oldVal, newVal);
        };
        
        // 同步属性到 Track
        SyncTrackProperties();
    }
    
    // 绑定 LineUp 按钮事件
    if (lineUpButton_) {
        lineUpButton_->Click += [this]() {
            OnLineUpClick();
        };
    }
    
    // 绑定 LineDown 按钮事件
    if (lineDownButton_) {
        lineDownButton_->Click += [this]() {
            OnLineDownClick();
        };
    }
    
    UpdateVisualState(false);
}

// ========== 视觉状态 ==========

void ScrollBar::UpdateVisualState(bool useTransitions) {
    // 可以在此添加视觉状态切换逻辑
    // 例如：Normal, MouseOver, Disabled 等
}

// ========== 事件处理 ==========

void ScrollBar::OnTrackValueChanged(float oldVal, float newVal) {
    // Track 的值变化了，同步到 ScrollBar
    // 注意：避免循环更新
    if (std::abs(GetValue() - newVal) > 0.001f) {
        DependencyObject::SetValue(ValueProperty(), newVal);
        ValueChanged(oldVal, newVal);
        RaiseScrollEvent(ScrollEventType::ThumbTrack, newVal);
    }
}

void ScrollBar::OnLineUpClick() {
    LineUp();
}

void ScrollBar::OnLineDownClick() {
    LineDown();
}

// ========== 辅助方法 ==========

void ScrollBar::RaiseScrollEvent(ScrollEventType type, float newValue) {
    ScrollEventArgs args(type, newValue);
    Scroll(args);
}

void ScrollBar::SyncTrackProperties() {
    if (track_) {
        track_->SetMinimum(GetMinimum());
        track_->SetMaximum(GetMaximum());
        track_->SetViewportSize(GetViewportSize());
        
        // 避免循环更新
        if (std::abs(track_->GetValue() - GetValue()) > 0.001f) {
            track_->SetValue(GetValue());
        }
    }
}

} // namespace fk::ui
