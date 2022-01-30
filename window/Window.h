#pragma once

#include <functional>
#include <vector>
#include <string>

#include "api/Context.h"
#include "api/Utilities.h"

namespace VALX
{
    enum class KeyCode
    {
        UNKNOWN = -1,

        SPACE = 32,
        APOSTROPHE = 39,
        COMMA = 44,
        MINUS = 45,
        PERIOD = 46,
        SLASH = 47,
        _0 = 48,
        _1 = 49,
        _2 = 50,
        _3 = 51,
        _4 = 52,
        _5 = 53,
        _6 = 54,
        _7 = 55,
        _8 = 56,
        _9 = 57,
        SEMICOLON = 59,
        EQUAL = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LEFT_BRACKET = 91,
        BACKSLASH = 92,
        RIGHT_BRACKET = 93,
        GRAVE_ACCENT = 96,
        WORLD_1 = 161,
        WORLD_2 = 162,

        ESCAPE = 256,
        ENTER = 257,
        TAB = 258,
        BACKSPACE = 259,
        INSERT = 260,
        DELETE = 261,
        RIGHT = 262,
        LEFT = 263,
        DOWN = 264,
        UP = 265,
        PAGE_UP = 266,
        PAGE_DOWN = 267,
        HOME = 268,
        END = 269,
        CAPS_LOCK = 280,
        SCROLL_LOCK = 281,
        NUM_LOCK = 282,
        PRINT_SCREEN = 283,
        PAUSE = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        KP_0 = 320,
        KP_1 = 321,
        KP_2 = 322,
        KP_3 = 323,
        KP_4 = 324,
        KP_5 = 325,
        KP_6 = 326,
        KP_7 = 327,
        KP_8 = 328,
        KP_9 = 329,
        KP_DECIMAL = 330,
        KP_DIVIDE = 331,
        KP_MULTIPLY = 332,
        KP_SUBTRACT = 333,
        KP_ADD = 334,
        KP_ENTER = 335,
        KP_EQUAL = 336,
        LEFT_SHIFT = 340,
        LEFT_CONTROL = 341,
        LEFT_ALT = 342,
        LEFT_SUPER = 343,
        RIGHT_SHIFT = 344,
        RIGHT_CONTROL = 345,
        RIGHT_ALT = 346,
        RIGHT_SUPER = 347,
        MENU = 348,
    };

    enum class MouseButton
    {
        _1 = 0,
        _2 = 1,
        _3 = 2,
        _4 = 3,
        _5 = 4,
        _6 = 5,
        _7 = 6,
        _8 = 7,
        LAST = MouseButton::_8,
        LEFT = MouseButton::_1,
        RIGHT = MouseButton::_2,
        MIDDLE = MouseButton::_3,
    };

    enum class CursorMode
    {
        NORMAL = 0x00034001,
        HIDDEN = 0x00034002,
        DISABLED = 0x00034003,
    };

    struct WindowCreateInfo
    {
        API GraphicAPI = API::VULKAN;
        int Width = 800;
        int Height = 600;
        int PositionX = 0;
        int PositionY = 32;
        std::string Title = "VALX";
        bool Transparent = false;
        bool Resizeable = true;
        bool TileBar = true;
    };

    class Window
    {
    public:
        using Handle = void*;
        using ResizeCallback = std::function<void(Window&, int, int)>;
        using KeyChangedCallback = std::function<void(Window&, KeyCode, bool)>;
        using MouseChangedCallback = std::function<void(Window&, MouseButton, bool)>;

    private:
        Window::Handle handle = nullptr;
        API GraphicAPI = API::VULKAN;
        ResizeCallback onResize;
        KeyChangedCallback onKeyChanged;
        MouseChangedCallback onMouseChanged;

        bool IsAPISupported(API api);

    public:
        Window(const WindowCreateInfo& options);
        ~Window();

        VALX_NO_COPY_NO_MOVE(Window);

        Window::Handle GetHandle() const;
        std::vector<std::string> GetAPIExtensions() const;

        void PollEvents() const;
        bool ShouldClose() const;
        void Close();

        int GetWidth() const;
        int GetHeight() const;

        int GetPositionX() const;
        int GetPositionY() const;

        double GetCursorPositionX() const;
        double GetCursorPositionY() const;
        CursorMode GetCursorMode() const;

        void SetSize(int width, int height);
        void SetPosition(int positionX, int positionY);
        void SetCursorPosition(double positionX, double positionY);
        void SetCursorMode(CursorMode mode);
        void SetTitle(const std::string& title);

        bool IsKeyPressed(KeyCode key) const;
        bool IsKeyReleased(KeyCode key) const;

        bool IsMousePressed(MouseButton button) const;
        bool IsMouseReleased(MouseButton button) const;

        double GetTimeSinceCreation() const;
        void SetTimeSinceCreation(double time);

        void OnResize(ResizeCallback callback);
        void OnKeyChanged(KeyChangedCallback callback);
        void OnMouseChanged(MouseChangedCallback callback);
    };
}