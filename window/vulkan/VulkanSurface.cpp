#include "VulkanSurface.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window/Window.h"

#include "api/Logger.h"

#include "backend/vulkan/VulkanContext.h"
#include "backend/vulkan/Utilities.h"

namespace VALX
{
    VulkanSurface::VulkanSurface(const Window& window)
    {
        VulkanContext* context = GetVulkanContext();

        this->width = window.GetWidth();
        this->height = window.GetHeight();
        VALX_VK_SUCCESS(glfwCreateWindowSurface(context->GetInstance(), static_cast<GLFWwindow*>(window.GetHandle()), nullptr, &this->surface));

        VkBool32 isSupported = VK_FALSE;
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfaceSupportKHR(context->GetPhysicalDevice(), context->GetMainQueueFamilyIndex(), this->surface, &isSupported));
        int result = glfwGetPhysicalDevicePresentationSupport(context->GetInstance(), context->GetPhysicalDevice(), context->GetMainQueueFamilyIndex());
        VALX_ASSERT(result == GLFW_TRUE && isSupported && "surface must be supported by main queue");
        GetCurrentLogger()->LogInfo("VulkanSurface", "surface created");
    }

    VulkanSurface::~VulkanSurface()
    {
        VulkanContext* context = GetVulkanContext();
        vkDestroySurfaceKHR(context->GetInstance(), this->surface, nullptr);
        GetCurrentLogger()->LogInfo("VulkanSurface", "surface destroyed");
    }

    Surface::Handle VulkanSurface::GetHandle() const
    {
        return static_cast<Surface::Handle>(this->surface);
    }

    uint32_t VulkanSurface::GetWidth() const
    {
        return this->width;
    }

    uint32_t VulkanSurface::GetHeight() const
    {
        return this->height;
    }
}