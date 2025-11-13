#include "fk/animation/Timeline.h"
#include <algorithm>

namespace fk::animation {

Timeline::Timeline() {
}

// 依赖属性定义
const binding::DependencyProperty& Timeline::BeginTimeProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "BeginTime",
        typeid(std::chrono::milliseconds),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(std::chrono::milliseconds(0))}
    );
    return property;
}

const binding::DependencyProperty& Timeline::DurationProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "Duration",
        typeid(Duration),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(Duration::Automatic())}
    );
    return property;
}

const binding::DependencyProperty& Timeline::SpeedRatioProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "SpeedRatio",
        typeid(double),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(1.0)}
    );
    return property;
}

const binding::DependencyProperty& Timeline::AutoReverseProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "AutoReverse",
        typeid(bool),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(false)}
    );
    return property;
}

const binding::DependencyProperty& Timeline::RepeatBehaviorProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "RepeatBehavior",
        typeid(RepeatBehavior),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(RepeatBehavior())}
    );
    return property;
}

const binding::DependencyProperty& Timeline::FillBehaviorProperty() {
    static auto& property = binding::DependencyProperty::Register(
        "FillBehavior",
        typeid(FillBehavior),
        typeid(Timeline),
        binding::PropertyMetadata{std::any(FillBehavior::HoldEnd)}
    );
    return property;
}

// 属性访问器实现
std::chrono::milliseconds Timeline::GetBeginTime() const {
    return std::any_cast<std::chrono::milliseconds>(GetValue(BeginTimeProperty()));
}

void Timeline::SetBeginTime(std::chrono::milliseconds value) {
    SetValue(BeginTimeProperty(), value);
}

Duration Timeline::GetDuration() const {
    return std::any_cast<Duration>(GetValue(DurationProperty()));
}

void Timeline::SetDuration(Duration value) {
    SetValue(DurationProperty(), value);
}

double Timeline::GetSpeedRatio() const {
    return std::any_cast<double>(GetValue(SpeedRatioProperty()));
}

void Timeline::SetSpeedRatio(double value) {
    SetValue(SpeedRatioProperty(), value);
}

bool Timeline::GetAutoReverse() const {
    return std::any_cast<bool>(GetValue(AutoReverseProperty()));
}

void Timeline::SetAutoReverse(bool value) {
    SetValue(AutoReverseProperty(), value);
}

RepeatBehavior Timeline::GetRepeatBehavior() const {
    return std::any_cast<RepeatBehavior>(GetValue(RepeatBehaviorProperty()));
}

void Timeline::SetRepeatBehavior(const RepeatBehavior& value) {
    SetValue(RepeatBehaviorProperty(), value);
}

FillBehavior Timeline::GetFillBehavior() const {
    return std::any_cast<FillBehavior>(GetValue(FillBehaviorProperty()));
}

void Timeline::SetFillBehavior(FillBehavior value) {
    SetValue(FillBehaviorProperty(), value);
}

// 时间线控制
void Timeline::Begin() {
    isActive_ = true;
    isPaused_ = false;
    currentTime_ = std::chrono::milliseconds(0);
    totalElapsedTime_ = std::chrono::milliseconds(0);
    currentIteration_ = 0;
}

void Timeline::Stop() {
    isActive_ = false;
    isPaused_ = false;
    currentTime_ = std::chrono::milliseconds(0);
    
    // 根据 FillBehavior 决定是否重置值
    if (GetFillBehavior() == FillBehavior::Stop) {
        UpdateCurrentValue(0.0);
    }
}

void Timeline::Pause() {
    if (isActive_) {
        isPaused_ = true;
    }
}

void Timeline::Resume() {
    if (isActive_ && isPaused_) {
        isPaused_ = false;
    }
}

void Timeline::Seek(std::chrono::milliseconds offset) {
    currentTime_ = offset;
    OnCurrentTimeInvalidated();
}

double Timeline::GetProgress() const {
    Duration duration = GetDuration();
    if (!duration.HasTimeSpan() || duration.timeSpan.count() == 0) {
        return 0.0;
    }
    
    double progress = static_cast<double>(currentTime_.count()) / 
                      static_cast<double>(duration.timeSpan.count());
    
    // 处理 AutoReverse
    if (GetAutoReverse()) {
        int cycle = static_cast<int>(progress);
        double fraction = progress - cycle;
        if (cycle % 2 == 1) {
            fraction = 1.0 - fraction;
        }
        progress = fraction;
    }
    
    return std::clamp(progress, 0.0, 1.0);
}

Duration Timeline::GetNaturalDuration() const {
    Duration duration = GetDuration();
    if (duration.automatic) {
        // 子类可以重写以提供自然持续时间
        return Duration(std::chrono::milliseconds(1000));
    }
    return duration;
}

void Timeline::OnCurrentTimeInvalidated() {
    CurrentTimeInvalidated(this, GetProgress());
}

void Timeline::Update(std::chrono::milliseconds deltaTime) {
    if (!isActive_ || isPaused_) {
        return;
    }
    
    // 应用速度比率
    double speedRatio = GetSpeedRatio();
    auto adjustedDelta = std::chrono::milliseconds(
        static_cast<long long>(deltaTime.count() * speedRatio)
    );
    
    // 检查是否应该开始
    auto beginTime = GetBeginTime();
    if (totalElapsedTime_ < beginTime) {
        totalElapsedTime_ += adjustedDelta;
        if (totalElapsedTime_ < beginTime) {
            return;  // 还没到开始时间
        }
        // 调整 delta 为超出开始时间的部分
        adjustedDelta = totalElapsedTime_ - beginTime;
    }
    
    currentTime_ += adjustedDelta;
    totalElapsedTime_ += adjustedDelta;
    
    Duration duration = GetNaturalDuration();
    RepeatBehavior repeat = GetRepeatBehavior();
    
    // 检查是否完成
    bool completed = false;
    if (duration.HasTimeSpan()) {
        auto durationMs = duration.timeSpan;
        if (GetAutoReverse()) {
            durationMs *= 2;  // AutoReverse 使持续时间翻倍
        }
        
        if (currentTime_ >= durationMs) {
            if (repeat.forever) {
                // 永久重复，循环回到开始
                currentTime_ = std::chrono::milliseconds(0);
                currentIteration_++;
            } else if (repeat.count > 1.0) {
                currentIteration_++;
                if (currentIteration_ < static_cast<int>(repeat.count)) {
                    currentTime_ = std::chrono::milliseconds(0);
                } else {
                    currentTime_ = durationMs;
                    completed = true;
                }
            } else {
                currentTime_ = durationMs;
                completed = true;
            }
        }
    }
    
    // 更新当前值
    double progress = GetProgress();
    UpdateCurrentValue(progress);
    OnCurrentTimeInvalidated();
    
    // 触发完成事件
    if (completed) {
        isActive_ = false;
        Completed(this);
    }
}

} // namespace fk::animation
