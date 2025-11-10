#include "fk/ui/Visual.h"

namespace fk::ui {

Visual::Visual() : visualParent_(nullptr), transform_(Matrix3x2::Identity()) {
}

Visual::~Visual() {
    // 清理子节点
    for (auto* child : visualChildren_) {
        if (child) {
            child->visualParent_ = nullptr;
        }
    }
}

void Visual::AddVisualChild(Visual* child) {
    if (child && child->visualParent_ != this) {
        // 从旧父节点移除
        if (child->visualParent_) {
            child->visualParent_->RemoveVisualChild(child);
        }
        
        visualChildren_.push_back(child);
        child->visualParent_ = this;
    }
}

void Visual::RemoveVisualChild(Visual* child) {
    if (!child) return;
    
    auto it = std::find(visualChildren_.begin(), visualChildren_.end(), child);
    if (it != visualChildren_.end()) {
        visualChildren_.erase(it);
        child->visualParent_ = nullptr;
    }
}

size_t Visual::GetVisualChildrenCount() const {
    return visualChildren_.size();
}

Visual* Visual::GetVisualChild(size_t index) const {
    if (index < visualChildren_.size()) {
        return visualChildren_[index];
    }
    return nullptr;
}

void Visual::SetTransform(const Matrix3x2& transform) {
    transform_ = transform;
    InvalidateVisual();
}

Matrix3x2 Visual::GetAbsoluteTransform() const {
    Matrix3x2 result = transform_;
    Visual* parent = visualParent_;
    
    while (parent) {
        result = parent->transform_ * result;
        parent = parent->visualParent_;
    }
    
    return result;
}

bool Visual::HitTest(const Point& point, HitTestResult& result) const {
    // 变换到局部坐标（需要逆变换）
    // 简化实现：如果变换只包含平移和缩放，可以直接计算逆变换
    // 完整实现需要矩阵求逆
    Point localPoint = point;
    
    // 从后向前检测子节点（绘制顺序相反）
    for (auto it = visualChildren_.rbegin(); it != visualChildren_.rend(); ++it) {
        if ((*it)->HitTest(localPoint, result)) {
            return true;
        }
    }
    
    // 检测自身边界
    Rect bounds = GetBounds();
    if (bounds.Contains(localPoint)) {
        result.visualHit = const_cast<Visual*>(this);
        result.pointHit = localPoint;
        return true;
    }
    
    return false;
}

void Visual::CollectDrawCommands(RenderContext& context) {
    // 默认实现：收集子节点的绘制命令
    for (auto* child : visualChildren_) {
        if (child) {
            child->CollectDrawCommands(context);
        }
    }
}

Rect Visual::GetBounds() const {
    // 默认实现：根据所有子节点计算边界框
    if (visualChildren_.empty()) {
        return Rect(0, 0, 0, 0);
    }
    
    Rect bounds = visualChildren_[0]->GetBounds();
    for (size_t i = 1; i < visualChildren_.size(); ++i) {
        // TODO: 合并子节点边界（需要考虑变换）
        // 当前简化实现
    }
    
    return bounds;
}

void Visual::InvalidateVisual() {
    // 标记当前节点需要重绘
    // 通知父节点向上传播无效化
    if (visualParent_) {
        visualParent_->InvalidateVisual();
    }
    // TODO: 与渲染系统集成，加入脏矩形队列
}

} // namespace fk::ui
