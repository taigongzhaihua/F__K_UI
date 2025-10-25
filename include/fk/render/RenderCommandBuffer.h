#pragma once

#include <vector>
#include "fk/render/RenderCommand.h"

namespace fk::render {

/**
 * @brief 渲染命令缓冲区 - 存储渲染命令
 */
class RenderCommandBuffer {
public:
    RenderCommandBuffer() = default;
    ~RenderCommandBuffer() = default;

    /**
     * @brief 添加命令
     */
    void AddCommand(RenderCommand command) {
        commands_.push_back(std::move(command));
    }

    /**
     * @brief 清空所有命令
     */
    void Clear() {
        commands_.clear();
    }

    /**
     * @brief 检查是否为空
     */
    bool IsEmpty() const {
        return commands_.empty();
    }

    /**
     * @brief 获取命令列表
     */
    const std::vector<RenderCommand>& GetCommands() const {
        return commands_;
    }

    /**
     * @brief 获取命令数量
     */
    size_t GetCommandCount() const {
        return commands_.size();
    }

private:
    std::vector<RenderCommand> commands_;
};

} // namespace fk::render
