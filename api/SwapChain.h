#pragma once

#include <cstdint>

namespace VALX
{
    class SwapChain
    {
    public:
        virtual void Recreate(uint32_t width, uint32_t height) = 0;
        virtual ~SwapChain() = default;
    };
}