#pragma once

#include "api/Buffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace VALX
{
    class VulkanBuffer : public Buffer
    {
        BufferInfo info = {};
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};

    public:
        VulkanBuffer(const BufferInfo& info);

        virtual const BufferInfo& GetInfo() const override;
        virtual Handle GetHandle() const override;
        virtual ~VulkanBuffer() override;
    };

    VkBufferUsageFlags ConvertBufferFlags(BufferFlags flags);
    VmaMemoryUsage ConvertBufferMemory(BufferMemory memory);
}