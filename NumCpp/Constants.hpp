/* Constants.hpp */
#pragma once

#include <limits>

namespace numcpp {
    
    constexpr double pi = 3.141592653589793;
    constexpr double e = 2.718281828459045;

    template <typename T>
    constexpr T inf = std::numeric_limits<T>::infinity();

}   // namespace numcpp
