#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "Utilities.h"
#include "ExternalFunctions.h"
#include "api/Logger.h"

namespace VALX
{
    VulkanBuffer::VulkanBuffer(const BufferInfo& info)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.size = info.Size;
        bufferCreateInfo.usage = ConvertBufferFlags(info.Flags);

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = ConvertBufferMemory(info.MemoryType);

        VALX_VK_SUCCESS(vmaCreateBuffer(GetVulkanContext()->GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &this->buffer, &this->allocation, &this->allocationInfo));
        this->info = info;

        VkDebugUtilsObjectNameInfoEXT debugName = {};
        debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugName.objectType = VK_OBJECT_TYPE_BUFFER;
        debugName.objectHandle = reinterpret_cast<uint64_t>(this->buffer);
        debugName.pObjectName = info.Name.c_str();
        funcs.vkSetDebugUtilsObjectNameEXT(GetVulkanContext()->GetDevice(), &debugName);

        GetCurrentLogger()->LogInfo("VulkanBuffer", fmt::format("buffer `{}` created", info.Name));
    }

    const BufferInfo& VulkanBuffer::GetInfo() const
    {
        return this->info;
    }

    Buffer::Handle VulkanBuffer::GetHandle() const
    {
        return static_cast<Buffer::Handle>(this->buffer);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vmaDestroyBuffer(GetVulkanContext()->GetAllocator(), this->buffer, this->allocation);
        GetCurrentLogger()->LogInfo("VulkanBuffer", fmt::format("buffer `{}` destroyed", info.Name));
    }

    VkBufferUsageFlags ConvertBufferFlags(BufferFlags flags)
    {
        VkBufferUsageFlags result = {};
        if (static_cast<bool>(flags & BufferFlags::COPY_SRC))
            result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        if (static_cast<bool>(flags & BufferFlags::COPY_DST))
            result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (static_cast<bool>(flags & BufferFlags::UNIFORM_BUFFER))
            result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        if (static_cast<bool>(flags & BufferFlags::STORAGE_BUFFER))
            result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        if (static_cast<bool>(flags & BufferFlags::INDEX_BUFFER))
            result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        if (static_cast<bool>(flags & BufferFlags::VERTEX_BUFFER))
            result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (static_cast<bool>(flags & BufferFlags::INDIRECT_BUFFER))
            result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        
        return result;
    }

    VmaMemoryUsage ConvertBufferMemory(BufferMemory memory)
    {
        switch (memory)
        {
        case VALX::BufferMemory::CPU_ONLY:
            return VMA_MEMORY_USAGE_CPU_ONLY;
        case VALX::BufferMemory::GPU_ONLY:
            return VMA_MEMORY_USAGE_GPU_ONLY;
        case VALX::BufferMemory::FROM_CPU_TO_GPU:
            return VMA_MEMORY_USAGE_CPU_TO_GPU;
        case VALX::BufferMemory::FROM_GPU_TO_CPU:
            return VMA_MEMORY_USAGE_GPU_TO_CPU;
        case VALX::BufferMemory::CPU_AS_BACKUP:
            return VMA_MEMORY_USAGE_CPU_COPY;
        default:
            VALX_ASSERT(false && "invalid buffer memory");
            return VMA_MEMORY_USAGE_GPU_ONLY;
        }
    }
}