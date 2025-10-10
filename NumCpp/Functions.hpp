/* Functions.hpp */
#pragma once

#include <cmath>

#include "Core/Matrix.hpp"
#include "Utils/FuncUtils.hpp"


namespace numcpp {

/* ====== Trigonomeric Functions ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto sin(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::sin(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto cos(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::cos(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto tan(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::tan(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto asin(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::asin(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto acos(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::acos(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto atan(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::atan(x); } );
}


template <typename T>
inline auto sin(T num) -> decltype(std::sin(num)) {
    return std::sin(num);
}

template <typename T>
inline auto cos(T num) -> decltype(std::cos(num)) {
    return std::cos(num);
}

template <typename T>
inline auto tan(T num) -> decltype(std::tan(num)) {
    return std::tan(num);
}

template <typename T>
inline auto asin(T num) -> decltype(std::asin(num)) {
    return std::asin(num);
}

template <typename T>
inline auto acos(T num) -> decltype(std::acos(num)) {
    return std::acos(num);
}

template <typename T>
inline auto atan(T num) -> decltype(std::atan(num)) {
    return std::atan(num);
}



/* ====== Hyperbolic Functions ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto sinh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::sinh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto cosh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::cosh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto tanh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::tanh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto asinh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::asinh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto acosh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::acosh(x); } );
}

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto atanh(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::atanh(x); } );
}


template <typename T>
inline auto sinh(T num) -> decltype(std::sinh(num)) {
    return std::sinh(num);
}

template <typename T>
inline auto cosh(T num) -> decltype(std::cosh(num)) {
    return std::cosh(num);
}

template <typename T>
inline auto tanh(T num) -> decltype(std::tanh(num)) {
    return std::tanh(num);
}

template <typename T>
inline auto asinh(T num) -> decltype(std::asinh(num)) {
    return std::asinh(num);
}

template <typename T>
inline auto acosh(T num) -> decltype(std::acosh(num)) {
    return std::acosh(num);
}

template <typename T>
inline auto atanh(T num) -> decltype(std::atanh(num)) {
    return std::atanh(num);
}



/* ====== Exponentiation ====== */

template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
auto exp(const NArray<dtype>& arr) {
    return util::elementwiseFunc( arr, [] (dtype x) { return std::exp(x); } );
}

template <typename T>
inline auto exp(T num) -> decltype(std::exp(num)) {
    return std::exp(num);
}


template <typename dtype, typename = std::enable_if_t<std::is_arithmetic_v<dtype>>>
NArray<dtype> pow(const NArray<dtype>& arr, const int& exponent) {
    NArray<dtype> out(arr.get_shape());
    dtype* data = out.get_data();
    for(int i = 0; i < arr.get_shape().get_total_size(); i++) {
        data[i] = std::pow(data[i], exponent);
    }
    return out;
}

template <typename T, typename U>
inline auto pow(T base, U exponent) -> decltype(std::pow(base, exponent)) {
    return std::pow(base, exponent);
}


} // namespace numcpp
