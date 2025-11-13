#pragma once

#include "fk/animation/Timeline.h"
#include "fk/animation/KeyFrame.h"
#include "fk/binding/DependencyObject.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace fk::animation {

// KeyFrameCollection - 关键帧集合
template<typename T>
class KeyFrameCollection {
public:
    void Add(std::shared_ptr<KeyFrame<T>> keyFrame) {
        keyFrames_.push_back(keyFrame);
        sorted_ = false;
    }
    
    void Clear() {
        keyFrames_.clear();
        sorted_ = false;
    }
    
    size_t Count() const {
        return keyFrames_.size();
    }
    
    std::shared_ptr<KeyFrame<T>> Get(size_t index) {
        return keyFrames_[index];
    }
    
    const std::shared_ptr<KeyFrame<T>> Get(size_t index) const {
        return keyFrames_[index];
    }
    
    // 排序关键帧（按时间）
    void Sort(std::chrono::milliseconds totalDuration) {
        if (sorted_) return;
        
        // 计算实际时间
        for (size_t i = 0; i < keyFrames_.size(); ++i) {
            auto& kf = keyFrames_[i];
            auto keyTime = kf->GetKeyTime();
            
            switch (keyTime.type) {
                case KeyTime::KeyTimeType::Uniform:
                    // 均匀分布
                    resolvedTimes_.push_back(
                        totalDuration * i / std::max<size_t>(1, keyFrames_.size() - 1)
                    );
                    break;
                    
                case KeyTime::KeyTimeType::Percent:
                    resolvedTimes_.push_back(
                        std::chrono::milliseconds(
                            static_cast<long long>(totalDuration.count() * keyTime.percent)
                        )
                    );
                    break;
                    
                case KeyTime::KeyTimeType::TimeSpan:
                    resolvedTimes_.push_back(keyTime.timeSpan);
                    break;
                    
                case KeyTime::KeyTimeType::Paced:
                    // Paced 需要更复杂的计算，暂时按均匀处理
                    resolvedTimes_.push_back(
                        totalDuration * i / std::max<size_t>(1, keyFrames_.size() - 1)
                    );
                    break;
            }
        }
        
        // 按时间排序
        std::vector<size_t> indices(keyFrames_.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            indices[i] = i;
        }
        
        std::sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
            return resolvedTimes_[a] < resolvedTimes_[b];
        });
        
        // 重新排列
        std::vector<std::shared_ptr<KeyFrame<T>>> sortedFrames;
        std::vector<std::chrono::milliseconds> sortedTimes;
        for (size_t idx : indices) {
            sortedFrames.push_back(keyFrames_[idx]);
            sortedTimes.push_back(resolvedTimes_[idx]);
        }
        
        keyFrames_ = sortedFrames;
        resolvedTimes_ = sortedTimes;
        sorted_ = true;
    }
    
    std::chrono::milliseconds GetResolvedTime(size_t index) const {
        return resolvedTimes_[index];
    }

private:
    std::vector<std::shared_ptr<KeyFrame<T>>> keyFrames_;
    std::vector<std::chrono::milliseconds> resolvedTimes_;
    bool sorted_{false};
};

// KeyFrameAnimation - 关键帧动画基类
template<typename T>
class KeyFrameAnimation : public Timeline {
public:
    KeyFrameAnimation() = default;
    virtual ~KeyFrameAnimation() = default;
    
    // 关键帧集合
    KeyFrameCollection<T>& KeyFrames() { return keyFrames_; }
    const KeyFrameCollection<T>& KeyFrames() const { return keyFrames_; }
    
    // 设置目标
    void SetTarget(binding::DependencyObject* target, const binding::DependencyProperty* property) {
        target_ = target;
        targetProperty_ = property;
        hasInitialValue_ = false;
    }
    
    binding::DependencyObject* GetTarget() const { return target_; }
    const binding::DependencyProperty* GetTargetProperty() const { return targetProperty_; }

protected:
    void UpdateCurrentValue(double progress) override {
        if (!target_ || !targetProperty_ || keyFrames_.Count() == 0) {
            return;
        }
        
        // 首次更新时，保存初始值
        if (!hasInitialValue_) {
            try {
                auto value = target_->GetValue(*targetProperty_);
                if (value.has_value()) {
                    initialValue_ = std::any_cast<T>(value);
                }
            } catch (...) {
                initialValue_ = GetDefaultValue();
            }
            hasInitialValue_ = true;
            
            // 排序关键帧
            auto duration = GetDuration();
            if (duration.HasTimeSpan()) {
                keyFrames_.Sort(duration.timeSpan);
            }
        }
        
        // 计算当前时间
        auto currentTime = GetCurrentTime();
        
        // 找到当前所在的关键帧区间
        T currentValue = initialValue_;
        
        size_t count = keyFrames_.Count();
        if (count == 0) {
            return;
        }
        
        // 如果在第一个关键帧之前
        if (currentTime < keyFrames_.GetResolvedTime(0)) {
            currentValue = InterpolateValue(
                initialValue_,
                keyFrames_.Get(0)->GetValue(),
                static_cast<double>(currentTime.count()) / 
                static_cast<double>(keyFrames_.GetResolvedTime(0).count())
            );
        }
        // 如果在最后一个关键帧之后
        else if (currentTime >= keyFrames_.GetResolvedTime(count - 1)) {
            currentValue = keyFrames_.Get(count - 1)->GetValue();
        }
        // 在关键帧区间中
        else {
            for (size_t i = 0; i < count - 1; ++i) {
                auto time1 = keyFrames_.GetResolvedTime(i);
                auto time2 = keyFrames_.GetResolvedTime(i + 1);
                
                if (currentTime >= time1 && currentTime < time2) {
                    auto kf1 = keyFrames_.Get(i);
                    auto kf2 = keyFrames_.Get(i + 1);
                    
                    double segmentProgress = 0.0;
                    auto segmentDuration = time2 - time1;
                    if (segmentDuration.count() > 0) {
                        segmentProgress = static_cast<double>((currentTime - time1).count()) /
                                        static_cast<double>(segmentDuration.count());
                    }
                    
                    currentValue = kf2->InterpolateValue(kf1->GetValue(), segmentProgress);
                    break;
                }
            }
        }
        
        // 应用到目标属性
        target_->SetValue(*targetProperty_, currentValue);
    }
    
    // 子类需要实现
    virtual T GetDefaultValue() const = 0;
    virtual T InterpolateValue(const T& from, const T& to, double progress) const = 0;

private:
    KeyFrameCollection<T> keyFrames_;
    binding::DependencyObject* target_{nullptr};
    const binding::DependencyProperty* targetProperty_{nullptr};
    T initialValue_;
    bool hasInitialValue_{false};
};

} // namespace fk::animation
