#pragma once

#include <vulkan/vulkan.h>

namespace VALX
{
    struct ExternalFunctions
    {
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
    };

    extern ExternalFunctions funcs;
}