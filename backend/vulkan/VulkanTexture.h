#pragma once

#include "api/Texture.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace VALX
{
    class VulkanTexture : public Texture
    {
        TextureInfo info = {};
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = {};
        VmaAllocationInfo allocationInfo = {};

    public:
        VulkanTexture(const TextureInfo& info);

        virtual const TextureInfo& GetInfo() const override;
        virtual Handle GetHandle() const override;
        virtual ~VulkanTexture() override;
    };

    VkImageType ConvertTextureTypeVulkan(TextureType type);
    VkSampleCountFlagBits ConvertSampleCountVulkan(SampleCount samples);
    VkImageUsageFlags ConvertTextureFlags(TextureFlags flags);
}