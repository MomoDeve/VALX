#pragma once

#include "ShaderStage.h"
#include <string>

namespace VALX
{
    enum class ShaderLanguage
    {
        GLSL,
        HLSL,
    };

    class ShaderLoader
    {
    public:
        virtual ShaderStageInfo LoadFromSourceFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language) = 0;
        virtual ShaderStageInfo LoadFromBinaryFile(const std::string& filepath, ShaderStage stage, ShaderLanguage language) = 0;
        virtual ShaderStageInfo LoadFromSourceString(const std::string& source, ShaderStage stage, ShaderLanguage language) = 0;

        virtual ~ShaderLoader() = default;
    };
}