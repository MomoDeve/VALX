#pragma once

#include "Utilities.h"
#include "Format.h"

#include <string>

namespace VALX
{
    enum class TextureType
    {
        TEXTURE_2D,
        TEXTURE_3D,
        TEXTURE_CUBE,
    };

    enum class SampleCount
    {
        SAMPLES_1,
        SAMPLES_2,
        SAMPLES_4,
        SAMPLES_8,
        SAMPLES_16,
        SAMPLES_32,
        SAMPLES_64,
    };

    enum class TextureFlags
    {
        NONE = 0,
        COPY_SRC = 1 << 0,
        COPY_DST = 1 << 1,
        SAMPLED = 1 << 2,
        STORAGE = 1 << 3,
        COLOR_ATTACHMENT = 1 << 4,
        DEPTH_STENCIL_ATTACHMENT = 1 << 5,
    };
    VALX_GENERATE_ENUM_OPS(TextureFlags)

    constexpr uint32_t ALL_MIPS = 0;

    struct TextureInfo
    {
        std::string Name = "";
        TextureType Type = TextureType::TEXTURE_2D;
        Format TextureFormat = Format::UNKNOWN;
        SampleCount Samples = SampleCount::SAMPLES_1;
        TextureFlags Flags = TextureFlags::NONE;
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 1;
        uint32_t Layers = 1;
        uint32_t Mips = 1;
    };

    class Texture
    {
    public:
        using Handle = void*;

        virtual const TextureInfo& GetInfo() const = 0;
        virtual Texture::Handle GetHandle() const = 0;
        virtual ~Texture() = default;
    };
}