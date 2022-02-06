#pragma once

#include "api/ShaderLoader.h"

namespace VALX
{
    class VulkanShaderLoader : public ShaderLoader
    {
    public:
        virtual ShaderStageInfo LoadFromSourceFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language) override;
        virtual ShaderStageInfo LoadFromBinaryFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language) override;
        virtual ShaderStageInfo LoadFromSourceString(const std::string& source, ShaderStage stage, ShaderLanguage language) override;
    };
}