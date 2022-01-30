#pragma once

#include "api/Format.h"
#include <vulkan/vulkan.h>

namespace VALX
{
    VkFormat ConvertFormatVulkan(Format format);
}