#pragma once

#include "api/Context.h"
#include "api/Utilities.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace VALX
{
    class VulkanContext : public Context
    {
        VkInstance instance = VK_NULL_HANDLE;
        uint32_t apiVersion = {};

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties physicalDeviceProperties = {};

        VkDevice device = VK_NULL_HANDLE;

        VkQueue mainQueue = VK_NULL_HANDLE;
        uint32_t mainQueueFamilyIndex = 0;
        VkQueue computeQueue = VK_NULL_HANDLE;
        uint32_t computeQueueFamilyIndex = 0;
        std::vector<VkQueue> transferQueues;
        uint32_t transferQueueFamilyIndex = 0;

        VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;

        VmaAllocator allocator = nullptr;

        std::unique_ptr<ShaderLoader> shaderLoader = nullptr;
        std::unique_ptr<TextureLoader> textureLoader = nullptr;
    public:
        VulkanContext(const ContextCreateInfo& info);
        ~VulkanContext();

        virtual TextureLoader* GetTextureLoader() override;
        virtual ShaderLoader* GetShaderLoader() override;

        virtual std::unique_ptr<Surface> CreateSurface(const class Window& window) override;
        virtual std::unique_ptr<SwapChain> CreateSwapChain(const Surface& surface) override;
        virtual std::unique_ptr<Texture> CreateTexture(const TextureInfo& info) override;
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferInfo& info) override;
        virtual std::unique_ptr<Shader> CreateShader(const ShaderInfo& info) override;
        virtual std::unique_ptr<Sampler> CreateSampler(const SamplerInfo& info) override;
        
        VALX_NO_COPY_NO_MOVE(VulkanContext);

        VkInstance GetInstance() const;
        uint32_t GetAPIVersion() const;
        VkPhysicalDevice GetPhysicalDevice() const;
        VkDevice GetDevice() const;
        VmaAllocator GetAllocator() const;
        VkQueue GetMainQueue() const;
        VkQueue GetComputeQueue() const;
        size_t GetTransferQueueCount() const;
        VkQueue GetTransferQueue(size_t index) const;
        uint32_t GetMainQueueFamilyIndex() const;
        uint32_t GetComputeQueueFamilyIndex() const;
        uint32_t GetTransferQueueFamilyIndex() const;
    };

    VulkanContext* GetVulkanContext();

    VkPhysicalDeviceType ConvertDeviceTypeVulkan(DeviceType type);
}