#include "VulkanCommandBuffer.h"
#include "Utilities.h"

namespace VALX
{
    VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer commandBuffer)
        : commandBuffer(commandBuffer)
    {
    }

    void VulkanCommandBuffer::Begin(CommandBufferFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = ConvertCommandBufferFlagsVulkan(flags);

        VALX_VK_SUCCESS(vkBeginCommandBuffer(this->commandBuffer, &beginInfo));
    }

    void VulkanCommandBuffer::End()
    {
        VALX_VK_SUCCESS(vkEndCommandBuffer(this->commandBuffer));
    }

    VkCommandBufferUsageFlags ConvertCommandBufferFlagsVulkan(CommandBufferFlags flags)
    {
        VkCommandBufferUsageFlags result = {};
        if (static_cast<bool>(flags & CommandBufferFlags::SUBMIT_ONCE))
            result |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        return result;
    }
}