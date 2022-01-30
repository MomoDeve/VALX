#pragma once

#include <cstdint>

namespace VALX
{
    class Surface
    {
    public:
        using Handle = void*;

        virtual Surface::Handle GetHandle() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual ~Surface() = default;
    };
}