#pragma once

#include <vector>
#include <functional>
#include "fk/ui/Primitives.h"

namespace fk::ui {

/**
 * @brief Observable collection of points for Polygon and Path classes
 * 
 * Provides notification mechanism when points are added, removed, or modified.
 * This enables automatic geometry updates when the collection changes.
 */
class PointCollection {
public:
    using ChangeCallback = std::function<void()>;
    using Iterator = std::vector<core::Point>::iterator;
    using ConstIterator = std::vector<core::Point>::const_iterator;

    PointCollection() = default;
    
    /**
     * @brief Add a point to the collection
     */
    void Add(const core::Point& point) {
        points_.push_back(point);
        NotifyChanged();
    }
    
    /**
     * @brief Remove a point at the specified index
     */
    void RemoveAt(size_t index) {
        if (index < points_.size()) {
            points_.erase(points_.begin() + index);
            NotifyChanged();
        }
    }
    
    /**
     * @brief Clear all points
     */
    void Clear() {
        points_.clear();
        NotifyChanged();
    }
    
    /**
     * @brief Get point at index
     */
    const core::Point& operator[](size_t index) const {
        return points_[index];
    }
    
    /**
     * @brief Get point at index (mutable)
     */
    core::Point& operator[](size_t index) {
        return points_[index];
    }
    
    /**
     * @brief Get number of points
     */
    size_t Count() const {
        return points_.size();
    }
    
    /**
     * @brief Check if collection is empty
     */
    bool IsEmpty() const {
        return points_.empty();
    }
    
    /**
     * @brief Register callback for collection changes
     */
    void SetChangeCallback(ChangeCallback callback) {
        changeCallback_ = callback;
    }
    
    // Iterator support
    Iterator begin() { return points_.begin(); }
    Iterator end() { return points_.end(); }
    ConstIterator begin() const { return points_.begin(); }
    ConstIterator end() const { return points_.end(); }
    
private:
    void NotifyChanged() {
        if (changeCallback_) {
            changeCallback_();
        }
    }
    
    std::vector<core::Point> points_;
    ChangeCallback changeCallback_;
};

} // namespace fk::ui
