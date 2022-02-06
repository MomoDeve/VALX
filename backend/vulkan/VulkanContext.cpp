#include "backend/vulkan/VulkanContext.h"

#include <array>
#include <algorithm>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#undef VMA_IMPLEMENTATION

#include <vulkan/vk_enum_string_helper.h>
#include <ShaderLang.h>

#include "Utilities.h"
#include "ExternalFunctions.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandBuffer.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanShader.h"
#include "VulkanShaderLoader.h"
#include "window/Window.h"
#include "window/vulkan/VulkanSurface.h"
#include "api/Logger.h"

namespace VALX
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            GetCurrentLogger()->LogError("Vulkan Validation", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            GetCurrentLogger()->LogWarning("Vulkan Validation", pCallbackData->pMessage);
        }
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            GetCurrentLogger()->LogInfo("Vulkan Validation", pCallbackData->pMessage);
        }
        return VK_FALSE;
    }

    struct DeviceQueues
    {
        VkDeviceQueueCreateInfo MainQueueFamily = {};
        std::vector<float> MainQueuePriorities;
        VkDeviceQueueCreateInfo ComputeQueueFamily = {};
        std::vector<float> ComputeQueuePriorities;
        VkDeviceQueueCreateInfo TransferQueueFamily = {};
        std::vector<float> TransferQueuePriorities;
    };

    static DeviceQueues CollectDeviceQueues(VkPhysicalDevice physicalDevice, const VkPhysicalDeviceProperties& properties)
    {
        DeviceQueues deviceQueues;
        bool mainFamilyFound = false;
        bool computeFamilyFound = false;
        bool transferFamilyFound = false;

        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

        uint32_t queueFamilyIndex = 0;
        for (const VkQueueFamilyProperties& properties : queueFamilyProperties)
        {
            // main queue is the queue that supports both graphic and compute commands
            if (!mainFamilyFound && (properties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
            {
                // here we try to create 4 queues (1 main + 1 compute + 2 transfer) in case there are no separate compute/transfer queue families
                deviceQueues.MainQueueFamily = {};
                deviceQueues.MainQueueFamily.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                deviceQueues.MainQueueFamily.queueCount = std::min(4u, properties.queueCount);
                deviceQueues.MainQueueFamily.queueFamilyIndex = queueFamilyIndex;

                deviceQueues.MainQueuePriorities.assign(deviceQueues.MainQueueFamily.queueCount, 1.0f);
                deviceQueues.MainQueueFamily.pQueuePriorities = deviceQueues.MainQueuePriorities.data();

                mainFamilyFound = true;
            }
            // compute queue is the queue that supports compute commands, but does not support graphic commands
            else if (!computeFamilyFound && (properties.queueFlags & VK_QUEUE_COMPUTE_BIT) && !(properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                // here we require only one compute queue as we will have one main render queue and one async compute queue
                deviceQueues.ComputeQueueFamily = {};
                deviceQueues.ComputeQueueFamily.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                deviceQueues.ComputeQueueFamily.queueCount = 1u;
                deviceQueues.ComputeQueueFamily.queueFamilyIndex = queueFamilyIndex;
                
                deviceQueues.ComputeQueuePriorities.assign(deviceQueues.ComputeQueueFamily.queueCount, 1.0f);
                deviceQueues.ComputeQueueFamily.pQueuePriorities = deviceQueues.ComputeQueuePriorities.data();

                computeFamilyFound = true;
            }
            // transfer queue is the queue which supports transfer commands but does not support graphic or compute commands
            else if (!transferFamilyFound && (properties.queueFlags & VK_QUEUE_TRANSFER_BIT) && !(properties.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)))
            {
                // here we require as many async transfer queues as possible as they will be used for async resource loading
                deviceQueues.TransferQueueFamily = {};
                deviceQueues.TransferQueueFamily.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                deviceQueues.TransferQueueFamily.queueCount = properties.queueCount;
                deviceQueues.TransferQueueFamily.queueFamilyIndex = queueFamilyIndex;

                deviceQueues.TransferQueuePriorities.assign(deviceQueues.TransferQueueFamily.queueCount, 1.0f);
                deviceQueues.TransferQueueFamily.pQueuePriorities = deviceQueues.TransferQueuePriorities.data();

                transferFamilyFound = true;
            }
            queueFamilyIndex++;
        }

        VALX_ASSERT(deviceQueues.MainQueueFamily.queueCount > 0 && "cannot find main queue");
        return deviceQueues;
    }

    VulkanContext::VulkanContext(const ContextCreateInfo& info)
    {
        // VkInstance creation
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, info.APIMajorVersion, info.APIMinorVersion, 0);
        applicationInfo.pApplicationName = info.ApplicationName.c_str();
        applicationInfo.pEngineName = info.EngineName.c_str();

        this->apiVersion = applicationInfo.apiVersion;

        std::vector<const char*> extensions;
        for (const std::string& extension : info.Extensions)
        {
            extensions.push_back(extension.c_str());
        }
        if (info.UseValidationLayer)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        std::vector<const char*> layers;
        if (info.UseValidationLayer)
        {
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledLayerNames = layers.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());

        VALX_VK_SUCCESS(vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance));
        GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("vulkan {}.{} context created", info.APIMajorVersion, info.APIMinorVersion));

        // VkPhysicalDevice selection
        uint32_t physicalDeviceCount = 0;
        VALX_VK_SUCCESS(vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr));
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        VALX_VK_SUCCESS(vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data()));

        for (const VkPhysicalDevice& device : physicalDevices)
        {
            VkPhysicalDeviceProperties properties = {};
            vkGetPhysicalDeviceProperties(device, &properties);
            if (properties.apiVersion < this->apiVersion)
            {
                GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("skipping device {}, API version too old", properties.deviceName));
                continue; // too old API version
            }
            if (info.PreferredDeviceType != DeviceType::ANY && properties.deviceType != ConvertDeviceTypeVulkan(info.PreferredDeviceType))
            {
                GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("skipping device {}, as its type is ", string_VkPhysicalDeviceType(properties.deviceType)));
                continue; // not preferred device
            }
            this->physicalDevice = device;
            this->physicalDeviceProperties = properties;
        }
        VALX_ASSERT(this->physicalDevice != VK_NULL_HANDLE && "no suitable physical device");
        GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("selected device: {}", this->physicalDeviceProperties.deviceName));

        // VkDevice creation
        DeviceQueues deviceQueues = CollectDeviceQueues(this->physicalDevice, this->physicalDeviceProperties);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        if (deviceQueues.MainQueueFamily.queueCount > 0)
            queueCreateInfos.push_back(deviceQueues.MainQueueFamily);
        if (deviceQueues.ComputeQueueFamily.queueCount > 0)
            queueCreateInfos.push_back(deviceQueues.ComputeQueueFamily);
        if (deviceQueues.TransferQueueFamily.queueCount > 0)
            queueCreateInfos.push_back(deviceQueues.TransferQueueFamily);

        std::vector<const char*> deviceExtensions;
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        deviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
        deviceExtensions.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {};
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;
        descriptorIndexingFeatures.shaderUniformBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageImageArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderInputAttachmentArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderUniformTexelBufferArrayNonUniformIndexing = true;
        descriptorIndexingFeatures.shaderStorageTexelBufferArrayNonUniformIndexing = true;

        VkPhysicalDeviceMultiviewFeatures multiviewFeatures = {};
        multiviewFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
        multiviewFeatures.multiview = true;
        multiviewFeatures.pNext = &descriptorIndexingFeatures;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &multiviewFeatures;
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

        VALX_VK_SUCCESS(vkCreateDevice(this->physicalDevice, &deviceCreateInfo, nullptr, &this->device));
        GetCurrentLogger()->LogInfo("VulkanContext", "device created");

        // external functions
        funcs.vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkCreateDebugUtilsMessengerEXT");
        funcs.vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, "vkDestroyDebugUtilsMessengerEXT");
        funcs.vkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(this->device, "vkSetDebugUtilsObjectNameEXT");

        // VkQueue selection
        if (deviceQueues.MainQueueFamily.queueCount > 0)
        {
            this->mainQueueFamilyIndex = deviceQueues.MainQueueFamily.queueFamilyIndex;
            vkGetDeviceQueue(this->device, this->mainQueueFamilyIndex, 0, &this->mainQueue);
        }
        if (deviceQueues.ComputeQueueFamily.queueCount > 0)
        {
            this->computeQueueFamilyIndex = deviceQueues.ComputeQueueFamily.queueFamilyIndex;
            vkGetDeviceQueue(this->device, this->computeQueueFamilyIndex, 0, &this->computeQueue);
        }
        if (deviceQueues.TransferQueueFamily.queueCount > 0)
        {
            this->transferQueueFamilyIndex = deviceQueues.TransferQueueFamily.queueFamilyIndex;
            this->transferQueues.resize(deviceQueues.TransferQueueFamily.queueCount);
            for (uint32_t i = 0; i < this->transferQueues.size(); i++)
            {
                vkGetDeviceQueue(this->device, this->transferQueueFamilyIndex, i, &this->transferQueues[i]);
            }
        }
        // case when we only have multiple unified queues
        if (this->computeQueue == VK_NULL_HANDLE && deviceQueues.MainQueueFamily.queueCount > 1)
        {
            this->computeQueueFamilyIndex = deviceQueues.MainQueueFamily.queueFamilyIndex;
            vkGetDeviceQueue(this->device, this->computeQueueFamilyIndex, 1, &this->computeQueue);
        }
        if (this->transferQueues.empty() && deviceQueues.MainQueueFamily.queueCount > 2)
        {
            this->transferQueueFamilyIndex = deviceQueues.MainQueueFamily.queueFamilyIndex;
            this->transferQueues.resize(deviceQueues.MainQueueFamily.queueCount - 2);
            for (uint32_t i = 0; i < this->transferQueues.size(); i++)
            {
                vkGetDeviceQueue(this->device, this->transferQueueFamilyIndex, i + 2, &this->transferQueues[i]);
            }
        }
        GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("using main queue from queue family #{}", this->mainQueueFamilyIndex));
        GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("using compute queue from queue family #{}", this->computeQueueFamilyIndex));
        GetCurrentLogger()->LogInfo("VulkanContext", fmt::format("using transfer queue from queue family #{}", this->transferQueueFamilyIndex));

        // debug callback creation
        if (info.UseValidationLayer)
        {
            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo = {};
            debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsMessengerCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
            debugUtilsMessengerCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugUtilsMessengerCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;

            VALX_VK_SUCCESS(funcs.vkCreateDebugUtilsMessengerEXT(this->instance, &debugUtilsMessengerCreateInfo, nullptr, &this->debugUtilsMessenger));
            GetCurrentLogger()->LogInfo("VulkanContext", "redirecting validation messages to the logger");
        }

        // allocator creation
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion = this->apiVersion;
        allocatorCreateInfo.physicalDevice = this->physicalDevice;
        allocatorCreateInfo.device = this->device;
        allocatorCreateInfo.instance = this->instance;
        VALX_VK_SUCCESS(vmaCreateAllocator(&allocatorCreateInfo, &this->allocator));
        GetCurrentLogger()->LogInfo("VulkanContext", "allocator created");

        // compiler creation
        glslang::InitializeProcess();
        GetCurrentLogger()->LogInfo("VulkanContext", "online compiler initialized");

        this->textureLoader = std::unique_ptr<TextureLoader>(new TextureLoader());
        this->shaderLoader = std::unique_ptr<ShaderLoader>(new VulkanShaderLoader());
    }

    VulkanContext::~VulkanContext()
    {
        glslang::FinalizeProcess();

        vmaDestroyAllocator(this->allocator);

        if (this->debugUtilsMessenger != VK_NULL_HANDLE)
            funcs.vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugUtilsMessenger, nullptr);

        vkDestroyDevice(this->device, nullptr);

        vkDestroyInstance(this->instance, nullptr);

        GetCurrentLogger()->LogInfo("VulkanContext", "vulkan context destroyed");
    }

    TextureLoader* VulkanContext::GetTextureLoader()
    {
        return this->textureLoader.get();
    }

    ShaderLoader* VulkanContext::GetShaderLoader()
    {
        return this->shaderLoader.get();
    }

    std::unique_ptr<Surface> VulkanContext::CreateSurface(const Window& window)
    {
        return std::unique_ptr<Surface>(new VulkanSurface(window));
    }

    std::unique_ptr<SwapChain> VulkanContext::CreateSwapChain(const Surface& surface)
    {
        return std::unique_ptr<SwapChain>(new VulkanSwapChain(surface));
    }

    std::unique_ptr<Texture> VulkanContext::CreateTexture(const TextureInfo& info)
    {
        return std::unique_ptr<Texture>(new VulkanTexture(info));
    }

    std::unique_ptr<Buffer> VulkanContext::CreateBuffer(const BufferInfo& info)
    {
        return std::unique_ptr<Buffer>(new VulkanBuffer(info));
    }

    std::unique_ptr<Shader> VulkanContext::CreateShader(const ShaderInfo& info)
    {
        return std::unique_ptr<Shader>(new VulkanShader(info));
    }

    VkInstance VulkanContext::GetInstance() const
    {
        return this->instance;
    }

    uint32_t VulkanContext::GetAPIVersion() const
    {
        return this->apiVersion;
    }

    VkPhysicalDevice VulkanContext::GetPhysicalDevice() const
    {
        return this->physicalDevice;
    }

    VkDevice VulkanContext::GetDevice() const
    {
        return this->device;
    }

    VmaAllocator VulkanContext::GetAllocator() const
    {
        return this->allocator;
    }

    VkQueue VulkanContext::GetMainQueue() const
    {
        return this->mainQueue;
    }

    VkQueue VulkanContext::GetComputeQueue() const
    {
        return this->computeQueue;
    }

    size_t VulkanContext::GetTransferQueueCount() const
    {
        return this->transferQueues.size();
    }

    VkQueue VulkanContext::GetTransferQueue(size_t index) const
    {
        return this->transferQueues[index];
    }

    uint32_t VulkanContext::GetMainQueueFamilyIndex() const
    {
        return this->mainQueueFamilyIndex;
    }

    uint32_t VulkanContext::GetComputeQueueFamilyIndex() const
    {
        return this->computeQueueFamilyIndex;
    }

    uint32_t VulkanContext::GetTransferQueueFamilyIndex() const
    {
        return this->transferQueueFamilyIndex;
    }

    VulkanContext* GetVulkanContext()
    {
        return static_cast<VulkanContext*>(GetCurrentContext());
    }

    VkPhysicalDeviceType ConvertDeviceTypeVulkan(DeviceType type)
    {
        switch (type)
        {
        case VALX::DeviceType::CPU:
            return VK_PHYSICAL_DEVICE_TYPE_CPU;
        case VALX::DeviceType::DISCRETE_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case VALX::DeviceType::INTEGRATED_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        case VALX::DeviceType::VIRTUAL_GPU:
            return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
        case VALX::DeviceType::ANY:
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
        default:
            VALX_ASSERT(false && "invalid device type");
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
        }
    }
}