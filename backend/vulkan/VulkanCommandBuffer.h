#pragma once

#include "api/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace VALX
{
    class VulkanCommandBuffer : public CommandBuffer
    {
        VkCommandBuffer commandBuffer;

    public:
        VulkanCommandBuffer(VkCommandBuffer commandBuffer);

        virtual void Begin(CommandBufferFlags flags) override;
        virtual void End() override;
    };

    VkCommandBufferUsageFlags ConvertCommandBufferFlagsVulkan(CommandBufferFlags flags);
}