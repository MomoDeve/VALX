#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Surface.h"
#include "SwapChain.h"
#include "Texture.h"
#include "Buffer.h"

namespace VALX
{
    enum class API
    {
        VULKAN,
    };

    enum class DeviceType
    {
        CPU,
        DISCRETE_GPU,
        INTEGRATED_GPU,
        VIRTUAL_GPU,
        ANY,
    };

    struct ContextCreateInfo
    {
        uint32_t APIMajorVersion = 1;
        uint32_t APIMinorVersion = 2;
        bool UseValidationLayer = true;
        std::string ApplicationName = "VALX";
        std::string EngineName = "VALX";
        DeviceType PreferredDeviceType = DeviceType::DISCRETE_GPU;
        std::vector<std::string> Extensions;
    };

    class Context
    {
    public:
        virtual std::unique_ptr<Surface> CreateSurface(const class Window& window) = 0;
        virtual std::unique_ptr<SwapChain> CreateSwapChain(const Surface& surface) = 0;
        virtual std::unique_ptr<Texture> CreateTexture(const TextureInfo& info) = 0;
        virtual std::unique_ptr<Buffer> CreateBuffer(const BufferInfo& info) = 0;

        virtual ~Context() = default;
    };

    Context* SetCurrentContext(Context* context);
    Context* GetCurrentContext();
}