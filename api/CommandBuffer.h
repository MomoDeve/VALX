#pragma once

#include "Utilities.h"

namespace VALX
{
    enum class CommandBufferFlags
    {
        NONE = 0,
        SUBMIT_ONCE = 1 << 0,
    };
    VALX_GENERATE_ENUM_OPS(CommandBufferFlags)

    class CommandBuffer
    {
    public:
        virtual void Begin(CommandBufferFlags flags) = 0;
        virtual void End() = 0;
        virtual ~CommandBuffer() = default;
    };
}