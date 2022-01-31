#pragma once

#include "Utilities.h"
#include "Format.h"

#include <string>

namespace VALX
{
    enum class BufferMemory
    {
        CPU_ONLY,
        GPU_ONLY,
        FROM_CPU_TO_GPU,
        FROM_GPU_TO_CPU,
        CPU_AS_BACKUP
    };

    enum class BufferFlags
    {
        NONE = 0,
        COPY_SRC = 1 << 0,
        COPY_DST = 1 << 1,
        UNIFORM_BUFFER = 1 << 2,
        STORAGE_BUFFER = 1 << 3,
        INDEX_BUFFER = 1 << 4,
        VERTEX_BUFFER = 1 << 5,
        INDIRECT_BUFFER = 1 << 6,
    };
    VALX_GENERATE_ENUM_OPS(BufferFlags)

    struct BufferInfo
    {
        std::string Name = "";
        BufferFlags Flags = BufferFlags::NONE;
        BufferMemory MemoryType = BufferMemory::GPU_ONLY;
        uint32_t Size = 0;
    };

    class Buffer
    {
    public:
        using Handle = void*;

        virtual const BufferInfo& GetInfo() const = 0;
        virtual Buffer::Handle GetHandle() const = 0;
        virtual ~Buffer() = default;
    };
}