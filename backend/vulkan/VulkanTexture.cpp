#include "VulkanTexture.h"
#include "Utilities.h"
#include "VulkanFormat.h"
#include "VulkanContext.h"
#include "ExternalFunctions.h"
#include "api/Logger.h"

#include <algorithm>

namespace VALX
{
    static VkImageCreateFlags GetImageCreateFlags(const TextureInfo& info)
    {
        VkImageCreateFlags result = {};
        if (info.Layers > 1)
            result |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        if (info.Type == TextureType::TEXTURE_CUBE)
            result |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

        return result;
    }

    VulkanTexture::VulkanTexture(const TextureInfo& info)
    {
        if (info.Type == TextureType::TEXTURE_CUBE)
        {
            VALX_ASSERT(info.Depth == 1 && info.Layers % 6 == 0);
        }
        if (info.Type == TextureType::TEXTURE_3D)
        {
            VALX_ASSERT(info.Layers == 1);
        }

        VkImageCreateInfo imageCreateInfo = {};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.flags = GetImageCreateFlags(info);
        imageCreateInfo.extent.width = info.Width;
        imageCreateInfo.extent.height = info.Height;
        imageCreateInfo.extent.depth = info.Depth;
        imageCreateInfo.arrayLayers = info.Layers;
        imageCreateInfo.samples = ConvertSampleCountVulkan(info.Samples);
        imageCreateInfo.format = ConvertFormatVulkan(info.TextureFormat);
        imageCreateInfo.imageType = ConvertTextureTypeVulkan(info.Type);
        imageCreateInfo.mipLevels = (info.Mips == ALL_MIPS) ? GetMipLevelCount(std::max({ info.Width, info.Height, info.Depth })) : info.Mips;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.usage = ConvertTextureFlags(info.Flags);

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VALX_VK_SUCCESS(vmaCreateImage(GetVulkanContext()->GetAllocator(), &imageCreateInfo, &allocationCreateInfo, &this->image, &this->allocation, &this->allocationInfo));
        this->info = info;

        VkDebugUtilsObjectNameInfoEXT debugName = {};
        debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugName.objectType = VK_OBJECT_TYPE_IMAGE;
        debugName.objectHandle = reinterpret_cast<uint64_t>(this->image);
        debugName.pObjectName = info.Name.c_str();
        funcs.vkSetDebugUtilsObjectNameEXT(GetVulkanContext()->GetDevice(), &debugName);

        GetCurrentLogger()->LogInfo("VulkanTexture", fmt::format("texture `{}` created", info.Name));
    }

    const TextureInfo& VulkanTexture::GetInfo() const
    {
        return this->info;
    }

    Texture::Handle VulkanTexture::GetHandle() const
    {
        return static_cast<Texture::Handle>(this->image);
    }

    VulkanTexture::~VulkanTexture()
    {
        vmaDestroyImage(GetVulkanContext()->GetAllocator(), this->image, this->allocation);
        GetCurrentLogger()->LogInfo("VulkanTexture", fmt::format("texture `{}` destroyed", info.Name));
    }

    VkImageType ConvertTextureTypeVulkan(TextureType type)
    {
        switch (type)
        {
        case VALX::TextureType::TEXTURE_2D:
            return VK_IMAGE_TYPE_2D;
        case VALX::TextureType::TEXTURE_3D:
            return VK_IMAGE_TYPE_3D;
        case VALX::TextureType::TEXTURE_CUBE:
            return VK_IMAGE_TYPE_2D;
        default:
            VALX_ASSERT(false && "invalid texture type");
            return VK_IMAGE_TYPE_2D;
        }
    }

    VkSampleCountFlagBits ConvertSampleCountVulkan(SampleCount samples)
    {
        switch (samples)
        {
        case VALX::SampleCount::SAMPLES_1:
            return VK_SAMPLE_COUNT_1_BIT;
        case VALX::SampleCount::SAMPLES_2:
            return VK_SAMPLE_COUNT_2_BIT;
        case VALX::SampleCount::SAMPLES_4:
            return VK_SAMPLE_COUNT_4_BIT;
        case VALX::SampleCount::SAMPLES_8:
            return VK_SAMPLE_COUNT_8_BIT;
        case VALX::SampleCount::SAMPLES_16:
            return VK_SAMPLE_COUNT_16_BIT;
        case VALX::SampleCount::SAMPLES_32:
            return VK_SAMPLE_COUNT_32_BIT;
        case VALX::SampleCount::SAMPLES_64:
            return VK_SAMPLE_COUNT_64_BIT;
        default:
            VALX_ASSERT(false && "invalid sample count");
            return VK_SAMPLE_COUNT_1_BIT;
        }
    }

    VkImageUsageFlags ConvertTextureFlags(TextureFlags flags)
    {
        VkImageUsageFlags result = {};
        if (static_cast<bool>(flags & TextureFlags::COPY_SRC))
            result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (static_cast<bool>(flags & TextureFlags::COPY_DST))
            result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (static_cast<bool>(flags & TextureFlags::SAMPLED))
            result |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (static_cast<bool>(flags & TextureFlags::STORAGE))
            result |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (static_cast<bool>(flags & TextureFlags::COLOR_ATTACHMENT))
            result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (static_cast<bool>(flags & TextureFlags::DEPTH_STENCIL_ATTACHMENT))
            result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        
        return result;
    }
}