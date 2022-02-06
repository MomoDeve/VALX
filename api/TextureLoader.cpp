#include "TextureLoader.h"
#include "Utilities.h"
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

namespace VALX
{
    static bool IsDDSImage(const std::string& filepath)
    {
        return std::filesystem::path(filepath).extension() == ".dds";
    }

    static bool IsZLIBImage(const std::string& filepath)
    {
        return std::filesystem::path(filepath).extension() == ".zlib";
    }

    static TextureType ConvertTextureType(const tinyddsloader::DDSFile& dds)
    {
        if (dds.IsCubemap())
            return TextureType::TEXTURE_CUBE;
        if (dds.GetTextureDimension() == tinyddsloader::DDSFile::TextureDimension::Texture3D)
            return TextureType::TEXTURE_3D;
        
        return TextureType::TEXTURE_2D;
    }

    static Format ConvertDDSFormat(tinyddsloader::DDSFile::DXGIFormat format)
    {
        switch (format)
        {
        case tinyddsloader::DDSFile::DXGIFormat::Unknown:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_Float:
            return Format::R32G32B32A32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_UInt:
            return Format::R32G32B32A32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32A32_SInt:
            return Format::R32G32B32A32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_Float:
            return Format::R32G32B32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_UInt:
            return Format::R32G32B32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32B32_SInt:
            return Format::R32G32B32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float:
            return Format::R16G16B16A16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm:
            return Format::R16G16B16A16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UInt:
            return Format::R16G16B16A16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SNorm:
            return Format::R16G16B16A16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_SInt:
            return Format::R16G16B16A16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_Float:
            return Format::R32G32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_UInt:
            return Format::R32G32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G32_SInt:
            return Format::R32G32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32G8X24_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::D32_Float_S8X24_UInt:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Float_X8X24_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::X32_Typeless_G8X24_UInt:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UNorm:
            return Format::A2R10G10B10_UNORM_PACK32;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10A2_UInt:
            return Format::A2R10G10B10_UINT_PACK32;
        case tinyddsloader::DDSFile::DXGIFormat::R11G11B10_Float:
            return Format::B10G11R11_UFLOAT_PACK32;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
            return Format::R8G8B8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
            return Format::R8G8B8A8_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UInt:
            return Format::R8G8B8A8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SNorm:
            return Format::R8G8B8A8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_SInt:
            return Format::R8G8B8A8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_Float:
            return Format::R16G16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UNorm:
            return Format::R16G16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_UInt:
            return Format::R16G16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SNorm:
            return Format::R16G16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16G16_SInt:
            return Format::R16G16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::D32_Float:
            return Format::D32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_Float:
            return Format::R32_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_UInt:
            return Format::R32_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R32_SInt:
            return Format::R32_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R24G8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::D24_UNorm_S8_UInt:
            return Format::D24_UNORM_S8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R24_UNorm_X8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::X24_Typeless_G8_UInt:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UNorm:
            return Format::R8G8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_UInt:
            return Format::R8G8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SNorm:
            return Format::R8G8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_SInt:
            return Format::R8G8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R16_Float:
            return Format::R16_SFLOAT;
        case tinyddsloader::DDSFile::DXGIFormat::D16_UNorm:
            return Format::D16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UNorm:
            return Format::R16_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_UInt:
            return Format::R16_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SNorm:
            return Format::R16_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R16_SInt:
            return Format::R16_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UNorm:
            return Format::R8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_UInt:
            return Format::R8_UINT;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SNorm:
            return Format::R8_SNORM;
        case tinyddsloader::DDSFile::DXGIFormat::R8_SInt:
            return Format::R8_SINT;
        case tinyddsloader::DDSFile::DXGIFormat::A8_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R1_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R9G9B9E5_SHAREDEXP:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R8G8_B8G8_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::G8R8_G8B8_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm:
            return Format::BC1_RGB_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC1_UNorm_SRGB:
            return Format::BC1_RGB_SRGB_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm:
            return Format::BC2_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC2_UNorm_SRGB:
            return Format::BC2_SRGB_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm:
            return Format::BC3_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC3_UNorm_SRGB:
            return Format::BC3_SRGB_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_UNorm:
            return Format::BC4_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC4_SNorm:
            return Format::BC4_SNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_UNorm:
            return Format::BC5_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC5_SNorm:
            return Format::BC5_SNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::B5G6R5_UNorm:
            return Format::B5G6R5_UNORM_PACK16;
        case tinyddsloader::DDSFile::DXGIFormat::B5G5R5A1_UNorm:
            return Format::B5G5R5A1_UNORM_PACK16;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm:
            return Format::B8G8R8A8_UNORM;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::R10G10B10_XR_BIAS_A2_UNorm:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm_SRGB:
            return Format::B8G8R8A8_SRGB;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::B8G8R8X8_UNorm_SRGB:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_UF16:
            return Format::BC6H_UFLOAT_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC6H_SF16:
            return Format::BC6H_SFLOAT_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_Typeless:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm:
            return Format::BC7_UNORM_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::BC7_UNorm_SRGB:
            return Format::BC7_SRGB_BLOCK;
        case tinyddsloader::DDSFile::DXGIFormat::AYUV:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::Y410:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::Y416:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::NV12:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::P010:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::P016:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::YUV420_OPAQUE:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::YUY2:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::Y210:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::Y216:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::NV11:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::AI44:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::IA44:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::P8:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::A8P8:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::B4G4R4A4_UNorm:
            return Format::B4G4R4A4_UNORM_PACK16;
        case tinyddsloader::DDSFile::DXGIFormat::P208:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::V208:
            return Format::UNKNOWN;
        case tinyddsloader::DDSFile::DXGIFormat::V408:
            return Format::UNKNOWN;
        default:
            return Format::UNKNOWN;
        }
    }

    static TextureData LoadImageUsingDDSLoader(const std::string& filepath)
    {
        tinyddsloader::DDSFile dds;
        auto loadResult = dds.Load(filepath.c_str());
        if (loadResult != tinyddsloader::Result::Success)
            return TextureData{};

        dds.Flip();
        auto imageData = dds.GetImageData();

        TextureData result;
        result.FilePath = std::filesystem::absolute(filepath).string();
        result.Width = dds.GetWidth();
        result.Height = dds.GetHeight();
        result.Depth = dds.GetDepth();
        result.MipCount = dds.GetMipCount();
        result.Layers = dds.GetArraySize();
        result.TextureFormat = ConvertDDSFormat(dds.GetFormat());
        result.Depth = imageData->m_depth;
        result.Type = ConvertTextureType(dds);

        result.Data.Layers.resize(dds.GetArraySize());
        for (uint32_t layer = 0; layer < result.Data.Layers.size(); layer++)
        {
            TextureData::LayerData& layerData = result.Data.Layers[layer];
            layerData.Mips.resize(dds.GetMipCount());
            for (uint32_t mip = 0; mip < layerData.Mips.size(); mip++)
            {
                TextureData::MipData& mipData = layerData.Mips[mip];
                const tinyddsloader::DDSFile::ImageData* image = dds.GetImageData(mip, layer);
                mipData.Bytes.assign(static_cast<char*>(image->m_mem), static_cast<char*>(image->m_mem) + image->m_memSlicePitch);
            }
        }

        return result;
    }

    static std::string ConvertZLIBToDDS(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ifstream::binary);
        if (!file.good()) return filepath;

        std::vector<char> compressedData{
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };

        int decompressedSize = 0;
        char* decompressedData = stbi_zlib_decode_malloc(compressedData.data(), (int)compressedData.size(), &decompressedSize);

        std::filesystem::path path(filepath);
        auto ddsFilepath = (path.parent_path() / path.stem()).string() + ".dds";
        std::ofstream ddsFile(ddsFilepath, std::ofstream::binary);
        ddsFile.write(decompressedData, decompressedSize);
        free(decompressedData);

        return ddsFilepath;
    }

    static TextureData LoadImageUsingZLIBLoader(const std::string& filepath)
    {
        auto ddsFilepath = ConvertZLIBToDDS(filepath);
        return LoadImageUsingDDSLoader(ddsFilepath);
    }

    static TextureData LoadImageUsingSTBLoader(const std::string& filepath)
    {
        int width = 0, height = 0, channels = 0;

        stbi_set_flip_vertically_on_load(true);
        uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        stbi_set_flip_vertically_on_load(false);

        uint32_t dataSize = width * height * GetPixelByteSize(Format::R8G8B8A8_UNORM);

        TextureData result;
        result.FilePath = std::filesystem::absolute(filepath).string();
        result.Width = static_cast<uint32_t>(width);
        result.Height = static_cast<uint32_t>(height);
        result.Depth = 1;
        result.Layers = 1;
        result.MipCount = 1;
        result.Type = TextureType::TEXTURE_2D;
        result.TextureFormat = Format::R8G8B8A8_UNORM;
        result.Data.Layers.resize(1);
        result.Data.Layers[0].Mips.resize(1);
        result.Data.Layers[0].Mips[0].Bytes.assign(data, data + dataSize);
        stbi_image_free(data);
        return result;
    }

    static std::vector<char> ExtractCubemapFace(const std::vector<char>& bytes, uint32_t faceWidth, uint32_t faceHeight, Format format, uint32_t sliceX, uint32_t sliceY)
    {
        std::vector<char> result(bytes.size() / 6);
        const uint32_t pixelSize = GetPixelByteSize(format);
        const uint32_t sourceWidth = faceWidth * 4;
    
        for (uint32_t i = 0; i < faceHeight; i++)
        {
            uint32_t y = (faceHeight - i - 1) + sliceY * faceHeight;
            uint32_t x = sliceX * faceWidth;
            uint32_t bytesInRow = faceWidth * pixelSize;
    
            std::memcpy(result.data() + i * bytesInRow, bytes.data() + (y * sourceWidth + x) * pixelSize, bytesInRow);
        }
        return result;
    };

    TextureData TextureLoader::LoadTextureFromFile(const std::string& filepath)
    {
        if (IsDDSImage(filepath))
            return LoadImageUsingDDSLoader(filepath);
        else if (IsZLIBImage(filepath))
            return LoadImageUsingZLIBLoader(filepath);
        else
            return LoadImageUsingSTBLoader(filepath);
    }

    TextureData TextureLoader::Convert2DTextureToCubeMap(const TextureData& texture)
    {
        VALX_ASSERT(texture.Type == TextureType::TEXTURE_2D);
        TextureData result;
        result.FilePath = texture.FilePath;
        result.Width = texture.Width / 4;
        result.Height = texture.Height / 3;
        result.Depth = texture.Depth;
        result.Layers = texture.Layers * 6;
        result.MipCount = texture.MipCount;
        result.TextureFormat = texture.TextureFormat;
        result.Type = TextureType::TEXTURE_CUBE;

        VALX_ASSERT(result.Width == result.Height);

        result.Data.Layers.resize(result.Layers);
        for (uint32_t layer = 0; layer < texture.Layers; layer++)
        {
            for (uint32_t face = 0; face < 6; face++)
            {
                result.Data.Layers[6 * layer + face].Mips.resize(result.MipCount);
            }

            const TextureData::LayerData& layerData = texture.Data.Layers[layer];
            for (uint32_t mip = 0; mip < texture.MipCount; mip++)
            {
                const TextureData::MipData& mipData = layerData.Mips[mip];
                uint32_t width = std::max(result.Width / (1 << mip), 1u);
                uint32_t height = std::max(result.Height / (1 << mip), 1u);

                result.Data.Layers[6 * layer + 0].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 2, 1);
                result.Data.Layers[6 * layer + 1].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 0, 1);
                result.Data.Layers[6 * layer + 2].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 1, 2);
                result.Data.Layers[6 * layer + 3].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 1, 0);
                result.Data.Layers[6 * layer + 4].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 1, 1);
                result.Data.Layers[6 * layer + 5].Mips[mip].Bytes = ExtractCubemapFace(mipData.Bytes, width, height, result.TextureFormat, 3, 1);
            }
        }

        return result;
    }
}