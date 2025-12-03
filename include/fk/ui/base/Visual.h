#pragma once

#include "fk/binding/DependencyObject.h"
#include "fk/ui/graphics/Primitives.h"
#include <vector>
#include <memory>

namespace fk {
    namespace render {
        class RenderContext;
    }
}

namespace fk::ui {

// 前向声明
class VisualCollection;

/**
 * @brief 视觉树节点基类
 * 
 * 职责：
 * - 管理视觉树父子关系
 * - 提供变换、命中测试
 * - 生成绘制命令
 * 
 * 继承：DependencyObject
 */
class Visual : public binding::DependencyObject {
public:
    Visual();
    virtual ~Visual();

    // ========== 视觉树管理 ==========
    
    /**
     * @brief 添加子视觉节点
     */
    void AddVisualChild(Visual* child);
    
    /**
     * @brief 移除子视觉节点
     */
    void RemoveVisualChild(Visual* child);
    
    /**
     * @brief 获取父视觉节点
     */
    Visual* GetVisualParent() const { return visualParent_; }
    
    /**
     * @brief 获取子节点数量
     */
    size_t GetVisualChildrenCount() const;
    
    /**
     * @brief 按索引获取子节点
     */
    Visual* GetVisualChild(size_t index) const;

    // ========== 变换 ==========
    
    /**
     * @brief 设置局部变换矩阵
     */
    void SetTransform(const Matrix3x2& transform);
    
    /**
     * @brief 获取局部变换矩阵
     */
    const Matrix3x2& GetTransform() const { return transform_; }
    
    /**
     * @brief 计算从根节点到当前节点的累积变换
     */
    Matrix3x2 GetAbsoluteTransform() const;

    // ========== 命中测试 ==========
    
    /**
     * @brief 命中测试
     * @param point 测试点（局部坐标）
     * @param result 命中结果
     * @return 是否命中
     */
    virtual bool HitTest(const Point& point, HitTestResult& result) const;

    // ========== 渲染 ==========
    
    /**
     * @brief 收集绘制命令
     * @param context 渲染上下文
     */
    virtual void CollectDrawCommands(render::RenderContext& context);
    
    /**
     * @brief 获取边界框（局部坐标）
     */
    virtual Rect GetBounds() const;
    
    /**
     * @brief 标记需要重新渲染
     */
    void InvalidateVisual();

protected:
    /**
     * @brief 访问子节点集合（派生类使用）
     */
    std::vector<Visual*>& GetVisualChildrenInternal() { return visualChildren_; }

private:
    // 友元声明：允许 VisualCollection 访问私有成员
    friend class VisualCollection;
    
    Visual* visualParent_{nullptr};
    std::vector<Visual*> visualChildren_;
    Matrix3x2 transform_;
};

} // namespace fk::ui
