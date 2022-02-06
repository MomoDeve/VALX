#pragma once

#include <cstdint>
#include <string>

#include "ShaderStage.h"

namespace VALX
{
    struct ShaderInfo
    {
        std::string Name;
        std::vector<ShaderStageInfo> Stages;
    };

    class Shader
    {
    public:
        using Handle = void*;

        virtual const std::string& GetName() const = 0;
        virtual Shader::Handle GetHandle() const = 0;
        virtual ~Shader() = default;
    };
}