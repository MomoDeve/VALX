#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace VALX
{
    enum class ShaderStage
    {
        VERTEX,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
    };

    struct ShaderStageInfo
    {
        ShaderStage Stage = ShaderStage::VERTEX;
        std::vector<uint8_t> Bytecode;
    };

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