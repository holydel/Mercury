#pragma once
#include <cstddef>

namespace mercury
{
    namespace utils
    {
        template <typename T, std::size_t N>
        constexpr std::size_t array_size(const T(&)[N]) noexcept {
            return N;
        }
    }
}
