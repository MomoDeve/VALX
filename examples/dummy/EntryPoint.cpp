#include <api/Context.h>
#include <window/Window.h>

// TODO: move to dll
#include <backend/vulkan/VulkanContext.h>
#include <backend/vulkan/VulkanSwapChain.h>

int main()
{
    VALX::WindowCreateInfo windowInfo;
    windowInfo.GraphicAPI = VALX::API::VULKAN;
    windowInfo.Width = 1280;
    windowInfo.Height = 720;
    windowInfo.TileBar = "Dummy Example";

    VALX::Window window(windowInfo);

    VALX::ContextCreateInfo contextInfo;
    contextInfo.APIMajorVersion = 1;
    contextInfo.APIMinorVersion = 2;
    contextInfo.ApplicationName = "Dummy Example";
    contextInfo.UseValidationLayer = true;
    contextInfo.Extensions = window.GetAPIExtensions();

    VALX::VulkanContext vulkanContext(contextInfo);
    VALX::SetCurrentContext(&vulkanContext);
    VALX::Context* context = VALX::GetCurrentContext();

    auto surface = context->CreateSurface(window);
    auto swapchain = context->CreateSwapChain(*surface);

    window.OnResize([&swapchain = *swapchain](VALX::Window& window, int width, int height)
    {
        swapchain.Recreate(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    });

    VALX::TextureInfo texInfo;
    texInfo.Name = "Simple Texture";
    texInfo.Width = 1024;
    texInfo.Height = 1024;
    texInfo.Flags = VALX::TextureFlags::SAMPLED;
    texInfo.Type = VALX::TextureType::TEXTURE_2D;
    texInfo.TextureFormat = VALX::Format::R8G8B8A8_UNORM;
    auto texture = context->CreateTexture(texInfo);

    while (!window.ShouldClose())
    {
        window.PollEvents();
    }
}