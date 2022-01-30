#include "Window.h"

#include "api/Logger.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VALX
{
    static int ConvertGraphicAPI(API api)
    {
        switch (api)
        {
        case VALX::API::VULKAN:
            return GLFW_NO_API;
        default:
            return GLFW_NO_API;
        }
    }

    bool Window::IsAPISupported(API api)
    {
        switch (api)
        {
        case VALX::API::VULKAN:
        {
            int result = glfwVulkanSupported();
            return static_cast<bool>(result);
        }
        default:
        {
            return false;
        }
        }
    }

    Window::Window(const WindowCreateInfo& options)
    {
        if (glfwInit() != GLFW_TRUE)
        {
            GetCurrentLogger()->LogError("Window", "context initialization failed");
            return;
        }

        if (!this->IsAPISupported(options.GraphicAPI))
        {
            GetCurrentLogger()->LogError("Window", "graphic api is not supported");
            return;
        }
        this->GraphicAPI = options.GraphicAPI;

        glfwWindowHint(GLFW_CLIENT_API, ConvertGraphicAPI(this->GraphicAPI));
        glfwWindowHint(GLFW_DECORATED, options.TileBar);
        glfwWindowHint(GLFW_RESIZABLE, options.Resizeable);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, options.Transparent);

        GLFWwindow* window = glfwCreateWindow(options.Width, options.Height, options.Title.c_str(), nullptr, nullptr);
        if (window == nullptr)
        {
            GetCurrentLogger()->LogError("Window", "window creation failed");
            return;
        }
        this->handle = window;
        GetCurrentLogger()->LogInfo("Window", "window created");

        glfwSetWindowPos(window, options.PositionX, options.PositionY);
        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetWindowSizeCallback(window, [](GLFWwindow* handle, int width, int height)
            {
                auto& window = *(Window*)glfwGetWindowUserPointer(handle);
                if (window.onResize) window.onResize(window, width, height);
            });
        glfwSetKeyCallback(window, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
            {
                auto& window = *(Window*)glfwGetWindowUserPointer(handle);
                if (window.onKeyChanged) window.onKeyChanged(window, (KeyCode)key, action == GLFW_PRESS);
            });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* handle, int button, int action, int mods)
            {
                auto& window = *(Window*)glfwGetWindowUserPointer(handle);
                if (window.onMouseChanged) window.onMouseChanged(window, (MouseButton)button, action == GLFW_PRESS);
            });
    }

    Window::~Window()
    {
        glfwDestroyWindow(static_cast<GLFWwindow*>(this->handle));
        this->handle = nullptr;
        GetCurrentLogger()->LogInfo("Window", "window destroyed");
    }

    Window::Handle Window::GetHandle() const
    {
        return this->handle;
    }

    std::vector<std::string> Window::GetAPIExtensions() const
    {
        switch (this->GraphicAPI)
        {
        case VALX::API::VULKAN:
        {
            uint32_t extensionCount = 0;
            const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
            return std::vector<std::string>(extensions, extensions + extensionCount);
        }
        default:
        {
            return {};
        }
        }
    }

    void Window::PollEvents() const
    {
        glfwPollEvents();
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(static_cast<GLFWwindow*>(this->handle));
    }

    void Window::Close()
    {
        glfwSetWindowShouldClose(static_cast<GLFWwindow*>(this->handle), true);
    }

    int Window::GetWidth() const
    {
        int width = 0, height = 0;
        glfwGetWindowSize(static_cast<GLFWwindow*>(this->handle), &width, &height);
        return width;
    }

    int Window::GetHeight() const
    {
        int width = 0, height = 0;
        glfwGetWindowSize(static_cast<GLFWwindow*>(this->handle), &width, &height);
        return height;
    }

    int Window::GetPositionX() const
    {
        int positionX = 0, positionY = 0;
        glfwGetWindowPos(static_cast<GLFWwindow*>(this->handle), &positionX, &positionY);
        return positionX;
    }

    int Window::GetPositionY() const
    {
        int positionX = 0, positionY = 0;
        glfwGetWindowPos(static_cast<GLFWwindow*>(this->handle), &positionX, &positionY);
        return positionY;
    }

    double Window::GetCursorPositionX() const
    {
        double positionX = 0, positionY = 0;
        glfwGetCursorPos(static_cast<GLFWwindow*>(this->handle), &positionX, &positionY);
        return positionX;
    }

    double Window::GetCursorPositionY() const
    {
        double positionX = 0, positionY = 0;
        glfwGetCursorPos(static_cast<GLFWwindow*>(this->handle), &positionX, &positionY);
        return positionY;
    }

    bool Window::IsKeyPressed(KeyCode key) const
    {
        return glfwGetKey(static_cast<GLFWwindow*>(this->handle), (int)key) == GLFW_PRESS;
    }

    bool Window::IsKeyReleased(KeyCode key) const
    {
        return glfwGetKey(static_cast<GLFWwindow*>(this->handle), (int)key) == GLFW_RELEASE;
    }

    bool Window::IsMousePressed(MouseButton button) const
    {
        return glfwGetMouseButton(static_cast<GLFWwindow*>(this->handle), (int)button) == GLFW_PRESS;
    }

    bool Window::IsMouseReleased(MouseButton button) const
    {
        return glfwGetMouseButton(static_cast<GLFWwindow*>(this->handle), (int)button) == GLFW_RELEASE;
    }

    CursorMode Window::GetCursorMode() const
    {
        return (CursorMode)glfwGetInputMode(static_cast<GLFWwindow*>(this->handle), GLFW_CURSOR);
    }

    void Window::SetSize(int width, int height)
    {
        glfwSetWindowSize(static_cast<GLFWwindow*>(this->handle), width, height);
    }

    void Window::SetPosition(int positionX, int positionY)
    {
        glfwSetWindowPos(static_cast<GLFWwindow*>(this->handle), positionX, positionY);
    }

    void Window::SetCursorPosition(double positionX, double positionY)
    {
        glfwSetCursorPos(static_cast<GLFWwindow*>(this->handle), positionX, positionY);
    }

    void Window::SetCursorMode(CursorMode mode)
    {
        glfwSetInputMode(static_cast<GLFWwindow*>(this->handle), GLFW_CURSOR, (int)mode);
    }

    void Window::SetTitle(const std::string& title)
    {
        glfwSetWindowTitle(static_cast<GLFWwindow*>(this->handle), title.c_str());
    }

    double Window::GetTimeSinceCreation() const
    {
        return glfwGetTime();
    }

    void Window::SetTimeSinceCreation(double time)
    {
        return glfwSetTime(time);
    }

    void Window::OnResize(ResizeCallback callback)
    {
        this->onResize = std::move(callback);
    }

    void Window::OnKeyChanged(KeyChangedCallback callback)
    {
        this->onKeyChanged = std::move(callback);
    }

    void Window::OnMouseChanged(MouseChangedCallback callback)
    {
        this->onMouseChanged = std::move(callback);
    }
}