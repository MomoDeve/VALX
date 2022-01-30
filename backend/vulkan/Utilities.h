#pragma once

#include "api/Utilities.h"
#include <vulkan/vulkan.h>

namespace VALX
{
    inline void CheckVulkanResult(VkResult result)
    {
        VALX_ASSERT(result == VK_SUCCESS);
    }

    #define VALX_VK_SUCCESS(...) CheckVulkanResult(__VA_ARGS__)
}