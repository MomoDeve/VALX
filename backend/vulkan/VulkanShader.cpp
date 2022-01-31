#include "VulkanShader.h"
#include "VulkanContext.h"
#include "ExternalFunctions.h"
#include "Utilities.h"
#include "api/Logger.h"

namespace VALX
{
    VulkanShader::VulkanShader(const ShaderInfo& info)
    {
        this->stages.resize(info.Stages.size());
        for (size_t i = 0; i < info.Stages.size(); i++)
        {
            const ShaderStageInfo& stageInfo = info.Stages[i];

            VkShaderModuleCreateInfo moduleCreateInfo = {};
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(stageInfo.Bytecode.data());
            moduleCreateInfo.codeSize = stageInfo.Bytecode.size() / sizeof(uint32_t);
            VALX_VK_SUCCESS(vkCreateShaderModule(GetVulkanContext()->GetDevice(), &moduleCreateInfo, nullptr, &this->stages[i].Module));
            this->stages[i].Stage = ConvertShaderStageVulkan(stageInfo.Stage);
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // TODO: initialize pipeline layout
        VALX_VK_SUCCESS(vkCreatePipelineLayout(GetVulkanContext()->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &this->pipelineLayout));
        this->name = info.Name;

        VkDebugUtilsObjectNameInfoEXT debugName = {};
        debugName.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        debugName.objectType = VK_OBJECT_TYPE_PIPELINE_LAYOUT;
        debugName.objectHandle = reinterpret_cast<uint64_t>(this->pipelineLayout);
        debugName.pObjectName = info.Name.c_str();
        funcs.vkSetDebugUtilsObjectNameEXT(GetVulkanContext()->GetDevice(), &debugName);

        GetCurrentLogger()->LogInfo("VulkanShader", fmt::format("shader `{}` created", info.Name));
    }

    const std::string& VulkanShader::GetName() const
    {
        return this->name;
    }

    Shader::Handle VulkanShader::GetHandle() const
    {
        return static_cast<Shader::Handle>(this->pipelineLayout);
    }

    VulkanShader::~VulkanShader()
    {
        for (const ShaderStage& stage : this->stages)
        {
            vkDestroyShaderModule(GetVulkanContext()->GetDevice(), stage.Module, nullptr);
        }
        vkDestroyPipelineLayout(GetVulkanContext()->GetDevice(), this->pipelineLayout, nullptr);

        GetCurrentLogger()->LogInfo("VulkanShader", fmt::format("shader `{}` destroyed", this->name));
    }

    VkShaderStageFlagBits ConvertShaderStageVulkan(ShaderStage stage)
    {
        switch (stage)
        {
        case VALX::ShaderStage::VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case VALX::ShaderStage::TESSELLATION_CONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case VALX::ShaderStage::TESSELLATION_EVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case VALX::ShaderStage::GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case VALX::ShaderStage::FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case VALX::ShaderStage::COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            VALX_ASSERT(false && "invalid shader stage");
            return VK_SHADER_STAGE_VERTEX_BIT;
        }
    }
}