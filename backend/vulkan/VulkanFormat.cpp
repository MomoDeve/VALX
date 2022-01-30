#include "VulkanFormat.h"
#include "Utilities.h"

namespace VALX
{
    VkFormat ConvertFormatVulkan(Format format)
    {
        switch (format)
        {
        case VALX::Format::UNKNOWN:
            return VK_FORMAT_UNDEFINED;
        case VALX::Format::R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case VALX::Format::R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;
        case VALX::Format::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case VALX::Format::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case VALX::Format::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case VALX::Format::D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case VALX::Format::D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        default:
            VALX_ASSERT(false && "invalid format");
            return VK_FORMAT_UNDEFINED;
        }
    }
}