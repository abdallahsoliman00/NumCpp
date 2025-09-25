/* Operations.hpp */
#pragma once

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
    return a / b;
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

}