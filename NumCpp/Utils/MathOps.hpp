/* Operations.hpp */
#pragma once

#include <cmath>
#include <type_traits>

#include "../Constants.hpp"


namespace numcpp::util {
/* ====== Operations =======*/
template <typename T> T add(T a, T b) {
    return a + b;
}

template <typename T> T subtract(T a, T b) {
    return a - b;
}

template <typename T> T multiply(T a, T b) {
    return a * b;
}

template <typename T> T divide(T a, T b) {
    if(b == 0) return inf<T>;
    else return a / b;
}

template <typename T> T pow(T a, T b) {
    return std::pow(a,b);
}


/* ====== Comparisons ====== */
template <typename T> T eq(T a, T b) {
    return a == b;
}

template <typename T> T neq(T a, T b) {
    return a != b;
}

template <typename T> T leq(T a, T b) {
    return a <= b;
}

template <typename T> T geq(T a, T b) {
    return a >= b;
}

template <typename T> T less_than(T a, T b) {
    return a < b;
}

template <typename T> T greater_than(T a, T b) {
    return a > b;
}

template <typename T>
int is_inf(T num) {
    if constexpr (std::is_floating_point_v<T>) {
        if(std::isinf(num)) {
            return (num > 0) ? 1 : -1;
        }
        return 0;
    } else {
        return 0;
    }
}

} // namespace numcpp::util