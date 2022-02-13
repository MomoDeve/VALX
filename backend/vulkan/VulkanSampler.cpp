#include "VulkanSampler.h"
#include "Utilities.h"
#include "VulkanContext.h"
#include "ExternalFunctions.h"
#include "api/Logger.h"

namespace VALX
{
    VkFilter ConvertFilter(Filter filter)
    {
        switch (filter)
        {
        case VALX::Filter::NEAREST:
            return VK_FILTER_NEAREST;
        case VALX::Filter::LINEAR:
            return VK_FILTER_LINEAR;
        default:
            VALX_ASSERT(false && "invalid filter");
            return VK_FILTER_NEAREST;
        }
    }

    VkSamplerMipmapMode ConvertMipMapFilter(Filter filter)
    {
        switch (filter)
        {
        case VALX::Filter::NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case VALX::Filter::LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        default:
            VALX_ASSERT(false && "invalid filter");
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        }
    }

    VkSamplerAddressMode ConvertAddressMode(AddressMode addressMode)
    {
        switch (addressMode)
        {
        case VALX::AddressMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case VALX::AddressMode::MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case VALX::AddressMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case VALX::AddressMode::CLAMP_TO_BORDER:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        default:
            VALX_ASSERT(false && "invalid address mode");
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }

    VkCompareOp ConvertCompareOp(CompareOp compare)
    {
        switch (compare)
        {
        case VALX::CompareOp::NEVER:
            return VK_COMPARE_OP_NEVER;
        case VALX::CompareOp::LESS:
            return VK_COMPARE_OP_LESS;
        case VALX::CompareOp::EQUAL:
            return VK_COMPARE_OP_EQUAL;
        case VALX::CompareOp::LESS_OR_EQUAL:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case VALX::CompareOp::GREATER:
            return VK_COMPARE_OP_GREATER;
        case VALX::CompareOp::NOT_EQUAL:
            return VK_COMPARE_OP_NOT_EQUAL;
        case VALX::CompareOp::GREATER_OR_EQUAL:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case VALX::CompareOp::ALWAYS:
            return VK_COMPARE_OP_ALWAYS;
        default:
            VALX_ASSERT(false && "invalid compare op");
            return VK_COMPARE_OP_NEVER;
        }
    }

    VkBorderColor ConvertBorderColor(BorderColor border)
    {
        switch (border)
        {
        case VALX::BorderColor::FLOAT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        case VALX::BorderColor::INT_TRANSPARENT_BLACK:
            return VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        case VALX::BorderColor::FLOAT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
        case VALX::BorderColor::INT_OPAQUE_BLACK:
            return VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        case VALX::BorderColor::FLOAT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
        case VALX::BorderColor::INT_OPAQUE_WHITE:
            return VK_BORDER_COLOR_INT_OPAQUE_WHITE;
        default:
            VALX_ASSERT(false && "invalid border color");
            return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        }
    }

    VulkanSampler::VulkanSampler(const SamplerInfo& info)
    {
        VkSamplerCreateInfo samplerCreateInfo = {};
        samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.magFilter = ConvertFilter(info.MagFilter);
        samplerCreateInfo.minFilter = ConvertFilter(info.MinFilter);
        samplerCreateInfo.mipmapMode = ConvertMipMapFilter(info.MipMapFilter);
        samplerCreateInfo.addressModeU = ConvertAddressMode(info.AddressModeU);
        samplerCreateInfo.addressModeV = ConvertAddressMode(info.AddressModeV);
        samplerCreateInfo.addressModeW = ConvertAddressMode(info.AddressModeW);
        samplerCreateInfo.mipLodBias = info.MipLodBias;
        samplerCreateInfo.anisotropyEnable = info.EnableAnisotropy;
        samplerCreateInfo.maxAnisotropy = info.MaxAnisotropy;
        samplerCreateInfo.compareEnable = (info.Compare != CompareOp::NEVER);
        samplerCreateInfo.compareOp = ConvertCompareOp(info.Compare);
        samplerCreateInfo.minLod = info.MinLod;
        samplerCreateInfo.maxLod = info.MaxLod;
        samplerCreateInfo.borderColor = ConvertBorderColor(info.Border);
        samplerCreateInfo.unnormalizedCoordinates = false;

        VALX_VK_SUCCESS(vkCreateSampler(GetVulkanContext()->GetDevice(), &samplerCreateInfo, nullptr, &this->sampler));
        this->info = info;

        VkDebugUtilsObjectNameInfoEXT debugName = {};
        debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugName.objectType = VK_OBJECT_TYPE_SAMPLER;
        debugName.objectHandle = reinterpret_cast<uint64_t>(this->sampler);
        debugName.pObjectName = info.Name.c_str();
        funcs.vkSetDebugUtilsObjectNameEXT(GetVulkanContext()->GetDevice(), &debugName);

        GetCurrentLogger()->LogInfo("VulkanSampler", fmt::format("sampler `{}` created", info.Name));
    }

    const SamplerInfo& VulkanSampler::GetInfo() const
    {
        return this->info;
    }

    Sampler::Handle VulkanSampler::GetHandle() const
    {
        return static_cast<Sampler::Handle>(this->sampler);
    }

    VulkanSampler::~VulkanSampler()
    {
        vkDestroySampler(GetVulkanContext()->GetDevice(), this->sampler, nullptr);
        GetCurrentLogger()->LogInfo("VulkanSampler", fmt::format("sampler `{}` destroyed", info.Name));
    }
}