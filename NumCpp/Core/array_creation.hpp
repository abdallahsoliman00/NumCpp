/* array_creation.hpp */
#pragma once

#include "NArray.hpp"
#include <cmath>

namespace numcpp {
    // TODO: Open https://numpy.org/doc/2.3/reference/routines.array-creation.html and implement functions.

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros(const size_t& size) {
        return NArray<T>(size, static_cast<T>(0));
    }

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros(const Shape& shape) {
        return NArray<T>(shape, static_cast<T>(0));
    }


    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), static_cast<T>(0));
    }

    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> zeros_like(const std::vector<U>& other) {
        return NArray<T>(other.size(), static_cast<T>(0));
    }


    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones(const size_t& size) {
        return NArray<T>(size, static_cast<T>(1));
    }

    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones(const Shape& shape) {
        return NArray<T>(shape, static_cast<T>(1));
    }


    template <typename T = double, typename U, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> ones_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape(), static_cast<T>(1));
    }


    NArray<double> linspace(const float& start, const float& stop, const size_t& count, const bool& endpoint = true) {
        float step = (stop - start)/(count - static_cast<float>(endpoint));
        std::vector<double> out(count);

        for(size_t i = 1; i < count; i++) {
            out[i] = start + (step * i);
        }
        return NArray(out);
    }


    NArray<double> arange(const float& start, const float& stop, const float& step = 1) {
        size_t count = static_cast<size_t>(1 + (stop - start)/step);
        std::vector<double> out(count);

        for(size_t i = 1; i < count; i++) {
            out[i] = start + step * i;
        }
        return NArray(std::move(out));
    }


    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> eye(size_t n, size_t m = 0) {
        if (m == 0) m = n;

        std::vector<T> out_data(n * m, static_cast<T>(0));

        for (size_t i = 0; i < n && i < m; ++i) {
            out_data[i * m + i] = static_cast<T>(1);
        }

        return NArray<T>(std::move(out_data), Shape({n, m}));
    }


    template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<T> identity(size_t n) {
        return eye<T>(n);
    }


    template <typename T = double>
    NArray<T> empty(const Shape& shape) {
        return NArray<T>(shape);
    }

    template <typename T = double>
    NArray<T> empty(Shape&& shape) {
        return NArray<T>(std::move(shape));
    }

    template <typename T = double>
    NArray<T> empty(std::initializer_list<T> list) {
        return NArray<T>(Shape(list));
    }

    template <typename T = double>
    NArray<T> empty(size_t size) {
        return NArray<T>(Shape(size));
    }

    template <typename T = double, typename U>
    NArray<T> empty_like(const NArray<U>& other) {
        return NArray<T>(other.get_shape());
    }


} // namespace numcpp
