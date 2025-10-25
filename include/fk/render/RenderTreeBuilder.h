#pragma once

#include <memory>
#include "fk/ui/Visual.h"

namespace fk::render {

class RenderScene;

/**
 * @brief 渲染树构建器 - 遍历 UI 可视树并生成渲染命令
 */
class RenderTreeBuilder {
public:
    RenderTreeBuilder() = default;
    ~RenderTreeBuilder() = default;

    /**
     * @brief 重建渲染场景
     * @param visualRoot 可视树根节点
     * @param scene 输出的渲染场景
     */
    void Rebuild(const ui::Visual& visualRoot, RenderScene& scene);

private:
    /**
     * @brief 递归遍历可视树节点
     */
    void TraverseVisual(const ui::Visual& visual, RenderScene& scene, float parentOpacity = 1.0f);

    /**
     * @brief 生成元素的渲染内容
     */
    void GenerateRenderContent(const ui::Visual& visual, RenderScene& scene, float opacity);

    // 变换栈（简化版，暂时只记录累计偏移）
    float currentOffsetX_{0.0f};
    float currentOffsetY_{0.0f};
};

} // namespace fk::render
