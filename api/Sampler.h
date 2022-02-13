#pragma once

#include <climits>
#include <string>

namespace VALX
{
    enum class Filter
    {
        NEAREST,
        LINEAR,
    };

    enum class AddressMode
    {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER,
    };

    enum class CompareOp
    {
        NEVER,
        LESS,
        EQUAL,
        LESS_OR_EQUAL,
        GREATER,
        NOT_EQUAL,
        GREATER_OR_EQUAL,
        ALWAYS,
    };

    enum class BorderColor
    {
        FLOAT_TRANSPARENT_BLACK,
        INT_TRANSPARENT_BLACK,
        FLOAT_OPAQUE_BLACK,
        INT_OPAQUE_BLACK,
        FLOAT_OPAQUE_WHITE,
        INT_OPAQUE_WHITE,
    };

    struct SamplerInfo
    {
        std::string Name;
        Filter MagFilter = Filter::NEAREST;
        Filter MinFilter = Filter::NEAREST;
        Filter MipMapFilter = Filter::NEAREST;
        AddressMode AddressModeU = AddressMode::REPEAT;
        AddressMode AddressModeV = AddressMode::REPEAT;
        AddressMode AddressModeW = AddressMode::REPEAT;
        float MipLodBias = 0.0f;
        bool EnableAnisotropy = true;
        float MaxAnisotropy = 1.0f;
        CompareOp Compare = CompareOp::NEVER;
        float MinLod = 0.0f;
        float MaxLod = FLT_MAX;
        BorderColor Border = BorderColor::FLOAT_OPAQUE_BLACK;
    };

    class Sampler
    {
    public:
        using Handle = void*;

        virtual const SamplerInfo& GetInfo() const = 0;
        virtual Sampler::Handle GetHandle() const = 0;
        virtual ~Sampler() = default;
    };
}