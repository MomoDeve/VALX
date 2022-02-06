#include <api/Context.h>
#include <window/Window.h>

// TODO: move to dll
#include <backend/vulkan/VulkanContext.h>
#include <backend/vulkan/VulkanSwapChain.h>

#include <fstream>
#include <filesystem>

int main()
{
    if (std::filesystem::exists(APPLICATION_WORKING_DIRECTORY))
        std::filesystem::current_path(APPLICATION_WORKING_DIRECTORY);

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
    auto shaderLoader = context->GetShaderLoader();

    auto surface = context->CreateSurface(window);
    auto swapchain = context->CreateSwapChain(*surface);

    window.OnResize([&swapchain = *swapchain](VALX::Window& window, int width, int height)
    {
        swapchain.Recreate(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    });

    VALX::TextureInfo textureInfo;
    textureInfo.Name = "Simple Texture";
    textureInfo.Width = 1024;
    textureInfo.Height = 1024;
    textureInfo.Flags = VALX::TextureFlags::SAMPLED | VALX::TextureFlags::COPY_DST;
    textureInfo.Type = VALX::TextureType::TEXTURE_2D;
    textureInfo.TextureFormat = VALX::Format::R8G8B8A8_UNORM;
    auto texture = context->CreateTexture(textureInfo);

    VALX::BufferInfo bufferInfo;
    bufferInfo.Size = 64;
    bufferInfo.Flags = VALX::BufferFlags::UNIFORM_BUFFER;
    bufferInfo.MemoryType = VALX::BufferMemory::FROM_CPU_TO_GPU;
    bufferInfo.Name = "Uniform Buffer";
    auto buffer = context->CreateBuffer(bufferInfo);

    VALX::ShaderInfo shaderInfo;
    shaderInfo.Stages.push_back(
        shaderLoader->LoadFromSourceFile("main_vertex.glsl", VALX::ShaderStage::VERTEX, VALX::ShaderLanguage::GLSL)
    );
    shaderInfo.Stages.push_back(
        shaderLoader->LoadFromSourceFile("main_fragment.glsl", VALX::ShaderStage::FRAGMENT, VALX::ShaderLanguage::GLSL)
    );
    shaderInfo.Name = "Main Shader";
    auto shader = context->CreateShader(shaderInfo);

    while (!window.ShouldClose())
    {
        window.PollEvents();
    }
}