#pragma once

#include "api/Shader.h"
#include <vulkan/vulkan.h>

namespace VALX
{
    class VulkanShader : public Shader
    {
        struct ShaderStage
        {
            VkShaderStageFlagBits Stage = {};
            VkShaderModule Module = VK_NULL_HANDLE;
        };
        std::string name;
        std::vector<ShaderStage> stages;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    public:
        VulkanShader(const ShaderInfo& info);

        virtual const std::string& GetName() const override;
        virtual Handle GetHandle() const override;
        virtual ~VulkanShader() override;
    };

    VkShaderStageFlagBits ConvertShaderStageVulkan(ShaderStage stage);
}