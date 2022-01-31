#include "VulkanShader.h"
#include "VulkanContext.h"
#include "ExternalFunctions.h"
#include "Utilities.h"
#include "api/Hash.h"
#include "api/Logger.h"

#include <spirv_reflect.h>
#include <unordered_map>
#include <algorithm>

// include only once!
#include <spirv_reflect.c>

namespace VALX
{
    inline void CheckSpirvReflectResult(SpvReflectResult result)
    {
        VALX_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);
    }

    #define VALX_SPV_SUCCESS(...) CheckSpirvReflectResult(__VA_ARGS__)

    struct ReflectionInfo
    {
        std::vector<VkDescriptorSetLayout> DescriptorSetLayouts;
        VkPushConstantRange PushConstantRange = {};
    };

    struct SetBinding
    {
        uint32_t Set = 0;
        uint32_t Binding = 0;
    };

    struct SetBindingHash
    {
        size_t operator()(const SetBinding& b) const
        {
            size_t hash = 0;
            HashCombine(hash, b.Set);
            HashCombine(hash, b.Binding);
            return hash;
        }
    };

    struct SetBindingEqual
    {
        bool operator()(const SetBinding& b1, const SetBinding& b2) const
        {
            return b1.Set == b2.Set && b1.Binding == b2.Binding;
        }
    };

    static ReflectionInfo GenerateLayoutFromReflection(const std::vector<ShaderStageInfo>& stages)
    {
        std::unordered_map<SetBinding, VkDescriptorSetLayoutBinding, SetBindingHash, SetBindingEqual> bindings;
        uint32_t maxSetIndex = 0;
        uint32_t maxPushConstantSize = 0;
        VkShaderStageFlags pushConstantStages = {};

        for (const ShaderStageInfo& stage : stages)
        {
            SpvReflectShaderModule module = {};
            VALX_SPV_SUCCESS(spvReflectCreateShaderModule(stage.Bytecode.size(), static_cast<const void*>(stage.Bytecode.data()), &module));

            uint32_t descriptorBindingCount = 0;
            VALX_SPV_SUCCESS(spvReflectEnumerateDescriptorBindings(&module, &descriptorBindingCount, nullptr));
            std::vector<SpvReflectDescriptorBinding*> descriptorBindings(descriptorBindingCount);
            VALX_SPV_SUCCESS(spvReflectEnumerateDescriptorBindings(&module, &descriptorBindingCount, descriptorBindings.data()));

            for (const SpvReflectDescriptorBinding* descriptorBinding : descriptorBindings)
            {
                VkDescriptorSetLayoutBinding binding = {};
                binding.binding = descriptorBinding->binding;
                binding.descriptorCount = descriptorBinding->count;
                binding.descriptorType = static_cast<VkDescriptorType>(descriptorBinding->descriptor_type);
                binding.stageFlags = ConvertShaderStageVulkan(stage.Stage);

                maxSetIndex = std::max(maxSetIndex, descriptorBinding->set);

                auto it = bindings.find({ descriptorBinding->set, descriptorBinding->binding });
                if (it == bindings.end())
                {
                    bindings[{ descriptorBinding->set, descriptorBinding->binding }] = binding;
                }
                else
                {
                    VALX_ASSERT(it->second.binding == binding.binding);
                    VALX_ASSERT(it->second.descriptorCount == binding.descriptorCount);
                    VALX_ASSERT(it->second.descriptorType == binding.descriptorType);
                    it->second.stageFlags |= binding.stageFlags;
                }
            }

            uint32_t pushConstantCount = 0;
            VALX_SPV_SUCCESS(spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, nullptr));
            std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
            VALX_SPV_SUCCESS(spvReflectEnumeratePushConstantBlocks(&module, &pushConstantCount, pushConstants.data()));

            uint32_t pushConstantSize = 0;
            for (const SpvReflectBlockVariable* pushConstant : pushConstants)
            {
                pushConstantSize += pushConstant->padded_size;
            }
            VALX_ASSERT(maxPushConstantSize == 0 || maxPushConstantSize == pushConstantSize);
            maxPushConstantSize = std::max(maxPushConstantSize, pushConstantSize);
            pushConstantStages |= ConvertShaderStageVulkan(stage.Stage);

            spvReflectDestroyShaderModule(&module);
        }

        std::vector<std::vector<VkDescriptorSetLayoutBinding>> bindingsPerSet(maxSetIndex + 1);
        for (const auto& [setBinding, binding] : bindings)
        {
            bindingsPerSet[setBinding.Set].push_back(binding);
        }

        ReflectionInfo reflection;
        reflection.DescriptorSetLayouts.resize(maxSetIndex + 1);
        for (size_t i = 0; i < reflection.DescriptorSetLayouts.size(); i++)
        {
            VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
            descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindingsPerSet[i].size());
            descriptorSetLayoutCreateInfo.pBindings = bindingsPerSet[i].data();
            VALX_VK_SUCCESS(vkCreateDescriptorSetLayout(GetVulkanContext()->GetDevice(), &descriptorSetLayoutCreateInfo, nullptr, &reflection.DescriptorSetLayouts[i]));
        }

        reflection.PushConstantRange = {};
        reflection.PushConstantRange.stageFlags = pushConstantStages;
        reflection.PushConstantRange.size = maxPushConstantSize;
        reflection.PushConstantRange.offset = 0;

        return reflection;
    }

    VulkanShader::VulkanShader(const ShaderInfo& info)
    {
        this->stages.resize(info.Stages.size());
        for (size_t i = 0; i < info.Stages.size(); i++)
        {
            const ShaderStageInfo& stageInfo = info.Stages[i];

            VkShaderModuleCreateInfo moduleCreateInfo = {};
            moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(stageInfo.Bytecode.data());
            moduleCreateInfo.codeSize = stageInfo.Bytecode.size();
            VALX_VK_SUCCESS(vkCreateShaderModule(GetVulkanContext()->GetDevice(), &moduleCreateInfo, nullptr, &this->stages[i].Module));
            this->stages[i].Stage = ConvertShaderStageVulkan(stageInfo.Stage);
        }

        ReflectionInfo reflection = GenerateLayoutFromReflection(info.Stages);

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(reflection.DescriptorSetLayouts.size());
        pipelineLayoutCreateInfo.pSetLayouts = reflection.DescriptorSetLayouts.data();
        pipelineLayoutCreateInfo.pushConstantRangeCount = reflection.PushConstantRange.size > 0 ? 1 : 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = &reflection.PushConstantRange;
        VALX_VK_SUCCESS(vkCreatePipelineLayout(GetVulkanContext()->GetDevice(), &pipelineLayoutCreateInfo, nullptr, &this->pipelineLayout));
        this->name = info.Name;
        this->descriptorSetLayouts = std::move(reflection.DescriptorSetLayouts);

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

        for (const VkDescriptorSetLayout layout : this->descriptorSetLayouts)
        {
            vkDestroyDescriptorSetLayout(GetVulkanContext()->GetDevice(), layout, nullptr);
        }

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