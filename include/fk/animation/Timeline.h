#pragma once

#include <chrono>
#include <memory>
#include <functional>
#include "fk/core/Event.h"
#include "fk/binding/DependencyObject.h"

namespace fk::animation {

// 持续时间结构
struct Duration {
    std::chrono::milliseconds timeSpan;
    bool automatic{false};  // 自动持续时间
    bool forever{false};    // 永久持续

    Duration() : timeSpan(0), automatic(true) {}
    explicit Duration(std::chrono::milliseconds ms) : timeSpan(ms) {}
    
    static Duration Automatic() { 
        Duration d; 
        d.automatic = true; 
        return d; 
    }
    
    static Duration Forever() { 
        Duration d; 
        d.forever = true; 
        return d; 
    }
    
    bool HasTimeSpan() const { return !automatic && !forever; }
};

// 填充行为枚举
enum class FillBehavior {
    HoldEnd,    // 动画结束后保持最终值
    Stop        // 动画结束后恢复到初始值
};

// 重复行为
struct RepeatBehavior {
    double count{1.0};      // 重复次数
    Duration duration;       // 重复时长
    bool forever{false};     // 永久重复
    
    RepeatBehavior() = default;
    explicit RepeatBehavior(double cnt) : count(cnt) {}
    explicit RepeatBehavior(Duration dur) : duration(dur) {}
    
    static RepeatBehavior Forever() {
        RepeatBehavior rb;
        rb.forever = true;
        return rb;
    }
};

// Timeline 基类 - 所有动画的基础类
class Timeline : public binding::DependencyObject {
public:
    Timeline();
    virtual ~Timeline() = default;

    // 依赖属性
    static const binding::DependencyProperty& BeginTimeProperty();
    static const binding::DependencyProperty& DurationProperty();
    static const binding::DependencyProperty& SpeedRatioProperty();
    static const binding::DependencyProperty& AutoReverseProperty();
    static const binding::DependencyProperty& RepeatBehaviorProperty();
    static const binding::DependencyProperty& FillBehaviorProperty();

    // 属性访问器
    std::chrono::milliseconds GetBeginTime() const;
    void SetBeginTime(std::chrono::milliseconds value);
    
    Duration GetDuration() const;
    void SetDuration(Duration value);
    
    double GetSpeedRatio() const;
    void SetSpeedRatio(double value);
    
    bool GetAutoReverse() const;
    void SetAutoReverse(bool value);
    
    RepeatBehavior GetRepeatBehavior() const;
    void SetRepeatBehavior(const RepeatBehavior& value);
    
    FillBehavior GetFillBehavior() const;
    void SetFillBehavior(FillBehavior value);

    // 事件
    core::Event<Timeline*> Completed;
    core::Event<Timeline*, double> CurrentTimeInvalidated;  // 当前时间改变

    // 时间线控制
    virtual void Begin();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();
    virtual void Seek(std::chrono::milliseconds offset);

    // 状态查询
    bool IsActive() const { return isActive_; }
    bool IsPaused() const { return isPaused_; }
    
    std::chrono::milliseconds GetCurrentTime() const { return currentTime_; }
    double GetProgress() const;  // 返回0.0-1.0的进度
    
    // 更新方法（公开用于手动驱动动画）
    void Update(std::chrono::milliseconds deltaTime);

protected:
    // 子类需要实现的方法
    virtual Duration GetNaturalDuration() const;
    virtual void OnCurrentTimeInvalidated();
    virtual void UpdateCurrentValue(double progress) = 0;

private:
    friend class AnimationClock;
    
    bool isActive_{false};
    bool isPaused_{false};
    std::chrono::milliseconds currentTime_{0};
    std::chrono::milliseconds totalElapsedTime_{0};
    int currentIteration_{0};
};

} // namespace fk::animation
