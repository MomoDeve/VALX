#include "VulkanSwapChain.h"

#include "VulkanContext.h"
#include "Utilities.h"
#include "api/Logger.h"

namespace VALX
{
    VulkanSwapChain::VulkanSwapChain(const Surface& surface)
    {
        this->surface = static_cast<VkSurfaceKHR>(surface.GetHandle());
        this->Recreate(surface.GetWidth(), surface.GetHeight());
    }

    VulkanSwapChain::~VulkanSwapChain()
    {
        VulkanContext* context = GetVulkanContext();
        vkDestroySwapchainKHR(context->GetDevice(), this->swapchain, nullptr);
        GetCurrentLogger()->LogInfo("VulkanSwapChain", "swapchain destroyed");
    }

    void VulkanSwapChain::Recreate(uint32_t width, uint32_t height)
    {
        VulkanContext* context = GetVulkanContext();

        uint32_t presentModeCount = 0;
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(context->GetPhysicalDevice(), surface, &presentModeCount, nullptr));
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfacePresentModesKHR(context->GetPhysicalDevice(), surface, &presentModeCount, presentModes.data()));

        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->GetPhysicalDevice(), surface, &surfaceCapabilities));

        uint32_t surfaceFormatCount = 0;
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(context->GetPhysicalDevice(), surface, &surfaceFormatCount, nullptr));
        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        VALX_VK_SUCCESS(vkGetPhysicalDeviceSurfaceFormatsKHR(context->GetPhysicalDevice(), surface, &surfaceFormatCount, surfaceFormats.data()));

        this->presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end())
            this->presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

        // determine present image count
        uint32_t presentImageCount = surfaceCapabilities.maxImageCount > 0 ? surfaceCapabilities.maxImageCount : 4;

        // find best surface format
        this->surfaceFormat = surfaceFormats.front();
        for (const auto& format : surfaceFormats)
        {
            if (format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM)
               surfaceFormat = format;
        }

        this->surfaceExtent = VkExtent2D{ width, height };
        if (surfaceCapabilities.maxImageExtent.width != 0xFFFFFFFF && surfaceCapabilities.maxImageExtent.height != 0xFFFFFFFF)
        {
            this->surfaceExtent = surfaceCapabilities.maxImageExtent;
        }
        this->surfaceExtent = VkExtent2D{ std::max(this->surfaceExtent.width, 1u), std::max(this->surfaceExtent.height, 1u) };

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = this->surface;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.imageColorSpace = this->surfaceFormat.colorSpace;
        swapchainCreateInfo.imageFormat = this->surfaceFormat.format;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageExtent = this->surfaceExtent;
        swapchainCreateInfo.minImageCount = presentImageCount;
        swapchainCreateInfo.presentMode = this->presentMode;
        swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
        swapchainCreateInfo.clipped = true;
        swapchainCreateInfo.oldSwapchain = this->swapchain;

        VALX_VK_SUCCESS(vkCreateSwapchainKHR(context->GetDevice(), &swapchainCreateInfo, nullptr, &this->swapchain));
        GetCurrentLogger()->LogInfo("VulkanSwapChain", fmt::format("swapchain created ({}, {})", this->surfaceExtent.width, this->surfaceExtent.height));

        if (swapchainCreateInfo.oldSwapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(context->GetDevice(), swapchainCreateInfo.oldSwapchain, nullptr);

        uint32_t swapchainImageCount = 0;
        VALX_VK_SUCCESS(vkGetSwapchainImagesKHR(context->GetDevice(), this->swapchain, &swapchainImageCount, nullptr));
        this->presentImages.resize(swapchainImageCount);
        VALX_VK_SUCCESS(vkGetSwapchainImagesKHR(context->GetDevice(), this->swapchain, &swapchainImageCount, this->presentImages.data()));
    }
}