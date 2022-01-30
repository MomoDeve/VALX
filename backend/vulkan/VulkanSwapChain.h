#include "api/SwapChain.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace VALX
{
    class VulkanSwapChain : public SwapChain
    {
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        VkSurfaceFormatKHR surfaceFormat = {};
        VkExtent2D surfaceExtent = {};
        std::vector<VkImage> presentImages;

    public:
        VulkanSwapChain(const class Surface& surface);
        ~VulkanSwapChain();
        virtual void Recreate(uint32_t width, uint32_t height) override;
    };
}