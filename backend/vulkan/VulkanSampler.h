#pragma once

#include "api/Sampler.h"
#include <vulkan/vulkan.h>

namespace VALX
{
    class VulkanSampler : public Sampler
    {
        SamplerInfo info = {};
        VkSampler sampler = VK_NULL_HANDLE;
    public:
        VulkanSampler(const SamplerInfo& info);

        virtual const SamplerInfo& GetInfo() const override;
        virtual Sampler::Handle GetHandle() const override;
        virtual ~VulkanSampler() override;
    };

    VkFilter ConvertFilter(Filter filter);
    VkSamplerMipmapMode ConvertMipMapFilter(Filter filter);
    VkSamplerAddressMode ConvertAddressMode(AddressMode addressMode);
    VkCompareOp ConvertCompareOp(CompareOp compare);
    VkBorderColor ConvertBorderColor(BorderColor border);
}