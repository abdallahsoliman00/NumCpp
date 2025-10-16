/* Operations.hpp */
#pragma once

#include <cmath>


namespace numcpp {

template <typename T>
bool isinf(T num) {
    if constexpr (std::is_floating_point_v<T>) {
        return std::isinf(num);
    } else {
        return false;
    }
}

} // namespace numcpp