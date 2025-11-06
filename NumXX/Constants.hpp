/* Constants.hpp */
#pragma once

#include <limits>
#include <cmath>

namespace numxx {
    
    constexpr double pi = M_PI;
    constexpr double e = M_E;

    template <typename T = float>
    constexpr T inf_t = std::numeric_limits<T>::infinity();

    constexpr float inf = inf_t<float>;

}   // namespace numxx
