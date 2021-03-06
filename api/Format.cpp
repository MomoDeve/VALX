#include "Format.h"
#include "Utilities.h"

namespace VALX
{
    uint32_t GetPixelByteSize(Format format)
    {
        switch (format)
        {
        case VALX::Format::UNKNOWN:
            return 0;
        case VALX::Format::R4G4_UNORM_PACK8:
            return 1;
        case VALX::Format::R4G4B4A4_UNORM_PACK16:
            return 2;
        case VALX::Format::B4G4R4A4_UNORM_PACK16:
            return 2;
        case VALX::Format::R5G6B5_UNORM_PACK16:
            return 2;
        case VALX::Format::B5G6R5_UNORM_PACK16:
            return 2;
        case VALX::Format::R5G5B5A1_UNORM_PACK16:
            return 2;
        case VALX::Format::B5G5R5A1_UNORM_PACK16:
            return 2;
        case VALX::Format::A1R5G5B5_UNORM_PACK16:
            return 2;
        case VALX::Format::R8_UNORM:
            return 1;
        case VALX::Format::R8_SNORM:
            return 1;
        case VALX::Format::R8_USCALED:
            return 1;
        case VALX::Format::R8_SSCALED:
            return 1;
        case VALX::Format::R8_UINT:
            return 1;
        case VALX::Format::R8_SINT:
            return 1;
        case VALX::Format::R8_SRGB:
            return 1;
        case VALX::Format::R8G8_UNORM:
            return 2;
        case VALX::Format::R8G8_SNORM:
            return 2;
        case VALX::Format::R8G8_USCALED:
            return 2;
        case VALX::Format::R8G8_SSCALED:
            return 2;
        case VALX::Format::R8G8_UINT:
            return 2;
        case VALX::Format::R8G8_SINT:
            return 2;
        case VALX::Format::R8G8_SRGB:
            return 2;
        case VALX::Format::R8G8B8_UNORM:
            return 3;
        case VALX::Format::R8G8B8_SNORM:
            return 3;
        case VALX::Format::R8G8B8_USCALED:
            return 3;
        case VALX::Format::R8G8B8_SSCALED:
            return 3;
        case VALX::Format::R8G8B8_UINT:
            return 3;
        case VALX::Format::R8G8B8_SINT:
            return 3;
        case VALX::Format::R8G8B8_SRGB:
            return 3;
        case VALX::Format::B8G8R8_UNORM:
            return 3;
        case VALX::Format::B8G8R8_SNORM:
            return 3;
        case VALX::Format::B8G8R8_USCALED:
            return 3;
        case VALX::Format::B8G8R8_SSCALED:
            return 3;
        case VALX::Format::B8G8R8_UINT:
            return 3;
        case VALX::Format::B8G8R8_SINT:
            return 3;
        case VALX::Format::B8G8R8_SRGB:
            return 3;
        case VALX::Format::R8G8B8A8_UNORM:
            return 4;
        case VALX::Format::R8G8B8A8_SNORM:
            return 4;
        case VALX::Format::R8G8B8A8_USCALED:
            return 4;
        case VALX::Format::R8G8B8A8_SSCALED:
            return 4;
        case VALX::Format::R8G8B8A8_UINT:
            return 4;
        case VALX::Format::R8G8B8A8_SINT:
            return 4;
        case VALX::Format::R8G8B8A8_SRGB:
            return 4;
        case VALX::Format::B8G8R8A8_UNORM:
            return 4;
        case VALX::Format::B8G8R8A8_SNORM:
            return 4;
        case VALX::Format::B8G8R8A8_USCALED:
            return 4;
        case VALX::Format::B8G8R8A8_SSCALED:
            return 4;
        case VALX::Format::B8G8R8A8_UINT:
            return 4;
        case VALX::Format::B8G8R8A8_SINT:
            return 4;
        case VALX::Format::B8G8R8A8_SRGB:
            return 4;
        case VALX::Format::A8B8G8R8_UNORM_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_SNORM_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_USCALED_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_SSCALED_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_UINT_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_SINT_PACK32:
            return 4;
        case VALX::Format::A8B8G8R8_SRGB_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_UNORM_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_SNORM_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_USCALED_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_SSCALED_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_UINT_PACK32:
            return 4;
        case VALX::Format::A2R10G10B10_SINT_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_UNORM_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_SNORM_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_USCALED_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_SSCALED_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_UINT_PACK32:
            return 4;
        case VALX::Format::A2B10G10R10_SINT_PACK32:
            return 4;
        case VALX::Format::R16_UNORM:
            return 2;
        case VALX::Format::R16_SNORM:
            return 2;
        case VALX::Format::R16_USCALED:
            return 2;
        case VALX::Format::R16_SSCALED:
            return 2;
        case VALX::Format::R16_UINT:
            return 2;
        case VALX::Format::R16_SINT:
            return 2;
        case VALX::Format::R16_SFLOAT:
            return 2;
        case VALX::Format::R16G16_UNORM:
            return 4;
        case VALX::Format::R16G16_SNORM:
            return 4;
        case VALX::Format::R16G16_USCALED:
            return 4;
        case VALX::Format::R16G16_SSCALED:
            return 4;
        case VALX::Format::R16G16_UINT:
            return 4;
        case VALX::Format::R16G16_SINT:
            return 4;
        case VALX::Format::R16G16_SFLOAT:
            return 4;
        case VALX::Format::R16G16B16_UNORM:
            return 6;
        case VALX::Format::R16G16B16_SNORM:
            return 6;
        case VALX::Format::R16G16B16_USCALED:
            return 6;
        case VALX::Format::R16G16B16_SSCALED:
            return 6;
        case VALX::Format::R16G16B16_UINT:
            return 6;
        case VALX::Format::R16G16B16_SINT:
            return 6;
        case VALX::Format::R16G16B16_SFLOAT:
            return 6;
        case VALX::Format::R16G16B16A16_UNORM:
            return 8;
        case VALX::Format::R16G16B16A16_SNORM:
            return 8;
        case VALX::Format::R16G16B16A16_USCALED:
            return 8;
        case VALX::Format::R16G16B16A16_SSCALED:
            return 8;
        case VALX::Format::R16G16B16A16_UINT:
            return 8;
        case VALX::Format::R16G16B16A16_SINT:
            return 8;
        case VALX::Format::R16G16B16A16_SFLOAT:
            return 8;
        case VALX::Format::R32_UINT:
            return 4;
        case VALX::Format::R32_SINT:
            return 4;
        case VALX::Format::R32_SFLOAT:
            return 4;
        case VALX::Format::R32G32_UINT:
            return 8;
        case VALX::Format::R32G32_SINT:
            return 8;
        case VALX::Format::R32G32_SFLOAT:
            return 8;
        case VALX::Format::R32G32B32_UINT:
            return 12;
        case VALX::Format::R32G32B32_SINT:
            return 12;
        case VALX::Format::R32G32B32_SFLOAT:
            return 12;
        case VALX::Format::R32G32B32A32_UINT:
            return 16;
        case VALX::Format::R32G32B32A32_SINT:
            return 16;
        case VALX::Format::R32G32B32A32_SFLOAT:
            return 16;
        case VALX::Format::R64_UINT:
            return 8;
        case VALX::Format::R64_SINT:
            return 8;
        case VALX::Format::R64_SFLOAT:
            return 8;
        case VALX::Format::R64G64_UINT:
            return 16;
        case VALX::Format::R64G64_SINT:
            return 16;
        case VALX::Format::R64G64_SFLOAT:
            return 16;
        case VALX::Format::R64G64B64_UINT:
            return 24;
        case VALX::Format::R64G64B64_SINT:
            return 24;
        case VALX::Format::R64G64B64_SFLOAT:
            return 24;
        case VALX::Format::R64G64B64A64_UINT:
            return 32;
        case VALX::Format::R64G64B64A64_SINT:
            return 32;
        case VALX::Format::R64G64B64A64_SFLOAT:
            return 32;
        case VALX::Format::B10G11R11_UFLOAT_PACK32:
            return 4;
        case VALX::Format::E5B9G9R9_UFLOAT_PACK32:
            return 4;
        case VALX::Format::D16_UNORM:
            return 2;
        case VALX::Format::X8_D24_UNORM_PACK32:
            return 4;
        case VALX::Format::D32_SFLOAT:
            return 4;
        case VALX::Format::S8_UINT:
            return 1;
        case VALX::Format::D16_UNORM_S8_UINT:
            return 3;
        case VALX::Format::D24_UNORM_S8_UINT:
            return 4;
        case VALX::Format::D32_SFLOAT_S8_UINT:
            return 5;
        case VALX::Format::BC1_RGB_UNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC1_RGB_SRGB_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC1_RGBA_UNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC1_RGBA_SRGB_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC2_UNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC2_SRGB_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC3_UNORM_BLOCK:
            break;
        case VALX::Format::BC3_SRGB_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC4_UNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC4_SNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC5_UNORM_BLOCK:
            break;
        case VALX::Format::BC5_SNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC6H_UFLOAT_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC6H_SFLOAT_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC7_UNORM_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        case VALX::Format::BC7_SRGB_BLOCK:
            VALX_ASSERT(false && "compressed format");
            return 0;
        default:
            VALX_ASSERT(false && "invalid format");
            return 0;
        }
    }
}