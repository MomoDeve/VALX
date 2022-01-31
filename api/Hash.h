#pragma once

#include <functional>
#include <cstddef>

namespace VALX
{
    template <typename T>
    void HashCombine(size_t& seed, const T& other)
    {
        seed ^= std::hash<T>{}(other)+0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}