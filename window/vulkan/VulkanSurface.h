#pragma once

#include "api/Surface.h"
#include "api/Utilities.h"

#include <vulkan/vulkan.h>

namespace VALX
{
    class VulkanSurface : public Surface
    {
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        uint32_t width = 0;
        uint32_t height = 0;
    public:
        VulkanSurface(const class Window& window);
        ~VulkanSurface();

        VALX_NO_COPY_DEFAULT_MOVE(VulkanSurface);

        virtual Surface::Handle GetHandle() const override;
        virtual uint32_t GetWidth() const override;
        virtual uint32_t GetHeight() const override;
    };
}