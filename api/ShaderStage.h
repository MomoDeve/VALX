#pragma once

#include <vector>

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
        std::vector<char> Bytecode;
    };
}