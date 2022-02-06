#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "Format.h"
#include "TextureType.h"

namespace VALX
{
    struct TextureData
    {
        struct MipData
        {
            std::vector<char> Bytes;
        };

        struct LayerData
        {
            std::vector<MipData> Mips;
        };

        struct InternalData
        {
            std::vector<LayerData> Layers;
        };

        std::string FilePath;
        uint32_t Width = 0;
        uint32_t Height = 0;
        uint32_t Depth = 0;
        uint32_t MipCount = 0;
        uint32_t Layers = 0;
        TextureType Type = TextureType::TEXTURE_2D;
        Format TextureFormat = Format::R8G8B8A8_UNORM;
        InternalData Data;
    };

    class TextureLoader
    {
    public:
        TextureData LoadTextureFromFile(const std::string& filepath);
        TextureData Convert2DTextureToCubeMap(const TextureData& texture);
    };
}