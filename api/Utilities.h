#pragma once

#include <cassert>
#include <cstdint>

namespace VALX
{
    #define VALX_ASSERT(...) assert(__VA_ARGS__)

    inline uint32_t GetMipLevelCount(uint32_t size)
    {
        uint32_t count = 1;
        while (size >>= 1)
            ++count;
        return count;
    }

    #define VALX_NO_COPY(className) className(const className&) = delete; className& operator=(const className&) = delete
    #define VALX_NO_COPY_NO_MOVE(className) VALX_NO_COPY(className); className(className&&) = delete; className& operator=(className&&) = delete
    #define VALX_NO_COPY_DEFAULT_MOVE(className) VALX_NO_COPY(className); className(className&&) = default; className& operator=(className&&) = default

    #define VALX_GENERATE_ENUM_OPS(type)\
    inline type operator&(type f1, type f2)\
    {\
        return static_cast<type>(static_cast<uint64_t>(f1) & static_cast<uint64_t>(f2));\
    }\
    inline type operator|(type f1, type f2)\
    {\
        return static_cast<type>(static_cast<uint64_t>(f1) | static_cast<uint64_t>(f2));\
    }\
    inline type operator&=(type& f1, type f2)\
    {\
        return f1 = (f1 & f2);\
    }\
    inline type operator|=(type& f1, type f2)\
    {\
        return f1 = (f1 | f2);\
    }
}